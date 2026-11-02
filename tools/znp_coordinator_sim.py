#!/usr/bin/env python3
# ARMOR-NODE-ALARM - a Zigbee coordinator that exists only as a program: it speaks the serial protocol of a Texas Instruments Z-Stack coordinator (ZNP) over TCP, the way the node's bridge
# lends the radio's line, so the node's own Zigbee conversation (core/local_zigbee.hpp) can be tried end to end, failover included, with no radio.
#
#   py -3 -I tools/znp_coordinator_sim.py --listen 127.0.0.1:7000 \
#       --device front-door=00158d0001a2b3c4:0x1234 --device siren=00158d0001ffee01:0x3456 \
#       --report front-door:closed@0.5 --report front-door:open@3
#
# It answers SYS_PING, AF_REGISTER, ZDO_STARTUP_FROM_APP and ZDO_NWK_ADDR_REQ, announces the network as up, plays the Zone Status Change Notifications of the sensors it is told
# about (times in seconds after the network came up), and records what it is asked to send, in particular the warning a siren is told to sound. Every client that connects finds
# it as after a reset. The frame layouts are the ones the firmware uses; nothing here has been compared with a real coordinator.
# Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
from __future__ import annotations

import argparse
import socket
import sys
import threading
import time
from dataclasses import dataclass, field

SOF = 0xFE
SREQ, AREQ, SRSP = 1, 2, 3
SYS, AF, ZDO = 1, 4, 5
CLUSTER_IAS_ZONE, CLUSTER_IAS_WD = 0x0500, 0x0502


def encode(kind: int, subsystem: int, command: int, data: bytes = b"") -> bytes:
    """One frame: SOF, length, type and subsystem, command, data, and the XOR of everything between SOF and the checksum."""
    if len(data) > 250:
        raise ValueError("data too long for a frame")
    body = bytes([len(data), (kind << 5) | (subsystem & 0x1F), command]) + data
    fcs = 0
    for byte in body:
        fcs ^= byte
    return bytes([SOF]) + body + bytes([fcs])


class Parser:
    """Frames out of a stream of bytes; a frame with a wrong checksum is dropped and counted."""

    def __init__(self) -> None:
        self.buffer = bytearray()
        self.dropped = 0

    def feed(self, data: bytes) -> list[tuple[int, int, int, bytes]]:
        self.buffer.extend(data)
        frames = []
        while True:
            start = self.buffer.find(bytes([SOF]))
            if start < 0:
                self.buffer.clear()
                break
            del self.buffer[:start]
            if len(self.buffer) < 2:
                break
            length = self.buffer[1]
            if length > 250:
                self.dropped += 1
                del self.buffer[:1]
                continue
            total = length + 5
            if len(self.buffer) < total:
                break
            frame = bytes(self.buffer[:total])
            fcs = 0
            for byte in frame[1:-1]:
                fcs ^= byte
            if fcs != frame[-1]:
                self.dropped += 1
                del self.buffer[:1]
                continue
            del self.buffer[:total]
            frames.append((frame[2] >> 5, frame[2] & 0x1F, frame[3], frame[4:-1]))
        return frames


def zone_status_change(alarm: bool = False, tamper: bool = False, battery_low: bool = False, sequence: int = 1, zone_id: int = 1) -> bytes:
    """The ZCL frame of an IAS Zone Status Change Notification (cluster specific, from the server, command 0)."""
    status = (1 if alarm else 0) | (4 if tamper else 0) | (8 if battery_low else 0)
    return bytes([0x09, sequence & 0xFF, 0x00, status & 0xFF, status >> 8, 0x00, zone_id, 0x00, 0x00])


def incoming_message(cluster: int, source: int, zcl: bytes, link_quality: int = 200) -> bytes:
    """An AF_INCOMING_MSG: what the coordinator hands the host when a device sent a message to the host's endpoint."""
    data = bytearray()
    data += (0).to_bytes(2, "little") + cluster.to_bytes(2, "little") + source.to_bytes(2, "little")
    data += bytes([1, 1, 0, link_quality, 0]) + bytes(4) + bytes([7, len(zcl)]) + zcl
    return encode(AREQ, AF, 0x81, bytes(data))


