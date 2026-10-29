<p align="center">
  <img src="images/ARMOR_BANNER.svg" alt="Bannière d'ARMOR-NODE-ALARM" width="100%">
</p>

# 🚨 ARMOR-NODE-ALARM

<p align="center">
  <a href="README.md">🇺🇸 English</a> |
  <a href="README_spa.md">🇪🇸 Español</a> |
  🇫🇷 <b>Français</b> |
  <a href="README_ita.md">🇮🇹 Italiano</a> |
  <a href="README_deu.md">🇩🇪 Deutsch</a> |
  <a href="README_zho.md">🇨🇳 简体中文</a> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  <a href="README_por.md">🇵🇹 Português</a>
</p>

### Nœud et centrale d'alarme : les zones, l'armement, les temporisations, la sirène et le code PIN de l'alarme de la maison, avec le firmware du nœud et les règles testées sur un ordinateur (rien n'a tourné sur une carte)

<p align="center">
  <img src="https://img.shields.io/badge/License-GPL%203.0--or--later-blue.svg" alt="GPL 3.0 ou ultérieure">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599c.svg" alt="Langage">
  <img src="https://img.shields.io/badge/Zones-wired%20%2B%20Zigbee-ffb020.svg" alt="Zones">
  <img src="https://img.shields.io/badge/Checks-482%2C535-2ea44f.svg" alt="Checks">
  <img src="https://img.shields.io/badge/Tests-488930%20checks-2ea44f.svg" alt="Tests">
  <img src="https://img.shields.io/badge/Version-0.4.8-0f6cbd.svg" alt="Version">
  <img src="https://img.shields.io/badge/Maturity-functional-00E5FF.svg" alt="Maturité">
</p>

---

