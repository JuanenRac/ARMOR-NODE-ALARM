<p align="center">
  <img src="images/ARMOR_BANNER.svg" alt="ARMOR-NODE-ALARM-Banner" width="100%">
</p>

# 🚨 ARMOR-NODE-ALARM

<p align="center">
  <a href="README.md">🇺🇸 English</a> |
  <a href="README_spa.md">🇪🇸 Español</a> |
  <a href="README_fra.md">🇫🇷 Français</a> |
  <a href="README_ita.md">🇮🇹 Italiano</a> |
  🇩🇪 <b>Deutsch</b> |
  <a href="README_zho.md">🇨🇳 简体中文</a> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  <a href="README_por.md">🇵🇹 Português</a>
</p>

### Alarmknoten und Alarmzentrale: die Zonen, das Scharfschalten, die Verzögerungen, die Sirene und die PIN der Hausalarmanlage, mit der Firmware des Knotens und den auf einem Rechner getesteten Regeln (nichts lief auf einer Platine)

<p align="center">
  <img src="https://img.shields.io/badge/License-GPL%203.0--or--later-blue.svg" alt="GPL 3.0 oder neuer">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599c.svg" alt="Sprache">
  <img src="https://img.shields.io/badge/Zones-wired%20%2B%20Zigbee-ffb020.svg" alt="Zones">
  <img src="https://img.shields.io/badge/Checks-482%2C535-2ea44f.svg" alt="Checks">
  <img src="https://img.shields.io/badge/Tests-488930%20checks-2ea44f.svg" alt="Tests">
  <img src="https://img.shields.io/badge/Version-0.5.0-0f6cbd.svg" alt="Version">
  <img src="https://img.shields.io/badge/Maturity-functional-00E5FF.svg" alt="Reifegrad">
</p>

---

**Ehrlichkeitsprüfung - was heute läuft:** Es existiert der **Alarmkern** (was eine Zone ist, wie eine vom ADC gelesene verdrahtete Schleife zum Zustand wird, die Regeln der Zentrale: Scharfschalten, Verzögerungen, der Alarm, die Sirene, die PIN mit Sperre) und die darauf gebaute **Firmware des Knotens** (Einstellungen, Web-Panel, Nachrichten mit dem Server, die Brücke des Zigbee-Funkmoduls, ein Zigbee-Koordinator im Netzwerk, Relais, Bluetooth, signierte Updates), die für beide Platinen baut; der Kern, die Einstellungen, die Nachrichten und das Panel in einem echten Browser gegen einen Ersatzknoten werden auf einem Computer mit 488.930 Prüfungen in 18 Testprogrammen getestet. **Nichts lief auf einer Platine oder einer echten Anlage, keine Zone wurde verdrahtet und keine Sirene angeschlossen, die Zigbee-Kommunikation des Knotens mit einem Koordinator (lokal oder im Netzwerk) ist geschrieben und gegen einen Ersatzkoordinator getestet, lief aber nie gegen einen echten Koordinator oder Sensor, und dies ist keine zertifizierte Alarmanlage.**

---

## 📋 Auf einen Blick

| | |
|---|---|
| Version | `0.5.0` |
| Reifegrad | Funktionsfähig |
| Rolle | Alarmzentralen-Feldknoten für ESP32-S3 (Familie: Field Nodes, übergeordnet: ARMOR-COMMON) |
| Technik | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| Tests | 488930 Prüfungen in 18 Host-Programmen und 5 Python-Prüfungen (`ctest`) |
| Eingänge | Bis zu 8 verdrahtete Schleifen, Zigbee-Sensoren (IAS Zone) und Sirene (IAS WD), 4 Tasten |
| Läuft auf | Eine ESP32-S3-Platine (Ethernet oder WLAN) mit Basisplatine; Host-Tests auf jedem Computer |
| Auf echter Hardware | Noch keiner: keine Platine, Zone, Sirene oder kein Zigbee-Koordinator angeschlossen (nur Computertests, der Build und das Panel im Browser) |
| Lizenz | GPL-3.0-or-later |

