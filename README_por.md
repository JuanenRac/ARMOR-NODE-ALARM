<p align="center">
  <img src="images/ARMOR_BANNER.svg" alt="ARMOR-NODE-ALARM banner" width="100%">
</p>

# 🚨 ARMOR-NODE-ALARM

<p align="center">
  <a href="README.md">🇺🇸 English</a> |
  <a href="README_spa.md">🇪🇸 Español</a> |
  <a href="README_fra.md">🇫🇷 Français</a> |
  <a href="README_ita.md">🇮🇹 Italiano</a> |
  <a href="README_deu.md">🇩🇪 Deutsch</a> |
  <a href="README_zho.md">🇨🇳 简体中文</a> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  🇵🇹 <b>Português</b>
</p>

### Nó e central de alarme: as zonas, a armação, os atrasos, a sirene e o PIN do alarme da casa, com o firmware do nó e as regras testadas num computador (nada correu numa placa)

<p align="center">
  <img src="https://img.shields.io/badge/License-GPL%203.0--or--later-blue.svg" alt="GPL 3.0 or later">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-00599c.svg" alt="Language">
  <img src="https://img.shields.io/badge/Zones-wired%20%2B%20Zigbee-ffb020.svg" alt="Zones">
  <img src="https://img.shields.io/badge/Checks-482%2C535-2ea44f.svg" alt="Checks">
  <img src="https://img.shields.io/badge/Tests-488930%20checks-2ea44f.svg" alt="Tests">
  <img src="https://img.shields.io/badge/Version-0.4.8-0f6cbd.svg" alt="Version">
  <img src="https://img.shields.io/badge/Maturity-functional-00E5FF.svg" alt="Maturity">
</p>

---

**Verificação de honestidade - o que funciona hoje:** O que existe é o **núcleo do alarme** (o que é uma zona, como um laço com fios lido pelo ADC se torna um estado, as regras da central: armar, atrasos, o alarme, a sirene, o PIN com bloqueio) e o **firmware do nó** construído sobre ele (definições, painel web, mensagens com o servidor, a ponte do rádio Zigbee, um coordenador Zigbee na rede, relés, Bluetooth, atualizações assinadas), que compila para as duas placas; o núcleo, as definições, as mensagens e o painel num navegador real contra um nó de substituição são testados num computador com 477 588 verificações em 6 programas de teste. **Nada correu numa placa nem numa instalação real, nenhuma zona foi ligada com fios nem sirene ligada, a conversa Zigbee do próprio nó com um coordenador (local ou na rede) está escrita e testada contra um coordenador de substituição mas nunca correu contra um coordenador ou sensor reais, e isto não é um sistema de alarme certificado.**

---

## 📋 Num relance

| | |
|---|---|
| Versão | `0.4.8` |
| Maturidade | Funcional |
| Função | Nó de campo central de alarme para ESP32-S3 (família: Field Nodes, pai: ARMOR-COMMON) |
| Tecnologia | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| Testes | 488930 verificações em 18 programas de anfitrião e 5 em Python (`ctest`) |
| Entradas | Até 8 laços cablados, sensores Zigbee (IAS Zone) e sirene (IAS WD), 4 botões |
| Funciona em | Uma placa ESP32-S3 (Ethernet ou Wi-Fi) com a placa base; testes de anfitrião em qualquer computador |
| Em hardware real | Nenhum ainda: nenhuma placa, zona, sirene ou coordenador Zigbee ligados (só testes no computador, a compilação e o painel num navegador) |
| Licença | GPL-3.0-or-later |

