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

#define LED 2

U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float t= 0;
unsigned long sensorTime = 0;
unsigned long ledTime = 0;
bool ledStatus = false;

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
  Wire.setClock(100000);
}

void loop() {

  if(millis() - sensorTime > 1000){
    sensorTime += 1000;
    t= mlx.readObjectTempC();
    Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
    Serial.print("*C\tObject = "); Serial.print(t); Serial.println("*C");
    Serial.println();
    
    u8g2.clearBuffer();			
    // String() constructor with decimal-places argument — Arduino String reference
    // https://www.arduino.cc/reference/en/language/variables/data-types/string/
    String tempStr = String(t, 1) + "C";
    u8g2.setCursor(0, 20);
    const char* myCharPointer = tempStr.c_str();
    u8g2.drawStr(5, 20, myCharPointer);
    Serial.println(u8g2.getStrWidth(myCharPointer));
    u8g2.sendBuffer();	
  }
  if(millis() - ledTime > 300){
    ledTime += 300;
    ledStatus= !ledStatus;
    digitalWrite(LED, ledStatus);
  }
}