**Vérification d'honnêteté - ce qui fonctionne aujourd'hui :** Ce qui existe, c'est le **cœur de l'alarme** (ce qu'est une zone, comment une boucle câblée lue par l'ADC devient un état, les règles de la centrale : armement, temporisations, l'alarme, la sirène, le PIN avec verrouillage) et le **firmware du nœud** construit dessus (réglages, panneau web, messages avec le serveur, le pont de la radio Zigbee, un coordinateur Zigbee sur le réseau, relais, Bluetooth, mises à jour signées), qui se compile pour les deux cartes ; le cœur, les réglages, les messages et le panneau dans un vrai navigateur contre un nœud factice sont testés sur un ordinateur avec 477 588 vérifications dans 6 programmes de test. **Rien n'a tourné sur une carte ni sur une installation réelle, aucune zone n'a été câblée ni aucune sirène connectée, la conversation Zigbee du nœud avec un coordinateur (local ou sur le réseau) est écrite et testée contre un coordinateur factice mais n'a jamais tourné contre un vrai coordinateur ni un vrai capteur, et ce n'est pas un système d'alarme certifié.**

---

## 📋 En un coup d'œil

| | |
|---|---|
| Version | `0.4.8` |
| Maturité | Fonctionnel |
| Rôle | Nœud de terrain centrale d'alarme pour ESP32-S3 (famille : Field Nodes, parent : ARMOR-COMMON) |
| Technologie | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| Tests | 488930 vérifications dans 18 programmes hôte et 5 contrôles Python (`ctest`) |
| Entrées | Jusqu'à 8 boucles câblées, capteurs Zigbee (IAS Zone) et sirène (IAS WD), 4 boutons |
| Fonctionne sur | Une carte ESP32-S3 (Ethernet ou Wi-Fi) avec la carte de base ; tests sur hôte sur n'importe quel ordinateur |
| Sur matériel réel | Aucun pour l'instant : ni carte, ni zone, ni sirène, ni coordinateur Zigbee connectés (seulement des tests sur ordinateur, la compilation et le panneau dans un navigateur) |
| Licence | GPL-3.0-or-later |

**Aller à :** [Présentation](#-présentation) · [Architecture](#-architecture) · [Sécurité](#-sécurité-et-protection) · [Interfaces](#-interfaces) · [Configuration](#-configuration) · [Démarrage rapide](#-démarrage-rapide) · [Tests](#-tests) · [Dépannage](#-dépannage) · [Structure](#-structure-du-dépôt) · [Liés](#-projets-liés)

## 🚦 État du projet

Chaque ligne est une partie de ce dépôt et le feu dit où elle en est aujourd'hui. Elle est écrite d'après ce que le dépôt peut montrer (tests, compilations, exécutions), pas d'après ce qui est prévu.

🟢 Fait et vérifié · 🟡 Fonctionne, avec des limites connues · 🟠 À moitié fait ou pas encore vérifié · 🔴 Pas fait ou ne fonctionne pas

| Domaine | État | Ce que cela veut dire ici |
|---|---|---|
| Logiciel | 🟢 | Le firmware compile dans le conteneur ESP-IDF 5.5.5 pour les deux cartes (Wi-Fi et Ethernet). |
| Tests | 🟢 | 488 930 vérifications dans 18 programmes d'hôte réussissent (`ctest`) : les zones, les boucles câblées lues par le CAN, les règles d'armement, de temporisation, de sirène et de PIN, les réglages et les messages. |
| Protocoles et données | 🟠 | La conversation Zigbee du nœud lui-même avec un coordinateur, local ou sur le réseau, n'est testée que contre un coordinateur simulé ; elle n'a jamais rencontré un vrai coordinateur ni un vrai capteur. |
| Interface | 🟡 | Le panneau web a été essayé dans un vrai navigateur contre un nœud simulé, pas sur une carte. |
| Matériel | 🔴 | Aucune carte construite, aucune zone câblée et ni sirène ni coordinateur Zigbee connectés : rien n'a tourné sur une installation réelle. |
| Sécurité | 🟡 | Le PIN avec verrouillage, le code de configuration, la limite de connexions et les mises à jour signées sont testés sur l'hôte ; il n'y a pas eu de revue indépendante et ce n'est pas un système d'alarme certifié. |
| Mises à jour | 🟠 | Les contrôles de hash et de signature Ed25519 et la logique de retour arrière réussissent sur l'hôte ; une mise à jour par voie hertzienne n'a jamais été exécutée sur une carte. |
| Écriture sur les équipements | 🟠 | La sirène et les relais sont pilotés par les règles de l'alarme et testés contre des simulations ; aucun n'a été relié à une vraie sirène ou à un vrai relais. |
| Intégration | 🟢 | Les messages du nœud sont validés par rapport aux schémas partagés et aux vecteurs de conformité d'ARMOR-COMMON. |
| Documentation | 🟢 | Huit langues, le guide du firmware, les notes de câblage et le journal des modifications sont à jour. |

## 🎯 Présentation

* **Zones :** quatre types - *instantanée* (une porte, une fenêtre), *d'entrée* (la porte par laquelle on entre : elle lance la temporisation d'entrée), *intérieure* (mouvement : seulement en « absent ») et *toujours* (fumée, gaz, panique : armée ou non). Une zone peut être une boucle câblée ou un capteur Zigbee.
* **La boucle câblée :** un contact avec résistance de fin de ligne, lu comme une tension, donne quatre états sur une seule broche - normal, déclenché, et sabotage pour un fil en court-circuit ou coupé. Les seuils sont des réglages ; un filtre anti-rebond évite qu'un contact qui rebondit déclenche l'alarme.
* **La centrale :** armement *absent* et *présent*, une temporisation de sortie qui ne traite pas comme un intrus celui qui part, une temporisation d'entrée, une alarme qui **reste verrouillée** jusqu'à un désarmement correct, une sirène qui s'arrête seule au bout d'un moment, l'inhibition des zones ouvertes si on force (jamais un détecteur de fumée qui sonne) et un journal des cent derniers événements.
* **Le PIN :** l'armement n'en demande pas, le désarmement et l'arrêt de l'alarme si ; les PIN faux sont comptés et, après quelques-uns, tout est refusé un moment, même le bon PIN.
* **Couper la sirène sans désarmer :** tant que la sirène sonne, un PIN correct l'arrête aussitôt alors que le système reste armé - l'alarme reste verrouillée et enregistrée, une zone qui n'a pas encore sonné peut la relancer, et un PIN faux compte pour le verrouillage comme un faux PIN de désarmement.
* **Le plan :** une passerelle sur la carte du nœud électrique (radio Zigbee, zones câblées, relais de sirène) qui travaille avec ARMOR-SERVER et **continue de surveiller quand le serveur est absent** : une centrale reprend la radio au serveur par le pont Zigbee d'ARMOR-NODE-ELECTRICAL. Voir [la conception](docs/DESIGN.md) et [la reprise](docs/FAILOVER.md).
* **Le nœud :** firmware pour l'ESP32-S3 (Ethernet PoE ou Wi-Fi) avec jusqu'à huit zones câblées, la sirène sur un relais, le panneau web de tous les nœuds en huit langues, la radio Zigbee prêtée à Zigbee2MQTT et les messages `armor/alarm/<nœud>/state|command|result` ; le serveur ne peut armer et désarmer que si trois interrupteurs indépendants l'autorisent. Voir [le firmware](docs/NODE_FIRMWARE.md), [les messages](docs/ALARM_MESSAGES.md) et [le matériel](docs/HARDWARE.md).
* **Prendre la radio :** quand Zigbee2MQTT du serveur est absent depuis un temps donné, le nœud démarre lui-même le coordinateur (ZNP), lit les capteurs des zones qui en nomment un (IAS Zone) et commande des sirènes Zigbee (IAS WD), et la rend dès que le client revient.
* **Pas encore :** un vrai coordinateur ou capteur, le matériel et tout essai sur une installation réelle.
* **Un redémarrage ne désarme pas la maison :** le mode, une alarme en cours et les zones exclues sont gardés en flash et repris au démarrage ; une boucle coupée ou en court-circuit n'est jamais exclue ; les messages passent par une file (pas d'interblocage avec le broker) et le chien de garde redémarre la carte si la tâche qui lit les zones s'arrête.
* **Mises à jour par radio signées :** le nœud n'installe depuis GitHub qu'une image dont le SHA-256 correspond et qui porte une signature Ed25519 valide du responsable (la clé publique est dans le firmware, la privée ne quitte jamais le responsable) ; un fichier envoyé à la main par un administrateur dans le panneau reste accepté. Les pages et messages du panneau sont en huit langues.
* **Appairage Bluetooth avec un PIN :** le PIN à six chiffres est dérivé du code de configuration, imprimé sur la console USB et montré à un administrateur dans le panneau ; le téléphone doit le saisir avant de pouvoir écrire les réglages du nœud.
* **Sirène et redémarrages :** le relais de la sirène appartient à l'alarme seule (ni le broker, ni la centrale, ni le panneau ne peuvent le bouger, et il ne s'ouvre pas à la perte du broker) ; une zone qui n'avait pas encore sonné refait sonner la sirène après son délai ; après un redémarrage reviennent une alarme de fumée ou de panique système désarmé, une alarme silencieuse et un blocage ; une réinitialisation à la main est refusée alarme armée ; un capteur Zigbee qui ne parle jamais compte comme perdu.
* **Seule la maison peut utiliser le panneau :** le panneau répond par défaut au réseau privé (ou aux réseaux que son propriétaire indique, comme un VPN), refuse une liste qui exclurait celui qui l'enregistre, compte les mauvais mots de passe par personne et par adresse, et fonctionne derrière le proxy du serveur pour être atteint depuis l'extérieur sans publier le nœud.
* **Une zone illisible n'est pas affichée comme fermée :** si le convertisseur ne démarre pas ou qu'une boucle ne donne aucune lecture pendant cinq tours de suite, la zone passe en *défaut* (trouble) - elle ne surveille pas, ne fait jamais sonner la sirène, empêche l'armement (sauf en le forçant, et elle est alors exclue et notée) et le serveur avertit.
* **Liaison chiffrée facultative avec le broker (TLS) :** désactivée par défaut, le nœud parle donc MQTT non chiffré comme avant. Un interrupteur et une zone pour le certificat de l'autorité propre à l'installation (PEM, 2 Ko au plus) dans la page Broker lui font se connecter en `mqtts://`, ne faire confiance qu'à cette autorité et vérifier que le certificat du broker a été établi pour l'adresse atteinte ; ce qui n'est pas un seul certificat PEM est refusé. Couvert par des tests sur ordinateur ; la poignée de main n'a pas encore été exécutée sur une carte.
* **Profils d'armement :** une commande d'armer peut porter un profil (Absent, Présent, Nuit, le vôtre) qui choisit, pour cet armement, quelles zones surveillent, lesquelles déclenchent l'alarme tout de suite ou après la temporisation d'entrée, les temporisations de sortie et d'entrée et si la sirène sonne (une alarme silencieuse reste verrouillée, notée et signalée). Il ne touche jamais les zones fumée, panique ou sabotage, le PIN, le verrouillage, le silence, un armement forcé ni une zone en défaut ; le profil survit à un redémarrage et un armement sans profil se comporte exactement comme avant. Testé contre les anciennes règles par une exécution aléatoire et contre le contrat partagé.
* **Rapport de santé :** environ une fois par minute le nœud publie depuis combien de temps il tourne et pourquoi il a redémarré la dernière fois, sa mémoire libre, sa liaison et son signal Wi-Fi, combien de fois il s'est connecté au broker et la dernière erreur notée (`armor/health/<nœud>`, le contrat `node_health` d'ARMOR-COMMON). Le code qui le construit est partagé par tous les nœuds et testé sur un ordinateur ; il n'a pas tourné sur une carte.
* **Commande depuis le serveur :** Studio peut redémarrer le nœud proprement, le mettre en pause jusqu'à une heure (il reprend seul), mettre son travail en veille (sans fin, conservée aux redémarrages, le nœud reste connecté et continue de répondre), le faire se signaler, demander un instantané de diagnostic et le marquer en maintenance (`armor/control/<nœud>/...`, le contrat `node_control` d'ARMOR-COMMON). Chaque ordre se fait en deux temps avec un jeton à usage unique propre à cette opération, seul le serveur peut les écrire sur le broker, et il n'existe ni réinitialisation d'usine, ni effacement, ni sommeil profond. Tant que le nœud est en pause ou en veille, ses zones ne sont pas lues, rien n'est évalué, armé ni sonné, aucun état n'est publié et le relais de la sirène reste ouvert : la maison n'est PAS protégée, et le rapport de santé le dit. Le code est partagé par tous les nœuds et testé sur un ordinateur ; il n'a pas tourné sur une carte.
* **Diagnostic sans câble :** après un démarrage qui n'était pas propre (plantage, chien de garde, chute d'alimentation, allocation échouée), le rapport de santé en donne la raison pendant dix minutes ; un plantage laisse son vidage mémoire (core dump) à consulter, télécharger ou effacer depuis le panneau ; les réglages enregistrés sont copiés avant chaque installation de micrologiciel et récupérés si une mise à jour les abîme ; et un administrateur peut demander au nœud, via le serveur, un paquet de diagnostic ou son journal avec un jeton à usage unique.
* **Les options de sécurité du serveur :** la centrale applique les options que le serveur garde pour elle (`armor/alarm/<nœud>/policy`) et chacune peut être fixée sur le nœud lui-même : l'état armé enregistré est lié aux zones et au code PIN, les messages de zone Zigbee du serveur peuvent être coupés et, s'ils sont signés, ils sont vérifiés (signature avec une clé saisie dans le panneau et un numéro de séquence qui ne fait qu'augmenter). Testé sur un ordinateur avec les valeurs que calculent le contrat et le serveur ; non exécuté sur une carte.
* **Scellement facultatif des secrets enregistrés :** désactivé par défaut (`security.seal_secrets`, ou `nodes.nvs_encryption` du serveur). Activé, les mots de passe, jetons, clés, codes PIN et empreintes de mots de passe que le nœud enregistre sont scellés avec ChaCha20-Poly1305 et tout le reste reste lisible ; l'activer ou le désactiver les réécrit sans rien perdre, même si le nœud s'arrête en route. Aucun eFuse n'est brûlé et la flash n'est pas chiffrée : la clé est dans une partition de la même puce, donc cela ne protège **pas** contre quelqu'un qui tient la carte, et un nœud installé avant cette partition garde ses secrets tels quels jusqu'à ce qu'il soit flashé avec la nouvelle table de partitions.

## 🧩 Rôle dans A.R.M.O.R.

ARMOR-NODE-ALARM est la centrale d'alarme : les zones, l'armement, les temporisations, la sirène et le PIN se décident sur le nœud lui-même, si bien que la maison reste surveillée quand le serveur est absent. [ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) la montre, lie des capteurs Zigbee à ses zones et, quand vous l'autorisez, l'arme et la désarme ; le menu *Centrales d'alarme* et le *Concepteur d'alarme* d'[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) et l'application [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) la pilotent. Sa radio Zigbee peut être sur la carte ou un coordinateur sur le réseau.

| Nom | Comment | À quoi ça sert |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | Reçoit l'état de la centrale ; commande l'armement, le désarmement et le silence seulement derrière trois interrupteurs |
| ARMOR-STUDIO | menu des centrales d'alarme | Montre les zones, les événements et la politique, et ouvre le panneau du nœud |
| ARMOR-ANDROID-CONTROL | Bluetooth | Configure le nœud sur le terrain et montre les centrales d'alarme |
| ARMOR-COMMON | contrat d'alarme | Les messages sont vérifiés avec ses schémas ; le firmware commun en est copié |

## 🏛️ Architecture

<p align="center">
  <img src="images/architecture.svg" alt="Architecture de ARMOR-NODE-ALARM : qui l'appelle, ce qu'il contient et de quoi il dépend" width="100%">
</p>

## 🖼️ Captures d'écran

Capturées en 1920×1080 sur un banc d'essai, avec des données d'exemple et sans installation réelle derrière.

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="Résumé : l'état du nœud en un coup d'œil - réseau, broker, firmware et ce qu'il remonte." width="100%"><br><sub>Résumé : l'état du nœud en un coup d'œil - réseau, broker, firmware et ce qu'il remonte.</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="Zones : chaque zone filaire ou Zigbee de la centrale, son type, sa durée de maintien et son état en direct." width="100%"><br><sub>Zones : chaque zone filaire ou Zigbee de la centrale, son type, sa durée de maintien et son état en direct.</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Radio Zigbee : le pont série vers le module coordinateur, quel client peut l'utiliser et l'état du pont." width="100%"><br><sub>Radio Zigbee : le pont série vers le module coordinateur, quel client peut l'utiliser et l'état du pont.</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="Relais et sirène : les sorties de la carte de base et ce qui pilote chacune." width="100%"><br><sub>Relais et sirène : les sorties de la carte de base et ce qui pilote chacune.</sub></td></tr>
</table>

## 🔒 Sécurité et protection

| Menace | Défense |
|---|---|
| Mots de passe en clair ou compilés | PBKDF2-HMAC-SHA256 avec 10 000 tours ; aucun mot de passe dans le firmware (un nœud sans utilisateur passe en configuration) ; au plus 4 utilisateurs (admin ou lecteur) et de 8 à 64 caractères ; au plus 6 sessions, 30 minutes d'inactivité |
| Prendre un nœud tout juste flashé | La configuration exige un code de démarrage : celui du build (`CONFIG_ARMOR_SETUP_CODE`) ou un code aléatoire gardé en flash et affiché sur la console USB ; avec un secret de flotte c'est `HMAC-SHA256(secret, MAC)` ; c'est aussi la clé du Wi-Fi de configuration (WPA2, jamais ponté) |
| Le panneau atteint depuis Internet | Il répond par défaut aux réseaux privés d'une maison (10/8, 172.16/12, 192.168/16, 169.254/16, 127/8 et 100.64/10 où vivent certains VPN) ; le réglage `web.admin_sources` liste d'autres adresses et réseaux IPv4 (8 entrées, 120 caractères) ; tout le reste est refusé sauf l'identité publique `GET /api/v1/session` ; une liste qui exclurait celui qui l'enregistre est rejetée |
| Le panneau en clair en HTTP | HTTPS sur le 443 avec un certificat que le nœud fabrique lui-même (autosigné, en flash, empreinte SHA-256 visible pour comparer) ; le panneau peut être http, https ou les deux ; le cookie de session est toujours HttpOnly et SameSite=Strict (et Secure en TLS) |
| Un faux firmware depuis GitHub | Une image n'est installée qu'avec un SHA-256 concordant et une signature Ed25519 valide faite avec la clé du mainteneur (`no_checksum`, `no_signature`, `bad_signature`, `checksum_mismatch`, `wrong_firmware`, `not_newer`, `version_mismatch` la refusent) ; un envoi manuel par un administrateur n'est pas vérifié : une limite explicite |
| Détournement de la configuration Bluetooth | Un PIN à 6 chiffres dérivé du code de configuration, visible sur la console USB et pour un administrateur ; le téléphone doit le saisir avant d'écrire des réglages ; la fenêtre dure `CONFIG_ARMOR_BLE_SETUP_MINUTES` (10 par défaut, de 1 à 120) |
| Secrets stockés lus par qui a la carte | Optionnel (désactivé par défaut, interrupteur `security.seal_secrets` ou `nodes.nvs_encryption` du serveur) : ChaCha20-Poly1305 pour mots de passe, jetons, clés, PIN et hachages. Il ne chiffre pas la flash et ne grille aucun eFuse : la clé est dans une partition de la même puce, il ne protège donc pas contre qui tient la carte |
| Le lien avec le broker en clair | TLS optionnel (`mqtts://`) avec l'autorité propre du propriétaire (PEM jusqu'à 2 Ko) et vérification du nom ; désactivé par défaut. Le handshake n'a tourné qu'en tests sur hôte, pas sur une carte |
| Deviner le PIN de la centrale | La comparaison ne révèle pas où elle diffère ; des échecs consécutifs bloquent la centrale `lockout_s` (60 s par défaut, de 1 à 3600) fois 1, 5 et 15 pour les trois premiers blocages, même pour le bon PIN ; un blocage en cours est restauré après un redémarrage ; armer ne demande pas de PIN, désarmer et faire taire en demandent un |
| Un redémarrage qui désarme la maison | Le mode, l'alarme en cours et les zones exclues sont gardés en flash et récupérés ; une boucle coupée ou en court-circuit n'est jamais laissée de côté ; fumée, panique et sabotage déclenchés pendant le désarmement reviennent après un redémarrage ; la réinitialisation par BOOT n'efface que centrale DÉSARMÉE |
| Une zone illisible semble fermée | Elle montre `trouble` (le convertisseur ne démarre pas ou 5 tours sans lecture) et la centrale n'arme pas avec elle sauf forçage, auquel cas elle est exclue et notée ; un détecteur de fumée ou de panique qui sonne n'est jamais exclu |
| Un ordre du serveur que personne n'a autorisé | Armer, désarmer et faire taire depuis le serveur exigent TROIS interrupteurs (le réglage du nœud, la liste d'accès du broker et `ARMOR_ALARM_COMMANDS=1` sur le serveur), le serveur demande le PIN de l'utilisateur avant d'envoyer un désarmement ou un silence, et tout est désactivé par défaut ; le relais de la sirène appartient à l'alarme seule (ni le broker, ni la centrale, ni le panneau ne peuvent le bouger) |
| Faux messages Zigbee | Le topic `armor/alarm/<id>/zigbee` porte un HMAC-SHA256 sur nœud, zone, état, batterie et séquence avec la clé de la centrale (64 hex saisis dans le panneau), et la séquence ne fait que monter ; le topic de politique porte `config_hash`, `zone_feed` et sa signature, chacun pouvant être fixé sur le nœud |

