# MiniCar-STM32

`MiniCar-STM32` 是迷你小车数字孪生演示项目的 STM32 下位机固件。

它与重构后的 PC 上位机 `CarTwinViewer-PC` 配套使用：STM32 负责实体小车按键、电机、LED、蜂鸣器和 USART2 通信；PC 负责显示道路场景、障碍车和仪表盘。

## 当前功能

- 初始化 STM32F103C8 外设：按键、LED、蜂鸣器、电机、USART2。
- 手动驾驶：按左转/右转按键后，小车切换车道，并把坐标回传给 PC。
- 自动驾驶：接收 PC 发送的障碍车坐标，根据当前车道和障碍车位置选择避让车道。
- 模式切换：模式按键在手动/自动驾驶之间切换；PC 也可以通过 `%MODE=0/1$` 切换模式。
- telemetry 回传：周期性向 PC 发送心跳、坐标、收发帧计数、估算转速、MCU 状态位和 STM32 自身供电电压估算值。

## 硬件平台

- MCU：STM32F103C8
- 串口：USART2
- USART2 TX：PA2
- USART2 RX：PA3
- 波特率：9600
- Keil target：`MiniCar`
- 输出 hex：`OBJ\Template.hex`

按键引脚：

| 功能 | 引脚 |
| --- | --- |
| 模式键 | PC14 |
| 蜂鸣/灯光键 | PC15 |
| 左转键 | PB1 |
| 右转键 | PB0 |

## 目录结构

```text
HAL/       外设驱动，包括 UART、MOTOR、LED、KEY、BEEP
SYS/       系统支持代码和 delay
USER/      主循环、小车状态、协议常量和业务逻辑
MDK/       Keil MDK 工程文件
OBJ/       Keil 输出目录，构建后生成 hex / axf / map 等文件
```

## 编译

推荐使用 Keil MDK 5：

1. 打开 `D:\git\MiniCar-STM32\MDK\MiniCar.uvprojx`
2. 选择 target：`MiniCar`
3. 执行 `Project -> Rebuild all target files`
4. 构建成功后输出文件在：

```text
D:\git\MiniCar-STM32\OBJ\Template.hex
```

如果已经把 Keil `UV4.exe` 加入 PATH，也可以使用命令行构建：

```powershell
cd D:\git\MiniCar-STM32\MDK
UV4.exe -b .\MiniCar.uvprojx -t "MiniCar" -j0 -o ..\OBJ\build.log
```

如果没有加入 PATH，可使用常见安装路径：

```powershell
cd D:\git\MiniCar-STM32\MDK
& "C:\Keil_v5\UV4\UV4.exe" -b .\MiniCar.uvprojx -t "MiniCar" -j0 -o ..\OBJ\build.log
```

## 烧录

可以直接在 Keil 中使用 ST-LINK 下载：

1. 连接 ST-LINK 到 STM32
2. 确认 Keil debug/download 选择 ST-LINK
3. 点击 `Download`

如果安装了 STM32CubeProgrammer CLI，也可以烧录 hex：

```powershell
cd D:\git\MiniCar-STM32
STM32_Programmer_CLI -c port=SWD -w .\OBJ\Template.hex -v -rst
```

## 串口连接

推荐使用 USB-TTL 连接 PC：

```text
USB-TTL TX -> STM32 PA3 RX
USB-TTL RX -> STM32 PA2 TX
USB-TTL GND -> STM32 GND
```

串口参数：

```text
9600 baud, 8 data bits, 1 stop bit, no parity
```

注意：TX/RX 要交叉连接，GND 必须共地。不要把 5V 直接接到 STM32 的串口 IO。

## 串口协议

STM32 接收 PC 的障碍车坐标：

```text
%xxx\0yyy$
```

STM32 接收 PC 的模式命令：

```text
%MODE=0$  自动驾驶
%MODE=1$  手动驾驶
```

STM32 回传 telemetry：

```text
%X=10,Y=420,HB=1,RX=10,TX=20,RL=2100,RR=2100,V=3300,ST=7,PC=1,AGE=3$
```

字段说明：

| 字段 | 含义 |
| --- | --- |
| `X` / `Y` | 当前主车坐标 |
| `HB` | STM32 心跳计数 |
| `RX` / `TX` | STM32 侧接收/发送帧计数 |
| `RL` / `RR` | 左/右电机估算转速 |
| `V` | STM32 自身 VDDA/3.3V 供电估算值，单位 mV |
| `ST` | MCU 状态位，`0x04` 表示自动驾驶开启 |
| `PC` | STM32 认为 PC 链路是否在线 |
| `AGE` | 距离上次收到 PC 帧的周期数 |

说明：`V` 是通过内部 `Vrefint` 反推的 MCU 供电估算值，不是电池电压，也不是电机供电电压。若要读取电池电压，需要额外分压电路接入 ADC。

## 使用流程

1. 编译并烧录 STM32 固件。
2. 用 USB-TTL 连接 USART2。
3. 启动 `CarTwinViewer-PC`。
4. PC 自动扫描串口并开始发送障碍车坐标。
5. STM32 收到障碍车坐标后，在自动模式下判断是否需要避让。
6. STM32 回传坐标和 telemetry，PC 同步显示主车位置和仪表盘状态。

## 调试提示

- 按模式键后，两侧第 3 个流水灯亮表示自动模式；两侧第 1 个流水灯亮表示手动模式。
- 如果 PC 有 TX 但没有 RX，优先检查 PA2/PA3 是否接反、GND 是否共地、固件是否已烧录最新版本。
- 如果按键完全无效，先确认按键引脚电平和上拉配置，再看 `get_key()` 是否进入按下状态。
- 如果 PC 自动驾驶状态不更新，检查 STM32 回传帧中的 `ST` 字段是否包含 `0x04`。
- `MDK\MiniCar.uvoptx` 是 Keil 用户配置文件，通常不需要提交。

## 配套项目

- PC 上位机：`CarTwinViewer-PC`
- `CarTwinViewer-PC` 是原始 CarTwinViewer 上位机的 SDL2/C++ 重构版。