@dataclass
class Device:
    name: str
    ieee: int
    short: int


@dataclass
class Warning:
    short: int
    mode: int          # 0 stop, 1 burglar, 2 fire, ...
    strobe: bool
    seconds: int


@dataclass
class Coordinator:
    """The coordinator itself: bytes in, bytes out. Nothing here touches a socket or a clock, so it can be tested alone."""
    devices: list[Device] = field(default_factory=list)
    registered: bool = False
    started: bool = False
    pings: int = 0
    address_requests: list[int] = field(default_factory=list)
    warnings: list[Warning] = field(default_factory=list)
    enroll_responses: list[int] = field(default_factory=list)   # the short addresses answered
    unknown: list[tuple[int, int, int]] = field(default_factory=list)
    parser: Parser = field(default_factory=Parser)

    def reset(self) -> None:
        self.registered = False
        self.started = False
        self.parser = Parser()

    def device(self, name: str) -> Device:
        for device in self.devices:
            if device.name == name:
                return device
        raise KeyError(name)

    def feed(self, data: bytes) -> bytes:
        """Bytes from the host; the bytes to send back at once. The "network is up" message is not here: `network_up` makes it, after the start-up request."""
        out = b""
        for kind, subsystem, command, body in self.parser.feed(data):
            if kind != SREQ:
                continue
            out += self._request(subsystem, command, body)
        return out

    def network_up(self) -> bytes:
        """ZDO_STATE_CHANGE_IND: the device is up as the coordinator of the network."""
        return encode(AREQ, ZDO, 0xC0, bytes([9]))

    def report(self, name: str, **flags: bool) -> bytes:
        """A sensor's Zone Status Change Notification, as the host would receive it."""
        device = self.device(name)
        return incoming_message(CLUSTER_IAS_ZONE, device.short, zone_status_change(**flags))

    def _request(self, subsystem: int, command: int, body: bytes) -> bytes:
        if (subsystem, command) == (SYS, 0x01):
            self.pings += 1
            return encode(SRSP, SYS, 0x01, bytes([0x79, 0x00]))
        if (subsystem, command) == (AF, 0x00):
            self.registered = True
            return encode(SRSP, AF, 0x00, bytes([0x00]))
        if (subsystem, command) == (ZDO, 0x40):
            self.started = True
            return encode(SRSP, ZDO, 0x40, bytes([0x00]))
        if (subsystem, command) == (ZDO, 0x00) and len(body) >= 8:
            ieee = int.from_bytes(body[:8], "little")
            self.address_requests.append(ieee)
            out = encode(SRSP, ZDO, 0x00, bytes([0x00]))
            for device in self.devices:
                if device.ieee == ieee:
                    return out + encode(AREQ, ZDO, 0x80, bytes([0]) + ieee.to_bytes(8, "little") + device.short.to_bytes(2, "little") + bytes([0, 0]))
            return out + encode(AREQ, ZDO, 0x80, bytes([0xCD]) + ieee.to_bytes(8, "little") + bytes(4))   # no such device
        if (subsystem, command) == (AF, 0x01) and len(body) >= 10:
            destination = int.from_bytes(body[0:2], "little")
            cluster = int.from_bytes(body[4:6], "little")
            zcl = body[10:10 + body[9]]
            self._data_request(destination, cluster, zcl)
            return encode(SRSP, AF, 0x01, bytes([0x00]))
        self.unknown.append((subsystem, command, len(body)))
        return encode(SRSP, 0, 0x00, bytes([subsystem, command]))   # SYS_RPC_ERROR: command not supported

    def _data_request(self, destination: int, cluster: int, zcl: bytes) -> None:
        if cluster == CLUSTER_IAS_WD and len(zcl) >= 8 and zcl[0] & 0x03 == 0x01 and zcl[2] == 0x00:   # Start Warning
            info = zcl[3]
            self.warnings.append(Warning(destination, info >> 4, bool(info & 0x04),int.from_bytes(zcl[4:6], "little")))
        elif cluster == CLUSTER_IAS_ZONE and len(zcl) >= 5 and zcl[2] == 0x00:                          # Zone Enroll Response
            self.enroll_responses.append(destination)