Ce n'est pas un système d'alarme certifié ([docs/SAFETY.md](docs/SAFETY.md)) : c'est un projet domestique qui n'a jamais tourné sur une carte ni dans une vraie installation. Si le serveur est absent, la centrale décide seule, et si le nœud ou sa radio tombe aussi, personne ne surveille. Lisez [SECURITY.md](SECURITY.md) pour signaler une vulnérabilité.

## 🔌 Interfaces

| Partie | Ce qu'elle règle |
|---|---|
| MQTT | `armor/alarm/<id>/state` (non retenu : zones avec id, nom, type, état et exclusion, la phase), `/command`, `/result`, `/zigbee` et `/policy` (serveur vers centrale) ; `armor/device/<id>/<relais>/{state,set}`, `armor/health/<id>`, `armor/control/<id>/...` |
| Zones | Type `instant`, `entry`, `interior` ou `always` ; état `normal`, `triggered`, `tamper` ou `trouble` ; jusqu'à 8 boucles câblées (un contact avec résistance de fin de ligne lu par l'ADC donne quatre états sur une broche ; la carte Ethernet a 6 broches analogiques valides, la carte Wi-Fi 8), 4 boutons et un journal de 100 événements |
| Zigbee | Un pont série (ZNP) vers un coordinateur, des capteurs IAS Zone et des sirènes IAS WD ; le nœud prend la radio quand le Zigbee2MQTT du serveur se tait pendant `zigbee_silence_min` et la rend au retour du serveur ([docs/ZIGBEE.md](docs/ZIGBEE.md), [docs/FAILOVER.md](docs/FAILOVER.md)) |

