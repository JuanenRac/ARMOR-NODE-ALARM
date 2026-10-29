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
  🇨🇳 <b>简体中文</b> |
  <a href="README_jpn.md">🇯🇵 日本語</a> |
  <a href="README_por.md">🇵🇹 Português</a>
</p>

### 报警节点与报警主机：住宅报警的防区、布防、延时、警笛和 PIN，含节点固件和在电脑上测试过的规则（没有在电路板上运行过）

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

**诚实性检查 - 今天真正能运行的部分：** 目前存在的是**报警核心**（什么是防区、ADC 读取的有线回路如何变成状态、主机的规则：布防、延时、报警、警笛、带锁定的 PIN）以及建立在其上的**节点固件**（设置、网页面板、与服务器的消息、Zigbee 无线电的桥接、网络上的 Zigbee 协调器、继电器、蓝牙、签名更新），可为两种板子构建；核心、设置、消息以及在真实浏览器中对照替身节点的面板，在计算机上用 18 个测试程序中的 488,930 项检查进行测试。**没有任何东西在板上或真实安装中运行过，没有接过任何防区，也没有连接过警笛；节点自己与协调器（本地或网络上）的 Zigbee 对话已编写并对照替身协调器测试，但从未对照真实的协调器或传感器运行，而且这不是经过认证的报警系统。**

---

## 📋 速览

| | |
|---|---|
| 版本 | `0.4.8` |
| 成熟度 | 可用 |
| 角色 | ESP32-S3 报警面板现场节点（家族：Field Nodes，上级：ARMOR-COMMON） |
| 技术栈 | C++17, ESP-IDF 5.5.5, ESP32-S3 |
| 测试 | 488930 项检查（18 个主机程序和 5 项 Python 检查，`ctest`） |
| 输入 | 最多 8 个有线回路、Zigbee 传感器（IAS Zone）和警笛（IAS WD）、4 个按钮 |
| 运行位置 | 带底板的 ESP32-S3 电路板（以太网或 Wi-Fi）；主机测试可在任何计算机上运行 |
| 真实硬件 | 暂无：没有连接板子、防区、警笛或 Zigbee 协调器（仅有计算机测试、构建和浏览器中的面板） |
| 许可证 | GPL-3.0-or-later |