def run(coordinator: Coordinator, listen: tuple[str, int], reports: list[tuple[float, str, dict[str, bool]]], quit_after: float | None, ready: threading.Event | None = None, log=print) -> int:
    """Serves clients one after the other until `quit_after` seconds have passed since the first connection (never, if None). Returns the port it listened on."""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(listen)
    server.listen(1)
    port = server.getsockname()[1]
    log(f"listening {listen[0]}:{port}")
    if ready is not None:
        ready.set()
    first = None
    try:
        while True:
            server.settimeout(0.2)
            try:
                connection, _ = server.accept()
            except socket.timeout:
                if first is not None and quit_after is not None and time.monotonic() - first >= quit_after:
                    break
                continue
            first = first if first is not None else time.monotonic()
            log("client connected")
            _serve(coordinator, connection, reports, log)
            log("client gone")
            if quit_after is not None and time.monotonic() - first >= quit_after:
                break
    finally:
        server.close()
    return port


def _serve(coordinator: Coordinator, connection: socket.socket, reports: list[tuple[float, str, dict[str, bool]]], log) -> None:
    coordinator.reset()
    pending = sorted(reports, key=lambda r: r[0])
    up_at: float | None = None
    announce_at: float | None = None
    connection.settimeout(0.05)
    with connection:
        while True:
            now = time.monotonic()
            if announce_at is not None and now >= announce_at:
                connection.sendall(coordinator.network_up())
                log("network up")
                up_at, announce_at = now, None
            if up_at is not None:
                while pending and now - up_at >= pending[0][0]:
                    _, name, flags = pending.pop(0)
                    connection.sendall(coordinator.report(name, **flags))
                    log(f"report {name} {flags}")
            try:
                data = connection.recv(4096)
            except socket.timeout:
                continue
            except OSError:
                return
            if not data:
                return
            before = (coordinator.started, len(coordinator.warnings), len(coordinator.address_requests))
            connection.sendall(coordinator.feed(data))
            if coordinator.started and not before[0]:
                announce_at = time.monotonic() + 0.2
                log("started")
            for warning in coordinator.warnings[before[1]:]:
                log(f"warning short=0x{warning.short:04x} mode={warning.mode} strobe={int(warning.strobe)} seconds={warning.seconds}")
            for ieee in coordinator.address_requests[before[2]:]:
                log(f"address request {ieee:016x}")


def parse_device(text: str) -> Device:
    name, _, rest = text.partition("=")
    ieee, _, short = rest.partition(":")
    return Device(name, int(ieee, 16), int(short, 0))


def parse_report(text: str) -> tuple[float, str, dict[str, bool]]:
    what, _, at = text.partition("@")
    name, _, state = what.partition(":")
    flags = {"closed": {}, "open": {"alarm": True}, "tamper": {"tamper": True}, "battery": {"battery_low": True}}
    if state not in flags:
        raise argparse.ArgumentTypeError(f"state must be one of {', '.join(flags)}")
    return float(at or 0), name, flags[state]


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="A ZNP coordinator stand-in over TCP, for the node's own Zigbee conversation.")
    ap.add_argument("--listen", default="127.0.0.1:7000", help="address:port to listen on (port 0 picks one)")
    ap.add_argument("--device", action="append", default=[], help="name=ieee16hex:short (a sensor or a siren)")
    ap.add_argument("--report", action="append", default=[], type=parse_report, help="name:closed|open|tamper|battery@seconds after the network came up")
    ap.add_argument("--quit-after", type=float, default=None, help="stop this many seconds after the first client connected")
    args = ap.parse_args(argv)
    host, _, port = args.listen.rpartition(":")
    coordinator = Coordinator(devices=[parse_device(d) for d in args.device])
    run(coordinator, (host or "127.0.0.1", int(port)), args.report, args.quit_after, log=lambda line: print(line, flush=True))
    return 0


if __name__ == "__main__":
    sys.exit(main())