## ⚙️ Configuration

| Variable | Par défaut | Ce qu'elle règle |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | Les retards de sortie et d'entrée ; la sortie ne traite pas celui qui part en intrus |
| `panel.lockout_s` | 60 (1-3600) | Le blocage du PIN, fois 1, 5 et 15 pour les trois premiers |
| `zigbee_silence_min` | - | Combien de temps le Zigbee du serveur peut se taire avant que le nœud prenne la radio |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | Les réglages que tous les nœuds partagent : le nom (vide donne `armor-` et les derniers octets de la MAC), le code de configuration (vide : aléatoire), le secret de flotte (16+ caractères, jamais dans le dépôt), les premiers réglages du broker, le battement (de 2 à 300 s), les minutes de configuration Bluetooth (de 1 à 120) et la réinitialisation avec BOOT (8 s dans les 30 premières s) |

Les nouvelles sonneries après un silence sont limitées à 3 (`kMaxResounds`). Le relais de la sirène démarre ouvert à chaque redémarrage et ne retombe pas ouvert si le broker est perdu.

## 🚀 Démarrage rapide

**Prérequis:** un compilateur C++17 pour les tests sur hôte ; le conteneur ESP-IDF 5.5.5 pour construire l'image ; une carte ESP32-S3 (Ethernet ou Wi-Fi) avec la carte de base.

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

