# MiniCar-STM32

`MiniCar-STM32` 是迷你小车数字孪生项目的 STM32 下位机固件，运行于
STM32F103C8。它负责实体小车的按键输入、电机控制、灯光与蜂鸣器控制、自动避障
决策、OLED 驾驶仪表，以及与 PC 上位机之间的 USART2 通信。

配套 PC 上位机：
[Yunlk/CarTwinViewer-PC](https://github.com/Yunlk/CarTwinViewer-PC)

> 本仓库中的自动驾驶是基于 PC 发送的障碍车坐标进行的三车道避让演示，不是依赖
> 摄像头、超声波或激光雷达的真实环境感知系统。

## 功能概览

- STM32F103C8 标准外设库工程，使用 Keil MDK 5 构建。
- 手动驾驶：实体按键控制左右换道，坐标平滑变化并同步回 PC。
- 自动驾驶：接收 PC 端障碍车坐标，在危险区域内自动选择避让车道。
- 双向模式切换：模式按键或 PC 串口命令均可切换手动/自动驾驶。
- 小车外设：双电机、左右大灯、两组流水灯、蜂鸣器和四个按键。
- 串口遥测：回传坐标、心跳、收发帧计数、状态位、链路状态和 VDDA 估算值。
- OLED 仪表：显示驾驶模式、车道、PC 链路、VDDA 大号读数和最近收发摘要。
- OLED 热恢复：未检测到屏幕或通信失败后，固件会周期性尝试重新初始化。

## 系统结构

```text
CarTwinViewer-PC
    |
    | USB-TTL / USART2 / 9600 8N1
    | 障碍车坐标、模式命令、主车遥测
    v
STM32F103C8
    |-- KEY       模式、鸣笛/灯光、左转、右转
    |-- MOTOR     左右电机控制
    |-- LED/BEEP  灯光、流水灯、蜂鸣器
    |-- CAR       三车道坐标与自动避障逻辑
    |-- UART      协议解析、遥测和链路状态
    `-- OLED      SSD1306 驾驶仪表
```

## 硬件与软件环境

### 硬件

- MCU：STM32F103C8
- 下载器：ST-LINK，其他兼容 SWD 下载器也可使用
- PC 通信：3.3V 电平 USB-TTL
- OLED：SSD1306，128×64，I2C 地址 `0x3C`
- 小车底板：双电机、按键、流水灯、大灯和蜂鸣器

### 软件

- Keil MDK 5
- STM32F1 Device Family Pack
- ARM Compiler 5 工程配置
- 可选：STM32CubeProgrammer，用于命令行烧录

## 引脚分配

以下表格以当前源码中的宏定义为准。

| 模块 | 功能 | STM32 引脚 | 电气/软件说明 |
| --- | --- | --- | --- |
| USART2 | TX | PA2 | 连接 USB-TTL RX |
| USART2 | RX | PA3 | 连接 USB-TTL TX |
| 大灯 | 左灯 | PA4 | 低电平点亮 |
| 大灯 | 右灯 | PA5 | 低电平点亮 |
| 电机 | 左电机 | PA6 | 推挽输出 |
| 电机 | 右电机 | PA7 | 推挽输出 |
| 右侧流水灯 | LED1～LED5 | PA12、PA11、PA10、PA9、PA8 | 高电平点亮 |
| 左侧流水灯 | LED1～LED5 | PB4、PB5、PB6、PB7、PB8 | 高电平点亮 |
| 按键 | 左转 | PB0 | 内部上拉，按下为低电平 |
| 按键 | 右转 | PB1 | 内部上拉，按下为低电平 |
| OLED | SCL | PB10 | 软件 I2C 时钟 |
| OLED | SDA | PB11 | 软件 I2C 数据 |
| 蜂鸣器 | 控制 | PC13 | 高电平鸣响 |
| 按键 | 模式 | PC14 | 内部上拉，按下为低电平 |
| 按键 | 鸣笛/灯光 | PC15 | 短按鸣笛，长按切换大灯 |

注意：`PB6/PB7` 已被左侧流水灯占用，因此 OLED 没有使用默认 I2C1 引脚，而是
在空闲的 `PB10/PB11` 上实现软件 I2C。

## 硬件接线

### USB-TTL

```text
USB-TTL TX  -> STM32 PA3 / USART2 RX
USB-TTL RX  -> STM32 PA2 / USART2 TX
USB-TTL GND -> STM32 GND
```

串口参数：

```text
9600 baud, 8 data bits, 1 stop bit, no parity, no flow control
```

必须交叉连接 TX/RX，并确保 USB-TTL 与 STM32 共地。STM32F103 的 IO 使用 3.3V
逻辑电平，不要把 5V TTL 信号直接接入 PA2/PA3。

### SSD1306 OLED

```text
OLED VCC -> STM32 3.3V
OLED GND -> STM32 GND
OLED SCL -> STM32 PB10
OLED SDA -> STM32 PB11
```

默认参数：

| 参数 | 默认值 |
| --- | --- |
| 控制器 | SSD1306 |
| 分辨率 | 128×64 |
| I2C 地址 | `0x3C` |
| 刷新周期 | 约 250 ms |

大多数四针 OLED 模块已经带有 SDA/SCL 上拉电阻。如果模块没有上拉，需要分别用
约 `4.7kΩ` 电阻将 SDA、SCL 上拉到 3.3V。SH1106、128×32 或地址为 `0x3D` 的
模块不能直接套用当前默认配置。

## 获取与打开工程

```powershell
git clone https://github.com/Yunlk/MiniCar-STM32.git
cd .\MiniCar-STM32
```

使用 Keil 打开：

```text
MDK\MiniCar.uvprojx
```

Target 名称为 `MiniCar`。

## 编译

### Keil 图形界面

1. 打开 `MDK\MiniCar.uvprojx`。
2. 确认 Target 为 `MiniCar`。
3. 执行 `Project -> Rebuild all target files`。
4. 编译成功后检查 `OBJ\Template.hex`。

### Keil 命令行

如果 `UV4.exe` 已加入 `PATH`：

```powershell
cd .\MDK
UV4.exe -b .\MiniCar.uvprojx -t "MiniCar" -j0 -o ..\OBJ\build.log
```

常见的完整路径写法：

```powershell
cd .\MDK
& "C:\Keil_v5\UV4\UV4.exe" `
  -b .\MiniCar.uvprojx `
  -t "MiniCar" `
  -j0 `
  -o ..\OBJ\build.log
```

构建日志：`OBJ\build.log`

固件文件：`OBJ\Template.hex`

## 烧录

### 使用 Keil 和 ST-LINK

1. 使用 SWDIO、SWCLK、GND 和 3.3V 参考电压连接 ST-LINK。
2. 在 Keil 的 Debug/Utilities 中选择 ST-LINK。
3. 执行 `Flash -> Download`。
4. 下载完成后复位 STM32。

### 使用 STM32CubeProgrammer CLI

```powershell
STM32_Programmer_CLI `
  -c port=SWD `
  -w .\OBJ\Template.hex `
  -v `
  -rst
```

## 快速联调

1. 按照上面的表格连接 OLED、USB-TTL 和 ST-LINK。
2. 在 Keil 中重新编译并烧录最新固件。
3. 复位后，OLED 应显示 `MINICAR / SYSTEM BOOT` 启动画面。
4. 打开配套的 `CarTwinViewer-PC`。
5. 上位机连接 USB-TTL 对应的 COM 口，并开始发送障碍车坐标。
6. OLED 顶部的 `PC OFF` 应变为 `PC ON`，RX 计数开始增加。
7. 按模式键切换自动驾驶，OLED 顶部应从 `MAN` 切换为 `AUTO`。
8. 观察实体小车动作、OLED 坐标和 PC 红车位置是否同步。

## 按键操作

| 按键 | 手动模式 | 自动模式 |
| --- | --- | --- |
| 模式键 PC14 | 切换到自动模式 | 切换到手动模式并取消当前自动换道目标 |
| 鸣笛/灯光键 PC15 短按 | 蜂鸣器鸣响 | 蜂鸣器鸣响 |
| 鸣笛/灯光键 PC15 长按 | 切换左右大灯 | 切换左右大灯 |
| 左转键 PB0 | 请求向左换一条车道 | 当前代码不执行手动换道 |
| 右转键 PB1 | 请求向右换一条车道 | 当前代码不执行手动换道 |

换道不会超出三车道边界。在最左车道按左转、最右车道按右转时不会产生新的换道
目标。

## 三车道坐标

| 车道 | 编号 | 中心 X 坐标 |
| --- | ---: | ---: |
| 左车道 | `LANE_0` | `10` |
| 中间车道 | `LANE_1` | `130` |
| 右车道 | `LANE_2` | `240` |

主车初始状态：

```text
模式：手动
车道：LANE_1
坐标：X=130, Y=420
速度参数：10
```

`car_updatePos()` 每次循环按 `CHANGE(speed) = 2 × speed` 调整 X 坐标，使 PC 上的
红车逐步换道，而不是瞬间跳到目标坐标。

## 自动驾驶逻辑

自动模式下，STM32 使用 PC 最近发送的障碍车坐标进行简单避让：

1. 根据主车和障碍车的 X 坐标判断各自车道。
2. 只处理障碍车 Y 坐标位于约 `160～460` 的前向危险区域。
3. 如果障碍车不在当前车道，主车保持当前车道。
4. 如果当前车道被占用：
   - 左车道或右车道优先切到中间车道；
   - 中间车道切到左车道。
5. 已经存在换道目标时，不重复创建新的目标。

这是确定性演示策略，没有多障碍物建模、道路边界感知、轨迹规划或实体传感器输入。

## OLED 驾驶仪表

屏幕分为三部分：

```text
┌ AUTO ─ L1 ─────── PC ON ┐
│ VDDA      3.30 V         │
│                   MCU    │
├ UART2 9600 8N1 ──────────┤
│ TX:0020 X130 Y420        │
│ RX:0010 X240 Y180        │
└───────────────────────────┘
```

- 顶部：驾驶模式、当前车道和 PC 链路状态。
- 中部：大号 VDDA 估算值。
- 底部：STM32 当前使用的 `UART2`、通信参数、TX/RX 帧计数和数据摘要。

STM32 只能知道自身使用的是 USART2，无法知道 Windows 将 USB-TTL 命名为 `COM3`
还是 `COM7`。若要在 OLED 上显示 Windows COM 名称，需要扩展协议，由 PC 主动发送
端口名称。

OLED 使用 1 KB 绘图缓冲和 1 KB 页面影子缓冲。刷新时只发送发生变化的页面，减少
软件 I2C 对驾驶主循环的影响。OLED 不存在时不会阻止小车启动，固件会约每 2 秒
重试一次初始化。

## VDDA 估算原理

固件不是直接测量电池电压，而是读取 STM32 内部约 1.20V 的 `Vrefint`，再反推
ADC 参考电压 `VDDA`：

```text
ADC_RAW = Vrefint / VDDA × 4095
VDDA_mV = 1200 × 4095 / ADC_RAW
```

例如内部参考通道读数约为 `1489`：

```text
VDDA ≈ 1200 × 4095 / 1489 ≈ 3300 mV
```

该值存在芯片个体差异、ADC 误差、温度漂移和电源噪声，只适合作为 MCU 供电状态
参考。它不是电池电压，也不是电机电源电压。读取电池电压需要额外的电阻分压电路
和外部 ADC 通道。

## 串口协议

### 帧格式

所有帧使用 `%` 开始、`$` 结束：

```text
%payload$
```

STM32 的接收缓冲区为 100 字节。超过缓冲区的数据会被截断，协议本身暂时没有长度、
校验和、序号或转义机制。

### PC -> STM32：障碍车坐标

兼容当前 PC 上位机的旧格式：

```text
%xxx\0yyy$
```

- `xxx`：三位障碍车 X 坐标。
- `\0`：一个真实的 NUL 字节 `0x00`，不是两个可见字符 `\` 和 `0`。
- `yyy`：三位障碍车 Y 坐标。

例如 X=240、Y=180 时，payload 字节为：

```text
32 34 30 00 31 38 30
```

即 ASCII `240`、一个 `00` 字节、ASCII `180`。

### PC -> STM32：驾驶模式

```text
%MODE=0$   自动驾驶
%MODE=1$   手动驾驶
```

模式常量沿用现有工程定义：`CAR_AUTO=0`，`CAR_MANUAL=1`。

### STM32 -> PC：遥测帧

示例：

```text
%X=130,Y=420,HB=82,RX=10,TX=20,RL=1200,RR=1200,V=3300,ST=27,PC=1,AGE=3$
```

| 字段 | 含义 |
| --- | --- |
| `X`、`Y` | 主车当前数字孪生坐标 |
| `HB` | STM32 遥测心跳计数 |
| `RX` | STM32 已接收的结束帧计数 |
| `TX` | STM32 已发送的遥测帧计数 |
| `RL`、`RR` | 左右电机估算转速，不是编码器实测值 |
| `V` | MCU 的 VDDA 估算值，单位 mV |
| `ST` | 位掩码形式的 MCU 状态 |
| `PC` | STM32 判断 PC 链路是否在线，`1` 在线、`0` 离线 |
| `AGE` | 距离最近 PC 帧经过的主循环计数 |

`ST` 状态位：

| 位 | 十六进制 | 含义 |
| ---: | ---: | --- |
| 0 | `0x01` | MCU 正在运行 |
| 1 | `0x02` | PC 链路在线 |
| 2 | `0x04` | 自动驾驶已开启 |
| 3 | `0x08` | 左电机处于运行状态 |
| 4 | `0x10` | 右电机处于运行状态 |

遥测通常约每 100 ms 发送一次。大约 2 秒没有收到 PC 帧时，STM32 会把 PC 链路
判定为离线。

## 目录结构

```text
MiniCar-STM32/
|-- HAL/
|   |-- BEEP/       蜂鸣器驱动
|   |-- KEY/        按键扫描和长按判断
|   |-- LED/        大灯与流水灯驱动
|   |-- MOTOR/      双电机控制及运行状态
|   |-- OLED/       SSD1306、绘图函数和驾驶仪表
|   `-- UART/       USART2、协议解析、遥测和 VDDA 读取
|-- SYS/
|   `-- delay/      SysTick 延时
|-- USER/
|   |-- main.c      初始化和主循环
|   |-- car.c/.h    三车道状态与驾驶逻辑
|   `-- command.h   协议、车道和周期常量
|-- MDK/            Keil 工程文件
|-- OBJ/            编译输出目录
`-- README.md
```

## 常见问题

### OLED 完全不亮

依次检查：

1. 模块是否为 SSD1306 128×64。
2. 地址是否为 `0x3C`。
3. SCL/SDA 是否分别连接 PB10/PB11。
4. 是否共地，供电是否为 3.3V。
5. 模块是否带 I2C 上拉电阻。
6. Keil 工程中是否已经包含 `HAL/OLED` 下的四个文件。

### PC 有发送数据，但 STM32 没有接收

- 检查 USB-TTL TX 是否接 PA3。
- 检查 USB-TTL RX 是否接 PA2。
- 检查 GND 是否共地。
- 检查是否使用 9600 8N1。
- 检查坐标帧中间是否是真实的 NUL 字节。
- 观察 OLED 的 RX 计数是否变化。

### OLED 一直显示 PC OFF

说明 STM32 超时未收到有效结束帧。检查上位机是否连接了正确 COM 口，以及是否持续
发送以 `%` 开始、`$` 结束的数据。

### 模式按键按下后 PC 状态不变化

检查 STM32 遥测帧中的 `ST` 是否包含 `0x04`。模式切换时固件会立即发送一帧遥测，
上位机应根据该状态位更新自动驾驶开关。

### VDDA 显示 0、NO DATA 或数值不稳定

- `NO DATA` 表示 ADC 超时、结果为 0，或数值超出显示保护范围。
- 当前实现是单次采样，可通过多次平均和低通滤波进一步稳定。
- VDDA 是估算值；需要更高精度时应使用万用表校准内部参考常量。

### 按键完全无效

- 检查 PC14、PC15、PB0、PB1 是否被拉低。
- 检查按键是否与工程定义一致。
- 注意旧文档可能把 PB0/PB1 的左右方向写反；当前源码定义为 PB0 左转、PB1 右转。

## 已知限制

- 自动驾驶只处理单个障碍车坐标。
- 没有真实环境传感器输入。
- 串口协议没有 CRC、长度和转义机制。
- 坐标协议包含 NUL 字节，不便于普通串口终端手工发送。
- `RL/RR` 是根据运行状态和速度参数估算的值，不是编码器测量值。
- VDDA 是内部参考源反推值，不是电池电压。
- 电机控制仍包含阻塞式延时，动作期间 OLED 和按键刷新会暂停。
- OLED 软件 I2C 会占用一定 CPU 时间，因此只以低频、变化页方式刷新。

## 后续改进

- 将坐标、模式和控制命令统一为带类型、长度和 CRC 的二进制协议。
- 将蜂鸣器、流水灯和转向动作改为非阻塞状态机。
- 为 VDDA 增加多次采样、滤波和校准参数。
- 增加电池分压采样，显示真实电池电压。
- 增加编码器后，将 `RL/RR` 替换为真实转速。
- 支持多个障碍物和更合理的车道选择策略。
- 增加独立的硬件接线图、协议抓包示例和自动化协议测试。

## 配套仓库

- PC 上位机：[Yunlk/CarTwinViewer-PC](https://github.com/Yunlk/CarTwinViewer-PC)
- STM32 下位机：本仓库

PC 上位机是原始 CarTwinViewer 的 SDL2/C++ 重构版本，两个仓库需要配套使用。
