<p align="center">
  <img src="images/ARMOR_BANNER.svg" alt="Banner de ARMOR-NODE-ALARM" width="100%">
</p>

# 🚨 ARMOR-NODE-ALARM

<p align="center">
  <a href="README.md">🇺🇸 English</a> |
  🇪🇸 <b>Español</b> |
  <a href="README_fra.md">🇫🇷 Français</a> |
  <a href="README_ita.md">🇮🇹 Italiano</a> |
  <a href="README_deu.md">🇩🇪 Deutsch</a> |
  <a href="README_zho.md">🇨🇳 简体中文</a> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  <a href="README_por.md">🇵🇹 Português</a>
</p>

### Nodo y central de alarma: las zonas, el armado, los retardos, la sirena y el PIN de la alarma de la casa, con el firmware del nodo y las reglas probadas en un ordenador (nada ha corrido en una placa)

<p align="center">
  <img src="https://img.shields.io/badge/License-GPL%203.0--or--later-blue.svg" alt="GPL 3.0 o posterior">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599c.svg" alt="Lenguaje">
  <img src="https://img.shields.io/badge/Zones-wired%20%2B%20Zigbee-ffb020.svg" alt="Zones">
  <img src="https://img.shields.io/badge/Checks-482%2C535-2ea44f.svg" alt="Checks">
  <img src="https://img.shields.io/badge/Tests-488930%20checks-2ea44f.svg" alt="Pruebas">
  <img src="https://img.shields.io/badge/Version-0.4.8-0f6cbd.svg" alt="Versión">
  <img src="https://img.shields.io/badge/Maturity-functional-00E5FF.svg" alt="Madurez">
</p>

---

**Comprobación de honestidad - qué funciona hoy:** Lo que existe es el **núcleo de la alarma** (qué es una zona, cómo un bucle cableado leído por el ADC se convierte en un estado, las reglas de la central: armado, retardos, la alarma, la sirena, el PIN con bloqueo) y el **firmware del nodo** construido sobre él (ajustes, panel web, mensajes con el servidor, el puente de la radio Zigbee, un coordinador Zigbee en la red, relés, Bluetooth, actualizaciones firmadas), que compila para las dos placas; el núcleo, los ajustes, los mensajes y el panel en un navegador real contra un nodo de pega se prueban en un ordenador con 488.930 comprobaciones en 18 programas de test. **Nada ha corrido en una placa ni en una instalación real, no se ha cableado ninguna zona ni conectado ninguna sirena, la conversación Zigbee del propio nodo con un coordinador (local o en la red) está escrita y probada contra un coordinador de pega pero nunca ha corrido contra un coordinador ni un sensor reales, y esto no es un sistema de alarma certificado.**

---

## 📋 De un vistazo

| | |
|---|---|
| Versión | `0.4.8` |
| Madurez | Funcional |
| Función | Nodo de campo central de alarma para ESP32-S3 (familia: Field Nodes, padre: ARMOR-COMMON) |
| Tecnología | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| Pruebas | 488930 comprobaciones en 18 programas de host y 5 de Python (`ctest`) |
| Entradas | Hasta 8 lazos cableados, sensores Zigbee (IAS Zone) y sirena (IAS WD), 4 botones |
| Funciona en | Una placa ESP32-S3 (Ethernet o Wi-Fi) con la placa base; pruebas de host en cualquier ordenador |
| En hardware real | Ninguno todavía: ninguna placa, zona, sirena ni coordinador Zigbee conectados (solo tests en ordenador, la compilación y el panel en un navegador) |
| Licencia | GPL-3.0-or-later |

