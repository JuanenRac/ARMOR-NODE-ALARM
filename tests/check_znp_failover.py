#!/usr/bin/env python3
# ARMOR-NODE-ALARM - the coordinator stand-in (tools/znp_coordinator_sim.py) and, when the failover node program is given, the failover run against it:
#   1. the stand-in alone: the frames encode as the firmware's tests say, the start-up conversation is answered, a sensor's report and a siren's warning are read right;
#   2. with the program built from tests/znp_failover_node.cpp as the first argument: the node takes the radio over, brings the coordinator up, learns the sensor and the siren, arms, alarms
#      when the sensor opens, tells the siren to sound, and lets the radio go when the server is back. Without the argument that part is skipped (it needs a compiler and sockets).
# Copyright (C) 2026 JuanenRac (Electro Hobby 3D). GPL-3.0-or-later.
from __future__ import annotations

import subprocess
import sys
import threading
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import znp_coordinator_sim as sim  # noqa: E402

failures = 0


def check(condition: bool, what: str) -> None:
    global failures
    if not condition:
        failures += 1
        print(f"FAIL {what}")


DOOR, SIREN = 0x00158D0001A2B3C4, 0x00158D0001FFEE01


def alone() -> None:
    # the same bytes the firmware's host tests expect
    check(sim.encode(sim.SREQ, sim.SYS, 0x01) == bytes([0xFE, 0x00, 0x21, 0x01, 0x20]), "SYS_PING bytes")
    check(sim.encode(sim.SRSP, sim.SYS, 0x01, bytes([0x79, 0x00])) == bytes([0xFE, 0x02, 0x61, 0x01, 0x79, 0x00, 0x1B]), "SYS_PING answer bytes")
    parser = sim.Parser()
    good = sim.encode(sim.AREQ, sim.ZDO, 0xC0, bytes([9]))
    bad = bytearray(good)
    bad[-1] ^= 0xFF
    frames = parser.feed(b"\x00\x13" + bytes(bad) + good)
    check(parser.dropped == 1 and frames == [(sim.AREQ, sim.ZDO, 0xC0, bytes([9]))], "a bad checksum is dropped and the next frame kept")

    c = sim.Coordinator(devices=[sim.Device("door", DOOR, 0x1234), sim.Device("siren", SIREN, 0x3456)])
    reply = sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.SYS, 0x01)))
    check(reply == [(sim.SRSP, sim.SYS, 0x01, bytes([0x79, 0x00]))] and c.pings == 1, "ping answered")
    reply = sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.AF, 0x00, bytes([1]))))
    check(reply[0][:3] == (sim.SRSP, sim.AF, 0x00) and c.registered, "register answered")
    reply = sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.ZDO, 0x40, bytes([100, 0]))))
    check(reply[0][:3] == (sim.SRSP, sim.ZDO, 0x40) and c.started, "start-up answered")
    check(sim.Parser().feed(c.network_up()) == [(sim.AREQ, sim.ZDO, 0xC0, bytes([9]))], "network up")
    reply = sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.ZDO, 0x00, DOOR.to_bytes(8, "little") + bytes([0, 0]))))
    check(len(reply) == 2 and reply[1][:3] == (sim.AREQ, sim.ZDO, 0x80) and reply[1][3][0] == 0 and int.from_bytes(reply[1][3][9:11], "little") == 0x1234, "known device gets its short address")
    reply = sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.ZDO, 0x00, (0x1122).to_bytes(8, "little") + bytes([0, 0]))))
    check(reply[1][3][0] != 0, "unknown device gets an error status")
    # a sensor's report is an incoming message from the short address with the cluster and the status in it
    report = sim.Parser().feed(c.report("door", alarm=True, battery_low=True))[0]
    data = report[3]
    check(report[:3] == (sim.AREQ, sim.AF, 0x81) and int.from_bytes(data[2:4], "little") == 0x0500 and int.from_bytes(data[4:6], "little") == 0x1234, "report frame")
    check(data[-9:] == bytes([0x09, 0x01, 0x00, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00]), "alarm and low battery in the status")
    # a siren told to sound and to stop
    warning = bytes([0x11, 9, 0x00, 0x17, 60, 0, 50, 1])
    request = (0x3456).to_bytes(2, "little") + bytes([1, 1]) + (0x0502).to_bytes(2, "little") + bytes([9, 0, 30, len(warning)]) + warning
    check(sim.Parser().feed(c.feed(sim.encode(sim.SREQ, sim.AF, 0x01, request)))[0][:3] == (sim.SRSP, sim.AF, 0x01), "data request answered")
    stop = bytes([0x11, 10, 0x00, 0x03, 0, 0, 0, 0])
    c.feed(sim.encode(sim.SREQ, sim.AF, 0x01, (0x3456).to_bytes(2, "little") + bytes([1, 1]) + (0x0502).to_bytes(2, "little") + bytes([10, 0, 30, len(stop)]) + stop))
    check(c.warnings == [sim.Warning(0x3456, 1, True, 60), sim.Warning(0x3456, 0, False, 0)], "warnings read")
    c.feed(sim.encode(sim.SREQ, sim.AF, 0x01, (0x1234).to_bytes(2, "little") + bytes([1, 1]) + (0x0500).to_bytes(2, "little") + bytes([11, 0, 30, 5]) + bytes([0x11, 5, 0x00, 0x00, 1])))
    check(c.enroll_responses == [0x1234], "enroll response read")
    check(sim.Parser().feed(c.feed(sim.encode(sim.SREQ, 7, 0x55)))[0][:2] == (sim.SRSP, 0) and c.unknown, "an unsupported request is answered with an error, not ignored")


