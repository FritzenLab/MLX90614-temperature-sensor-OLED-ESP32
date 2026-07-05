/***************************************************
  MLX90614 sensor library
  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license.
  OLED display info from here https://fritzenlab.net/2025/01/03/tiny-0-49-oled-display/
 ****************************************************/

#include <Adafruit_MLX90614.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <esp_system.h> // Required for esp_random()

#define LED 2

U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float t= 0;
uint32_t sensorTime = 0;
uint32_t ledTime = 0;
bool ledStatus = false;
const char* varToPrint = " ";
uint32_t ledPreviousMillis = 0;

// Avoids modulo bias using rejection sampling
uint32_t randomBounded(uint32_t bound) {
  if (bound == 0) return 0;  // avoid division by zero

  uint32_t x;
  const uint32_t limit =
    UINT32_MAX - ((UINT32_MAX + 1ULL) % bound);

  // Reject values in the incomplete top interval so every output
  // has exactly the same probability (avoids modulo bias).
  do {
    x = esp_random();
  } while (x >= limit);  // Retry until unbiased

  return x % bound;
}

// Returns a random number in the range [minVal, maxVal)
uint32_t randomRange(uint32_t minVal, uint32_t maxVal) {
  if (maxVal <= minVal) return minVal;  
  return minVal + randomBounded(maxVal - minVal);
}

void setup() {
  pinMode(LED, OUTPUT);
  u8g2.begin();
  // Setting the display i2c clock to 100kHz is necessary for the MLX90614
  // sensor to work, since it communicates on that speed only. 
  // Otherwise the 0.49" OLED display would use 400kHz.
  u8g2.setBusClock(100000);
  u8g2.setFont(u8g2_font_10x20_tf);
  Serial.begin(115200);
  while (!Serial);

  Serial.println("MLX90614 + OLED");

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
  Wire.setClock(100000); // this is only necessary at power up, later
  // "u8g2.setBusClock(100000);" above assumes and corrects the issue
  ledTime = randomRange(80, 800);
}

void loop() {

  if(millis() - sensorTime > 1000){
    sensorTime += 1000;
    t= mlx.readObjectTempC();
    /*Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
    Serial.print("*C\tObject = "); Serial.print(t); Serial.println("*C");
    Serial.println();*/
    
    u8g2.clearBuffer();			
    // String() constructor with decimal-places argument — Arduino String reference
    // https://www.arduino.cc/reference/en/language/variables/data-types/string/
    String tempStr = String(t, 1) + "C";
    u8g2.setCursor(0, 20);
    varToPrint = tempStr.c_str();
    u8g2.drawStr(5, 20, varToPrint);
    // this print is just to get the printable string width, 
    // it helped me and Claude.ai pick the best font
    //Serial.println(u8g2.getStrWidth(varToPrint));
    u8g2.sendBuffer();	
  }

  if(millis() - ledPreviousMillis > ledTime){
    ledPreviousMillis = millis();
    ledTime = randomRange(80, 800);
    ledStatus = !ledStatus;
    digitalWrite(LED, ledStatus);
    Serial.println(ledTime);  
  }
}