**跳转到：** [概述](#-概述) · [架构](#-架构) · [安全](#-安全与防护) · [接口](#-接口) · [配置](#-配置) · [快速开始](#-快速开始) · [测试](#-测试) · [故障排除](#-故障排除) · [结构](#-仓库结构) · [相关项目](#-相关项目)

## 🚦 项目状态

每一行是本仓库的一个部分，灯号表示它今天做到了哪一步。内容依据仓库能够展示的证据（测试、构建、运行），而不是计划。

🟢 已完成并验证 · 🟡 可用，有已知限制 · 🟠 部分完成或尚未验证 · 🔴 未完成或不可用

| 领域 | 状态 | 在这里的含义 |
|---|---|---|
| 软件 | 🟢 | 固件可在 ESP-IDF 5.5.5 容器中为两块板（Wi-Fi 和以太网）构建。 |
| 测试 | 🟢 | 18 个主机程序中的 488,930 项检查通过（`ctest`）：防区、由 ADC 读取的有线回路、布防、延时、警笛和 PIN 规则、设置和消息。 |
| 协议与数据 | 🟠 | 节点自身与协调器（本地或网络上）的 Zigbee 通信只对照模拟协调器测试过；从未遇到过真实的协调器或传感器。 |
| 界面 | 🟡 | 网页面板已在真实浏览器中对模拟节点测试过，尚未在板子上测试。 |
| 硬件 | 🔴 | 尚未制作板子，没有接线的防区，也没有连接警笛或 Zigbee 协调器：还没有在真实的安装中运行过。 |
| 安全 | 🟡 | 带锁定的 PIN、设置码、登录限制和签名更新已在主机上测试；没有经过独立审查，这也不是经过认证的报警系统。 |
| 更新 | 🟠 | 哈希和 Ed25519 签名检查以及回滚逻辑在主机上通过；空中更新从未在板子上运行过。 |
| 写入设备 | 🟠 | 警笛和继电器由报警规则驱动，并对照模拟测试过；都没有连接到真实的警笛或继电器。 |
| 集成 | 🟢 | 节点的消息已对照 ARMOR-COMMON 的共享模式和一致性向量验证。 |
| 文档 | 🟢 | 八种语言、固件指南、接线说明和更新日志均为最新。 |

## 🎯 概述

* **防区：** 四种类型 - *即时*（门、窗）、*进入*（进门用的门：触发进入延时）、*内部*（移动探测：仅在“离家”时）和*全天*（烟雾、燃气、紧急按钮：无论布防与否）。防区可以是有线回路或 Zigbee 传感器。
* **有线回路：** 带终端电阻的触点，以电压方式读取，一个引脚给出四种状态 - 正常、触发，以及线路短路或剪断的防拆。阈值是设置项；去抖器避免抖动的触点引发报警。
* **主机：** *离家*和*在家*两种布防，不把离开的人当作入侵者的退出延时，进入延时，需要正确撤防才会解除的**锁定报警**，一段时间后自动停止的警笛，强制布防时旁路已打开的防区（响着的烟雾探测器永远不会被旁路），以及最近一百条事件的日志。
* **PIN：** 布防不需要，撤防和消音需要；输错的 PIN 会被计数，几次之后一段时间内一切都会被拒绝，连正确的 PIN 也不例外。
* **不撤防也能消音：** 警笛鸣响时，正确的 PIN 可让它立即停止，而系统仍保持布防 - 报警保持锁定并被记录，尚未响过的防区可再次触发警笛，输错的 PIN 与撤防时输错的 PIN 一样计入锁定。
* **计划：** 在电气节点电路板上的网关（Zigbee 无线电、有线防区、警笛继电器），与 ARMOR-SERVER 配合，并在**服务器离线时继续守护**：主机通过 ARMOR-NODE-ELECTRICAL 的 Zigbee 桥接从服务器手中接管无线电。参见[设计](docs/DESIGN.md)和[接管](docs/FAILOVER.md)。
* **节点：** ESP32-S3 固件（PoE 以太网或 Wi-Fi），最多八个有线防区，警笛接在继电器上，所有节点通用的八种语言网页面板，借给 Zigbee2MQTT 的 Zigbee 无线电，以及消息 `armor/alarm/<节点>/state|command|result`；只有三个相互独立的开关都允许时，服务器才能布防和撤防。参见[固件](docs/NODE_FIRMWARE.md)、[消息](docs/ALARM_MESSAGES.md)和[硬件](docs/HARDWARE.md)。
* **接管无线电：** 服务器的 Zigbee2MQTT 缺席设定时间后，节点自行启动协调器（ZNP），读取指定了传感器的防区（IAS Zone）并控制 Zigbee 警笛（IAS WD），客户端一回来就交还。
* **尚未完成：** 真实的协调器或传感器、硬件，以及在真实设施上的任何测试。
* **重启不会让房子撤防：** 模式、正在进行的报警和被旁路的防区保存在闪存中，启动时恢复；被剪断或短路的回路永远不会被旁路；消息经队列发送（与代理不会死锁），读取防区的任务停止时看门狗会重启开发板。
* **签名的无线更新：**节点只会从 GitHub 安装 SHA-256 匹配且带有维护者有效 Ed25519 签名的镜像（公钥在固件中，私钥从不离开维护者）；管理员在面板中手动上传的文件仍然被接受。面板的页面和消息有八种语言。
* **带 PIN 的蓝牙配对：**六位 PIN 由设置码派生，打印在 USB 控制台上并向管理员显示在面板中；手机必须输入它才能写入节点的设置。
* **警笛与重启：**警笛继电器只属于报警系统（代理、中心和面板都不能控制它，失去代理时它也不会断开）；尚未鸣响过的防区会在警笛超时后让它再次鸣响；重启后，撤防状态下的烟雾或紧急报警、已静音的报警和锁定都会恢复；布防时拒绝手动复位；从未发声的 Zigbee 传感器视为丢失。
* **只有家中才能使用面板：**面板默认只响应私有网络（或所有者列出的网络，如 VPN），拒绝会把保存者自己挡在外面的列表，按人和按地址统计错误密码，并可在服务器代理之后工作，从而无需公开节点就能在家外访问。
* **无法读取的防区不会显示为已闭合：** 如果转换器无法启动，或某回路连续五轮没有读数，该防区会进入*故障*（trouble）状态：它不守护、从不触发警笛、会阻止布防（强制布防时将其排除并记录），服务器会发出警告。
* **可选的与代理加密连接(TLS):** 默认关闭,因此节点仍像以前一样使用未加密的 MQTT。代理页面中的一个开关和一个用于填写本安装自有证书颁发机构证书(PEM,最大 2 KB)的框,会让节点用 `mqtts://` 连接、只信任该机构,并检查代理证书是否为其所到达的地址而签发;不是单个 PEM 证书的内容会被拒绝。已有主机测试覆盖;握手尚未在电路板上运行。
* **布防配置：** 布防命令可以带一个配置（离家、在家、夜间或自定义），为这次布防选择哪些分区守护、哪些分区立即报警或在进入延时后报警、退出和进入延时，以及警笛是否鸣响（静音报警仍会锁定、记录和通知）。它绝不触及烟雾、紧急按钮或防拆分区、PIN、锁定、消音、强制布防或故障分区；配置在重启后保留，不带配置的布防行为与以前完全一致。已通过随机运行对照旧规则测试，并对照共享契约测试。
* **健康报告：** 节点大约每分钟发布一次：已运行多久、上次为何重启、可用内存、连接与 Wi-Fi 信号、连接代理的次数以及记录的最后一个错误（`armor/health/<节点>`，即 ARMOR-COMMON 的 `node_health` 契约）。生成它的代码由所有节点共享并在计算机上测试；尚未在开发板上运行。
* **由服务器控制：** Studio 可以干净地重启节点、将其暂停至多一小时（自动恢复）、让其工作进入待机（无期限，重启后仍保持，节点保持连接并继续应答）、让它显示自己、请求诊断快照，并把它标记为维护中（`armor/control/<节点>/...`，ARMOR-COMMON 的 `node_control` 契约）。每条命令分两步，使用只属于该操作的一次性令牌，只有服务器可以在代理上写入，并且没有恢复出厂设置、擦除或深度睡眠。节点处于暂停或待机时，不读取防区，不做任何判断、布防或鸣响，不发布状态，警笛继电器保持断开：住宅没有受到保护，健康报告会如实说明。 该代码由所有节点共用，并已在计算机上测试；尚未在电路板上运行过。
* **无需线缆的诊断：** 在一次不干净的启动之后（崩溃、看门狗、电源跌落、内存分配失败），健康报告会在十分钟内说明原因；崩溃会留下核心转储（core dump），可在面板中查看、下载或删除；已保存的设置会在每次固件安装前被复制，并在更新损坏它们时恢复；管理员还可以通过服务器，用一次性令牌向节点索取诊断包或其日志。
* **服务器的安全选项：**面板会应用服务器为它保存的选项（`armor/alarm/<节点>/policy`），每一项也可以在节点本身固定：已保存的布防状态与防区和 PIN 绑定，服务器的 Zigbee 防区消息可以关闭，带签名时会被校验（用在面板中输入的密钥验签，序列号只增不减）。已在计算机上按合同和服务器计算的值测试；尚未在开发板上运行。
* **可选的已存储机密封存:** 默认关闭(`security.seal_secrets`,或服务器的 `nodes.nvs_encryption`)。开启后,节点保存的密码、令牌、密钥、PIN 和密码哈希会用 ChaCha20-Poly1305 封存,其余内容仍可读取;开启或关闭时会无损重写,即使节点中途停止也不会丢失。不会烧写 eFuse,闪存也不加密:密钥在同一芯片的一个分区中,因此**不能**防范持有电路板的人;在该分区出现之前安装的节点会保持原样,直到用新的分区表重新烧录。

## 🧩 在 A.R.M.O.R. 中的角色

ARMOR-NODE-ALARM 是报警主机：防区、布防、延时、警笛和 PIN 都在节点自身上决定，因此服务器不在时住宅仍受守护。[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER) 显示它，把 Zigbee 传感器绑定到其防区，并在你允许时对它布防和撤防；[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO) 的*报警主机*菜单和*报警设计器*以及 [ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL) 应用可控制它。它的 Zigbee 无线电可以在板上，也可以是网络上的协调器。

| 名称 | 方式 | 用途 |
|---|---|---|
| ARMOR-SERVER | MQTT `armor/alarm/<id>/{state,command,result,zigbee,policy}` | 接收面板状态；仅在三个开关都开启时下达布防、撤防和静音命令 |
| ARMOR-STUDIO | 报警面板菜单 | 显示区域、事件和策略，并打开节点面板 |
| ARMOR-ANDROID-CONTROL | Bluetooth | 在现场设置节点并显示报警面板 |
| ARMOR-COMMON | 报警合约 | 消息按其模式校验；公共固件由此复制 |

## 🏛️ 架构

<p align="center">
  <img src="images/architecture.svg" alt="ARMOR-NODE-ALARM 的架构：谁调用它、它包含什么、它依赖什么" width="100%">
</p>

## 🖼️ 屏幕截图

在测试台上以 1920×1080 截取，使用示例数据，背后没有真实的安装。

<table>
<tr><td width="50%" valign="top"><img src="images/screenshots/overview.png" alt="概览：节点状态一目了然——网络、代理、固件及其上报内容。" width="100%"><br><sub>概览：节点状态一目了然——网络、代理、固件及其上报内容。</sub></td><td width="50%" valign="top"><img src="images/screenshots/zones.png" alt="防区：主机的每个有线或 Zigbee 防区，包括类型、保持时间和实时状态。" width="100%"><br><sub>防区：主机的每个有线或 Zigbee 防区，包括类型、保持时间和实时状态。</sub></td></tr>
<tr><td width="50%" valign="top"><img src="images/screenshots/zigbee.png" alt="Zigbee 无线电：通往协调器模块的串口桥接、允许使用它的客户端以及桥接状态。" width="100%"><br><sub>Zigbee 无线电：通往协调器模块的串口桥接、允许使用它的客户端以及桥接状态。</sub></td><td width="50%" valign="top"><img src="images/screenshots/inputs-outputs.png" alt="继电器和警笛：底板的输出以及每个输出由什么驱动。" width="100%"><br><sub>继电器和警笛：底板的输出以及每个输出由什么驱动。</sub></td></tr>
</table>

## 🔒 安全与防护

| 威胁 | 防御 |
|---|---|
| 明文或编译进固件的密码 | PBKDF2-HMAC-SHA256，10 000 轮；固件中没有密码（没有用户的节点会进入设置）；最多 4 个用户（管理员或查看者），8 到 64 个字符；最多 6 个会话，空闲 30 分钟 |
| 接管刚刷写的节点 | 设置需要启动码：构建时的码（`CONFIG_ARMOR_SETUP_CODE`），或保存在闪存中并显示在 USB 控制台上的随机码；有机群密钥时为 `HMAC-SHA256(密钥, MAC)`；它也是设置用 Wi-Fi 的密钥（WPA2，绝不桥接） |
| 从互联网访问面板 | 默认只响应家庭的私有网络（10/8、172.16/12、192.168/16、169.254/16、127/8，以及某些 VPN 使用的 100.64/10）；设置 `web.admin_sources` 可列出其他 IPv4 地址和网络（8 项，120 个字符）；除公开身份 `GET /api/v1/session` 外，其余一律拒绝；会把保存者自己锁在外面的列表会被拒绝 |
| 通过 HTTP 明文访问面板 | 在 443 端口提供 HTTPS，证书由节点自己生成（自签名、存于闪存，显示 SHA-256 指纹以便比对）；面板可设为 http、https 或两者；会话 Cookie 始终为 HttpOnly 和 SameSite=Strict（TLS 下还有 Secure） |
| 来自 GitHub 的伪造固件 | 只有 SHA-256 匹配且带有维护者密钥有效 Ed25519 签名的镜像才会被安装（`no_checksum`、`no_signature`、`bad_signature`、`checksum_mismatch`、`wrong_firmware`、`not_newer`、`version_mismatch` 会拒绝）；管理员手动上传不做校验：这是明确的限制 |
| 劫持蓝牙设置 | 由设置码派生的 6 位 PIN，显示在 USB 控制台并对管理员可见；手机必须先输入才能写入设置；设置窗口时长为 `CONFIG_ARMOR_BLE_SETUP_MINUTES`（默认 10，范围 1 到 120） |
| 拿到电路板的人读取存储的机密 | 可选（默认关闭，开关为 `security.seal_secrets` 或服务器的 `nodes.nvs_encryption`）：用 ChaCha20-Poly1305 保护密码、令牌、密钥、PIN 和哈希。它不加密闪存也不烧写 eFuse：密钥位于同一芯片的分区中，因此无法防御拿到电路板的人 |
| 与代理的明文连接 | 可选 TLS（`mqtts://`），使用所有者自己的证书颁发机构（PEM 最大 2 KB）并检查名称；默认关闭。握手只在主机测试中运行过，没有在电路板上运行 |
| 猜测面板 PIN | 比较不会泄露差异所在；连续失败会把面板锁定 `lockout_s`（默认 60 秒，1 到 3600），前三次锁定分别乘以 1、5、15，即使输入正确 PIN 也会拒绝；进行中的锁定在重启后会恢复；布防不需要 PIN，撤防和静音需要 |
| 让住宅撤防的重启 | 模式、进行中的报警和被排除的区域都保存在闪存中并会恢复；断线或短路的回路绝不会被排除；撤防时触发的烟雾、紧急和防拆报警在重启后会重新出现；BOOT 重置只在面板已撤防时才擦除 |
| 无法读取的区域看起来是闭合的 | 显示 `trouble`（转换器无法启动或连续 5 轮无读数），除非强制，否则面板不会带着它布防，强制后它会被排除并记录；正在鸣响的烟雾或紧急探测器绝不会被排除 |
| 无人授权的服务器命令 | 来自服务器的布防、撤防和静音需要三个开关（节点设置、代理的访问列表和服务器上的 `ARMOR_ALARM_COMMANDS=1`），服务器在发送撤防或静音前会询问用户的 PIN，且一切默认关闭；警笛继电器只属于报警本身（代理、中心和面板都无法驱动它） |
| 伪造的 Zigbee 消息 | 主题 `armor/alarm/<id>/zigbee` 携带用面板密钥（在面板中输入的 64 位十六进制）对节点、区域、状态、电池和序号计算的 HMAC-SHA256，且序号只会增加；策略主题携带 `config_hash`、`zone_feed` 及其签名，每一项都可以在节点上固定 |

这不是经过认证的报警系统（[docs/SAFETY.md](docs/SAFETY.md)）：它是家庭项目，从未在电路板或真实安装中运行过。服务器缺失时面板独自决策，若节点或其无线电也失效，则无人监视。报告漏洞请阅读 [SECURITY.md](SECURITY.md)。

## 🔌 接口

| 部分 | 作用 |
|---|---|
| MQTT | `armor/alarm/<id>/state`（不保留：含 ID、名称、类型、状态和旁路的区域，以及阶段）、`/command`、`/result`、`/zigbee` 和 `/policy`（服务器到面板）；`armor/device/<id>/<继电器>/{state,set}`、`armor/health/<id>`、`armor/control/<id>/...` |
| 区域 | 类型为 `instant`、`entry`、`interior` 或 `always`；状态为 `normal`、`triggered`、`tamper` 或 `trouble`；最多 8 个有线回路（带终端电阻的触点由 ADC 读取，一个引脚给出四种状态；以太网电路板有 6 个可用模拟引脚，Wi-Fi 电路板有 8 个）、4 个按钮和 100 条事件日志 |
| Zigbee | 通往协调器的串口桥（ZNP）、IAS Zone 传感器和 IAS WD 警笛；当服务器的 Zigbee2MQTT 沉默超过 `zigbee_silence_min` 时，节点接管无线电，服务器恢复后归还（[docs/ZIGBEE.md](docs/ZIGBEE.md)、[docs/FAILOVER.md](docs/FAILOVER.md)） |

## ⚙️ 配置

| 变量 | 默认值 | 作用 |
|---|---|---|
| `panel.exit_delay_s`, `panel.entry_delay_s` | 30, 30 (0-600) | 离开和进入的延时；离开的人不会被当作入侵者 |
| `panel.lockout_s` | 60 (1-3600) | PIN 锁定时间，前三次分别乘以 1、5、15 |
| `zigbee_silence_min` | - | 服务器的 Zigbee 可沉默多久，之后节点接管无线电 |
| `CONFIG_ARMOR_NODE_ID`, `_SETUP_CODE`, `_FLEET_SECRET`, `_MQTT_*`, `_HEARTBEAT_S`, `_BLE_SETUP_MINUTES`, `_BOOT_FACTORY_RESET` | - | 所有节点共有的设置：名称（为空则为 `armor-` 加 MAC 的最后字节）、设置码（为空则随机）、机群密钥（16 个字符以上，绝不放入仓库）、最初的代理设置、心跳（2 到 300 秒）、蓝牙设置的分钟数（1 到 120）以及用 BOOT 重置（前 30 秒内按 8 秒） |

静音后的再次鸣响限制为 3 次（`kMaxResounds`）。警笛继电器每次重启时处于断开状态，代理丢失时也不会自动断开。

## 🚀 快速开始

**要求:** 用于主机测试的 C++17 编译器；用于构建镜像的 ESP-IDF 5.5.5 容器；带底板的 ESP32-S3 电路板（以太网或 Wi-Fi）。

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host   # host tests, -Werror
node tools/panel_mock.mjs --user admin:adminpass123   # the panel without a board
tools/build_node.sh generic s3-wifi                  # the image in the ESP-IDF container (or s3-eth)
```

参见[设计](docs/DESIGN.md)、[主机如何接替服务器](docs/FAILOVER.md)和[安全说明](docs/SAFETY.md)。

## 🧪 测试

```bash
cmake -S tests -B build/host && cmake --build build/host && ctest --test-dir build/host
python -I tools/armor_ci_validate.py
```

`ctest` 运行 18 个主机测试程序和 5 个 Python 检查；主机程序统计的检查共 488930 项，使用 `-Werror` 构建。

Python 检查：八种语言的面板文字、任务、保存路径、受锁保护的蓝牙会话以及面板调用的路由（部分节点还包括 HTTP 状态码或字体字形）

主机测试需要任意 C++17 编译器（Linux、WSL、MSYS2）；网页面板还会在真实浏览器中对着模拟节点（`tools/panel_mock.mjs`、`tools/panel_browser_test.mjs`）进行验证。

大部分检查是对面板规则的穷举遍历；回路、警笛和 Zigbee 连接都是替身。

## 🏗️ 构建与发布

`tools/build_node.sh` 在 ESP-IDF 5.5.5 容器中构建镜像，发布版带有镜像、其 SHA-256 和 Ed25519 签名，每块电路板一份；镜像第一次通过 USB-C 写入，之后从面板更新。

## 🩺 故障排除

| 现象 | 原因 | 处理 |
|---|---|---|
| 忘记了密码，或节点不再加入网络 | 处于设置状态且 90 秒后仍无地址的节点会开启自己的 Wi-Fi `ARMOR-SETUP-xxxxxx`（打开 `http://192.168.4.1/`） | 要重新开始，请在上电后 30 秒内按住 BOOT 8 秒（会清除设置和用户），或使用面板中的*恢复出厂设置* |
| 来自 GitHub 的更新被拒绝 | 发布版缺少校验和或签名，或二者不匹配（`e_no_checksum`、`e_no_signature`、`e_checksum_mismatch`、`e_bad_signature`），没有适用于此电路板的镜像（`e_no_asset_for_board`），或属于另一个项目（`e_wrong_firmware`） | 节点未发生任何变化；请重试，或由管理员手动上传文件 |
| 面板无法布防 | 某个区域处于 `trouble`（转换器无法启动或连续 5 轮无读数） | 修复接线，或强制布防（该区域随后被排除并记录） |
| 即使 PIN 正确也被拒绝 | 多次失败后面板被锁定（即使正确 PIN 也会被拒绝） | 等待 `lockout_s`；锁定在重启后仍然有效 |
| BOOT 重置没有反应 | 面板处于布防状态：重置只会擦除已撤防的面板 | 先用 PIN 撤防 |

## 📂 仓库结构

<p align="center">
  <img src="images/repository-structure.svg" alt="ARMOR-NODE-ALARM 的仓库结构" width="100%">
</p>

<details>
<summary>目录树的文本版</summary>

```text
ARMOR-NODE-ALARM/
├── main/                                固件：一个 ESP-IDF 组件
├── core/                                不依赖 ESP-IDF 的逻辑，在计算机上测试
├── panel/                               嵌入固件的网页面板
├── tests/                               主机测试和 Python 检查
├── docs/                                11 份指南：安全、消息、Zigbee、故障接管
├── tools/                               构建、打包和检查工具
├── .github/                             CI 工作流以及 Issue 和 Pull Request 模板
├── images/                              横幅、图标和本 README 的两幅图
├── build.sh build.bat                   通过共享的项目工作流构建
├── test.sh test.bat                     通过共享工作流运行测试
├── run.sh run.bat                       通过共享的项目工作流启动
├── armor.project.json                   清单：版本、角色、成熟度、构建命令
├── CHANGELOG.md                         每个版本的变化
├── SECURITY.md SUPPORT.md LICENSE       安全策略、支持和许可证
├── CONTRIBUTING.md CODE_OF_CONDUCT.md   如何贡献与行为准则
├── CMakeLists.txt                       ESP-IDF 项目
├── partitions.csv                       闪存分区表
├── sdkconfig.defaults                   固件设置
├── sdkconfig.board.s3-eth               某块电路板的设置
├── sdkconfig.board.s3-wifi              某块电路板的设置
├── secrets/                             节点文件示例（真实文件不纳入版本管理）
├── build-test.sh build-test.bat         通过共享工作流运行测试
└── .gitattributes .gitignore            Git 设置
```

</details>

## 🗺️ 路线图与已知限制

**目前已知的限制**

* 节点自身与协调器（本地或网络上）的 Zigbee 通信只对照模拟协调器测试过；从未遇到过真实的协调器或传感器。
* 网页面板已在真实浏览器中对模拟节点测试过，尚未在板子上测试。
* 尚未制作板子，没有接线的防区，也没有连接警笛或 Zigbee 协调器：还没有在真实的安装中运行过。
* 带锁定的 PIN、设置码、登录限制和签名更新已在主机上测试；没有经过独立审查，这也不是经过认证的报警系统。
* 哈希和 Ed25519 签名检查以及回滚逻辑在主机上通过；空中更新从未在板子上运行过。
* 警笛和继电器由报警规则驱动，并对照模拟测试过；都没有连接到真实的警笛或继电器。

**下一步**

* 连接真实的回路和警笛，用真实的协调器和传感器试验 Zigbee 连接，并在真实安装中运行面板，之后才能做出更多声明。

## 🌍 语言

README 和文本提供英语、西班牙语、法语、意大利语、德语、汉语、日语和欧洲葡萄牙语版本。

## 🔗 相关项目

**A.R.M.O.R.**（Autonomous Radar & Multimodal Observation Range）是一个由相互独立的仓库组成的周界安全系统。每个仓库都有自己的版本、自己的测试和自己的 README；这是整个系列：

* **[ARMOR-COMMON](https://github.com/JuanenRac/ARMOR-COMMON)** - 消息契约、验证器、一致性向量和生成的类型
* **[ARMOR-NODE-RADAR](https://github.com/JuanenRac/ARMOR-NODE-RADAR)** - 面向 ESP32-S3 的现场节点固件，配三个雷达和自带的网页面板
* **[ARMOR-NODE-SOLAR](https://github.com/JuanenRac/ARMOR-NODE-SOLAR)** - 太阳能逆变器和电池的协议，以及网关节点的消息
* **[ARMOR-NODE-ELECTRICAL](https://github.com/JuanenRac/ARMOR-NODE-ELECTRICAL)** - 电气节点：电表、电网读数的消息和切换规则
* **ARMOR-NODE-ALARM**（本仓库） - 报警节点与面板：防区、布防、延时、警笛和 PIN，可配合服务器也可独立使用
* **[ARMOR-NODE-HMI](https://github.com/JuanenRac/ARMOR-NODE-HMI)** - 触摸面板：壁挂屏幕上的系统状态、布防与确认，以及语音助手的所在
* **[ARMOR-NETWORK](https://github.com/JuanenRac/ARMOR-NETWORK)** - 本地网络：其设备、互联网以及变化
* **[ARMOR-SERVER](https://github.com/JuanenRac/ARMOR-SERVER)** - 中央协调器：遥测、报警、设备、太阳能读数和摄像头
* **[ARMOR-STUDIO](https://github.com/JuanenRac/ARMOR-STUDIO)** - 网页控制台：摄像头、雷达、报警、太阳能以及 2D/3D 场地设计器
* **[ARMOR-ANDROID-CONTROL](https://github.com/JuanenRac/ARMOR-ANDROID-CONTROL)** - 带实时 2D/3D 雷达的 Android 操作员客户端
* **[ARMOR-IOS-CONTROL](https://github.com/JuanenRac/ARMOR-IOS-CONTROL)** - iPhone 操作员客户端
* **[ARMOR-SERVER-AI](https://github.com/JuanenRac/ARMOR-SERVER-AI)** - 视觉推理策略，会解释其决策且从不驱动设备
* **[ARMOR-VISION](https://github.com/JuanenRac/ARMOR-VISION)** - 面向 Jetson Orin NX 的摄像头区域、遮罩和检测规则
* **[ARMOR-VOICE-AI](https://github.com/JuanenRac/ARMOR-VOICE-AI)** - 离线语音意图，附带无法伪造的确认
* **[ARMOR-HARDWARE](https://github.com/JuanenRac/ARMOR-HARDWARE)** - 外壳、电子器件和台架验收矩阵
* **[ARMOR-DEVOPS](https://github.com/JuanenRac/ARMOR-DEVOPS)** - 部署、CM5 测试台、备份和 TLS
* **[ARMOR-SIMULATOR](https://github.com/JuanenRac/ARMOR-SIMULATOR)** - 带可重复故障的离线遥测模拟器
* **[ARMOR-UPDATER](https://github.com/JuanenRac/ARMOR-UPDATER)** - 检测、安装和更新生态系统自身的仓库
* **[ARMOR-DOCS](https://github.com/JuanenRac/ARMOR-DOCS)** - 架构、安全基线和能力矩阵

## 📚 文档

* [安全](docs/SAFETY.md)：面板承诺什么、不承诺什么
* [报警消息](docs/ALARM_MESSAGES.md)、[Zigbee](docs/ZIGBEE.md)和[故障接管](docs/FAILOVER.md)
* [设计](docs/DESIGN.md)、[硬件](docs/HARDWARE.md)、[底板](docs/BASE_BOARD.md)和[节点固件](docs/NODE_FIRMWARE.md)
* [台架测试](docs/BENCH_TESTS.md)、[设置码](docs/SETUP_CODE.md)和[蓝牙配置](docs/BLE_PROVISIONING.md)
* [能力矩阵：哪些已被证明，哪些没有](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/CAPABILITY_MATRIX.md)
* [项目目录：版本以及各仓库之间如何相互依赖](https://github.com/JuanenRac/ARMOR-DOCS/blob/main/docs/PROJECT_CATALOG.md)
* [本仓库的更新日志](CHANGELOG.md)

## 🤝 参与贡献与社区

* 提交 Pull Request 之前请运行 `ctest --test-dir build/host` and `python -I tools/armor_ci_validate.py`：必须通过。参见 [CONTRIBUTING.md](CONTRIBUTING.md) 和[行为准则](CODE_OF_CONDUCT.md)。
* README 提供八种语言，由 ARMOR-DOCS 中的源文件生成：文本的修改在那里进行。
* 组件之间消息的变更从 ARMOR-COMMON 开始。
* 不要把仅仅构建过的东西描述为已在板子上运行。
* 请私下报告漏洞：[SECURITY.md](SECURITY.md)。问题、想法和报告：electrohobby3d@gmail.com（[SUPPORT.md](SUPPORT.md)）。

## 👤 作者

**JuanenRac (Electro Hobby 3D)** · electrohobby3d@gmail.com

## 📜 许可证

GPL-3.0-or-later - 见 [LICENSE](LICENSE)。