**Ir a:** [Resumen](#-resumen) · [Arquitectura](#-arquitectura) · [Seguridad](#-seguridad-y-protección) · [Interfaces](#-interfaces) · [Configuración](#-configuración) · [Inicio rápido](#-inicio-rápido) · [Pruebas](#-pruebas) · [Problemas](#-resolución-de-problemas) · [Estructura](#-estructura-del-repositorio) · [Relacionados](#-proyectos-relacionados)

## 🚦 Estado del proyecto

Cada línea es una parte de este repositorio y la luz dice hasta dónde llega hoy. Está escrito con lo que el repositorio puede demostrar (pruebas, compilaciones, ejecuciones), no con lo que está planeado.

🟢 Hecho y verificado · 🟡 Funciona, con límites conocidos · 🟠 Parcial o sin verificar todavía · 🔴 No hecho o no funciona

| Área | Estado | Qué significa aquí |
|---|---|---|
| Software | 🟢 | El firmware compila en el contenedor ESP-IDF 5.5.5 para las dos placas (Wi-Fi y Ethernet). |
| Pruebas | 🟢 | Pasan 488.930 comprobaciones en 18 programas de host (`ctest`): las zonas, los lazos cableados leídos por el ADC, las reglas de armado, retardos, sirena y PIN, los ajustes y los mensajes. |
| Protocolos y datos | 🟠 | La conversación Zigbee del propio nodo con un coordinador, local o en la red, solo se ha probado contra un coordinador simulado; nunca se ha encontrado con un coordinador o un sensor reales. |
| Interfaz | 🟡 | El panel web se ha ejercitado en un navegador real contra un nodo simulado, no en una placa. |
| Hardware | 🔴 | Ninguna placa construida, ninguna zona cableada y ninguna sirena o coordinador Zigbee conectados: nada ha funcionado en una instalación real. |
| Seguridad | 🟡 | El PIN con bloqueo, el código de configuración, el límite de accesos y las actualizaciones firmadas están probados en el host; no ha habido una revisión independiente y esto no es un sistema de alarma certificado. |
| Actualizaciones | 🟠 | Las comprobaciones de hash y de firma Ed25519 y la lógica de vuelta atrás pasan en el host; una actualización por aire nunca se ha ejecutado en una placa. |
| Escritura en los equipos | 🟠 | La sirena y los relés los gobiernan las reglas de la alarma y se han probado contra simulaciones; ninguno se ha conectado a una sirena o un relé reales. |
| Integración | 🟢 | Los mensajes del nodo se validan contra los esquemas compartidos y los vectores de conformidad de ARMOR-COMMON. |
| Documentación | 🟢 | Ocho idiomas, la guía del firmware, las notas de cableado y el historial de cambios están al día. |

## 🎯 Resumen

* **Zonas:** cuatro tipos - *instantánea* (una puerta, una ventana), *de entrada* (la puerta por la que entras: inicia el retardo de entrada), *interior* (movimiento: solo en "fuera") y *siempre* (humo, gas, pánico: armada o no). Una zona puede ser un bucle cableado o un sensor Zigbee.
* **El bucle cableado:** un contacto con resistencia de fin de línea, leído como tensión, da cuatro estados en un solo pin - normal, disparada y sabotaje por un cable en cortocircuito o cortado. Los límites son ajustes; un filtro antirrebote evita que un contacto que rebota dispare la alarma.
* **La central:** armado *fuera* y *en casa*, un retardo de salida que no trata como intruso a quien se va, un retardo de entrada, una alarma que **se mantiene** hasta un desarmado correcto, una sirena que se para sola pasado un tiempo, anulación de zonas abiertas si se fuerza (nunca un detector de humo sonando) y un registro de los últimos cien eventos.
* **El PIN:** armar no lo pide, desarmar y silenciar sí; los PIN erróneos se cuentan y, tras unos pocos, se rechaza todo durante un tiempo, incluso el PIN correcto.
* **Silenciar sin desarmar:** mientras suena la sirena, un PIN correcto la para al instante con el sistema aún armado: la alarma sigue enclavada y registrada, una zona que aún no ha sonado puede volver a hacerla sonar, y un PIN erróneo cuenta para el bloqueo como uno erróneo de desarmar.
* **El plan:** una pasarela en la placa del nodo eléctrico (radio Zigbee, zonas cableadas, relé de sirena) que trabaja con ARMOR-SERVER y **sigue vigilando cuando el servidor falta**: una central toma la radio al servidor por el puente Zigbee de ARMOR-NODE-ELECTRICAL. Véanse [el diseño](docs/DESIGN.md) y [el relevo](docs/FAILOVER.md).
* **El nodo:** firmware para el ESP32-S3 (Ethernet PoE o Wi-Fi) con hasta ocho zonas cableadas, la sirena en un relé, el panel web de todos los nodos en ocho idiomas, la radio Zigbee prestada a Zigbee2MQTT y los mensajes `armor/alarm/<nodo>/state|command|result`; el servidor solo puede armar y desarmar si tres interruptores independientes lo permiten. Véanse [el firmware](docs/NODE_FIRMWARE.md), [los mensajes](docs/ALARM_MESSAGES.md) y [el hardware](docs/HARDWARE.md).
* **Tomar la radio:** cuando Zigbee2MQTT del servidor lleva un tiempo ausente, el nodo levanta el coordinador por sí mismo (ZNP), lee los sensores de las zonas que nombran uno (IAS Zone) y manda a sirenas Zigbee (IAS WD), y lo suelta en cuanto vuelve el cliente.
* **Todavía no:** un coordinador o sensor reales, el hardware y cualquier prueba en una instalación real.
* **Un reinicio no desarma la casa:** el modo, una alarma en marcha y las zonas excluidas se guardan en la flash y se recuperan al arrancar; un lazo cortado o en cortocircuito nunca se excluye; los mensajes pasan por una cola (sin bloqueo mutuo con el broker) y el perro guardián reinicia la placa si se para la tarea que lee las zonas.
* **Actualizaciones por aire firmadas:** el nodo solo instala desde GitHub una imagen cuyo SHA-256 coincide y que lleva una firma Ed25519 válida del responsable (la clave pública está en el firmware, la privada nunca sale del responsable); un archivo subido a mano por un administrador en el panel se sigue aceptando. Las páginas y mensajes del panel están en ocho idiomas.
* **Emparejamiento Bluetooth con PIN:** el PIN de seis cifras se deriva del código de configuración, se imprime en la consola USB y se muestra a un administrador en el panel; el móvil tiene que escribirlo antes de poder cambiar los ajustes del nodo.
* **Sirena y reinicios:** el relé de la sirena es solo de la alarma (ni el broker, ni la central, ni el panel pueden moverlo, y no se abre al perder el broker); una zona que aún no había sonado vuelve a hacer sonar la sirena pasado su tiempo; tras un reinicio vuelven una alarma de humo o pánico con el sistema desarmado, una alarma silenciada y un bloqueo; un reset a mano se rechaza con la alarma armada; un sensor Zigbee que nunca habla cuenta como perdido.
* **Solo la casa puede usar el panel:** el panel responde por defecto a la red privada (o a las redes que su dueño indique, como una VPN), rechaza una lista que dejaría fuera a quien la guarda, cuenta las contraseñas erróneas por persona y por dirección, y funciona detrás del proxy del servidor para llegar a él desde fuera de casa sin publicar el nodo.
* **Una zona que no se puede leer no se muestra como cerrada:** si el conversor no arranca o un lazo no da lectura cinco rondas seguidas, la zona pasa a *avería* (trouble): no vigila, nunca hace sonar la sirena, impide armar (salvo forzando, y entonces queda excluida y anotada) y el servidor avisa.
* **Enlace cifrado opcional con el broker (TLS):** desactivado por defecto, así que el nodo habla MQTT sin cifrar como antes. Un interruptor y una caja para el certificado de la autoridad propia de la instalación (PEM, 2 KB como máximo) en la página Broker hacen que se conecte con `mqtts://`, confíe solo en esa autoridad y compruebe que el certificado del broker se hizo para la dirección a la que llegó; lo que no sea un único certificado PEM se rechaza. Cubierto con pruebas en el ordenador; el handshake aún no se ha ejecutado en una placa.
* **Perfiles de armado:** un comando de armar puede llevar un perfil (Fuera, En casa, Noche, el tuyo) que elige, para ese armado, qué zonas vigilan, cuáles disparan la alarma al instante o tras el retardo de entrada, los retardos de salida y entrada y si suena la sirena (una alarma silenciosa se queda enganchada, anotada y avisada). Nunca toca las zonas de humo, pánico o sabotaje, el PIN, el bloqueo, el silencio, un armado forzado ni una zona con problema; el perfil sobrevive a un reinicio y un armado sin perfil se comporta exactamente como antes. Probado contra las reglas antiguas con una ejecución aleatoria y contra el contrato compartido.
* **Informe de salud:** cada minuto más o menos el nodo publica cuánto lleva encendido y por qué se reinició la última vez, su memoria libre, su enlace y su señal Wi-Fi, cuántas veces se conectó al broker y el último error que anotó (`armor/health/<nodo>`, el contrato `node_health` de ARMOR-COMMON). El código que lo construye lo comparten todos los nodos y está probado en un ordenador; no se ha ejecutado en una placa.
* **Control desde el servidor:** Studio puede reiniciar el nodo limpiamente, pausarlo hasta una hora (se reanuda solo), poner su trabajo en espera (sin fin, se conserva al reiniciar, el nodo sigue conectado y respondiendo), hacer que se muestre, pedirle una instantánea de diagnóstico y marcarlo en mantenimiento (`armor/control/<nodo>/...`, el contrato `node_control` de ARMOR-COMMON). Cada orden son dos pasos con un token de un solo uso que pertenece a esa operación, solo el servidor puede escribirlas en el broker, y no existe restablecimiento de fábrica, borrado ni sueño profundo. Mientras el nodo está en pausa o en espera no se leen sus zonas, no se evalúa, arma ni hace sonar nada, no se publica estado y el relé de la sirena se mantiene abierto: la casa NO está protegida, y el informe de salud lo dice. El código es compartido por todos los nodos y se prueba en un ordenador; no ha funcionado en una placa.
* **Diagnóstico sin cable:** tras un arranque que no fue limpio (un cuelgue, un watchdog, una caída de la alimentación, una reserva de memoria fallida) el informe de salud dice el motivo durante diez minutos; un fallo deja su volcado de núcleo (core dump) para verlo, descargarlo o borrarlo desde el panel; los ajustes guardados se copian antes de cada instalación de firmware y se recuperan si una actualización los daña; y un administrador puede pedir al nodo, a través del servidor, un paquete de diagnóstico o su registro con un token de un solo uso.
* **Las opciones de seguridad del servidor:** la central aplica las opciones que el servidor guarda para ella (`armor/alarm/<nodo>/policy`) y cada una puede fijarse en el propio nodo: el estado armado guardado se ata a las zonas y al PIN, los mensajes de zona Zigbee del servidor pueden apagarse y, si van firmados, se comprueban (firma con una clave escrita en el panel y un número de secuencia que solo crece). Probado en un ordenador con los valores que calculan el contrato y el servidor; no se ha ejecutado en una placa.
* **Sellado opcional de los secretos guardados:** desactivado por defecto (`security.seal_secrets`, o `nodes.nvs_encryption` del servidor). Activado, las contraseñas, tokens, claves, PIN y hashes de contraseña que guarda el nodo quedan sellados con ChaCha20-Poly1305 y todo lo demás sigue legible; activarlo o desactivarlo los reescribe sin perder nada, aunque el nodo se pare a medias. No se quema ningún eFuse ni se cifra la flash: la clave está en una partición del mismo chip, así que **no** protege frente a quien tenga la placa, y un nodo instalado antes de esa partición conserva sus secretos como estaban hasta que se grabe con la nueva tabla de particiones.

## 🧩 Papel en A.R.M.O.R.

ARMOR-NODE-ALARM es la central de alarma: las zonas, el armado, los retardos, la sirena y el PIN se deciden en el propio nodo, así que la casa sigue vigilada cuando el servidor no está. [ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) la muestra, ata sensores Zigbee a sus zonas y, cuando lo permites, la arma y desarma; el menú *Centrales de alarma* y el *Diseñador de alarma* de [ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) y la app [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) la manejan. Su radio Zigbee puede estar en la placa o ser un coordinador en la red.

| Nombre | Cómo | Para qué |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | Recibe el estado de la central; ordena armar, desarmar y silenciar solo tras tres interruptores |
| ARMOR-STUDIO | menú de centrales de alarma | Muestra las zonas, los eventos y la política, y abre el panel del nodo |
| ARMOR-ANDROID-CONTROL | Bluetooth | Configura el nodo en el campo y muestra las centrales de alarma |
| ARMOR-COMMON | contrato de alarma | Los mensajes se comprueban con sus esquemas; el firmware común se copia de él |

## 🏛️ Arquitectura

<p align="center">
  <img src="images/architecture.svg" alt="Arquitectura de ARMOR-NODE-ALARM: quién lo llama, qué contiene y de qué depende" width="100%">
</p>

## 🖼️ Capturas de pantalla

Capturadas a 1920×1080 en un banco de pruebas, con datos de ejemplo y sin ninguna instalación real detrás.

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="Resumen: el estado del nodo de un vistazo - red, broker, firmware y lo que reporta." width="100%"><br><sub>Resumen: el estado del nodo de un vistazo - red, broker, firmware y lo que reporta.</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="Zonas: cada zona cableada o Zigbee del panel, su tipo, tiempo de retención y estado en vivo." width="100%"><br><sub>Zonas: cada zona cableada o Zigbee del panel, su tipo, tiempo de retención y estado en vivo.</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Radio Zigbee: el puente serie hacia el módulo coordinador, qué cliente puede usarlo y el estado del puente." width="100%"><br><sub>Radio Zigbee: el puente serie hacia el módulo coordinador, qué cliente puede usarlo y el estado del puente.</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="Relés y sirena: las salidas de la placa base y qué acciona cada una." width="100%"><br><sub>Relés y sirena: las salidas de la placa base y qué acciona cada una.</sub></td></tr>
</table>

## 🔒 Seguridad y protección

| Amenaza | Defensa |
|---|---|
| Contraseñas en claro o compiladas | PBKDF2-HMAC-SHA256 con 10 000 rondas; ninguna contraseña en el firmware (un nodo sin usuario entra en puesta en marcha); máximo 4 usuarios (admin o visor) y de 8 a 64 caracteres; máximo 6 sesiones, 30 minutos de inactividad |
| Tomar un nodo recién grabado | La puesta en marcha exige un código: el de la compilación (`CONFIG_ARMOR_SETUP_CODE`) o uno aleatorio guardado en flash y mostrado en la consola USB; con secreto de flota es `HMAC-SHA256(secreto, MAC)`; también es la clave del Wi-Fi de configuración (WPA2, nunca puenteado) |
| El panel alcanzado desde internet | Responde por defecto a las redes privadas de una casa (10/8, 172.16/12, 192.168/16, 169.254/16, 127/8 y 100.64/10 donde viven algunas VPN); el ajuste `web.admin_sources` lista otras direcciones y redes IPv4 (8 entradas, 120 caracteres); lo de fuera se rechaza salvo la identidad pública `GET /api/v1/session`; se rechaza una lista que dejaría fuera a quien la guarda |
| El panel en claro por HTTP | HTTPS en el 443 con un certificado que el nodo hace él mismo (autofirmado, en flash, huella SHA-256 visible para comparar); el panel puede ser http, https o ambos; la cookie de sesión es siempre HttpOnly y SameSite=Strict (y Secure por TLS) |
| Un firmware falso desde GitHub | Una imagen solo se instala con un SHA-256 que coincide y una firma Ed25519 válida hecha con la clave del mantenedor (`no_checksum`, `no_signature`, `bad_signature`, `checksum_mismatch`, `wrong_firmware`, `not_newer`, `version_mismatch` la rechazan); una subida manual de un administrador no se comprueba: es un límite explícito |
| Secuestro de la puesta en marcha por Bluetooth | Un PIN de 6 dígitos derivado del código de puesta en marcha, visible en la consola USB y para el administrador; el móvil debe introducirlo antes de escribir ajustes; la ventana dura `CONFIG_ARMOR_BLE_SETUP_MINUTES` (10 por defecto, de 1 a 120) |
| Secretos guardados leídos por quien tiene la placa | Opcional (apagado por defecto, interruptor `security.seal_secrets` o `nodes.nvs_encryption` del servidor): ChaCha20-Poly1305 para contraseñas, tokens, claves, PIN y hashes. No cifra la flash ni quema un eFuse: la clave está en una partición del mismo chip, así que no protege frente a quien tenga la placa |
| El enlace con el broker en claro | TLS opcional (`mqtts://`) con la autoridad propia del dueño (PEM de hasta 2 KB) y comprobación del nombre; apagado por defecto. El handshake solo se ha ejecutado en pruebas de host, no en una placa |
| Adivinar el PIN de la central | La comparación no filtra dónde difiere; los fallos seguidos bloquean la central `lockout_s` (60 s por defecto, de 1 a 3600) multiplicado por 1, 5 y 15 en los tres primeros bloqueos, incluso para el PIN correcto; un bloqueo en curso se restaura tras reiniciar; armar no lleva PIN, desarmar y silenciar sí |
| Un reinicio que desarma la casa | El modo, la alarma en marcha y las zonas excluidas se guardan en flash y se recuperan; un lazo cortado o en corto nunca se deja fuera; humo, pánico y sabotaje que saltaron con el sistema desarmado vuelven tras reiniciar; el reinicio de fábrica con BOOT solo borra con la central DESARMADA |
| Una zona ilegible parece cerrada | Muestra `trouble` (el convertidor no arranca o 5 rondas sin lectura) y la central no arma con ella salvo forzado, y entonces queda excluida y anotada; un detector de humo o pánico que está sonando nunca se excluye |
| Una orden del servidor que nadie autorizó | Armar, desarmar y silenciar desde el servidor exigen TRES interruptores (el ajuste del nodo, la lista de acceso del broker y `ARMOR_ALARM_COMMANDS=1` en el servidor), el servidor pide el PIN del usuario antes de enviar un desarme o un silencio, y todo está apagado por defecto; el relé de la sirena pertenece solo a la alarma (ni el broker, ni la central ni el panel pueden moverlo) |
| Mensajes Zigbee falsos | El topic `armor/alarm/<id>/zigbee` lleva un HMAC-SHA256 sobre nodo, zona, estado, batería y secuencia con la clave de la central (64 hex tecleados en el panel), y la secuencia solo sube; el topic de política lleva `config_hash`, `zone_feed` y su firma, cada uno de los cuales se puede fijar en el nodo |

No es un sistema de alarma certificado ([docs/SAFETY.md](docs/SAFETY.md)): es un proyecto doméstico y nunca ha corrido en una placa ni en una instalación real. Si falta el servidor la central decide sola, y si además falla el nodo o su radio nadie vigila. Lee [SECURITY.md](SECURITY.md) para informar de una vulnerabilidad.

## 🔌 Interfaces

| Parte | Qué define |
|---|---|
| MQTT | `armor/alarm/<id>/state` (no retenido: zonas con id, nombre, tipo, estado y exclusión, la fase), `/command`, `/result`, `/zigbee` y `/policy` (servidor a central); `armor/device/<id>/<relé>/{state,set}`, `armor/health/<id>`, `armor/control/<id>/...` |
| Zonas | Tipo `instant`, `entry`, `interior` o `always`; estado `normal`, `triggered`, `tamper` o `trouble`; hasta 8 lazos cableados (un contacto con resistencia de fin de línea leído por el ADC da cuatro estados en un pin; la placa Ethernet tiene 6 pines analógicos válidos, la Wi-Fi 8), 4 botones y un registro de 100 eventos |
| Zigbee | Un puente serie (ZNP) a un coordinador, sensores IAS Zone y sirenas IAS WD; el nodo toma la radio si el Zigbee2MQTT del servidor calla durante `zigbee_silence_min` y la devuelve cuando vuelve el servidor ([docs/ZIGBEE.md](docs/ZIGBEE.md), [docs/FAILOVER.md](docs/FAILOVER.md)) |

## ⚙️ Configuración

| Variable | Por defecto | Qué define |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | Los retardos de salida y de entrada; la salida no trata como intruso a quien se va |
| `panel.lockout_s` | 60 (1-3600) | El bloqueo del PIN, multiplicado por 1, 5 y 15 en los tres primeros |
| `zigbee_silence_min` | - | Cuánto puede callar el Zigbee del servidor antes de que el nodo tome la radio |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | Los ajustes que comparten todos los nodos: el nombre (vacío da `armor-` y los últimos bytes de la MAC), el código de puesta en marcha (vacío: aleatorio), el secreto de flota (16+ caracteres, nunca en el repositorio), los primeros ajustes del broker, el latido (de 2 a 300 s), los minutos de puesta en marcha por Bluetooth (de 1 a 120) y el reinicio con BOOT (8 s en los primeros 30 s) |

Los resonidos tras un silencio se limitan a 3 (`kMaxResounds`). El relé de la sirena arranca abierto en cada reinicio y no cae a abierto si se pierde el broker.

## 🚀 Inicio rápido

**Requisitos:** un compilador C++17 para las pruebas de host; el contenedor ESP-IDF 5.5.5 para compilar la imagen; una placa ESP32-S3 (Ethernet o Wi-Fi) con la placa base.

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

Véanse [el diseño](docs/DESIGN.md), [cómo la central toma el relevo del servidor](docs/FAILOVER.md) y las [notas de seguridad](docs/SAFETY.md).

## 🧪 Pruebas

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

18 programas de prueba de host y 5 comprobaciones en Python se ejecutan con `ctest`; 488930 comprobaciones contadas por los programas de host, compilados con `-Werror`.

Comprobaciones Python: los textos del panel en ocho idiomas, las tareas, las rutas de guardado, la sesión Bluetooth bajo su bloqueo y las rutas que llama el panel (y, en algunos nodos, los códigos HTTP o los glifos de las fuentes)

Las pruebas de host necesitan cualquier compilador C++17 (Linux, WSL, MSYS2); el panel web también se ejercita en un navegador real contra un nodo de pruebas (`tools/panel_mock.mjs`, `tools/panel_browser_test.mjs`).

La mayoría de las comprobaciones son barridos exhaustivos de las reglas de la central; los lazos, la sirena y el enlace Zigbee son sustitutos.

## 🏗️ Compilación y publicación

`tools/build_node.sh` compila la imagen en el contenedor ESP-IDF 5.5.5 y la versión lleva la imagen, su SHA-256 y su firma Ed25519, una por placa; la imagen se graba por USB-C la primera vez y después se actualiza desde el panel.

## 🩺 Resolución de problemas

| Ves | Porque | Haz |
|---|---|---|
| Perdiste la contraseña o el nodo ya no se une a la red | Un nodo con puesta en marcha y sin dirección tras 90 s abre su propio Wi-Fi `ARMOR-SETUP-xxxxxx` (abre `http://192.168.4.1/`) | Para empezar de cero mantén BOOT 8 segundos en los primeros 30 segundos tras encender (borra ajustes y usuarios), o usa *Restablecer de fábrica* en el panel |
| Se rechaza una actualización desde GitHub | La versión no trae suma de comprobación ni firma, no coinciden (`e_no_checksum`, `e_no_signature`, `e_checksum_mismatch`, `e_bad_signature`), no trae imagen para esta placa (`e_no_asset_for_board`) o es de otro proyecto (`e_wrong_firmware`) | El nodo no cambió; reintenta o deja que un administrador suba el fichero a mano |
| La central no arma | Una zona está en `trouble` (el convertidor no arranca o 5 rondas sin lectura) | Arregla el cableado, o fuerza el armado (la zona queda entonces excluida y anotada) |
| Se rechaza el PIN aunque sea correcto | La central está bloqueada tras intentos fallidos (se rechaza incluso el PIN correcto) | Espera `lockout_s`; el bloqueo sobrevive a un reinicio |
| El reinicio con BOOT no hace nada | La central está armada: el reinicio solo borra una central desarmada | Desarma primero con el PIN |

## 📂 Estructura del repositorio

<p align="center">
  <img src="images/repository-structure.svg" alt="Estructura del repositorio de ARMOR-NODE-ALARM" width="100%">
</p>

<details>
<summary>Versión en texto del árbol</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                el firmware: un componente de ESP-IDF
├── core/                                la lógica sin ESP-IDF, probada en un ordenador
├── panel/                               el panel web embebido en el firmware
├── tests/                               pruebas de host y comprobaciones Python
├── docs/                                11 guías: seguridad, mensajes, Zigbee, relevo
├── tools/                               herramientas de compilación, empaquetado y comprobación
├── .github/                             flujo de CI y plantillas de issues y pull requests
├── images/                              banner, icono y las dos imágenes de este README
├── build.sh build.bat                   compilar con el flujo de proyecto compartido
├── test.sh test.bat                     ejecutar las pruebas con el flujo compartido
├── run.sh run.bat                       arrancar con el flujo de proyecto compartido
├── armor.project.json                   manifiesto: versión, rol, madurez, comando de build
├── CHANGELOG.md                         qué cambió en cada versión
├── SECURITY.md SUPPORT.md LICENSE       política de seguridad, soporte y licencia
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   cómo contribuir y el código de conducta
├── CMakeLists.txt                       el proyecto ESP-IDF
├── partitions.csv                       la tabla de la flash
├── sdkconfig.defaults                   los ajustes del firmware
├── sdkconfig.board.s3-eth               los ajustes de una placa
├── sdkconfig.board.s3-wifi              los ajustes de una placa
├── secrets/                             ejemplo del fichero del nodo (los reales no se versionan)
├── build-test.sh build-test.bat         ejecutar las pruebas con el flujo compartido
└── .gitattributes .gitignore            ajustes de Git
```

</details>

## 🗺️ Hoja de ruta y límites conocidos

**Límites conocidos hoy**

* La conversación Zigbee del propio nodo con un coordinador, local o en la red, solo se ha probado contra un coordinador simulado; nunca se ha encontrado con un coordinador o un sensor reales.
* El panel web se ha ejercitado en un navegador real contra un nodo simulado, no en una placa.
* Ninguna placa construida, ninguna zona cableada y ninguna sirena o coordinador Zigbee conectados: nada ha funcionado en una instalación real.
* El PIN con bloqueo, el código de configuración, el límite de accesos y las actualizaciones firmadas están probados en el host; no ha habido una revisión independiente y esto no es un sistema de alarma certificado.
* Las comprobaciones de hash y de firma Ed25519 y la lógica de vuelta atrás pasan en el host; una actualización por aire nunca se ha ejecutado en una placa.
* La sirena y los relés los gobiernan las reglas de la alarma y se han probado contra simulaciones; ninguno se ha conectado a una sirena o un relé reales.

**Lo siguiente**

* Cablear lazos y una sirena reales, probar el enlace Zigbee con un coordinador y sensores reales, y ejecutar la central en una instalación real antes de afirmar nada más.

## 🌍 Idiomas

El README y los textos existen en inglés, español, francés, italiano, alemán, chino, japonés y portugués de Portugal.

## 🔗 Proyectos relacionados

**A.R.M.O.R.** (Autonomous Radar & Multimodal Observation Range) es un sistema de seguridad perimetral hecho de repositorios independientes. Cada uno tiene su propia versión, sus propias pruebas y su propio README; esta es la familia:

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - Contratos de mensajes, validadores, vectores de conformidad y tipos generados
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - Firmware de nodo de campo para ESP32-S3 con tres radares y su propio panel web
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - Protocolos de inversores y baterías solares y los mensajes de un nodo pasarela
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - Nodo eléctrico: medidores, el mensaje de las lecturas de la red y las reglas de maniobra
* **ARMOR-NODE-ALARM** (este repositorio) - Nodo y panel de alarma: zonas, armado, retardos, sirena y PIN, con el servidor o sin él
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - Panel táctil: el estado del sistema en una pantalla de pared, armar y reconocer, y el hogar del asistente de voz
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - La red local: sus dispositivos, internet y lo que cambia
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - Coordinador central: telemetría, alarmas, dispositivos, lecturas solares y cámaras
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - Consola web: cámaras, radar, alarmas, energía solar y el diseñador 2D/3D del emplazamiento
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - Cliente de operador Android con radar 2D/3D en vivo
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - Cliente de operador para iPhone
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - Política de inferencia visual que explica sus decisiones y nunca actúa
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - Zonas de cámara, máscaras y reglas de detección para la Jetson Orin NX
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - Intenciones de voz sin conexión con una confirmación que no se puede falsificar
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - Cajas, electrónica y la matriz de aceptación en banco
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - Despliegue, el banco de pruebas de la CM5, copias de seguridad y TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - Simulador de telemetría sin conexión con fallos repetibles
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - Detecta, instala y actualiza los repositorios del propio ecosistema
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - Arquitectura, base de seguridad y la matriz de capacidades

## 📚 Documentación

* [Seguridad](docs/SAFETY.md): lo que promete la central y lo que no
* [Mensajes de alarma](docs/ALARM_MESSAGES.md), [Zigbee](docs/ZIGBEE.md) y [relevo](docs/FAILOVER.md)
* [Diseño](docs/DESIGN.md), [hardware](docs/HARDWARE.md), [placa base](docs/BASE_BOARD.md) y [firmware del nodo](docs/NODE_FIRMWARE.md)
* [Pruebas de banco](docs/BENCH_TESTS.md), [código de puesta en marcha](docs/SETUP_CODE.md) y [alta por Bluetooth](docs/BLE_PROVISIONING.md)
* [Matriz de capacidades: qué está demostrado y qué no](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [Catálogo de proyectos: versiones y cómo dependen unos repositorios de otros](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [Registro de cambios de este repositorio](CHANGELOG.md)

## 🤝 Contribuir y comunidad

* Antes de un pull request ejecuta `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py`: debe pasar. Consulta [CONTRIBUTING.md](CONTRIBUTING.md) y el [código de conducta](CODE_OF_CONDUCT.md).
* El README existe en ocho idiomas y se genera desde las fuentes de ARMOR-DOCS: un cambio de texto se hace allí.
* Un cambio en un mensaje entre componentes empieza en ARMOR-COMMON.
* No describas como funcionando en una placa lo que solo se ha compilado.
* Informa de una vulnerabilidad en privado: [SECURITY.md](SECURITY.md). Preguntas, ideas e informes: electrohobby3d@gmail.com ([SUPPORT.md](SUPPORT.md)).

## 👤 AUTOR

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 LICENCIA

GPL-3.0-or-later - véase [LICENSE](LICENSE).
