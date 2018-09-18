int sensorPin = A0;   
int sensorValue = 0.0; 
float Vout=0.0;
float P=0.0;
float velo=0.0;
float offset=0.0;
void setup() {

  // declare the ledPin as an OUTPUT:
   Serial.begin(9600); 

       int i=0;
    int sum=0.0;
    int offset=0.0;
    Serial.println("init...");
    for(i=0;i<20;i++)
    {
         sensorValue = analogRead(sensorPin)-512;
         sum+=sensorValue;
    }
    offset=sum/20.0;
    Serial.println("Ok");
}

void loop() {
 
       sensorValue = analogRead(sensorPin)-offset; 
       Vout=(5*sensorValue)/1024.0;
       P=Vout-2.5; 
       velo=sqrt(2*P*1000/1.2928);          
       Serial.print("Presure = " );                       
       Serial.print(P*1000); 
       Serial.println("Pa");
       Serial.print("velocidade = " );                       
       Serial.print(velo); 
       Serial.println("m/s");
       Serial.println();
       delay(1500);   
    
}
