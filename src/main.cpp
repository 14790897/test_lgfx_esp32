#include <Arduino.h>
#include "lgfx_setup.hpp"

LGFX lcd;

void setup() {
  Serial.begin(115200);
  // 等待USB CDC串口被主机打开（最多3秒），UART时此判断始终为真
  unsigned long t0 = millis();
  while (!Serial && (millis() - t0) < 3000) {
    delay(50);
  }
  Serial.println("Starting TFT display test...");
  Serial.println("ESP32-S3-DevKitM-1 Board");

  // Print pin configuration
  Serial.printf("Pin configuration:\n");
  Serial.printf("MOSI: %d, SCLK: %d, CS: %d, DC: %d, RST: %d, BL: %d\n",
                TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST, TFT_BL);

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
  lcd.setBrightness(255);

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
}

void loop() {
  // Spin a small rectangle to show it's alive
  static int x = 10, y = 100, dx = 2, dy = 2;
  lcd.fillRect(x, y, 20, 20, TFT_BLACK);
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.println(y);
  x += dx; y += dy;
  if (x < 0 || x + 20 >= TFT_WIDTH)
    dx = -dx;
  if (y < 60 || y + 20 >= TFT_HEIGHT)
    dy = -dy;
  lcd.fillRect(x, y, 20, 20, TFT_WHITE);
  delay(16);
}
