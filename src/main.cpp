#include <Arduino.h>
#include "lgfx_setup.hpp"

LGFX lcd;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ST7789 display test...");
  
  // Print pin configuration
  Serial.printf("Pin configuration:\n");
  Serial.printf("MOSI: %d, SCLK: %d, CS: %d, DC: %d, RST: %d, BL: %d\n", 
                ST7789_MOSI, ST7789_SCLK, ST7789_CS, ST7789_DC, ST7789_RST, ST7789_BL);
  
  Serial.println("Initializing LCD...");
  if (lcd.init()) {
    Serial.println("LCD initialization successful!");
  } else {
    Serial.println("LCD initialization FAILED!");
  }
  
  Serial.printf("LCD width: %d, height: %d\n", lcd.width(), lcd.height());
  
  lcd.setRotation(0); // 0-3
  
  // Force backlight on using GPIO
  pinMode(ST7789_BL, OUTPUT);
  digitalWrite(ST7789_BL, HIGH);
  Serial.println("Backlight forced ON");
  
  lcd.fillScreen(TFT_BLACK);

  // If backlight is connected via PWM, ensure it is on
  lcd.setBrightness(255);

  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setTextSize(2);
  lcd.setCursor(10, 10);
  lcd.println("LovyanGFX ST7789");
  lcd.setCursor(10, 35);
  lcd.printf("%dx%d\n", ST7789_WIDTH, ST7789_HEIGHT);

  // Simple color bars
  const int w = ST7789_WIDTH / 6;
  lcd.fillRect(0*w, 60, w, ST7789_HEIGHT-60, TFT_RED);
  lcd.fillRect(1*w, 60, w, ST7789_HEIGHT-60, TFT_ORANGE);
  lcd.fillRect(2*w, 60, w, ST7789_HEIGHT-60, TFT_YELLOW);
  lcd.fillRect(3*w, 60, w, ST7789_HEIGHT-60, TFT_GREEN);
  lcd.fillRect(4*w, 60, w, ST7789_HEIGHT-60, TFT_CYAN);
  lcd.fillRect(5*w, 60, w, ST7789_HEIGHT-60, TFT_BLUE);
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
  if (x < 0 || x + 20 >= ST7789_WIDTH) dx = -dx;
  if (y < 60 || y + 20 >= ST7789_HEIGHT) dy = -dy;
  lcd.fillRect(x, y, 20, 20, TFT_WHITE);
  delay(16);
}
