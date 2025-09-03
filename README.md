ESP32-S3 + LovyanGFX SPI TFT 示例

一个使用 LovyanGFX 驱动 SPI TFT 屏的 PlatformIO 工程，已将引脚宏统一为通用的 `TFT_*` 命名，便于在不同面板与接线间切换。

特性
- 使用 Arduino 框架与 LovyanGFX（lib_deps = lovyan03/LovyanGFX）。
- 通过 `platformio.ini` 的 build_flags 向代码传入屏幕尺寸、偏移、SPI 引脚、背光等参数。
- 适配 ESP32 系列（自动选择合适的 SPI host；对 S3/C3 进行了处理）。
- 串口日志默认走 USB CDC，波特率 115200。

快速开始
- 安装 VS Code + PlatformIO 扩展。
- 使用 USB-C 数据线连接开发板（ESP32-S3-DevKitM-1 或兼容）。
- 根据你的接线，编辑 `platformio.ini` 中 `[common]` 下的 `tft_*` 参数（见下文）。
- 构建与烧录：
  - 命令行：`pio run -e esp32-s3-devkitm-1 -t upload`
  - 打开串口监视器（115200）：`pio device monitor -b 115200`

重要文件
- src/main.cpp：示例程序，打印日志、初始化屏幕并绘制简单色条。
- src/lgfx_setup.hpp：LovyanGFX 硬件配置（SPI、面板、背光）。
- platformio.ini：工程配置（目标板、依赖库、构建宏、串口参数）。

参数配置（platformio.ini -> [common]）
- tft_width/tft_height：屏幕逻辑尺寸（像素）。
- tft_offset_x/tft_offset_y：面板偏移（某些面板在特定方向存在可视区域偏移）。
- tft_freq：SPI 写频率，默认 40000000（40MHz）。
- tft_mosi/tft_sclk/tft_miso：SPI 引脚；若 tft_miso = -1 则使用 3-wire SPI。
- tft_cs/tft_dc/tft_rst：CS、D/C、RESET 引脚（若硬连到固定电平可设为 -1）。
- tft_bl：背光引脚（若固定背光，可设为 -1）。
- tft_bl_active_high：背光有效电平（1=高电平点亮；0=低电平点亮）。

这些参数通过以下通用宏传入代码：
- 尺寸类：TFT_WIDTH/TFT_HEIGHT/TFT_OFFSET_X/TFT_OFFSET_Y/TFT_FREQ
- 引脚类：TFT_MOSI/TFT_MISO/TFT_SCLK/TFT_CS/TFT_DC/TFT_RST/TFT_BL/TFT_BL_ACTIVE

触摸与亮度 UI
- 已内置基于 LovyanGFX 的触摸读取逻辑（XPT2046 可选）。
- 屏幕顶部会渲染 “- / +” 两个按钮，点击调整背光亮度（0..255）。
- 启用方法：在 `platformio.ini` 的 `[common]` 设置 `touch_xpt2046 = 1`，并设置 `touch_cs`（可选 `touch_irq`）。
- 旋转与校准：若坐标方向不正确，可调整 `touch_rotation`（0..3）；如范围不匹配，可在 `src/lgfx_setup.hpp` 中细调 `x_min/x_max/y_min/y_max`。

面板型号
当前在 `src/lgfx_setup.hpp` 中使用的是 `lgfx::Panel_ILI9341`。如你的屏是 ST7789 或其他型号，请将面板类替换为对应的 LovyanGFX 面板类，例如：

```cpp
// src/lgfx_setup.hpp 中：
lgfx::Panel_ST7789 _panel;     // 若使用 ST7789
// lgfx::Panel_ILI9341 _panel; // 原 ILI9341 示例
```

必要时调整：
- invert：有些面板需要反色显示（true/false）。
- rgb_order：若出现色彩交换，尝试改变 RGB 顺序（true/false）。
- offset_x/offset_y：根据面板寻址对齐需要微调。

串口日志
- 本项目已启用 USB CDC：ARDUINO_USB_MODE=1 与 ARDUINO_USB_CDC_ON_BOOT=1。日志默认从 USB 虚拟串口输出（115200）。
- 若串口监视器没有日志：
  - 确认选择了正确的 COM 口（Windows 设备管理器查看）。
  - 确认数据线可传输数据（非仅充电线）。
  - 打开监视器后按一次板上的 RST 键。
  - 也可在 platformio.ini 指定：monitor_port = COMx。

若使用外部 USB-UART 模块而非 USB CDC，可改用 UART0 输出：

```cpp
// 例如在 setup() 中：
Serial0.begin(115200, SERIAL_8N1, 43, 44); // ESP32-S3: RX=GPIO44, TX=GPIO43（按需修改）
// 并将日志调用从 Serial 切换到 Serial0
```

并可移除 ARDUINO_USB_CDC_ON_BOOT 相关宏，避免重复枚举串口。

运行效果
- 串口输出：初始化信息、引脚配置、屏幕尺寸、循环中的坐标打印。
- 屏幕显示：清屏 + 文本标题 + 分栏色条 + 一个运动的小矩形（证明刷新正常）。

常见问题
- 颜色反了或偏色：在 `src/lgfx_setup.hpp` 的面板配置中切换 invert 或 rgb_order。
- 无显示：检查 TFT_CS/TFT_DC/TFT_RST/TFT_BL 是否连接正确、是否需要上拉/下拉；确认 SPI 频率是否过高。
- 屏幕方向：调用 `lcd.setRotation(0..3)` 切换方向。
- 3-wire/4-wire：若未接 MISO（-1），代码会自动启用 3-wire；若接了 MISO，则使用 4-wire。

许可证
此示例未附加特定许可证，按需要自行添加许可证条款。