Voir [la conception](docs/DESIGN.md), [comment la centrale prend le relais du serveur](docs/FAILOVER.md) et les [notes de sécurité](docs/SAFETY.md).

## 🧪 Tests

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

18 programmes de test sur hôte et 5 contrôles Python tournent sous `ctest` ; 488930 vérifications comptées par les programmes hôte, compilés avec `-Werror`.

Contrôles Python : les textes du panneau en huit langues, les tâches, les chemins de sauvegarde, la session Bluetooth sous son verrou et les routes appelées par le panneau (et, dans certains nœuds, les codes HTTP ou les glyphes des polices)

Les tests sur hôte demandent un compilateur C++17 quelconque (Linux, WSL, MSYS2) ; le panneau web est aussi exercé dans un vrai navigateur contre un nœud de substitution (`tools/panel_mock.mjs`, `tools/panel_browser_test.mjs`).

La plupart des vérifications sont des balayages exhaustifs des règles de la centrale ; les boucles, la sirène et la liaison Zigbee sont des substituts.

## 🏗️ Compilation et publication

`tools/build_node.sh` construit l'image dans le conteneur ESP-IDF 5.5.5 et la version porte l'image, son SHA-256 et sa signature Ed25519, une par carte ; l'image est écrite par USB-C la première fois, puis mise à jour depuis le panneau.

