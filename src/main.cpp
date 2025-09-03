#include <Arduino.h>
#include "lgfx_setup.hpp"

LGFX lcd;

// UI constants
static const int BTN_H = 40;
static const int BTN_W = 80;
static const int BTN_MARGIN = 8;
static const int TOP_BAR_H = BTN_H + BTN_MARGIN * 2;
static int g_brightness = 255; // 0..255

static void draw_ui()
{
  // Top bar background
  lcd.fillRect(0, 0, lcd.width(), TOP_BAR_H, TFT_DARKGREY);
  // Minus button on the left
  lcd.fillRoundRect(BTN_MARGIN, BTN_MARGIN, BTN_W, BTN_H, 6, TFT_BLACK);
  lcd.drawString("-", BTN_MARGIN + BTN_W / 2 - 4, BTN_MARGIN + 10);
  // Plus button on the right
  lcd.fillRoundRect(lcd.width() - BTN_MARGIN - BTN_W, BTN_MARGIN, BTN_W, BTN_H, 6, TFT_BLACK);
  lcd.drawString("+", lcd.width() - BTN_MARGIN - BTN_W / 2 - 4, BTN_MARGIN + 10);
  // Brightness text in the middle
  lcd.setTextColor(TFT_WHITE, TFT_DARKGREY);
  lcd.setTextSize(2);
  lcd.setCursor(lcd.width() / 2 - 60, BTN_MARGIN + 12);
  lcd.printf("Brightness: %3d", g_brightness);
}

static void apply_brightness()
{
  if (g_brightness < 0)
    g_brightness = 0;
  if (g_brightness > 255)
    g_brightness = 255;
  lcd.setBrightness(g_brightness);
  // update text only
  lcd.setTextColor(TFT_WHITE, TFT_DARKGREY);
  lcd.fillRect(lcd.width() / 2 - 70, BTN_MARGIN + 8, 160, 24, TFT_DARKGREY);
  lcd.setCursor(lcd.width() / 2 - 60, BTN_MARGIN + 12);
  lcd.printf("Brightness: %3d", g_brightness);
}

static bool in_rect(int x, int y, int rx, int ry, int rw, int rh)
{
  return x >= rx && x < rx + rw && y >= ry && y < ry + rh;
}

void setup() {
  Serial.begin(115200);
  // 等待USB CDC串口被主机打开（最多3秒），UART时此判断始终为真
  unsigned long t0 = millis();
  while (!Serial && (millis() - t0) < 3000) {
    delay(50);
  }
  Serial.println("Starting TFT display test...");
  Serial.println("ESP32-S3-DevKitM-1 Board");
  // Print touch config for debugging
#if defined(TOUCH_XPT2046)
  Serial.printf("Touch cfg: XPT2046=%d CS=%d IRQ=%d ROT=%d\n", (int)TOUCH_XPT2046, (int)TOUCH_CS, (int)TOUCH_IRQ, (int)TOUCH_ROTATION);
#endif

  // Print pin configuration
  Serial.printf("Pin configuration:\n");
  Serial.printf("MOSI: %d, SCLK: %d, CS: %d, DC: %d, RST: %d, BL: %d\n",
                TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST, TFT_BL);
#if defined(TOUCH_XPT2046)
  Serial.printf("Touch pins: SCLK=%d, MOSI=%d, MISO=%d, CS=%d, IRQ=%d\n",
                (int)TOUCH_SCLK, (int)TOUCH_MOSI, (int)TOUCH_MISO, (int)TOUCH_CS, (int)TOUCH_IRQ);
#endif

  Serial.println("Initializing LCD...");
  if (lcd.init()) {
    Serial.println("LCD initialization successful!");
  } else {
    Serial.println("LCD initialization FAILED!");
  }
  
  Serial.printf("LCD width: %d, height: %d\n", lcd.width(), lcd.height());
  
  lcd.setRotation(0); // 0-3
  
  // Force backlight on using GPIO
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  Serial.println("Backlight forced ON");
  
  lcd.fillScreen(TFT_BLACK);

  // If backlight is connected via PWM, ensure it is on
  lcd.setBrightness(g_brightness);

  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setTextSize(2);
  lcd.setCursor(10, 10);
  lcd.println("LovyanGFX TFT");
  lcd.setCursor(10, 35);
  lcd.printf("%dx%d\n", TFT_WIDTH, TFT_HEIGHT);

  // Simple color bars
  const int w = TFT_WIDTH / 6;
  lcd.fillRect(0*w, 60, w, TFT_HEIGHT-60, TFT_RED);
  lcd.fillRect(1*w, 60, w, TFT_HEIGHT-60, TFT_ORANGE);
  lcd.fillRect(2*w, 60, w, TFT_HEIGHT-60, TFT_YELLOW);
  lcd.fillRect(3*w, 60, w, TFT_HEIGHT-60, TFT_GREEN);
  lcd.fillRect(4*w, 60, w, TFT_HEIGHT-60, TFT_CYAN);
  lcd.fillRect(5*w, 60, w, TFT_HEIGHT-60, TFT_BLUE);

  // Draw UI layer on top
  draw_ui();
}

void loop() {
  // Touch to adjust brightness
  uint16_t tx, ty;
  if (lcd.getTouch(&tx, &ty))
  {
    // Visual marker to confirm touch is read
    lcd.fillCircle(tx, ty, 3, TFT_YELLOW);
    // Debounce: wait a short moment for stable point
    delay(20);
    // Check buttons
    bool changed = false;
    if (in_rect(tx, ty, BTN_MARGIN, BTN_MARGIN, BTN_W, BTN_H))
    {
      g_brightness -= 16;
      changed = true;
    }
    else if (in_rect(tx, ty, lcd.width() - BTN_MARGIN - BTN_W, BTN_MARGIN, BTN_W, BTN_H))
    {
      g_brightness += 16;
      changed = true;
    }
    if (changed)
    {
      apply_brightness();
      Serial.printf("Touch (%u,%u) -> Brightness %d\n", tx, ty, g_brightness);
      // Wait for release to avoid auto-repeat too fast
      while (lcd.getTouch(&tx, &ty))
      {
        delay(10);
      }
    }
  }
  delay(10);
}