def failover(exe: str) -> None:
    c = sim.Coordinator(devices=[sim.Device("front-door", DOOR, 0x1234), sim.Device("siren", SIREN, 0x3456)])
    ready = threading.Event()
    lines: list[str] = []
    port_box: list[int] = []

    def serve() -> None:
        import socket  # the port is not known until it listens: bind here and tell the main thread
        probe = socket.socket()
        probe.bind(("127.0.0.1", 0))
        port_box.append(probe.getsockname()[1])
        probe.close()
        sim.run(c, ("127.0.0.1", port_box[0]), [(0.5, "front-door", {}), (3.0, "front-door", {"alarm": True})], 12.0, ready, log=lines.append)

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()
    ready.wait(5)
    check(bool(port_box), "the stand-in listens")
    run = subprocess.run([exe, "127.0.0.1", str(port_box[0]), f"{DOOR:016x}", f"{SIREN:016x}"], capture_output=True, text=True, timeout=60)
    thread.join(20)
    words = [line.split()[0] for line in run.stdout.splitlines()]
    check(run.returncode == 0, f"the node program ended well (code {run.returncode}): {run.stdout!r} {run.stderr!r}")
    order = ["TAKEOVER", "READY", "ARM", "SIREN", "RELEASED"]
    check("ALARM" in words, "the panel alarms")
    positions = [words.index(w) if w in words else -1 for w in order]
    check(all(p >= 0 for p in positions) and positions == sorted(positions), f"the failover happens in order: {words}")
    check("UPDATE front-door normal" in run.stdout and "UPDATE front-door triggered" in run.stdout, "the sensor's reports reach the zone")
    check("SIREN on 1" in run.stdout, "the siren is told to sound")
    check("ARM ok" in run.stdout, "the panel armed")
    check(c.pings >= 1 and c.registered and c.started, "the coordinator was brought up")
    check(DOOR in c.address_requests and SIREN in c.address_requests, "the sensor and the siren were looked for")
    check(any(w.short == 0x3456 and w.mode == 1 and w.strobe and w.seconds == 60 for w in c.warnings), f"the siren received a burglar warning: {c.warnings}")
    check(any("client gone" in line for line in lines), "the node let the radio go")


def main() -> int:
    alone()
    if len(sys.argv) > 1:
        failover(sys.argv[1])
    else:
        print("note: failover run skipped (no node program given)")
    print(f"{'PASS' if failures == 0 else 'FAIL'}: znp coordinator stand-in ({failures} failures)")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