## 🩺 Dépannage

| Vous voyez | Parce que | Faites |
|---|---|---|
| Vous avez perdu le mot de passe ou le nœud ne rejoint plus le réseau | Un nœud en configuration sans adresse après 90 s ouvre son propre Wi-Fi `ARMOR-SETUP-xxxxxx` (ouvrez `http://192.168.4.1/`) | Pour recommencer, maintenez BOOT 8 secondes dans les 30 premières secondes après la mise sous tension (efface réglages et utilisateurs), ou utilisez *Réinitialisation d'usine* dans le panneau |
| Une mise à jour depuis GitHub est refusée | La version n'a ni somme de contrôle ni signature, elles ne concordent pas (`e_no_checksum`, `e_no_signature`, `e_checksum_mismatch`, `e_bad_signature`), n'a pas d'image pour cette carte (`e_no_asset_for_board`) ou est d'un autre projet (`e_wrong_firmware`) | Le nœud n'a pas changé ; réessayez ou laissez un administrateur envoyer le fichier à la main |
| La centrale n'arme pas | Une zone est en `trouble` (le convertisseur ne démarre pas ou 5 tours sans lecture) | Réparez le câblage, ou forcez l'armement (la zone est alors exclue et notée) |
| Le PIN est refusé même quand il est bon | La centrale est bloquée après des échecs (même le bon PIN est refusé) | Attendez `lockout_s` ; le blocage survit à un redémarrage |
| La réinitialisation par BOOT ne fait rien | La centrale est armée : la réinitialisation n'efface qu'une centrale désarmée | Désarmez d'abord avec le PIN |