**Springe zu:** [Überblick](#-überblick) · [Architektur](#-architektur) · [Sicherheit](#-sicherheit-und-schutz) · [Schnittstellen](#-schnittstellen) · [Konfiguration](#-konfiguration) · [Schnellstart](#-schnellstart) · [Tests](#-tests) · [Probleme](#-fehlerbehebung) · [Struktur](#-struktur-des-repositorys) · [Verwandte](#-verwandte-projekte)

## 🚦 Projektstatus

Jede Zeile ist ein Teil dieses Repositorys, und die Ampel sagt, wie weit er heute ist. Sie beruht auf dem, was das Repository zeigen kann (Tests, Builds, Läufe), nicht auf dem, was geplant ist.

🟢 Fertig und überprüft · 🟡 Funktioniert, mit bekannten Grenzen · 🟠 Teilweise fertig oder noch nicht überprüft · 🔴 Nicht gemacht oder funktioniert nicht

| Bereich | Stand | Was das hier bedeutet |
|---|---|---|
| Software | 🟢 | Die Firmware wird im ESP-IDF-5.5.5-Container für beide Platinen (WLAN und Ethernet) gebaut. |
| Tests | 🟢 | 488.930 Prüfungen in 18 Host-Programmen bestehen (`ctest`): die Zonen, die vom ADC gelesenen verdrahteten Schleifen, die Regeln für Scharfschalten, Verzögerungen, Sirene und PIN, die Einstellungen und die Nachrichten. |
| Protokolle und Daten | 🟠 | Die Zigbee-Kommunikation des Knotens selbst mit einem Koordinator, lokal oder im Netz, ist nur gegen einen nachgebildeten Koordinator getestet; sie ist nie auf einen echten Koordinator oder Sensor getroffen. |
| Oberfläche | 🟡 | Das Web-Panel wurde in einem echten Browser gegen einen nachgebildeten Knoten erprobt, nicht auf einer Platine. |
| Hardware | 🔴 | Keine Platine gebaut, keine Zone verdrahtet und weder Sirene noch Zigbee-Koordinator angeschlossen: Bisher lief nichts an einer echten Anlage. |
| Sicherheit | 🟡 | Die PIN mit Sperre, der Einrichtungscode, die Anmeldebegrenzung und die signierten Aktualisierungen sind auf dem Host getestet; es gab keine unabhängige Prüfung, und dies ist kein zertifiziertes Alarmsystem. |
| Aktualisierungen | 🟠 | Die Hash- und Ed25519-Signaturprüfungen und die Rückfalllogik bestehen auf dem Host; eine Aktualisierung über die Luft wurde nie auf einer Platine ausgeführt. |
| Schreiben auf Geräte | 🟠 | Die Sirene und die Relais werden von den Alarmregeln gesteuert und gegen Nachbildungen getestet; keines wurde an eine echte Sirene oder ein echtes Relais angeschlossen. |
| Integration | 🟠 | Die Nachrichten des Knotens werden gegen die gemeinsamen Schemas und Konformitätsvektoren von ARMOR-COMMON geprüft. Sein MQTT-Client wird in den Tests gegen einen echten Mosquitto gespielt; noch keine Platine hat sie an einen Server geschickt. |
| Dokumentation | 🟢 | Acht Sprachen, die Firmware-Anleitung, die Verdrahtungshinweise und das Änderungsprotokoll sind aktuell. |

## 🎯 Überblick

* **Zonen:** vier Arten - *sofort* (eine Tür, ein Fenster), *Eingang* (die Tür, durch die man hereinkommt: sie startet die Eingangsverzögerung), *Innenraum* (Bewegung: nur bei „abwesend“) und *immer* (Rauch, Gas, Panik: scharf oder nicht). Eine Zone kann eine verdrahtete Schleife oder ein Zigbee-Sensor sein.
* **Die verdrahtete Schleife:** ein Kontakt mit Abschlusswiderstand, als Spannung gelesen, ergibt vier Zustände an einem Pin - normal, ausgelöst und Sabotage bei kurzgeschlossenem oder durchtrenntem Draht. Die Grenzwerte sind Einstellungen; ein Entpreller verhindert, dass ein prellender Kontakt den Alarm auslöst.
* **Die Zentrale:** Scharfschalten *abwesend* und *anwesend*, eine Ausgangsverzögerung, die den Gehenden nicht als Eindringling behandelt, eine Eingangsverzögerung, ein Alarm, der **bis zum richtigen Unscharfschalten bleibt**, eine Sirene, die nach einer Zeit von selbst aufhört, das Überbrücken offener Zonen bei Erzwingen (nie ein ertönender Rauchmelder) und ein Protokoll der letzten hundert Ereignisse.
* **Die PIN:** zum Scharfschalten ist keine nötig, zum Unscharfschalten und Stummschalten schon; falsche PINs werden gezählt, und nach einigen wird alles eine Weile abgelehnt, auch die richtige PIN.
* **Stummschalten ohne Unscharfschalten:** solange die Sirene ertönt, stoppt eine richtige PIN sie sofort bei weiterhin scharfem System - der Alarm bleibt verriegelt und gespeichert, eine Zone, die noch nicht ausgelöst hat, kann sie erneut ertönen lassen, und eine falsche PIN zählt für die Sperre wie eine falsche PIN beim Unscharfschalten.
* **Der Plan:** ein Gateway auf der Platine des Elektroknotens (Zigbee-Funk, verdrahtete Zonen, Sirenenrelais), das mit ARMOR-SERVER arbeitet und **weiter überwacht, wenn der Server fehlt**: eine Zentrale übernimmt das Funkmodul vom Server über die Zigbee-Brücke von ARMOR-NODE-ELECTRICAL. Siehe [den Entwurf](docs/DESIGN.md) und [die Übernahme](docs/FAILOVER.md).
* **Der Knoten:** Firmware für den ESP32-S3 (PoE-Ethernet oder WLAN) mit bis zu acht verdrahteten Zonen, der Sirene an einem Relais, dem Webpanel aller Knoten in acht Sprachen, dem an Zigbee2MQTT verliehenen Zigbee-Funk und den Nachrichten `armor/alarm/<Knoten>/state|command|result`; der Server darf nur scharf- und unscharfschalten, wenn drei unabhängige Schalter es erlauben. Siehe [die Firmware](docs/NODE_FIRMWARE.md), [die Nachrichten](docs/ALARM_MESSAGES.md) und [die Hardware](docs/HARDWARE.md).
* **Funkmodul übernehmen:** ist Zigbee2MQTT des Servers eine festgelegte Zeit weg, startet der Knoten den Koordinator selbst (ZNP), liest die Sensoren der Zonen, die einen nennen (IAS Zone), und steuert Zigbee-Sirenen (IAS WD); er gibt es frei, sobald der Client zurück ist.
* **Noch nicht:** ein echter Koordinator oder Sensor, die Hardware und jeder Test an einer echten Anlage.
* **Ein Neustart schaltet das Haus nicht unscharf:** Modus, ein laufender Alarm und die ausgeschlossenen Zonen werden im Flash gehalten und beim Start zurückgeholt; eine unterbrochene oder kurzgeschlossene Schleife wird nie ausgeschlossen; Nachrichten laufen über eine Warteschlange (kein Deadlock mit dem Broker), und der Watchdog startet die Platine neu, wenn die Aufgabe, die die Zonen liest, stehen bleibt.
* **Signierte Over-the-Air-Updates:** der Knoten installiert von GitHub nur ein Abbild, dessen SHA-256 stimmt und das eine gültige Ed25519-Signatur des Betreuers trägt (der öffentliche Schlüssel steckt in der Firmware, der private verlässt den Betreuer nie); eine von einem Administrator im Panel von Hand hochgeladene Datei wird weiterhin akzeptiert. Seiten und Meldungen des Panels sind in acht Sprachen.
* **Bluetooth-Kopplung mit PIN:** die sechsstellige PIN wird aus dem Einrichtungscode abgeleitet, auf der USB-Konsole ausgegeben und einem Administrator im Panel angezeigt; das Telefon muss sie eingeben, bevor es die Einstellungen des Knotens schreiben darf.
* **Sirene und Neustarts:** Das Sirenenrelais gehört allein der Alarmanlage (weder Broker noch Zentrale noch Panel können es bewegen, und es öffnet sich beim Verlust des Brokers nicht); eine Zone, die noch nicht gelöst hatte, lässt die Sirene nach ihrer Zeit erneut ertönen; nach einem Neustart kehren ein Rauch- oder Panikalarm bei unscharfem System, ein stummgeschalteter Alarm und eine Sperre zurück; ein Zurücksetzen von Hand wird bei scharfer Anlage abgelehnt; ein Zigbee-Sensor, der nie spricht, gilt als verloren.
* **Nur das Haus darf das Panel nutzen:** Das Panel antwortet standardmäßig dem privaten Netz (oder den Netzen, die der Besitzer nennt, etwa einem VPN), lehnt eine Liste ab, die den Speichernden aussperren würde, zählt falsche Passwörter je Person und je Adresse und funktioniert hinter dem Proxy des Servers, sodass man es von außerhalb des Hauses erreicht, ohne den Knoten zu veröffentlichen.
* **Eine Zone, die sich nicht lesen lässt, gilt nicht als geschlossen:** startet der Wandler nicht oder liefert eine Schleife fünf Runden lang keinen Wert, steht die Zone auf *Störung* (trouble) - sie überwacht nicht, löst nie die Sirene aus, verhindert das Scharfschalten (außer erzwungen; dann wird sie ausgelassen und vermerkt) und der Server warnt.
* **Optionale verschlüsselte Verbindung zum Broker (TLS):** standardmäßig aus, der Knoten spricht also wie bisher unverschlüsseltes MQTT. Ein Schalter und ein Feld für das Zertifikat der eigenen Zertifizierungsstelle der Anlage (PEM, höchstens 2 KB) auf der Broker-Seite sorgen dafür, dass er sich mit `mqtts://` verbindet, nur dieser Stelle vertraut und prüft, ob das Zertifikat des Brokers für die erreichte Adresse ausgestellt wurde; alles, was nicht genau ein PEM-Zertifikat ist, wird abgelehnt. Durch Host-Tests abgedeckt; der Handshake wurde auf einer Platine noch nicht ausgeführt.
* **Scharfschaltprofile:** Ein Scharfschaltbefehl kann ein Profil tragen (Abwesend, Zuhause, Nacht, eigenes), das für dieses Scharfschalten festlegt, welche Zonen überwachen, welche sofort oder nach der Eingangsverzögerung alarmieren, die Ausgangs- und Eingangsverzögerung und ob die Sirene ertönt (ein stiller Alarm bleibt verriegelt, protokolliert und gemeldet). Es berührt nie Rauch-, Panik- oder Sabotagezonen, die PIN, die Sperre, das Stummschalten, ein erzwungenes Scharfschalten oder eine Zone mit Störung; das Profil übersteht einen Neustart, und ein Scharfschalten ohne Profil verhält sich genau wie zuvor. Getestet gegen die alten Regeln mit einem Zufallslauf und gegen den gemeinsamen Vertrag.
* **Zustandsbericht:** etwa einmal pro Minute veröffentlicht der Knoten, wie lange er läuft und warum er zuletzt neu gestartet ist, seinen freien Speicher, seine Verbindung und sein WLAN-Signal, wie oft er sich mit dem Broker verbunden hat und den letzten protokollierten Fehler (`armor/health/<Knoten>`, der Vertrag `node_health` von ARMOR-COMMON). Der Code, der ihn erzeugt, wird von allen Knoten geteilt und auf einem Computer getestet; auf einer Platine ist er noch nicht gelaufen.
* **Steuerung vom Server aus:** Studio kann den Knoten sauber neu starten, bis zu einer Stunde pausieren (er läuft von selbst weiter), seine Arbeit in den Standby setzen (ohne Ende, über Neustarts hinweg gespeichert, der Knoten bleibt verbunden und antwortet weiter), ihn sich zeigen lassen, einen Diagnose-Schnappschuss anfordern und ihn als in Wartung markieren (`armor/control/<Knoten>/...`, der Vertrag `node_control` von ARMOR-COMMON). Jeder Befehl besteht aus zwei Schritten mit einem Einmal-Token, das zu dieser Operation gehört; nur der Server darf sie am Broker schreiben, und es gibt kein Zurücksetzen auf Werkseinstellungen, kein Löschen und keinen Tiefschlaf. Solange der Knoten pausiert oder im Standby ist, werden seine Zonen nicht gelesen, nichts wird ausgewertet, scharf geschaltet oder ausgelöst, kein Zustand wird veröffentlicht und das Sirenenrelais bleibt offen: Das Haus ist NICHT geschützt, und der Gesundheitsbericht sagt es. Der Code wird von allen Knoten geteilt und auf einem Rechner getestet; auf einer Platine ist er noch nicht gelaufen.
* **Diagnose ohne Kabel:** Nach einem unsauberen Start (Absturz, Watchdog, Spannungseinbruch, fehlgeschlagene Speicherreservierung) nennt der Gesundheitsbericht zehn Minuten lang den Grund; ein Absturz hinterlässt sein Core-Dump, das man im Panel ansehen, herunterladen oder löschen kann; die gespeicherten Einstellungen werden vor jeder Firmware-Installation kopiert und wiederhergestellt, falls ein Update sie beschädigt; und ein Administrator kann den Knoten über den Server mit einem Einmal-Token nach einem Diagnosepaket oder seinem Protokoll fragen.
* **Die Sicherheitsoptionen des Servers:** Die Zentrale wendet die Optionen an, die der Server für sie führt (`armor/alarm/<Knoten>/policy`), und jede lässt sich am Knoten selbst festlegen: Der gespeicherte scharfe Zustand ist an Zonen und PIN gebunden, die Zigbee-Zonenmeldungen des Servers lassen sich abschalten und werden, wenn sie signiert sind, geprüft (Signatur mit einem im Panel eingegebenen Schlüssel und eine nur steigende Folgenummer). Auf einem Computer gegen die Werte getestet, die Vertrag und Server berechnen; nicht auf einem Board ausgeführt.
* **Optionales Versiegeln gespeicherter Geheimnisse:** standardmäßig aus (`security.seal_secrets` oder `nodes.nvs_encryption` des Servers). Eingeschaltet werden die Passwörter, Token, Schlüssel, PINs und Passwort-Hashes, die der Knoten speichert, mit ChaCha20-Poly1305 versiegelt, alles andere bleibt lesbar; Ein- und Ausschalten schreibt sie ohne Verlust um, auch wenn der Knoten mittendrin stehen bleibt. Es wird kein eFuse gebrannt und der Flash nicht verschlüsselt: Der Schlüssel liegt in einer Partition desselben Chips und schützt daher **nicht** vor jemandem, der die Platine hat; ein Knoten, der vor dieser Partition installiert wurde, behält seine Geheimnisse unverändert, bis er mit der neuen Partitionstabelle geflasht wird.

## 🧩 Rolle in A.R.M.O.R.

ARMOR-NODE-ALARM ist die Alarmzentrale: Zonen, Scharfschalten, Verzögerungen, Sirene und PIN werden auf dem Knoten selbst entschieden, sodass das Haus bewacht bleibt, wenn der Server fehlt. [ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) zeigt sie, ordnet Zigbee-Sensoren ihren Zonen zu und schaltet sie, wenn Sie es erlauben, scharf und unscharf; das Menü *Alarmzentralen* und der *Alarm-Designer* von [ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) sowie die App [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) steuern sie. Ihr Zigbee-Funkmodul kann auf der Platine sein oder ein Koordinator im Netzwerk.

| Name | Wie | Wozu |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | Empfängt den Zustand der Zentrale; ordnet Scharf-, Unscharfschalten und Stummschalten nur hinter drei Schaltern an |
| ARMOR-STUDIO | Menü Alarmzentralen | Zeigt die Zonen, die Ereignisse und die Richtlinie und öffnet das Panel des Knotens |
| ARMOR-ANDROID-CONTROL | Bluetooth | Richtet den Knoten vor Ort ein und zeigt die Alarmzentralen |
| ARMOR-COMMON | Alarm-Vertrag | Die Nachrichten werden gegen seine Schemas geprüft; die gemeinsame Firmware stammt daraus |

## 🏛️ Architektur

<p align="center">
  <img src="images/architecture.svg" alt="Architektur von ARMOR-NODE-ALARM: wer es aufruft, was es enthält und wovon es abhängt" width="100%">
</p>

## 🖼️ Screenshots

Aufgenommen mit 1920×1080 auf einem Prüfstand, mit Beispieldaten und ohne eine echte Anlage dahinter.

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="Übersicht: der Zustand des Knotens auf einen Blick - Netzwerk, Broker, Firmware und was er meldet." width="100%"><br><sub>Übersicht: der Zustand des Knotens auf einen Blick - Netzwerk, Broker, Firmware und was er meldet.</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="Zonen: jede verdrahtete oder Zigbee-Zone der Zentrale, ihre Art, Haltezeit und der Live-Zustand." width="100%"><br><sub>Zonen: jede verdrahtete oder Zigbee-Zone der Zentrale, ihre Art, Haltezeit und der Live-Zustand.</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Zigbee-Funk: die serielle Brücke zum Koordinatormodul, welcher Client sie nutzen darf, und der Zustand der Brücke." width="100%"><br><sub>Zigbee-Funk: die serielle Brücke zum Koordinatormodul, welcher Client sie nutzen darf, und der Zustand der Brücke.</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="Relais und Sirene: die Ausgänge der Basisplatine und was jeden ansteuert." width="100%"><br><sub>Relais und Sirene: die Ausgänge der Basisplatine und was jeden ansteuert.</sub></td></tr>
</table>

## 🔒 Sicherheit und Schutz

| Bedrohung | Abwehr |
|---|---|
| Passwörter im Klartext oder einkompiliert | PBKDF2-HMAC-SHA256 mit 10 000 Runden; kein Passwort in der Firmware (ein Knoten ohne Benutzer geht in die Einrichtung); höchstens 4 Benutzer (Admin oder Betrachter) und 8 bis 64 Zeichen; höchstens 6 Sitzungen, 30 Minuten Leerlauf |
| Einen frisch geflashten Knoten übernehmen | Die Einrichtung braucht einen Startcode: den des Builds (`CONFIG_ARMOR_SETUP_CODE`) oder einen zufälligen im Flash, der auf der USB-Konsole erscheint; mit Flottengeheimnis ist er `HMAC-SHA256(Geheimnis, MAC)`; er ist auch der Schlüssel des Einrichtungs-WLANs (WPA2, nie überbrückt) |
| Das Panel aus dem Internet erreicht | Es antwortet standardmäßig den privaten Netzen eines Hauses (10/8, 172.16/12, 192.168/16, 169.254/16, 127/8 und 100.64/10, wo manche VPNs liegen); die Einstellung `web.admin_sources` listet weitere IPv4-Adressen und Netze (8 Einträge, 120 Zeichen); alles außerhalb wird abgewiesen außer der öffentlichen Identität `GET /api/v1/session`; eine Liste, die den Speichernden aussperren würde, wird abgelehnt |
| Das Panel im Klartext über HTTP | HTTPS auf 443 mit einem vom Knoten selbst erzeugten Zertifikat (selbstsigniert, im Flash, SHA-256-Fingerabdruck zum Vergleichen sichtbar); das Panel kann http, https oder beides sein; das Sitzungscookie ist immer HttpOnly und SameSite=Strict (und Secure über TLS) |
| Eine gefälschte Firmware von GitHub | Ein Image wird nur mit passendem SHA-256 und gültiger Ed25519-Signatur mit dem Schlüssel des Maintainers installiert (`no_checksum`, `no_signature`, `bad_signature`, `checksum_mismatch`, `wrong_firmware`, `not_newer`, `version_mismatch` lehnen es ab); ein manueller Upload durch einen Administrator wird nicht geprüft: eine ausdrückliche Grenze |
| Kapern der Bluetooth-Einrichtung | Eine 6-stellige PIN, abgeleitet vom Einrichtungscode, auf der USB-Konsole und für einen Administrator sichtbar; das Telefon muss sie vor dem Schreiben von Einstellungen eingeben; das Einrichtungsfenster dauert `CONFIG_ARMOR_BLE_SETUP_MINUTES` (standardmäßig 10, 1 bis 120) |
| Gespeicherte Geheimnisse, gelesen von jemandem mit der Platine | Optional (standardmäßig aus, Schalter `security.seal_secrets` oder `nodes.nvs_encryption` des Servers): ChaCha20-Poly1305 für Passwörter, Token, Schlüssel, PINs und Hashes. Es verschlüsselt nicht den Flash und brennt kein eFuse: Der Schlüssel liegt in einer Partition desselben Chips und schützt daher nicht vor dem, der die Platine hat |
| Die Broker-Verbindung im Klartext | Optionales TLS (`mqtts://`) mit der eigenen Zertifizierungsstelle des Besitzers (PEM bis 2 KB) und Namensprüfung; standardmäßig aus. Der Handshake lief nur in Host-Tests, nicht auf einer Platine |
| Erraten der Panel-PIN | Der Vergleich verrät nicht, wo er abweicht; aufeinanderfolgende Fehler sperren das Panel für `lockout_s` (standardmäßig 60 s, 1 bis 3600) mal 1, 5 und 15 bei den ersten drei Sperren, auch für die richtige PIN; eine laufende Sperre wird nach einem Neustart wiederhergestellt; Scharfschalten braucht keine PIN, Unscharfschalten und Stummschalten schon |
| Ein Neustart, der das Haus unscharf macht | Der Modus, der laufende Alarm und die ausgeschlossenen Zonen werden im Flash gehalten und wiederhergestellt; eine unterbrochene oder kurzgeschlossene Schleife wird nie ausgelassen; Rauch, Panik und Sabotage, die im unscharfen Zustand ausgelöst haben, kommen nach einem Neustart zurück; der BOOT-Reset löscht nur bei UNSCHARFEM Panel |
| Eine nicht lesbare Zone sieht geschlossen aus | Sie zeigt `trouble` (der Wandler startet nicht oder 5 Runden ohne Messwert), und das Panel schaltet mit ihr nur erzwungen scharf, dann wird sie ausgeschlossen und vermerkt; ein klingelnder Rauch- oder Panikmelder wird nie ausgeschlossen |
| Ein Befehl vom Server, den niemand erlaubt hat | Scharf-, Unscharfschalten und Stummschalten vom Server brauchen DREI Schalter (die Knoteneinstellung, die Zugriffsliste des Brokers und `ARMOR_ALARM_COMMANDS=1` am Server), der Server fragt die PIN des Benutzers vor dem Senden eines Unscharf- oder Stummbefehls, und alles ist standardmäßig aus; das Sirenenrelais gehört allein dem Alarm (Broker, Zentrale und Panel können es nicht bewegen) |
| Gefälschte Zigbee-Nachrichten | Das Topic `armor/alarm/<id>/zigbee` trägt einen HMAC-SHA256 über Knoten, Zone, Zustand, Batterie und Folgenummer mit dem Schlüssel der Zentrale (64 Hex, im Panel eingetippt), und die Folgenummer steigt nur; das Richtlinien-Topic trägt `config_hash`, `zone_feed` und seine Signatur, die jeweils am Knoten festgelegt werden können |

Dies ist kein zertifiziertes Alarmsystem ([docs/SAFETY.md](docs/SAFETY.md)): Es ist ein Heimprojekt und lief nie auf einer Platine oder in einer echten Installation. Fehlt der Server, entscheidet die Zentrale allein, und fällt zusätzlich der Knoten oder sein Funkmodul aus, überwacht niemand. Lies [SECURITY.md](SECURITY.md), um eine Schwachstelle zu melden.

## 🔌 Schnittstellen

| Teil | Was sie festlegt |
|---|---|
| MQTT | `armor/alarm/<id>/state` (nicht retained: Zonen mit ID, Name, Art, Zustand und Überbrückung, die Phase), `/command`, `/result`, `/zigbee` und `/policy` (Server zur Zentrale); `armor/device/<id>/<Relais>/{state,set}`, `armor/health/<id>`, `armor/control/<id>/...` |
| Zonen | Art `instant`, `entry`, `interior` oder `always`; Zustand `normal`, `triggered`, `tamper` oder `trouble`; bis zu 8 verdrahtete Schleifen (ein Kontakt mit Abschlusswiderstand, vom ADC gelesen, ergibt vier Zustände an einem Pin; die Ethernet-Platine hat 6 gültige Analogpins, die WLAN-Platine 8), 4 Tasten und ein Protokoll von 100 Ereignissen |
| Zigbee | Eine serielle Brücke (ZNP) zu einem Koordinator, IAS-Zone-Sensoren und IAS-WD-Sirenen; der Knoten übernimmt das Funkmodul, wenn der Zigbee2MQTT des Servers `zigbee_silence_min` lang schweigt, und gibt es zurück, wenn der Server wiederkommt ([docs/ZIGBEE.md](docs/ZIGBEE.md), [docs/FAILOVER.md](docs/FAILOVER.md)) |

## ⚙️ Konfiguration

| Variable | Standard | Was sie festlegt |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | Die Ausgangs- und Eingangsverzögerung; beim Gehen gilt die Person nicht als Eindringling |
| `panel.lockout_s` | 60 (1-3600) | Die PIN-Sperre, mal 1, 5 und 15 bei den ersten drei |
| `zigbee_silence_min` | - | Wie lange das Zigbee des Servers schweigen darf, bevor der Knoten das Funkmodul übernimmt |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | Die Einstellungen, die alle Knoten teilen: der Name (leer ergibt `armor-` und die letzten Bytes der MAC), der Einrichtungscode (leer: zufällig), das Flottengeheimnis (16+ Zeichen, nie im Repository), die ersten Broker-Einstellungen, der Herzschlag (2 bis 300 s), die Minuten der Bluetooth-Einrichtung (1 bis 120) und der Reset mit BOOT (8 s in den ersten 30 s) |
| `system.login_limit_per_minute` | 30 (0-120) | Falsche Passwörter pro Minute, bevor das Panel die Anmeldung für alle sperrt (0 = kein globales Limit); in Studio (eine Schaltfläche an jedem Knoten) oder mit `POST /api/v1/admin/nodes/settings` lesen und ändern, wobei die Panel-Anmeldung einmal benutzt und nie gespeichert wird |

Erneutes Auslösen nach einem Stummschalten ist auf 3 begrenzt (`kMaxResounds`). Das Sirenenrelais startet bei jedem Neustart offen und fällt nicht auf offen, wenn der Broker verloren geht.

## 🚀 Schnellstart

**Voraussetzungen:** ein C++17-Compiler für die Host-Tests; der ESP-IDF-5.5.5-Container zum Bauen des Images; eine ESP32-S3-Platine (Ethernet oder WLAN) mit der Basisplatine.

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

Siehe [den Entwurf](docs/DESIGN.md), [wie die Zentrale vom Server übernimmt](docs/FAILOVER.md) und die [Sicherheitshinweise](docs/SAFETY.md).

## 🧪 Tests

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

18 Host-Testprogramme und 5 Python-Prüfungen laufen unter `ctest`; 488930 Prüfungen, gezählt von den Host-Programmen, mit `-Werror` gebaut.

Python-Prüfungen: die Panel-Texte in acht Sprachen, die Tasks, die Speicherpfade, die Bluetooth-Sitzung unter ihrer Sperre und die vom Panel aufgerufenen Routen (und in manchen Knoten die HTTP-Codes oder die Glyphen der Schriften)

Die Host-Tests brauchen einen beliebigen C++17-Compiler (Linux, WSL, MSYS2); das Web-Panel wird außerdem in einem echten Browser gegen einen Ersatzknoten geprüft (`tools/panel_mock.mjs`, `tools/panel_browser_test.mjs`).

Die meisten Prüfungen sind erschöpfende Durchläufe der Panel-Regeln; Schleifen, Sirene und Zigbee-Verbindung sind Ersatz.

## 🏗️ Build und Veröffentlichung

`tools/build_node.sh` baut das Image im ESP-IDF-5.5.5-Container, und die Version trägt das Image, sein SHA-256 und seine Ed25519-Signatur, je eines pro Platine; das Image wird beim ersten Mal per USB-C geschrieben und danach vom Panel aktualisiert.

## 🩺 Fehlerbehebung

| Sie sehen | Weil | Tun Sie |
|---|---|---|
| Du hast das Passwort verloren oder der Knoten tritt dem Netz nicht mehr bei | Ein Knoten in der Einrichtung ohne Adresse nach 90 s öffnet ein eigenes WLAN `ARMOR-SETUP-xxxxxx` (öffne `http://192.168.4.1/`) | Zum Neustart BOOT 8 Sekunden in den ersten 30 Sekunden nach dem Einschalten halten (löscht Einstellungen und Benutzer) oder im Panel *Werksreset* nutzen |
| Ein Update von GitHub wird abgelehnt | Die Version enthält keine Prüfsumme oder Signatur, sie passen nicht (`e_no_checksum`, `e_no_signature`, `e_checksum_mismatch`, `e_bad_signature`), hat kein Image für diese Platine (`e_no_asset_for_board`) oder gehört zu einem anderen Projekt (`e_wrong_firmware`) | Der Knoten blieb unverändert; erneut versuchen oder einen Administrator die Datei von Hand hochladen lassen |
| Das Panel schaltet nicht scharf | Eine Zone steht auf `trouble` (der Wandler startet nicht oder 5 Runden ohne Messwert) | Behebe die Verdrahtung oder erzwinge das Scharfschalten (die Zone wird dann ausgeschlossen und vermerkt) |
| Die PIN wird abgelehnt, auch wenn sie stimmt | Das Panel ist nach Fehlversuchen gesperrt (auch die richtige PIN wird abgelehnt) | Warte `lockout_s`; die Sperre überlebt einen Neustart |
| Der BOOT-Reset bewirkt nichts | Das Panel ist scharf: Der Reset löscht nur ein unscharfes Panel | Zuerst mit der PIN unscharf schalten |

## 📂 Struktur des Repositorys

<p align="center">
  <img src="images/repository-structure.svg" alt="Repository-Struktur von ARMOR-NODE-ALARM" width="100%">
</p>

<details>
<summary>Textversion des Baums</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                die Firmware: eine ESP-IDF-Komponente
├── core/                                die Logik ohne ESP-IDF, am Rechner getestet
├── panel/                               das in die Firmware eingebettete Web-Panel
├── tests/                               Host-Tests und Python-Prüfungen
├── docs/                                11 Anleitungen: Sicherheit, Nachrichten, Zigbee, Ausfallübernahme
├── tools/                               Werkzeuge für Build, Packen und Prüfung
├── .github/                             CI-Workflow sowie Vorlagen für Issues und Pull Requests
├── images/                              Banner, Symbol und die zwei Bilder dieses READMEs
├── build.sh build.bat                   Build über den gemeinsamen Projekt-Workflow
├── test.sh test.bat                     Tests über den gemeinsamen Workflow ausführen
├── run.sh run.bat                       Start über den gemeinsamen Projekt-Workflow
├── armor.project.json                   Manifest: Version, Rolle, Reifegrad, Build-Befehl
├── CHANGELOG.md                         was sich in jeder Version geändert hat
├── SECURITY.md SUPPORT.md LICENSE       Sicherheitsrichtlinie, Support und Lizenz
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   Mitwirkung und Verhaltenskodex
├── CMakeLists.txt                       das ESP-IDF-Projekt
├── partitions.csv                       die Flash-Tabelle
├── sdkconfig.defaults                   die Firmware-Einstellungen
├── sdkconfig.board.s3-eth               die Einstellungen einer Platine
├── sdkconfig.board.s3-wifi              die Einstellungen einer Platine
├── secrets/                             Beispiel der Knotendatei (die echten sind nicht versioniert)
├── build-test.sh build-test.bat         Tests über den gemeinsamen Workflow ausführen
└── .gitattributes .gitignore            Git-Einstellungen
```

</details>

## 🗺️ Roadmap und bekannte Grenzen

**Bekannte Grenzen heute**

* Die Zigbee-Kommunikation des Knotens selbst mit einem Koordinator, lokal oder im Netz, ist nur gegen einen nachgebildeten Koordinator getestet; sie ist nie auf einen echten Koordinator oder Sensor getroffen.
* Das Web-Panel wurde in einem echten Browser gegen einen nachgebildeten Knoten erprobt, nicht auf einer Platine.
* Keine Platine gebaut, keine Zone verdrahtet und weder Sirene noch Zigbee-Koordinator angeschlossen: Bisher lief nichts an einer echten Anlage.
* Die PIN mit Sperre, der Einrichtungscode, die Anmeldebegrenzung und die signierten Aktualisierungen sind auf dem Host getestet; es gab keine unabhängige Prüfung, und dies ist kein zertifiziertes Alarmsystem.
* Die Hash- und Ed25519-Signaturprüfungen und die Rückfalllogik bestehen auf dem Host; eine Aktualisierung über die Luft wurde nie auf einer Platine ausgeführt.
* Die Sirene und die Relais werden von den Alarmregeln gesteuert und gegen Nachbildungen getestet; keines wurde an eine echte Sirene oder ein echtes Relais angeschlossen.

**Als Nächstes**

* Echte Schleifen und eine Sirene verdrahten, die Zigbee-Verbindung mit einem echten Koordinator und Sensoren erproben und das Panel in einer echten Installation betreiben, bevor mehr behauptet wird.

## 🌍 Sprachen

README und Texte gibt es auf Englisch, Spanisch, Französisch, Italienisch, Deutsch, Chinesisch, Japanisch und europäischem Portugiesisch.

## 🔗 Verwandte Projekte

**A.R.M.O.R.** (Autonomous Radar & Multimodal Observation Range) ist ein Perimeter-Sicherheitssystem aus unabhängigen Repositorys. Jedes hat eine eigene Version, eigene Tests und ein eigenes README; hier ist die Familie:

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - Nachrichtenverträge, Validierer, Konformitätsvektoren und generierte Typen
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - Feldknoten-Firmware für ESP32-S3 mit drei Radaren und eigenem Web-Panel
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - Protokolle für Solar-Wechselrichter und -Batterien und die Nachrichten eines Gateway-Knotens
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - Elektroknoten: Zähler, die Nachricht der Netzmesswerte und die Regeln fürs Schalten
* **ARMOR-NODE-ALARM** (dieses Repository) - Alarmknoten und Alarmzentrale: Zonen, Scharfschalten, Verzögerungen, Sirene und PIN, mit dem Server oder ohne ihn
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - Touch-Panel: der Systemzustand auf einem Wandbildschirm, Scharf- und Quittieren sowie das Zuhause des Sprachassistenten
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - Das lokale Netzwerk: seine Geräte, das Internet und was sich ändert
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - Zentraler Koordinator: Telemetrie, Alarme, Geräte, Solarmesswerte und Kameras
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - Web-Konsole: Kameras, Radar, Alarme, Solarenergie und 2D/3D-Standortdesigner
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - Android-Bedienclient mit Live-Radar in 2D/3D
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - iPhone-Bedienclient
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - Visuelle Inferenzrichtlinie, die ihre Entscheidungen erklärt und nie handelt
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - Kamerazonen, Masken und Erkennungsregeln für den Jetson Orin NX
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - Offline-Sprachabsichten mit einer nicht fälschbaren Bestätigung
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - Gehäuse, Elektronik und die Abnahmematrix am Prüfstand
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - Bereitstellung, CM5-Prüfstand, Backup und TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - Offline-Telemetriesimulator mit wiederholbaren Fehlern
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - Erkennt, installiert und aktualisiert die eigenen Repositorys des Ökosystems
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - Architektur, Sicherheitsgrundlage und die Fähigkeitsmatrix

## 📚 Dokumentation

* [Sicherheit](docs/SAFETY.md): was das Panel verspricht und was nicht
* [Alarmnachrichten](docs/ALARM_MESSAGES.md), [Zigbee](docs/ZIGBEE.md) und [Ausfallübernahme](docs/FAILOVER.md)
* [Entwurf](docs/DESIGN.md), [Hardware](docs/HARDWARE.md), [Basisplatine](docs/BASE_BOARD.md) und [Knoten-Firmware](docs/NODE_FIRMWARE.md)
* [Prüfstandtests](docs/BENCH_TESTS.md), [Einrichtungscode](docs/SETUP_CODE.md) und [Bluetooth-Einrichtung](docs/BLE_PROVISIONING.md)
* [Fähigkeitsmatrix: was belegt ist und was nicht](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [Projektkatalog: Versionen und wie die Repositorys voneinander abhängen](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [Änderungsverlauf dieses Repositorys](CHANGELOG.md)

## 🤝 Mitwirken und Community

* Führe vor einem Pull Request `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py` aus: Es muss bestehen. Siehe [CONTRIBUTING.md](CONTRIBUTING.md) und den [Verhaltenskodex](CODE_OF_CONDUCT.md).
* Das README gibt es in acht Sprachen, und es wird aus den Quellen in ARMOR-DOCS erzeugt: Eine Textänderung erfolgt dort.
* Eine Änderung an einer Nachricht zwischen Komponenten beginnt in ARMOR-COMMON.
* Beschreibe nichts als auf einer Platine funktionierend, was nur gebaut wurde.
* Melde eine Schwachstelle privat: [SECURITY.md](SECURITY.md). Fragen, Ideen und Meldungen: electrohobby3d@gmail.com ([SUPPORT.md](SUPPORT.md)).

## 👤 AUTOR

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 LIZENZ

GPL-3.0-or-later - siehe [LICENSE](LICENSE).