**Ir para:** [Visão geral](#-visão-geral) · [Arquitetura](#-arquitetura) · [Segurança](#-segurança-e-proteção) · [Interfaces](#-interfaces) · [Configuração](#-configuração) · [Início rápido](#-início-rápido) · [Testes](#-testes) · [Problemas](#-resolução-de-problemas) · [Estrutura](#-estrutura-do-repositório) · [Relacionados](#-projetos-relacionados)

## 🚦 Estado do projeto

Cada linha é uma parte deste repositório e a luz diz até onde chega hoje. É escrita com o que o repositório consegue demonstrar (testes, compilações, execuções), não com o que está previsto.

🟢 Feito e verificado · 🟡 Funciona, com limites conhecidos · 🟠 Feito a meio ou ainda por verificar · 🔴 Por fazer ou a não funcionar

| Área | Estado | O que significa aqui |
|---|---|---|
| Software | 🟢 | O firmware compila no contentor ESP-IDF 5.5.5 para as duas placas (Wi-Fi e Ethernet). |
| Testes | 🟢 | Passam 488.930 verificações em 18 programas de anfitrião (`ctest`): as zonas, os laços cablados lidos pelo ADC, as regras de armar, atrasos, sirene e PIN, as definições e as mensagens. |
| Protocolos e dados | 🟠 | A conversa Zigbee do próprio nó com um coordenador, local ou na rede, só foi testada contra um coordenador simulado; nunca encontrou um coordenador ou um sensor reais. |
| Interface | 🟡 | O painel web foi exercitado num navegador real contra um nó simulado, não numa placa. |
| Hardware | 🔴 | Nenhuma placa construída, nenhuma zona cablada e nenhuma sirene ou coordenador Zigbee ligados: nada funcionou numa instalação real. |
| Segurança | 🟡 | O PIN com bloqueio, o código de configuração, o limite de acessos e as atualizações assinadas estão testados no anfitrião; não houve revisão independente e este não é um sistema de alarme certificado. |
| Atualizações | 🟠 | As verificações de hash e de assinatura Ed25519 e a lógica de retrocesso passam no anfitrião; uma atualização pelo ar nunca foi executada numa placa. |
| Escrita nos equipamentos | 🟠 | A sirene e os relés são comandados pelas regras do alarme e testados contra simulações; nenhum foi ligado a uma sirene ou a um relé reais. |
| Integração | 🟢 | As mensagens do nó são validadas contra os esquemas partilhados e os vetores de conformidade do ARMOR-COMMON. |
| Documentação | 🟢 | Oito idiomas, o guia do firmware, as notas de cablagem e o registo de alterações estão atualizados. |

## 🎯 Visão geral

* **Zonas:** quatro tipos - *instantânea* (uma porta, uma janela), *de entrada* (a porta por onde entra: inicia o atraso de entrada), *interior* (movimento: só em «ausente») e *sempre* (fumo, gás, pânico: armada ou não). Uma zona pode ser um laço com fio ou um sensor Zigbee.
* **O laço com fio:** um contacto com uma resistência de fim de linha, lido como tensão, dá quatro estados num só pino - normal, disparado, e violação por um fio em curto-circuito ou cortado. Os limites são definições; um filtro de repique impede que um contacto que salta dispare o alarme.
* **A central:** armação *ausente* e *em casa*, um atraso de saída que não trata como intruso quem sai, um atraso de entrada, um alarme que **fica retido** até um desarme correto, uma sirene que pára sozinha passado um tempo, desvio de zonas abertas quando forçado (nunca um detetor de fumo a soar), e um registo dos últimos cem eventos.
* **O PIN:** armar não precisa de nenhum, desarmar e silenciar sim; os PIN errados contam-se e, passados alguns, tudo se recusa durante um tempo, mesmo o PIN certo.
* **Silenciar sem desarmar:** enquanto a sirene soa, um PIN certo pára-a logo com o sistema ainda armado - o alarme fica retido e registado, uma zona que ainda não soou pode voltar a fazê-la soar, e um PIN errado conta para o bloqueio como um errado de desarmar.
* **O plano:** uma ponte na placa do nó elétrico (um rádio Zigbee, zonas com fio, um relé de sirene) que funciona com o ARMOR-SERVER e **continua a vigiar quando o servidor está ausente**: uma central toma o rádio ao servidor através da ponte Zigbee do ARMOR-NODE-ELECTRICAL. Veja [o desenho](docs/DESIGN.md) e [a continuidade](docs/FAILOVER.md).
* **O nó:** firmware para o ESP32-S3 (Ethernet PoE ou Wi-Fi) com até oito zonas com fio, a sirene num relé, o painel web de cada nó em oito idiomas, o rádio Zigbee emprestado ao Zigbee2MQTT, e as mensagens `armor/alarm/<node>/state|command|result`; o servidor só pode armar e desarmar quando três interruptores independentes o permitem. Veja [o firmware](docs/NODE_FIRMWARE.md), [as mensagens](docs/ALARM_MESSAGES.md) e [o hardware](docs/HARDWARE.md).
* **Tomar o rádio:** depois de o Zigbee2MQTT do servidor ter estado ausente durante um tempo definido, o nó levanta ele próprio o coordenador (ZNP), lê os sensores das zonas que o nomeiam (IAS Zone) e comanda sirenes Zigbee (IAS WD), e larga-o assim que o cliente volta.
* **Ainda não:** um coordenador ou sensor real, o hardware e qualquer teste numa instalação real.
* **Um reinício não desarma a casa:** o modo, um alarme em curso e as zonas deixadas de fora guardam-se na flash e retomam-se no arranque; um laço cortado ou em curto-circuito nunca se deixa de fora; as mensagens passam por uma fila (sem bloqueio mútuo com o broker) e o cão de guarda reinicia a placa se a tarefa que lê as zonas parar.
* **Atualizações pelo ar assinadas:** o nó só instala do GitHub uma imagem cujo SHA-256 coincide e que leva uma assinatura Ed25519 válida do responsável (a chave pública está no firmware, a privada nunca sai do responsável); um ficheiro carregado à mão por um administrador no painel continua a ser aceite. As páginas e mensagens do painel estão em oito idiomas.
* **Emparelhamento Bluetooth com PIN:** o PIN de seis dígitos deriva do código de configuração, é impresso na consola USB e mostrado a um administrador no painel; o telemóvel tem de o introduzir antes de poder escrever as definições do nó.
* **Sirene e reinícios:** o relé da sirene pertence só ao alarme (nem o broker, nem a central, nem o painel o podem mover, e não abre ao perder o broker); uma zona que ainda não tinha tocado volta a fazer soar a sirene depois do seu tempo; após um reinício voltam um alarme de fumo ou pânico com o sistema desarmado, um alarme silenciado e um bloqueio; um reset à mão é recusado com o alarme armado; um sensor Zigbee que nunca fala conta como perdido.
* **Só a casa pode usar o painel:** o painel responde por omissão à rede privada (ou às redes que o dono indicar, como uma VPN), recusa uma lista que deixaria de fora quem a guarda, conta as palavras-passe erradas por pessoa e por endereço e funciona atrás do proxy do servidor para ser alcançado de fora de casa sem publicar o nó.
* **Uma zona que não pode ser lida não aparece como fechada:** se o conversor não arranca ou um laço não dá leitura cinco rondas seguidas, a zona passa a *avaria* (trouble) - não vigia, nunca faz soar a sirene, impede armar (salvo forçando, e então fica excluída e registada) e o servidor avisa.
* **Ligação cifrada opcional ao broker (TLS):** desativada por predefinição, por isso o nó fala MQTT sem cifra como antes. Um interruptor e uma caixa para o certificado da autoridade própria da instalação (PEM, no máximo 2 KB) na página Broker fazem-no ligar-se com `mqtts://`, confiar só nessa autoridade e verificar que o certificado do broker foi feito para o endereço alcançado; o que não for um único certificado PEM é recusado. Coberto por testes no computador; o handshake ainda não foi executado numa placa.
* **Perfis de armar:** um comando de armar pode levar um perfil (Ausente, Em casa, Noite, o seu) que escolhe, para esse armar, que zonas vigiam, quais disparam o alarme logo ou após o atraso de entrada, os atrasos de saída e de entrada e se a sirene toca (um alarme silencioso fica trancado, registado e avisado). Nunca toca nas zonas de fumo, pânico ou sabotagem, no PIN, no bloqueio, no silêncio, num armar forçado nem numa zona com problema; o perfil sobrevive a um reinício e um armar sem perfil comporta-se exatamente como antes. Testado contra as regras antigas com uma execução aleatória e contra o contrato partilhado.
* **Relatório de saúde:** cerca de uma vez por minuto o nó publica há quanto tempo está ligado e porque reiniciou da última vez, a memória livre, a ligação e o sinal Wi-Fi, quantas vezes se ligou ao broker e o último erro que registou (`armor/health/<nó>`, o contrato `node_health` do ARMOR-COMMON). O código que o constrói é partilhado por todos os nós e testado num computador; nunca correu numa placa.
* **Controlo a partir do servidor:** o Studio pode reiniciar o nó de forma limpa, pô-lo em pausa até uma hora (retoma sozinho), pôr o seu trabalho em espera (sem fim, mantida nos reinícios, o nó continua ligado e a responder), fazê-lo mostrar-se, pedir um instantâneo de diagnóstico e marcá-lo em manutenção (`armor/control/<nó>/...`, o contrato `node_control` do ARMOR-COMMON). Cada ordem tem dois passos com um token de uso único que pertence a essa operação, só o servidor as pode escrever no broker, e não existe reposição de fábrica, apagamento nem sono profundo. Enquanto o nó está em pausa ou em espera, as suas zonas não são lidas, nada é avaliado, armado ou tocado, nenhum estado é publicado e o relé da sirene fica aberto: a casa NÃO está protegida, e o relatório de saúde di-lo. O código é partilhado por todos os nós e testado num computador; ainda não correu numa placa.
* **Diagnóstico sem cabo:** depois de um arranque que não foi limpo (um bloqueio, um watchdog, uma quebra de alimentação, uma reserva de memória falhada), o relatório de saúde diz o motivo durante dez minutos; uma falha deixa o seu core dump para ver, descarregar ou apagar a partir do painel; as definições guardadas são copiadas antes de cada instalação de firmware e recuperadas se uma atualização as danificar; e um administrador pode pedir ao nó, através do servidor, um pacote de diagnóstico ou o seu registo com um token de utilização única.
* **As opções de segurança do servidor:** a central aplica as opções que o servidor guarda para ela (`armor/alarm/<nó>/policy`) e cada uma pode ser fixada no próprio nó: o estado armado guardado fica atado às zonas e ao PIN, as mensagens de zona Zigbee do servidor podem ser desligadas e, se assinadas, são verificadas (assinatura com uma chave escrita no painel e um número de sequência que só aumenta). Testado num computador com os valores que o contrato e o servidor calculam; não executado numa placa.
* **Selagem opcional dos segredos guardados:** desativada por predefinição (`security.seal_secrets`, ou `nodes.nvs_encryption` do servidor). Ativada, as palavras-passe, os tokens, as chaves, os PIN e os hashes de palavras-passe que o nó guarda ficam selados com ChaCha20-Poly1305 e tudo o resto continua legível; ativá-la ou desativá-la reescreve-os sem perder nada, mesmo que o nó pare a meio. Não se queima nenhum eFuse nem se cifra a flash: a chave está numa partição do mesmo chip, pelo que **não** protege contra quem tenha a placa, e um nó instalado antes dessa partição mantém os segredos como estavam até ser gravado com a nova tabela de partições.

## 🧩 Papel no A.R.M.O.R.

O ARMOR-NODE-ALARM é a central de alarme: as zonas, o armar, os atrasos, a sirene e o PIN decidem-se no próprio nó, por isso a casa continua vigiada quando o servidor não está. O [ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) mostra-a, liga sensores Zigbee às suas zonas e, quando o permitir, arma-a e desarma-a; o menu *Centrais de alarme* e o *Desenhador de alarme* do [ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) e a aplicação [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) comandam-na. O seu rádio Zigbee pode estar na placa ou ser um coordenador na rede.

| Nome | Como | Para quê |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | Recebe o estado da central; ordena armar, desarmar e silenciar só atrás de três interruptores |
| ARMOR-STUDIO | menu de centrais de alarme | Mostra as zonas, os eventos e a política, e abre o painel do nó |
| ARMOR-ANDROID-CONTROL | Bluetooth | Configura o nó no terreno e mostra as centrais de alarme |
| ARMOR-COMMON | contrato de alarme | As mensagens são verificadas com os seus esquemas; o firmware comum é copiado dele |

## 🏛️ Arquitetura

<p align="center">
  <img src="images/architecture.svg" alt="Arquitetura de ARMOR-NODE-ALARM: quem o chama, o que contém e de que depende" width="100%">
</p>

## 🖼️ Capturas de ecrã

Capturadas a 1920×1080 numa bancada de testes, com dados de exemplo e sem qualquer instalação real por trás.

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="Resumo: o estado do nó num relance - rede, broker, firmware e o que reporta." width="100%"><br><sub>Resumo: o estado do nó num relance - rede, broker, firmware e o que reporta.</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="Zonas: cada zona com fios ou Zigbee do painel, o seu tipo, o tempo de retenção e o estado em direto." width="100%"><br><sub>Zonas: cada zona com fios ou Zigbee do painel, o seu tipo, o tempo de retenção e o estado em direto.</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Rádio Zigbee: a ponte série para o módulo coordenador, que cliente a pode usar e o estado da ponte." width="100%"><br><sub>Rádio Zigbee: a ponte série para o módulo coordenador, que cliente a pode usar e o estado da ponte.</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="Relés e sirene: as saídas da placa base e o que aciona cada uma." width="100%"><br><sub>Relés e sirene: as saídas da placa base e o que aciona cada uma.</sub></td></tr>
</table>

## 🔒 Segurança e proteção

| Ameaça | Defesa |
|---|---|
| Palavras-passe em claro ou compiladas | PBKDF2-HMAC-SHA256 com 10 000 rondas; nenhuma palavra-passe no firmware (um nó sem utilizador entra em configuração); no máximo 4 utilizadores (admin ou visualizador) e de 8 a 64 caracteres; no máximo 6 sessões, 30 minutos de inatividade |
| Tomar um nó acabado de gravar | A configuração exige um código de arranque: o da compilação (`CONFIG_ARMOR_SETUP_CODE`) ou um aleatório guardado na flash e mostrado na consola USB; com segredo de frota é `HMAC-SHA256(segredo, MAC)`; é também a chave do Wi-Fi de configuração (WPA2, nunca em ponte) |
| O painel alcançado a partir da internet | Responde por omissão às redes privadas de uma casa (10/8, 172.16/12, 192.168/16, 169.254/16, 127/8 e 100.64/10 onde vivem algumas VPN); a definição `web.admin_sources` lista outros endereços e redes IPv4 (8 entradas, 120 caracteres); tudo o que está fora é recusado exceto a identidade pública `GET /api/v1/session`; uma lista que deixaria de fora quem a guarda é rejeitada |
| O painel em claro por HTTP | HTTPS no 443 com um certificado que o nó cria sozinho (autoassinado, na flash, impressão digital SHA-256 visível para comparar); o painel pode ser http, https ou ambos; o cookie de sessão é sempre HttpOnly e SameSite=Strict (e Secure por TLS) |
| Um firmware falso do GitHub | Uma imagem só se instala com um SHA-256 coincidente e uma assinatura Ed25519 válida feita com a chave do mantenedor (`no_checksum`, `no_signature`, `bad_signature`, `checksum_mismatch`, `wrong_firmware`, `not_newer`, `version_mismatch` recusam-na); um envio manual por um administrador não é verificado: um limite explícito |
| Sequestro da configuração Bluetooth | Um PIN de 6 dígitos derivado do código de configuração, visível na consola USB e ao administrador; o telemóvel tem de o introduzir antes de escrever definições; a janela dura `CONFIG_ARMOR_BLE_SETUP_MINUTES` (10 por omissão, de 1 a 120) |
| Segredos guardados lidos por quem tem a placa | Opcional (desligado por omissão, interruptor `security.seal_secrets` ou `nodes.nvs_encryption` do servidor): ChaCha20-Poly1305 para palavras-passe, tokens, chaves, PIN e hashes. Não cifra a flash nem queima um eFuse: a chave está numa partição do mesmo chip, por isso não protege contra quem tem a placa |
| A ligação ao broker em claro | TLS opcional (`mqtts://`) com a autoridade própria do dono (PEM até 2 KB) e verificação do nome; desligado por omissão. O handshake só correu em testes de anfitrião, não numa placa |
| Adivinhar o PIN da central | A comparação não revela onde difere; falhas seguidas bloqueiam a central `lockout_s` (60 s por omissão, de 1 a 3600) vezes 1, 5 e 15 nos três primeiros bloqueios, mesmo para o PIN certo; um bloqueio em curso é restaurado após um reinício; armar não leva PIN, desarmar e silenciar sim |
| Um reinício que desarma a casa | O modo, o alarme em curso e as zonas excluídas guardam-se na flash e recuperam-se; um laço cortado ou em curto nunca fica de fora; fumo, pânico e sabotagem disparados com o sistema desarmado voltam após reiniciar; o reset com BOOT só apaga com a central DESARMADA |
| Uma zona ilegível parece fechada | Mostra `trouble` (o conversor não arranca ou 5 rondas sem leitura) e a central não arma com ela a não ser forçado, e então fica excluída e anotada; um detetor de fumo ou pânico que está a soar nunca é excluído |
| Uma ordem do servidor que ninguém autorizou | Armar, desarmar e silenciar a partir do servidor exigem TRÊS interruptores (a definição do nó, a lista de acesso do broker e `ARMOR_ALARM_COMMANDS=1` no servidor), o servidor pede o PIN do utilizador antes de enviar um desarme ou um silêncio, e tudo está desligado por omissão; o relé da sirene pertence só ao alarme (nem o broker, nem a central nem o painel o podem mover) |
| Mensagens Zigbee forjadas | O tópico `armor/alarm/<id>/zigbee` leva um HMAC-SHA256 sobre nó, zona, estado, bateria e sequência com a chave da central (64 hex digitados no painel), e a sequência só sobe; o tópico da política leva `config_hash`, `zone_feed` e a sua assinatura, cada um dos quais pode ser fixado no nó |

Não é um sistema de alarme certificado ([docs/SAFETY.md](docs/SAFETY.md)): é um projeto doméstico e nunca correu numa placa nem numa instalação real. Se o servidor faltar a central decide sozinha, e se também falhar o nó ou o seu rádio ninguém vigia. Leia [SECURITY.md](SECURITY.md) para comunicar uma vulnerabilidade.

## 🔌 Interfaces

| Parte | O que define |
|---|---|
| MQTT | `armor/alarm/<id>/state` (não retido: zonas com id, nome, tipo, estado e exclusão, a fase), `/command`, `/result`, `/zigbee` e `/policy` (servidor para central); `armor/device/<id>/<relé>/{state,set}`, `armor/health/<id>`, `armor/control/<id>/...` |
| Zonas | Tipo `instant`, `entry`, `interior` ou `always`; estado `normal`, `triggered`, `tamper` ou `trouble`; até 8 laços cablados (um contacto com resistência de fim de linha lido pelo ADC dá quatro estados num pino; a placa Ethernet tem 6 pinos analógicos válidos, a Wi-Fi 8), 4 botões e um registo de 100 eventos |
| Zigbee | Uma ponte série (ZNP) para um coordenador, sensores IAS Zone e sirenes IAS WD; o nó toma o rádio quando o Zigbee2MQTT do servidor se cala durante `zigbee_silence_min` e devolve-o quando o servidor volta ([docs/ZIGBEE.md](docs/ZIGBEE.md), [docs/FAILOVER.md](docs/FAILOVER.md)) |

## ⚙️ Configuração

| Variável | Predefinição | O que define |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | Os atrasos de saída e de entrada; a saída não trata como intruso quem se vai |
| `panel.lockout_s` | 60 (1-3600) | O bloqueio do PIN, vezes 1, 5 e 15 nos três primeiros |
| `zigbee_silence_min` | - | Quanto tempo o Zigbee do servidor se pode calar antes de o nó tomar o rádio |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | As definições que todos os nós partilham: o nome (vazio dá `armor-` e os últimos bytes do MAC), o código de configuração (vazio: aleatório), o segredo de frota (16+ caracteres, nunca no repositório), as primeiras definições do broker, o batimento (de 2 a 300 s), os minutos de configuração Bluetooth (de 1 a 120) e a reposição com BOOT (8 s nos primeiros 30 s) |

Os novos toques depois de um silêncio estão limitados a 3 (`kMaxResounds`). O relé da sirene arranca aberto a cada reinício e não cai para aberto se o broker se perder.

## 🚀 Início rápido

**Requisitos:** um compilador C++17 para os testes de anfitrião; o contentor ESP-IDF 5.5.5 para compilar a imagem; uma placa ESP32-S3 (Ethernet ou Wi-Fi) com a placa base.

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

Veja o [desenho](docs/DESIGN.md), [como a central toma o lugar do servidor](docs/FAILOVER.md) e as [notas de segurança](docs/SAFETY.md).

## 🧪 Testes

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

18 programas de teste de anfitrião e 5 verificações Python correm com `ctest`; 488930 verificações contadas pelos programas de anfitrião, compiladas com `-Werror`.

Verificações Python: os textos do painel em oito línguas, as tarefas, os caminhos de gravação, a sessão Bluetooth sob o seu bloqueio e as rotas que o painel chama (e, em alguns nós, os códigos HTTP ou os glifos dos tipos de letra)

Os testes de anfitrião precisam de qualquer compilador C++17 (Linux, WSL, MSYS2); o painel web também é exercitado num navegador real contra um nó de substituição (`tools/panel_mock.mjs`, `tools/panel_browser_test.mjs`).

A maior parte das verificações são varrimentos exaustivos das regras da central; os laços, a sirene e a ligação Zigbee são substitutos.

## 🏗️ Compilação e publicação

`tools/build_node.sh` compila a imagem no contentor ESP-IDF 5.5.5 e a versão leva a imagem, o seu SHA-256 e a sua assinatura Ed25519, uma por placa; a imagem grava-se por USB-C da primeira vez e depois atualiza-se a partir do painel.

## 🩺 Resolução de problemas

| Vê | Porque | Faça |
|---|---|---|
| Perdeu a palavra-passe ou o nó já não se liga à rede | Um nó em configuração e sem endereço após 90 s abre o seu próprio Wi-Fi `ARMOR-SETUP-xxxxxx` (abra `http://192.168.4.1/`) | Para recomeçar mantenha BOOT 8 segundos nos primeiros 30 segundos após ligar (apaga definições e utilizadores), ou use *Reposição de fábrica* no painel |
| Uma atualização do GitHub é recusada | A versão não traz soma de verificação nem assinatura, não coincidem (`e_no_checksum`, `e_no_signature`, `e_checksum_mismatch`, `e_bad_signature`), não traz imagem para esta placa (`e_no_asset_for_board`) ou é de outro projeto (`e_wrong_firmware`) | O nó não mudou; tente de novo ou deixe um administrador enviar o ficheiro à mão |
| A central não arma | Uma zona está em `trouble` (o conversor não arranca ou 5 rondas sem leitura) | Corrija a cablagem, ou force a armação (a zona fica então excluída e anotada) |
| O PIN é recusado mesmo estando certo | A central está bloqueada após tentativas falhadas (até o PIN certo é recusado) | Espere `lockout_s`; o bloqueio sobrevive a um reinício |
| O reset com BOOT não faz nada | A central está armada: o reset só apaga uma central desarmada | Desarme primeiro com o PIN |

## 📂 Estrutura do repositório

<p align="center">
  <img src="images/repository-structure.svg" alt="Estrutura do repositório de ARMOR-NODE-ALARM" width="100%">
</p>

<details>
<summary>Versão em texto da árvore</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                o firmware: um componente ESP-IDF
├── core/                                a lógica sem ESP-IDF, testada num computador
├── panel/                               o painel web embebido no firmware
├── tests/                               testes de anfitrião e verificações Python
├── docs/                                11 guias: segurança, mensagens, Zigbee, redundância
├── tools/                               ferramentas de compilação, empacotamento e verificação
├── .github/                             fluxo de CI e modelos de issues e pull requests
├── images/                              faixa, ícone e as duas imagens deste README
├── build.sh build.bat                   compilar pelo fluxo de projeto partilhado
├── test.sh test.bat                     executar os testes pelo fluxo partilhado
├── run.sh run.bat                       arrancar pelo fluxo de projeto partilhado
├── armor.project.json                   manifesto: versão, função, maturidade, comando de build
├── CHANGELOG.md                         o que mudou em cada versão
├── SECURITY.md SUPPORT.md LICENSE       política de segurança, apoio e licença
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   como contribuir e o código de conduta
├── CMakeLists.txt                       o projeto ESP-IDF
├── partitions.csv                       a tabela da flash
├── sdkconfig.defaults                   as definições do firmware
├── sdkconfig.board.s3-eth               as definições de uma placa
├── sdkconfig.board.s3-wifi              as definições de uma placa
├── secrets/                             exemplo do ficheiro do nó (os verdadeiros não são versionados)
├── build-test.sh build-test.bat         executar os testes pelo fluxo partilhado
└── .gitattributes .gitignore            definições do Git
```

</details>

## 🗺️ Roteiro e limites conhecidos

**Limites conhecidos hoje**

* A conversa Zigbee do próprio nó com um coordenador, local ou na rede, só foi testada contra um coordenador simulado; nunca encontrou um coordenador ou um sensor reais.
* O painel web foi exercitado num navegador real contra um nó simulado, não numa placa.
* Nenhuma placa construída, nenhuma zona cablada e nenhuma sirene ou coordenador Zigbee ligados: nada funcionou numa instalação real.
* O PIN com bloqueio, o código de configuração, o limite de acessos e as atualizações assinadas estão testados no anfitrião; não houve revisão independente e este não é um sistema de alarme certificado.
* As verificações de hash e de assinatura Ed25519 e a lógica de retrocesso passam no anfitrião; uma atualização pelo ar nunca foi executada numa placa.
* A sirene e os relés são comandados pelas regras do alarme e testados contra simulações; nenhum foi ligado a uma sirene ou a um relé reais.

**A seguir**

* Cablar laços e uma sirene reais, experimentar a ligação Zigbee com um coordenador e sensores reais, e correr a central numa instalação real antes de afirmar mais seja o que for.

## 🌍 Idiomas

O README e os textos existem em inglês, espanhol, francês, italiano, alemão, chinês, japonês e português europeu.

## 🔗 Projetos relacionados

O **A.R.M.O.R.** (Autonomous Radar & Multimodal Observation Range) é um sistema de segurança perimetral feito de repositórios independentes. Cada um tem a sua própria versão, os seus próprios testes e o seu próprio README; esta é a família:

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - Contratos de mensagens, validadores, vetores de conformidade e tipos gerados
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - Firmware de nó de campo para ESP32-S3 com três radares e o seu próprio painel web
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - Protocolos de inversores e baterias solares e as mensagens de um nó porta de ligação
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - Nó elétrico: contadores, a mensagem das leituras da rede e as regras de comutação
* **ARMOR-NODE-ALARM** (este repositório) - Nó e painel de alarme: zonas, armar, atrasos, sirene e PIN, com o servidor ou sem ele
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - Painel tátil: o estado do sistema num ecrã de parede, armar e reconhecer, e a casa do assistente de voz
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - A rede local: os seus dispositivos, a internet e o que muda
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - Coordenador central: telemetria, alarmes, dispositivos, leituras solares e câmaras
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - Consola web: câmaras, radar, alarmes, energia solar e o desenhador 2D/3D do local
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - Cliente de operador Android com radar 2D/3D em direto
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - Cliente de operador para iPhone
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - Política de inferência visual que explica as suas decisões e nunca atua
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - Zonas de câmara, máscaras e regras de deteção para o Jetson Orin NX
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - Intenções de voz offline com uma confirmação que não pode ser falsificada
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - Caixas, eletrónica e a matriz de aceitação em bancada
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - Implantação, a bancada de testes do CM5, cópias de segurança e TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - Simulador de telemetria offline com falhas repetíveis
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - Deteta, instala e atualiza os repositórios do próprio ecossistema
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - Arquitetura, base de segurança e a matriz de capacidades

## 📚 Documentação

* [Segurança](docs/SAFETY.md): o que a central promete e o que não
* [Mensagens de alarme](docs/ALARM_MESSAGES.md), [Zigbee](docs/ZIGBEE.md) e [redundância](docs/FAILOVER.md)
* [Desenho](docs/DESIGN.md), [hardware](docs/HARDWARE.md), [placa base](docs/BASE_BOARD.md) e [firmware do nó](docs/NODE_FIRMWARE.md)
* [Testes de bancada](docs/BENCH_TESTS.md), [código de configuração](docs/SETUP_CODE.md) e [provisionamento Bluetooth](docs/BLE_PROVISIONING.md)
* [Matriz de capacidades: o que está demonstrado e o que não está](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [Catálogo de projetos: versões e como os repositórios dependem uns dos outros](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [Registo de alterações deste repositório](CHANGELOG.md)

## 🤝 Contribuir e comunidade

* Antes de um pull request, execute `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py`: tem de passar. Consulte [CONTRIBUTING.md](CONTRIBUTING.md) e o [código de conduta](CODE_OF_CONDUCT.md).
* O README existe em oito idiomas e é gerado a partir das fontes do ARMOR-DOCS: uma alteração ao texto faz-se lá.
* Uma alteração a uma mensagem entre componentes começa no ARMOR-COMMON.
* Não descreva como a funcionar numa placa aquilo que só foi compilado.
* Comunique uma vulnerabilidade em privado: [SECURITY.md](SECURITY.md). Perguntas, ideias e relatórios: electrohobby3d@gmail.com ([SUPPORT.md](SUPPORT.md)).

## 👤 AUTOR

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 LICENÇA

GPL-3.0-or-later - consulte [LICENSE](LICENSE).
