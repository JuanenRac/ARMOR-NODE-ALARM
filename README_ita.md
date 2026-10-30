<p align="center">
  <img src="images/ARMOR_BANNER.svg" alt="Banner di ARMOR-NODE-ALARM" width="100%">
</p>

# 🚨 ARMOR-NODE-ALARM

<p align="center">
  <a href="README.md">🇺🇸 English</a> |
  <a href="README_spa.md">🇪🇸 Español</a> |
  <a href="README_fra.md">🇫🇷 Français</a> |
  🇮🇹 <b>Italiano</b> |
  <a href="README_deu.md">🇩🇪 Deutsch</a> |
  <a href="README_zho.md">🇨🇳 简体中文</a> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  <a href="README_por.md">🇵🇹 Português</a>
</p>

### Nodo e centrale d'allarme: le zone, l'inserimento, i ritardi, la sirena e il PIN dell'allarme di casa, con il firmware del nodo e le regole provate su un computer (nulla ha girato su una scheda)

<p align="center">
  <img src="https://img.shields.io/badge/License-GPL%203.0--or--later-blue.svg" alt="GPL 3.0 o successiva">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599c.svg" alt="Linguaggio">
  <img src="https://img.shields.io/badge/Zones-wired%20%2B%20Zigbee-ffb020.svg" alt="Zones">
  <img src="https://img.shields.io/badge/Checks-482%2C535-2ea44f.svg" alt="Checks">
  <img src="https://img.shields.io/badge/Tests-488930%20checks-2ea44f.svg" alt="Test">
  <img src="https://img.shields.io/badge/Version-0.4.9-0f6cbd.svg" alt="Versione">
  <img src="https://img.shields.io/badge/Maturity-functional-00E5FF.svg" alt="Maturità">
</p>

---

**Controllo di onestà - cosa funziona oggi:** Ciò che esiste è il **nucleo dell'allarme** (cos'è una zona, come un anello cablato letto dall'ADC diventa uno stato, le regole della centrale: inserimento, ritardi, l'allarme, la sirena, il PIN con blocco) e il **firmware del nodo** costruito su di esso (impostazioni, pannello web, messaggi con il server, il ponte della radio Zigbee, un coordinatore Zigbee in rete, relè, Bluetooth, aggiornamenti firmati), che si compila per entrambe le schede; il nucleo, le impostazioni, i messaggi e il pannello in un vero browser contro un nodo sostitutivo sono provati su un computer con 488.930 controlli in 18 programmi di test. **Nulla ha girato su una scheda né su un impianto reale, nessuna zona è stata cablata né sirena collegata, la conversazione Zigbee del nodo con un coordinatore (locale o in rete) è scritta e provata contro un coordinatore sostitutivo ma non ha mai girato contro un coordinatore o un sensore reali, e questo non è un sistema d'allarme certificato.**

---

## 📋 In breve

| | |
|---|---|
| Versione | `0.4.9` |
| Maturità | Funzionale |
| Ruolo | Nodo di campo centrale di allarme per ESP32-S3 (famiglia: Field Nodes, padre: ARMOR-COMMON) |
| Tecnologia | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| Test | 488930 verifiche in 18 programmi host e 5 verifiche Python (`ctest`) |
| Ingressi | Fino a 8 anelli cablati, sensori Zigbee (IAS Zone) e sirena (IAS WD), 4 pulsanti |
| Gira su | Una scheda ESP32-S3 (Ethernet o Wi-Fi) con la scheda base; test su host su qualsiasi computer |
| Su hardware reale | Ancora nessuno: nessuna scheda, zona, sirena o coordinatore Zigbee collegati (solo test su computer, la compilazione e il pannello in un browser) |
| Licenza | GPL-3.0-or-later |

