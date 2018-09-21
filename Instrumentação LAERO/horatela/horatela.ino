#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid     = "Laeromec";
const char* password = "aerodromo";
WiFiServer server(80);

#include <Wire.h>
#include <Adafruit_BMP085.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

Adafruit_BMP085 bmp;

String formattedDate;
String dayStamp;
String timeStamp;

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

    
 // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the table 
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:black; font-weight: bold; background-color: #ffffff; padding: 1px; }");
            
            // Web Page Heading
            client.println("</style></head><body><h1>Aquisicao Laero</h1>");
            
            client.println("<table><tr><th>Medidas</th><th>Parametros</th></tr>");
             // NTC 21/09
         client.println("<tr><td>Hora</td><td><span class=\"sensor\">");
            client.println(timeStamp); 
            client.println(" </span></td></tr>");
            client.println("</body></html>");
            client.println("<tr><td>Data</td><td><span class=\"sensor\">");
            client.println(dayStamp); 
            client.println(" </span></td></tr>");
            client.println("</body></html>");
            
            client.println("<tr><td>Temp. BMP180 s</td><td><span class=\"sensor\">");
            client.println(bmp.readTemperature());
            client.println(" oC</span></td></tr>");  
            client.println("<tr><td>Pressao BMP180</td><td><span class=\"sensor\">");
            client.println(bmp.readPressure());
            client.println(" Pa</span></td></tr>");       
            client.println("<tr><td>Altitude BMP180</td><td><span class=\"sensor\">");
            client.println(bmp.readAltitude());
            client.println(" m</span></td></tr>");
            client.println("<tr><td>Pressao no nivel do mar BMP180</td><td><span class=\"sensor\">");
            client.println(bmp.readSealevelPressure());
            client.println(" Pa</span></td></tr>"); 
            client.println("<tr><td>Altitude real BMP180</td><td><span class=\"sensor\">");
            client.println(bmp.readAltitude(102000));
            client.println(" m</span></td></tr>"); 
             client.println("<tr><td>Temperature DHT11</td><td><span class=\"sensor\">");
            client.println(localTemp);
            client.println(" oc</span></td></tr>");       
             client.println("<tr><td>UmidadeDHT11</td><td><span class=\"sensor\">");
            client.println(localHum);     
            client.println(" %</span></td></tr>");
             client.println("<tr><td>Tubo de pitot</td><td><span class=\"sensor\">");
         // tubo de pitot alteracao 20/09/2018
            client.println(Vout); 
            client.println(" </span></td></tr>");
            client.println("</body></html>");
            
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            client.print("Clique <a href=\"/H\">Aqui</a> para ligar o led.<br>");
            client.print("Clique <a href=\"/L\">Aqui</a> para desligar o led.<br>");    
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
       client.println("<meta http-equiv=\"refresh\" content=\"3\">");  // this line will refresh the webpage every 5 seconds
    client.stop();                                     // to change the refresh rate, change the 5 to any desired number of seconds 
    Serial.println("Client Disconnected.");
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); 
  delay(10);
  
  connectWiFi();

  timeClient.begin();
   timeClient.setTimeOffset(-10800);
 
  dht.begin();

   if (!bmp.begin()) {
        Serial.println("nao foi encontrando o bmp180, favor verificar as conexoes!");
  }

  
}

int value = 0;

void loop()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("TZ");
  dayStamp = formattedDate.substring(0, splitT);

  timeStamp = formattedDate.substring(splitT,formattedDate.length());
  Vout=0;
  for(int i=0;i<100;i++){
Vout += analogRead(ANALOG_PIN_0);
  }
  Vout = Vout/100;
  getDHT();
  WiFiLocalWebPageCtrl();
  
  
}

