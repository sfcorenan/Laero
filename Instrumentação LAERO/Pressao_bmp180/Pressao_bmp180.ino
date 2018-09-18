
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
        Serial.println("nao foi encontrando o bmp180, favor verificar as conexoes!");
  }
}
  
void loop() {
  Serial.print("Temperatura no BMP180= ");
  Serial.print(bmp.readTemperature());
  Serial.println(" oC");
    
  Serial.print("Pressao no BMP180 = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude no BMP180= ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");

  Serial.print("Pressao no nivel do mar no BMP180 = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print("Altitude real no BMP180 = ");
  Serial.print(bmp.readAltitude(102000));
  Serial.println(" meters");
    
  Serial.println();
  delay(2000);
}
