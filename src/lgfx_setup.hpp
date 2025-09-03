// LGFX SPI panel setup using generic pin macros from platformio.ini
#pragma once

#ifndef LGFX_USE_V1
#define LGFX_USE_V1
#endif

#include <LovyanGFX.hpp>

#ifndef TFT_WIDTH
#define TFT_WIDTH 240
#endif
#ifndef TFT_HEIGHT
#define TFT_HEIGHT 240
#endif
#ifndef TFT_OFFSET_X
#define TFT_OFFSET_X 0
#endif
#ifndef TFT_OFFSET_Y
#define TFT_OFFSET_Y 0
#endif
#ifndef TFT_FREQ
#define TFT_FREQ 40000000
#endif

#ifndef TFT_MOSI
#define TFT_MOSI -1
#endif
#ifndef TFT_MISO
#define TFT_MISO -1
#endif
#ifndef TFT_SCLK
#define TFT_SCLK -1
#endif
#ifndef TFT_CS
#define TFT_CS -1
#endif
#ifndef TFT_DC
#define TFT_DC -1
#endif
#ifndef TFT_RST
#define TFT_RST -1
#endif
#ifndef TFT_BL
#define TFT_BL -1
#endif
#ifndef TFT_BL_ACTIVE
#define TFT_BL_ACTIVE 1
#endif

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Bus_SPI _bus;
  lgfx::Panel_ILI9341 _panel;
  lgfx::Light_PWM _light;

public:
  LGFX()
  {
    // ---- SPI bus config ----
    auto bus_cfg = _bus.config();

// ESP32-C3 uses SPI2_HOST, ESP32-S3 uses SPI3_HOST (VSPI)
#if defined(ARDUINO_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C3)
    bus_cfg.spi_host = SPI2_HOST;
#elif defined(ARDUINO_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    bus_cfg.spi_host = SPI3_HOST;  // ESP32-S3 uses SPI3 (VSPI)
#else
#ifdef VSPI_HOST
    bus_cfg.spi_host = VSPI_HOST;
#else
    bus_cfg.spi_host = SPI3_HOST;
#endif
#endif

    // bus_cfg.spi_mode = 0;
    bus_cfg.freq_write = TFT_FREQ;
    bus_cfg.freq_read = 16000000;

    bus_cfg.pin_sclk = TFT_SCLK;
    bus_cfg.pin_mosi = TFT_MOSI;
    bus_cfg.pin_dc = TFT_DC;

    // If MISO is provided, use 4-wire; otherwise 3-wire
    if (TFT_MISO >= 0)
    {
      bus_cfg.pin_miso = TFT_MISO;
      bus_cfg.spi_3wire = false;
    }
    else
    {
      bus_cfg.pin_miso = -1;
      bus_cfg.spi_3wire = true;
    }

    bus_cfg.use_lock = true;
    bus_cfg.dma_channel = 1;

    _bus.config(bus_cfg);
    _panel.setBus(&_bus);

    // ---- Panel config ----
    auto panel_cfg = _panel.config();
    panel_cfg.pin_cs = TFT_CS;   // -1 if not used
    panel_cfg.pin_rst = TFT_RST; // -1 if not used
    panel_cfg.pin_busy = -1;

    panel_cfg.memory_width = TFT_WIDTH;
    panel_cfg.memory_height = TFT_HEIGHT;
    panel_cfg.panel_width = TFT_WIDTH;
    panel_cfg.panel_height = TFT_HEIGHT;
    panel_cfg.offset_x = TFT_OFFSET_X;
    panel_cfg.offset_y = TFT_OFFSET_Y;
    panel_cfg.offset_rotation = 0;

    panel_cfg.readable = false;
    panel_cfg.invert   = true;   // Many ST7789 need invert=on; change if colors look wrong
    panel_cfg.rgb_order = false; // Change to true if your colors are swapped
    panel_cfg.dlen_16bit = false;
    panel_cfg.bus_shared = false;

    _panel.config(panel_cfg);

    // ---- Backlight (optional) ----
    if (TFT_BL >= 0)
    {
      auto light_cfg = _light.config();
      light_cfg.pin_bl = TFT_BL;
      light_cfg.freq = 44100;
      light_cfg.pwm_channel = 7;
      light_cfg.invert = (TFT_BL_ACTIVE == 0);
      _light.config(light_cfg);
      _panel.setLight(&_light);
    }

    setPanel(&_panel);
  }
};
