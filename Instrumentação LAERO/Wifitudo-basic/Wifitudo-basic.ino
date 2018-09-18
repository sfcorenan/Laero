#include <WiFi.h>
const char* ssid     = "Laeromec";
const char* password = "aerodromo";
WiFiServer server(80);

/* DHT Temperature and Humidity Sensor */

#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

#include "DHT.h"
#define DHTPIN 23  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float localHum = 0;
float localTemp = 0;

int sensorPin = 12;   
int sensorValue = 0; 
float Vout=0;
float P=0;
int i=0;

/* LED */
#define LED_PIN 2

//Analog Input
#define ANALOG_PIN_0 36
int analog_value = 0;

void connectWiFi(void)
{
  Serial.println();
  Serial.println();
  Serial.print("Conectando em: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Coloque: ");
  Serial.print(WiFi.localIP());
  Serial.println(" , para acessar as informacoes");
  
  server.begin();
  for(i=0;i<10;i++){
  digitalWrite(LED_PIN, HIGH);
  delay (500);
    digitalWrite(LED_PIN,LOW);
  delay (500);
  }
}

void getDHT()
{
  float tempIni = localTemp;
  float humIni = localHum;
  localTemp = dht.readTemperature();
  localHum = dht.readHumidity();
  if (isnan(localHum) || isnan(localTemp))   // Check if any reads failed and exit early (to try again).
  {
    localTemp = tempIni;
    localHum = humIni;
    return;
  }
}


void WiFiLocalWebPageCtrl(void)
{
  WiFiClient client = server.available();   // listen for incoming clients
  //client = server.available();
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
  
            // the content of the HTTP response follows the header:
            //WiFiLocalWebPageCtrl(); 
              client.print(" A temperature pelo DHT11 eh: ");
              client.print(localTemp);
              client.print("  oC<br>");
              client.print(" A umidade pelo DHT11 eh:     ");
              client.print(localHum);
              client.print(" % <br>");
              client.print("<br>");
              
              client.print("Temperatura no BMP180= ");
              client.print(bmp.readTemperature());
              client.println(" oC <br>");
              client.print("Pressao no BMP180 = ");
              client.print(bmp.readPressure());
              client.print(" Pa <br>");
              client.print("Altitude no BMP180= ");
              client.print(bmp.readAltitude());
              client.print(" m <br>");
              client.print("Pressao no nivel do mar no BMP180 = ");
              client.print(bmp.readSealevelPressure());
              client.print(" Pa<br> ");
              client.print("Altitude real no BMP180 = ");
              client.print(bmp.readAltitude(102000));
              client.print(" m<br>");
              client.print("<br>");
              
              
              client.print("Clique <a href=\"/H\">Aqui</a> para ligar o led.<br>");
              client.print("Clique <a href=\"/L\">Aqui</a> para desligar o led.<br>");         

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_PIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_PIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
  delay(10);
  
  connectWiFi();
 
  dht.begin();

   if (!bmp.begin()) {
        Serial.println("nao foi encontrando o bmp180, favor verificar as conexoes!");
  }

  
}

int value = 0;

void loop()
{

  getDHT();
  WiFiLocalWebPageCtrl();
  
}