## 📂 Structure du dépôt

<p align="center">
  <img src="images/repository-structure.svg" alt="Structure du dépôt de ARMOR-NODE-ALARM" width="100%">
</p>

<details>
<summary>Version texte de l'arborescence</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                le firmware : un composant ESP-IDF
├── core/                                la logique sans ESP-IDF, testée sur ordinateur
├── panel/                               le panneau web intégré au firmware
├── tests/                               tests sur hôte et contrôles Python
├── docs/                                11 guides : sécurité, messages, Zigbee, relève
├── tools/                               outils de build, d'empaquetage et de contrôle
├── .github/                             workflow CI et modèles d'issues et de pull requests
├── images/                              bannière, icône et les deux images de ce README
├── build.sh build.bat                   build par le flux de projet partagé
├── test.sh test.bat                     lancer les tests par le flux partagé
├── run.sh run.bat                       démarrer par le flux de projet partagé
├── armor.project.json                   manifeste : version, rôle, maturité, commande de build
├── CHANGELOG.md                         ce qui a changé à chaque version
├── SECURITY.md SUPPORT.md LICENSE       politique de sécurité, support et licence
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   comment contribuer et le code de conduite
├── CMakeLists.txt                       le projet ESP-IDF
├── partitions.csv                       la table de la flash
├── sdkconfig.defaults                   les réglages du firmware
├── sdkconfig.board.s3-eth               les réglages d'une carte
├── sdkconfig.board.s3-wifi              les réglages d'une carte
├── secrets/                             exemple du fichier de nœud (les vrais ne sont pas versionnés)
├── build-test.sh build-test.bat         lancer les tests par le flux partagé
└── .gitattributes .gitignore            réglages Git
```

</details>

## 🗺️ Feuille de route et limites connues

**Limites connues aujourd'hui**

* La conversation Zigbee du nœud lui-même avec un coordinateur, local ou sur le réseau, n'est testée que contre un coordinateur simulé ; elle n'a jamais rencontré un vrai coordinateur ni un vrai capteur.
* Le panneau web a été essayé dans un vrai navigateur contre un nœud simulé, pas sur une carte.
* Aucune carte construite, aucune zone câblée et ni sirène ni coordinateur Zigbee connectés : rien n'a tourné sur une installation réelle.
* Le PIN avec verrouillage, le code de configuration, la limite de connexions et les mises à jour signées sont testés sur l'hôte ; il n'y a pas eu de revue indépendante et ce n'est pas un système d'alarme certifié.
* Les contrôles de hash et de signature Ed25519 et la logique de retour arrière réussissent sur l'hôte ; une mise à jour par voie hertzienne n'a jamais été exécutée sur une carte.
* La sirène et les relais sont pilotés par les règles de l'alarme et testés contre des simulations ; aucun n'a été relié à une vraie sirène ou à un vrai relais.

**Ensuite**

* Câbler de vraies boucles et une sirène, essayer la liaison Zigbee avec un vrai coordinateur et de vrais capteurs, et faire tourner la centrale dans une vraie installation avant d'affirmer quoi que ce soit de plus.

## 🌍 Langues

Le README et les textes existent en anglais, espagnol, français, italien, allemand, chinois, japonais et portugais européen.

## 🔗 Projets liés

**A.R.M.O.R.** (Autonomous Radar & Multimodal Observation Range) est un système de sécurité périmétrique composé de dépôts indépendants. Chacun a sa propre version, ses propres tests et son propre README ; voici la famille :

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - Contrats de messages, validateurs, vecteurs de conformité et types générés
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - Firmware du nœud de terrain pour ESP32-S3 avec trois radars et son propre panneau web
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - Protocoles des onduleurs et batteries solaires et messages d'un nœud passerelle
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - Nœud électrique : compteurs, le message des mesures du réseau et les règles de commutation
* **ARMOR-NODE-ALARM** (ce dépôt) - Nœud et centrale d'alarme : zones, armement, temporisations, sirène et PIN, avec ou sans le serveur
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - Panneau tactile : l'état du système sur un écran mural, armer et acquitter, et la maison de l'assistant vocal
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - Le réseau local : ses appareils, internet et ce qui change
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - Coordinateur central : télémétrie, alarmes, appareils, relevés solaires et caméras
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - Console web : caméras, radar, alarmes, énergie solaire et concepteur de site 2D/3D
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - Client Android de l'opérateur avec radar 2D/3D en direct
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - Client opérateur iPhone
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - Politique d'inférence visuelle qui explique ses décisions et n'agit jamais
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - Zones de caméra, masques et règles de détection pour la Jetson Orin NX
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - Intentions vocales hors ligne avec une confirmation impossible à falsifier
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - Boîtiers, électronique et matrice d'acceptation sur banc
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - Déploiement, banc d'essai CM5, sauvegarde et TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - Simulateur de télémétrie hors ligne avec des pannes reproductibles
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - Détecte, installe et met à jour les propres dépôts de l'écosystème
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - Architecture, base de sécurité et matrice des capacités

## 📚 Documentation

* [Sécurité](docs/SAFETY.md) : ce que la centrale promet et ce qu'elle ne promet pas
* [Messages d'alarme](docs/ALARM_MESSAGES.md), [Zigbee](docs/ZIGBEE.md) et [relève](docs/FAILOVER.md)
* [Conception](docs/DESIGN.md), [matériel](docs/HARDWARE.md), [carte de base](docs/BASE_BOARD.md) et [firmware du nœud](docs/NODE_FIRMWARE.md)
* [Essais de banc](docs/BENCH_TESTS.md), [code de configuration](docs/SETUP_CODE.md) et [mise en service Bluetooth](docs/BLE_PROVISIONING.md)
* [Matrice des capacités : ce qui est prouvé et ce qui ne l'est pas](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [Catalogue des projets : versions et dépendances entre les dépôts](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [Historique des modifications de ce dépôt](CHANGELOG.md)

## 🤝 Contribuer et communauté

* Avant une pull request, lancez `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py` : il doit réussir. Voir [CONTRIBUTING.md](CONTRIBUTING.md) et le [code de conduite](CODE_OF_CONDUCT.md).
* Le README existe en huit langues et est généré à partir des sources d'ARMOR-DOCS : une modification du texte se fait là-bas.
* Une modification d'un message entre composants commence dans ARMOR-COMMON.
* Ne décrivez pas comme fonctionnant sur une carte ce qui a seulement été compilé.
* Signalez une vulnérabilité en privé : [SECURITY.md](SECURITY.md). Questions, idées et rapports : electrohobby3d@gmail.com ([SUPPORT.md](SUPPORT.md)).

## 👤 AUTEUR

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 LICENCE

GPL-3.0-or-later - voir [LICENSE](LICENSE).