**Vai a:** [Panoramica](#-panoramica) · [Architettura](#-architettura) · [Sicurezza](#-sicurezza-e-protezione) · [Interfacce](#-interfacce) · [Configurazione](#-configurazione) · [Avvio rapido](#-avvio-rapido) · [Test](#-test) · [Problemi](#-risoluzione-dei-problemi) · [Struttura](#-struttura-del-repository) · [Correlati](#-progetti-correlati)

## 🚦 Stato del progetto

Ogni riga è una parte di questo repository e il semaforo dice a che punto è oggi. È scritta in base a ciò che il repository può mostrare (test, compilazioni, esecuzioni), non a ciò che è previsto.

🟢 Fatto e verificato · 🟡 Funziona, con limiti noti · 🟠 Fatto a metà o non ancora verificato · 🔴 Non fatto o non funzionante

| Area | Stato | Cosa significa qui |
|---|---|---|
| Software | 🟢 | Il firmware compila nel contenitore ESP-IDF 5.5.5 per entrambe le schede (Wi-Fi ed Ethernet). |
| Test | 🟢 | Superano 488.930 controlli in 18 programmi host (`ctest`): le zone, i circuiti cablati letti dall'ADC, le regole di inserimento, ritardi, sirena e PIN, le impostazioni e i messaggi. |
| Protocolli e dati | 🟠 | La conversazione Zigbee del nodo stesso con un coordinatore, locale o in rete, è provata solo contro un coordinatore simulato; non ha mai incontrato un coordinatore o un sensore reali. |
| Interfaccia | 🟡 | Il pannello web è stato provato in un browser reale contro un nodo simulato, non su una scheda. |
| Hardware | 🔴 | Nessuna scheda costruita, nessuna zona cablata e nessuna sirena o coordinatore Zigbee collegati: nulla ha funzionato su un impianto reale. |
| Sicurezza | 🟡 | Il PIN con blocco, il codice di configurazione, il limite di accessi e gli aggiornamenti firmati sono testati sull'host; non c'è stata una revisione indipendente e non è un sistema di allarme certificato. |
| Aggiornamenti | 🟠 | I controlli di hash e firma Ed25519 e la logica di ripristino passano sull'host; un aggiornamento via etere non è mai stato eseguito su una scheda. |
| Scrittura sulle apparecchiature | 🟠 | La sirena e i relè sono comandati dalle regole dell'allarme e provati contro simulazioni; nessuno è stato collegato a una sirena o a un relè reali. |
| Integrazione | 🟠 | I messaggi del nodo sono validati contro gli schemi condivisi e i vettori di conformità di ARMOR-COMMON. Il suo client MQTT viene provato contro un vero Mosquitto nei test; nessuna scheda li ha ancora inviati a un server. |
| Documentazione | 🟢 | Otto lingue, la guida del firmware, le note di cablaggio e il registro delle modifiche sono aggiornati. |

## 🎯 Panoramica

* **Zone:** quattro tipi - *istantanea* (una porta, una finestra), *d'ingresso* (la porta da cui si entra: avvia il ritardo d'ingresso), *interna* (movimento: solo in "fuori casa") e *sempre* (fumo, gas, panico: inserita o no). Una zona può essere un anello cablato o un sensore Zigbee.
* **L'anello cablato:** un contatto con resistenza di fine linea, letto come tensione, dà quattro stati su un solo pin - normale, scattato e manomissione per un filo in cortocircuito o tagliato. I limiti sono impostazioni; un filtro antirimbalzo impedisce che un contatto che rimbalza faccia scattare l'allarme.
* **La centrale:** inserimento *fuori casa* e *in casa*, un ritardo di uscita che non tratta da intruso chi esce, un ritardo d'ingresso, un allarme che **resta memorizzato** fino a un disinserimento corretto, una sirena che si ferma da sola dopo un po', l'esclusione delle zone aperte se si forza (mai un rilevatore di fumo che suona) e un registro degli ultimi cento eventi.
* **Il PIN:** l'inserimento non lo chiede, il disinserimento e il silenziamento sì; i PIN errati vengono contati e, dopo alcuni, tutto viene rifiutato per un po', anche il PIN giusto.
* **Silenziare senza disinserire:** finché la sirena suona, un PIN corretto la ferma subito con il sistema ancora inserito - l'allarme resta agganciato e registrato, una zona che non ha ancora suonato può farla suonare di nuovo, e un PIN errato conta per il blocco come uno errato di disinserimento.
* **Il piano:** un gateway sulla scheda del nodo elettrico (radio Zigbee, zone cablate, relè della sirena) che lavora con ARMOR-SERVER e **continua a sorvegliare quando il server manca**: una centrale prende la radio al server tramite il ponte Zigbee di ARMOR-NODE-ELECTRICAL. Vedi [il progetto](docs/DESIGN.md) e [il passaggio di consegne](docs/FAILOVER.md).
* **Il nodo:** firmware per l'ESP32-S3 (Ethernet PoE o Wi-Fi) con fino a otto zone cablate, la sirena su un relè, il pannello web di tutti i nodi in otto lingue, la radio Zigbee prestata a Zigbee2MQTT e i messaggi `armor/alarm/<nodo>/state|command|result`; il server può inserire e disinserire solo se tre interruttori indipendenti lo permettono. Vedi [il firmware](docs/NODE_FIRMWARE.md), [i messaggi](docs/ALARM_MESSAGES.md) e [l'hardware](docs/HARDWARE.md).
* **Prendere la radio:** quando Zigbee2MQTT del server è assente da un certo tempo, il nodo avvia da sé il coordinatore (ZNP), legge i sensori delle zone che ne nominano uno (IAS Zone) e comanda sirene Zigbee (IAS WD), e la rilascia appena il client torna.
* **Non ancora:** un coordinatore o sensore reali, l'hardware e qualsiasi prova su un impianto reale.
* **Un riavvio non disinserisce la casa:** la modalità, un allarme in corso e le zone escluse vengono conservati nella flash e ripresi all'avvio; un anello tagliato o in cortocircuito non viene mai escluso; i messaggi passano da una coda (nessuno stallo con il broker) e il watchdog riavvia la scheda se si ferma il task che legge le zone.
* **Aggiornamenti via etere firmati:** il nodo installa da GitHub solo un'immagine il cui SHA-256 coincide e che porta una firma Ed25519 valida del responsabile (la chiave pubblica è nel firmware, quella privata non lascia mai il responsabile); un file caricato a mano da un amministratore nel pannello resta accettato. Le pagine e i messaggi del pannello sono in otto lingue.
* **Abbinamento Bluetooth con PIN:** il PIN a sei cifre deriva dal codice di configurazione, è stampato sulla console USB e mostrato a un amministratore nel pannello; il telefono deve inserirlo prima di poter scrivere le impostazioni del nodo.
* **Sirena e riavvii:** il relè della sirena appartiene solo all'allarme (né il broker, né la centrale, né il pannello possono muoverlo, e non si apre alla perdita del broker); una zona che non aveva ancora suonato fa suonare di nuovo la sirena dopo il suo tempo; dopo un riavvio tornano un allarme di fumo o panico a sistema disinserito, un allarme silenziato e un blocco; un ripristino a mano viene rifiutato con l'allarme inserito; un sensore Zigbee che non parla mai conta come perso.
* **Solo la casa può usare il pannello:** il pannello risponde per impostazione predefinita alla rete privata (o alle reti indicate dal proprietario, come una VPN), rifiuta un elenco che escluderebbe chi lo salva, conta le password errate per persona e per indirizzo e funziona dietro il proxy del server per essere raggiunto da fuori casa senza pubblicare il nodo.
* **Una zona illeggibile non viene mostrata come chiusa:** se il convertitore non parte o un anello non dà lettura per cinque giri di fila, la zona passa in *guasto* (trouble) - non sorveglia, non fa mai suonare la sirena, impedisce l'inserimento (salvo forzandolo, e allora viene esclusa e annotata) e il server avvisa.
* **Collegamento cifrato facoltativo con il broker (TLS):** disattivato per impostazione predefinita, quindi il nodo parla MQTT non cifrato come prima. Un interruttore e una casella per il certificato dell'autorità propria dell'installazione (PEM, al massimo 2 KB) nella pagina Broker lo fanno collegare con `mqtts://`, fidarsi solo di quell'autorità e verificare che il certificato del broker sia stato fatto per l'indirizzo raggiunto; ciò che non è un solo certificato PEM viene rifiutato. Coperto da test sul computer; l'handshake non è ancora stato eseguito su una scheda.
* **Profili di inserimento:** un comando di inserimento può portare un profilo (Fuori casa, In casa, Notte, il tuo) che sceglie, per quell'inserimento, quali zone sorvegliano, quali fanno scattare l'allarme subito o dopo il ritardo d'ingresso, i ritardi di uscita e d'ingresso e se la sirena suona (un allarme silenzioso resta memorizzato, registrato e segnalato). Non tocca mai le zone di fumo, panico o manomissione, il PIN, il blocco, il silenziamento, un inserimento forzato né una zona in guasto; il profilo sopravvive a un riavvio e un inserimento senza profilo si comporta esattamente come prima. Provato contro le vecchie regole con un'esecuzione casuale e contro il contratto condiviso.
* **Rapporto di salute:** circa una volta al minuto il nodo pubblica da quanto è acceso e perché si è riavviato l'ultima volta, la memoria libera, il collegamento e il segnale Wi-Fi, quante volte si è connesso al broker e l'ultimo errore registrato (`armor/health/<nodo>`, il contratto `node_health` di ARMOR-COMMON). Il codice che lo costruisce è condiviso da tutti i nodi e testato su un computer; non è mai girato su una scheda.
* **Controllo dal server:** Studio può riavviare il nodo in modo pulito, metterlo in pausa fino a un'ora (riprende da solo), mettere il suo lavoro in standby (senza fine, conservato ai riavvii, il nodo resta connesso e continua a rispondere), farlo mostrare, chiedere un'istantanea diagnostica e segnarlo in manutenzione (`armor/control/<nodo>/...`, il contratto `node_control` di ARMOR-COMMON). Ogni ordine è in due passi con un token monouso legato a quell'operazione, solo il server può scriverli sul broker e non esistono ripristino di fabbrica, cancellazione né sonno profondo. Finché il nodo è in pausa o in standby le sue zone non vengono lette, nulla viene valutato, attivato o fatto suonare, nessuno stato viene pubblicato e il relè della sirena resta aperto: la casa NON è protetta, e il rapporto di salute lo dice. Il codice è condiviso da tutti i nodi e provato su un computer; non ha ancora girato su una scheda.
* **Diagnostica senza cavo:** dopo un avvio non pulito (un blocco, un watchdog, un calo di alimentazione, un'allocazione fallita) il rapporto di salute ne indica la causa per dieci minuti; un arresto anomalo lascia il suo core dump da vedere, scaricare o cancellare dal pannello; le impostazioni salvate vengono copiate prima di ogni installazione del firmware e recuperate se un aggiornamento le danneggia; e un amministratore può chiedere al nodo, tramite il server, un pacchetto di diagnostica o il suo registro con un token monouso.
* **Le opzioni di sicurezza del server:** la centrale applica le opzioni che il server tiene per lei (`armor/alarm/<nodo>/policy`) e ciascuna può essere fissata sul nodo stesso: lo stato inserito salvato è legato alle zone e al PIN, i messaggi di zona Zigbee del server possono essere spenti e, se firmati, vengono verificati (firma con una chiave digitata nel pannello e un numero di sequenza che può solo crescere). Provato su un computer con i valori che calcolano il contratto e il server; non eseguito su una scheda.
* **Sigillatura facoltativa dei segreti salvati:** disattivata per impostazione predefinita (`security.seal_secrets`, o `nodes.nvs_encryption` del server). Attiva, le password, i token, le chiavi, i PIN e gli hash delle password che il nodo salva restano sigillati con ChaCha20-Poly1305 e tutto il resto resta leggibile; attivarla o disattivarla li riscrive senza perdere nulla, anche se il nodo si ferma a metà. Nessun eFuse viene bruciato e la flash non è cifrata: la chiave si trova in una partizione dello stesso chip, quindi **non** protegge da chi ha la scheda, e un nodo installato prima di quella partizione mantiene i suoi segreti com'erano finché non viene flashato con la nuova tabella delle partizioni.

## 🧩 Ruolo in A.R.M.O.R.

ARMOR-NODE-ALARM è la centrale d'allarme: le zone, l'inserimento, i ritardi, la sirena e il PIN si decidono sul nodo stesso, così la casa resta sorvegliata quando il server è assente. [ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) la mostra, lega sensori Zigbee alle sue zone e, quando lo permetti, la inserisce e disinserisce; il menu *Centrali d'allarme* e il *Progettista dell'allarme* di [ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) e l'app [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) la comandano. La sua radio Zigbee può essere sulla scheda o un coordinatore in rete.

| Nome | Come | A cosa serve |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | Riceve lo stato della centrale; ordina inserimento, disinserimento e silenzio solo dietro tre interruttori |
| ARMOR-STUDIO | menu delle centrali di allarme | Mostra le zone, gli eventi e la politica, e apre il pannello del nodo |
| ARMOR-ANDROID-CONTROL | Bluetooth | Configura il nodo sul campo e mostra le centrali di allarme |
| ARMOR-COMMON | contratto di allarme | I messaggi sono verificati con i suoi schemi; il firmware comune è copiato da lì |

## 🏛️ Architettura

<p align="center">
  <img src="images/architecture.svg" alt="Architettura di ARMOR-NODE-ALARM: chi lo chiama, cosa contiene e da cosa dipende" width="100%">
</p>

## 🖼️ Schermate

Catturate a 1920×1080 su un banco di prova, con dati di esempio e senza alcuna installazione reale dietro.

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="Riepilogo: lo stato del nodo in breve - rete, broker, firmware e ciò che riporta." width="100%"><br><sub>Riepilogo: lo stato del nodo in breve - rete, broker, firmware e ciò che riporta.</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="Zone: ogni zona cablata o Zigbee della centrale, il tipo, il tempo di mantenimento e lo stato dal vivo." width="100%"><br><sub>Zone: ogni zona cablata o Zigbee della centrale, il tipo, il tempo di mantenimento e lo stato dal vivo.</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Radio Zigbee: il ponte seriale verso il modulo coordinatore, quale client può usarlo e lo stato del ponte." width="100%"><br><sub>Radio Zigbee: il ponte seriale verso il modulo coordinatore, quale client può usarlo e lo stato del ponte.</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="Relè e sirena: le uscite della scheda base e cosa comanda ciascuna." width="100%"><br><sub>Relè e sirena: le uscite della scheda base e cosa comanda ciascuna.</sub></td></tr>
</table>

## 🔒 Sicurezza e protezione

| Minaccia | Difesa |
|---|---|
| Password in chiaro o compilate | PBKDF2-HMAC-SHA256 con 10 000 round; nessuna password nel firmware (un nodo senza utenti entra in configurazione); al massimo 4 utenti (admin o visualizzatore) e da 8 a 64 caratteri; al massimo 6 sessioni, 30 minuti di inattività |
| Prendere un nodo appena programmato | La configurazione richiede un codice di avvio: quello della build (`CONFIG_ARMOR_SETUP_CODE`) o uno casuale tenuto in flash e mostrato sulla console USB; con un segreto di flotta è `HMAC-SHA256(segreto, MAC)`; è anche la chiave del Wi-Fi di configurazione (WPA2, mai in bridge) |
| Il pannello raggiunto da Internet | Risponde di default alle reti private di una casa (10/8, 172.16/12, 192.168/16, 169.254/16, 127/8 e 100.64/10 dove vivono alcune VPN); l'impostazione `web.admin_sources` elenca altri indirizzi e reti IPv4 (8 voci, 120 caratteri); tutto il resto è rifiutato tranne l'identità pubblica `GET /api/v1/session`; una lista che escluderebbe chi la salva è rifiutata |
| Il pannello in chiaro su HTTP | HTTPS sulla 443 con un certificato che il nodo crea da sé (autofirmato, in flash, impronta SHA-256 visibile per confrontarla); il pannello può essere http, https o entrambi; il cookie di sessione è sempre HttpOnly e SameSite=Strict (e Secure su TLS) |
| Un firmware falso da GitHub | Un'immagine si installa solo con uno SHA-256 coincidente e una firma Ed25519 valida fatta con la chiave del manutentore (`no_checksum`, `no_signature`, `bad_signature`, `checksum_mismatch`, `wrong_firmware`, `not_newer`, `version_mismatch` la rifiutano); un caricamento manuale da un amministratore non è verificato: un limite esplicito |
| Dirottamento della configurazione Bluetooth | Un PIN a 6 cifre derivato dal codice di configurazione, visibile sulla console USB e a un amministratore; il telefono deve digitarlo prima di scrivere le impostazioni; la finestra dura `CONFIG_ARMOR_BLE_SETUP_MINUTES` (10 di default, da 1 a 120) |
| Segreti memorizzati letti da chi ha la scheda | Opzionale (spento di default, interruttore `security.seal_secrets` o `nodes.nvs_encryption` del server): ChaCha20-Poly1305 per password, token, chiavi, PIN e hash. Non cifra la flash né brucia un eFuse: la chiave è in una partizione dello stesso chip, quindi non protegge da chi ha la scheda |
| Il collegamento al broker in chiaro | TLS opzionale (`mqtts://`) con l'autorità propria del proprietario (PEM fino a 2 KB) e verifica del nome; spento di default. L'handshake è girato solo nei test su host, non su una scheda |
| Indovinare il PIN della centrale | Il confronto non rivela dove differisce; errori consecutivi bloccano la centrale per `lockout_s` (60 s di default, da 1 a 3600) per 1, 5 e 15 nei primi tre blocchi, anche con il PIN giusto; un blocco in corso è ripristinato dopo un riavvio; inserire non richiede PIN, disinserire e silenziare sì |
| Un riavvio che disinserisce la casa | Il modo, l'allarme in corso e le zone escluse sono tenuti in flash e recuperati; un anello interrotto o in corto non viene mai lasciato fuori; fumo, panico e manomissione scattati a sistema disinserito tornano dopo un riavvio; il ripristino con BOOT cancella solo a centrale DISINSERITA |
| Una zona illeggibile sembra chiusa | Mostra `trouble` (il convertitore non parte o 5 giri senza lettura) e la centrale non inserisce con essa salvo forzatura, e allora è esclusa e annotata; un rilevatore di fumo o panico che sta suonando non viene mai escluso |
| Un ordine del server che nessuno ha autorizzato | Inserire, disinserire e silenziare dal server richiedono TRE interruttori (l'impostazione del nodo, la lista di accesso del broker e `ARMOR_ALARM_COMMANDS=1` sul server), il server chiede il PIN dell'utente prima di inviare un disinserimento o un silenzio, e tutto è spento di default; il relè della sirena appartiene solo all'allarme (né il broker, né la centrale, né il pannello possono muoverlo) |
| Messaggi Zigbee falsi | Il topic `armor/alarm/<id>/zigbee` porta un HMAC-SHA256 su nodo, zona, stato, batteria e sequenza con la chiave della centrale (64 hex digitati nel pannello), e la sequenza può solo salire; il topic della politica porta `config_hash`, `zone_feed` e la sua firma, ciascuno fissabile sul nodo |

Non è un sistema di allarme certificato ([docs/SAFETY.md](docs/SAFETY.md)): è un progetto domestico e non è mai girato su una scheda né in un impianto reale. Se manca il server la centrale decide da sola, e se cade anche il nodo o la sua radio nessuno sorveglia. Leggi [SECURITY.md](SECURITY.md) per segnalare una vulnerabilità.

## 🔌 Interfacce

| Parte | Cosa imposta |
|---|---|
| MQTT | `armor/alarm/<id>/state` (non retained: zone con id, nome, tipo, stato ed esclusione, la fase), `/command`, `/result`, `/zigbee` e `/policy` (server verso centrale); `armor/device/<id>/<relè>/{state,set}`, `armor/health/<id>`, `armor/control/<id>/...` |
| Zone | Tipo `instant`, `entry`, `interior` o `always`; stato `normal`, `triggered`, `tamper` o `trouble`; fino a 8 anelli cablati (un contatto con resistenza di fine linea letto dall'ADC dà quattro stati su un pin; la scheda Ethernet ha 6 pin analogici validi, la Wi-Fi 8), 4 pulsanti e un registro di 100 eventi |
| Zigbee | Un ponte seriale (ZNP) verso un coordinatore, sensori IAS Zone e sirene IAS WD; il nodo prende la radio quando lo Zigbee2MQTT del server tace per `zigbee_silence_min` e la restituisce al ritorno del server ([docs/ZIGBEE.md](docs/ZIGBEE.md), [docs/FAILOVER.md](docs/FAILOVER.md)) |

## ⚙️ Configurazione

| Variabile | Predefinito | Cosa imposta |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | I ritardi di uscita e di ingresso; l'uscita non tratta come intruso chi se ne va |
| `panel.lockout_s` | 60 (1-3600) | Il blocco del PIN, per 1, 5 e 15 nei primi tre |
| `zigbee_silence_min` | - | Quanto può tacere lo Zigbee del server prima che il nodo prenda la radio |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | Le impostazioni che tutti i nodi condividono: il nome (vuoto dà `armor-` e gli ultimi byte del MAC), il codice di configurazione (vuoto: casuale), il segreto di flotta (16+ caratteri, mai nel repository), le prime impostazioni del broker, il battito (da 2 a 300 s), i minuti di configurazione Bluetooth (da 1 a 120) e il ripristino con BOOT (8 s nei primi 30 s) |

I nuovi suoni dopo un silenzio sono limitati a 3 (`kMaxResounds`). Il relè della sirena parte aperto a ogni riavvio e non cade aperto se si perde il broker.

## 🚀 Avvio rapido

**Requisiti:** un compilatore C++17 per i test su host; il container ESP-IDF 5.5.5 per costruire l'immagine; una scheda ESP32-S3 (Ethernet o Wi-Fi) con la scheda base.

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

Vedi [il progetto](docs/DESIGN.md), [come la centrale subentra al server](docs/FAILOVER.md) e le [note di sicurezza](docs/SAFETY.md).

## 🧪 Test

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

18 programmi di test su host e 5 verifiche Python girano con `ctest`; 488930 verifiche contate dai programmi host, compilati con `-Werror`.

Verifiche Python: i testi del pannello in otto lingue, i task, i percorsi di salvataggio, la sessione Bluetooth sotto il suo lock e i percorsi chiamati dal pannello (e, in alcuni nodi, i codici HTTP o i glifi dei caratteri)

I test su host richiedono un qualsiasi compilatore C++17 (Linux, WSL, MSYS2); il pannello web è anche provato in un browser reale contro un nodo sostitutivo (`tools/panel_mock.mjs`, `tools/panel_browser_test.mjs`).

La maggior parte delle verifiche sono scansioni esaustive delle regole della centrale; anelli, sirena e collegamento Zigbee sono sostituti.

## 🏗️ Compilazione e rilascio

`tools/build_node.sh` costruisce l'immagine nel container ESP-IDF 5.5.5 e la versione porta l'immagine, il suo SHA-256 e la sua firma Ed25519, una per scheda; l'immagine si scrive via USB-C la prima volta e poi si aggiorna dal pannello.

## 🩺 Risoluzione dei problemi

| Vedi | Perché | Fai |
|---|---|---|
| Hai perso la password o il nodo non entra più in rete | Un nodo in configurazione senza indirizzo dopo 90 s apre un proprio Wi-Fi `ARMOR-SETUP-xxxxxx` (apri `http://192.168.4.1/`) | Per ricominciare tieni premuto BOOT per 8 secondi nei primi 30 secondi dopo l'accensione (cancella impostazioni e utenti), o usa *Ripristino di fabbrica* nel pannello |
| Un aggiornamento da GitHub è rifiutato | La versione non ha checksum o firma, non corrispondono (`e_no_checksum`, `e_no_signature`, `e_checksum_mismatch`, `e_bad_signature`), non ha un'immagine per questa scheda (`e_no_asset_for_board`) o è di un altro progetto (`e_wrong_firmware`) | Il nodo non è cambiato; riprova o lascia che un amministratore carichi il file a mano |
| La centrale non inserisce | Una zona è in `trouble` (il convertitore non parte o 5 giri senza lettura) | Correggi il cablaggio, o forza l'inserimento (la zona viene allora esclusa e annotata) |
| Il PIN è rifiutato anche se è giusto | La centrale è bloccata dopo tentativi falliti (anche il PIN giusto è rifiutato) | Attendi `lockout_s`; il blocco sopravvive a un riavvio |
| Il ripristino con BOOT non fa nulla | La centrale è inserita: il ripristino cancella solo una centrale disinserita | Disinserisci prima con il PIN |

## 📂 Struttura del repository

<p align="center">
  <img src="images/repository-structure.svg" alt="Struttura del repository di ARMOR-NODE-ALARM" width="100%">
</p>

<details>
<summary>Versione testuale dell'albero</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                il firmware: un componente ESP-IDF
├── core/                                la logica senza ESP-IDF, testata su un computer
├── panel/                               il pannello web incorporato nel firmware
├── tests/                               test su host e verifiche Python
├── docs/                                11 guide: sicurezza, messaggi, Zigbee, subentro
├── tools/                               strumenti di build, impacchettamento e verifica
├── .github/                             workflow CI e modelli per issue e pull request
├── images/                              banner, icona e le due immagini di questo README
├── build.sh build.bat                   build tramite il flusso di progetto condiviso
├── test.sh test.bat                     eseguire i test tramite il flusso condiviso
├── run.sh run.bat                       avvio tramite il flusso di progetto condiviso
├── armor.project.json                   manifesto: versione, ruolo, maturità, comando di build
├── CHANGELOG.md                         cosa è cambiato in ogni versione
├── SECURITY.md SUPPORT.md LICENSE       politica di sicurezza, supporto e licenza
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   come contribuire e il codice di condotta
├── CMakeLists.txt                       il progetto ESP-IDF
├── partitions.csv                       la tabella della flash
├── sdkconfig.defaults                   le impostazioni del firmware
├── sdkconfig.board.s3-eth               le impostazioni di una scheda
├── sdkconfig.board.s3-wifi              le impostazioni di una scheda
├── secrets/                             esempio del file del nodo (quelli veri non sono versionati)
├── build-test.sh build-test.bat         eseguire i test tramite il flusso condiviso
└── .gitattributes .gitignore            impostazioni di Git
```

</details>

## 🗺️ Roadmap e limiti noti

**Limiti noti oggi**

* La conversazione Zigbee del nodo stesso con un coordinatore, locale o in rete, è provata solo contro un coordinatore simulato; non ha mai incontrato un coordinatore o un sensore reali.
* Il pannello web è stato provato in un browser reale contro un nodo simulato, non su una scheda.
* Nessuna scheda costruita, nessuna zona cablata e nessuna sirena o coordinatore Zigbee collegati: nulla ha funzionato su un impianto reale.
* Il PIN con blocco, il codice di configurazione, il limite di accessi e gli aggiornamenti firmati sono testati sull'host; non c'è stata una revisione indipendente e non è un sistema di allarme certificato.
* I controlli di hash e firma Ed25519 e la logica di ripristino passano sull'host; un aggiornamento via etere non è mai stato eseguito su una scheda.
* La sirena e i relè sono comandati dalle regole dell'allarme e provati contro simulazioni; nessuno è stato collegato a una sirena o a un relè reali.

**Prossimi passi**

* Cablare anelli e una sirena reali, provare il collegamento Zigbee con un coordinatore e sensori reali, e far girare la centrale in un impianto reale prima di affermare altro.

## 🌍 Lingue

Il README e i testi esistono in inglese, spagnolo, francese, italiano, tedesco, cinese, giapponese e portoghese europeo.

## 🔗 Progetti correlati

**A.R.M.O.R.** (Autonomous Radar & Multimodal Observation Range) è un sistema di sicurezza perimetrale fatto di repository indipendenti. Ognuno ha la propria versione, i propri test e il proprio README; ecco la famiglia:

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - Contratti dei messaggi, validatori, vettori di conformità e tipi generati
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - Firmware del nodo di campo per ESP32-S3 con tre radar e un proprio pannello web
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - Protocolli di inverter e batterie solari e messaggi di un nodo gateway
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - Nodo elettrico: contatori, il messaggio delle letture della rete e le regole di manovra
* **ARMOR-NODE-ALARM** (questo repository) - Nodo e centrale d'allarme: zone, inserimento, ritardi, sirena e PIN, con il server o senza
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - Pannello touch: lo stato del sistema su uno schermo a parete, attivare e riconoscere gli allarmi, e la casa dell'assistente vocale
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - La rete locale: i suoi dispositivi, internet e ciò che cambia
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - Coordinatore centrale: telemetria, allarmi, dispositivi, letture solari e telecamere
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - Console web: telecamere, radar, allarmi, energia solare e progettista del sito 2D/3D
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - Client Android dell'operatore con radar 2D/3D in tempo reale
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - Client operatore per iPhone
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - Politica di inferenza visiva che spiega le sue decisioni e non agisce mai
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - Zone delle telecamere, maschere e regole di rilevamento per la Jetson Orin NX
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - Intenti vocali offline con una conferma impossibile da falsificare
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - Contenitori, elettronica e matrice di accettazione da banco
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - Distribuzione, banco di prova CM5, backup e TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - Simulatore di telemetria offline con guasti ripetibili
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - Rileva, installa e aggiorna i repository stessi dell'ecosistema
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - Architettura, base di sicurezza e matrice delle capacità

## 📚 Documentazione

* [Sicurezza](docs/SAFETY.md): cosa promette la centrale e cosa no
* [Messaggi di allarme](docs/ALARM_MESSAGES.md), [Zigbee](docs/ZIGBEE.md) e [subentro](docs/FAILOVER.md)
* [Progetto](docs/DESIGN.md), [hardware](docs/HARDWARE.md), [scheda base](docs/BASE_BOARD.md) e [firmware del nodo](docs/NODE_FIRMWARE.md)
* [Prove al banco](docs/BENCH_TESTS.md), [codice di configurazione](docs/SETUP_CODE.md) e [provisioning Bluetooth](docs/BLE_PROVISIONING.md)
* [Matrice delle capacità: cosa è provato e cosa no](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [Catalogo dei progetti: versioni e dipendenze tra i repository](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [Cronologia delle modifiche di questo repository](CHANGELOG.md)

## 🤝 Contribuire e comunità

* Prima di una pull request esegui `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py`: deve passare. Vedi [CONTRIBUTING.md](CONTRIBUTING.md) e il [codice di condotta](CODE_OF_CONDUCT.md).
* Il README esiste in otto lingue ed è generato dalle sorgenti in ARMOR-DOCS: una modifica al testo si fa lì.
* Una modifica a un messaggio tra componenti parte da ARMOR-COMMON.
* Non descrivere come funzionante su una scheda ciò che è stato solo compilato.
* Segnala una vulnerabilità in privato: [SECURITY.md](SECURITY.md). Domande, idee e segnalazioni: electrohobby3d@gmail.com ([SUPPORT.md](SUPPORT.md)).

## 👤 AUTORE

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 LICENZA

GPL-3.0-or-later - vedi [LICENSE](LICENSE).
