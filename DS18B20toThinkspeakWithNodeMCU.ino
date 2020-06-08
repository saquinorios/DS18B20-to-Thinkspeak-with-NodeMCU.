/* READING DS18B20 TEMPERATURE SENSOR WITH NODEMCU AND DATA SENDING TO THINKSPEAK
 * 
 * Connections DS18B20 and NodeMCU
 * DS18B20  NodeMCU
 *    GND     G
 *    VCC     3V
 *    SG      D2
 * 
 * 4.7K Pull Up resistor in 3V and D2/SG for OneWire comm with DS. 
 * 
 */
#include <OneWire.h>            //Library for the OneWire protocol.
#include <DallasTemperature.h>  //Library for temp sensors from Dallas Semiconductor
#include <ESP8266WiFi.h>        //Library for NodeMCU WiFi connections.
 
#define DS18B20 4   //DS18B20 sensor is connected to GPIO Pin 2 (D2)

String apiKey = "XXXXXXXXXXXXXXXX";     //Enter your Write API key from ThingSpeak.
const char* ssid =  "";     //Enter your WiFi Network's SSID betwen "".
const char* pass =  "";      //Enter your WiFi Network's Password  betwen "".
const char* server = "184.106.153.149"; //Thinkspeak's server IP.
float temp;                             //variable for temperature value.
unsigned long previousMillis = 0;       //Variable for time counting.

OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);
WiFiClient client;
 
void setup() 
{
  // All "Serial Comm" that is commented is for debugging with any Serial Monitor in PC
      //Serial.begin(115200);   //Serial Comm initialization.
      delay(1000);      //Initial Delay.
      sensor.begin();   //Sensor initialization.
 
      //Serial.print("Connecting to SSID: ");    //Messsage for connection to start
      //Serial.println(ssid);               //SSID's name.
      WiFi.begin(ssid, pass);   //Connecting to WiFi with SSID and pass defined before

      //Wait for connection succeded
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(100);
            //Serial.print("..");   //Writes '..' every 100 ms while connecting to WiFi.
     }
      //Serial.println("");   //Line space.
      //Serial.println("*** Connected ***");    //Wifi Conncected.
}
 
void loop() 
{
      unsigned long currentMillis = millis()/1000;  //Variable for time counting.

      if (currentMillis - previousMillis >= 60) {   //Sends temperature every 60 seconds.
      previousMillis = currentMillis;               //Updates time counting with actual time.
      
      //Serial.println("Reading sensor...");  //Reading sensor.
      sensor.requestTemperatures();           //Send start convertion command fo DS.
      temp = sensor.getTempCByIndex(0);       //Reading the result of the convertion
      //Serial.println("Reading: ");          //Print temperature label.
      //Serial.println(temp);                 //Print temperature.
      //Serial.println("");                   //Line space.
      enviarTemp(temp);                       //Call routine for sendig data to Thinkspeak
      }
   delay(1000);   //Wait 1 second.
   //Serial.println(currentMillis);           //Print actual time.
}

//Routin for send the data to Thinkspeak
void enviarTemp (float temp)    //Receives variable temp
{  
   WiFiClient client;   //Enabling Connection
  
   if (client.connect(server, 80)) { //Use 184.106.153.149 ó api.thingspeak.com
   //Serial.println("WiFi Client connected.");    //Print connection established.
   //Serial.println("Formando String a enviar."); //Print string building to send.
   //String to send
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   //Serial.println("Sending String to Thinkspeak.");   //Print sending data.
   //Actual data sending code
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   //Data sending done.
   delay(1000);   //Delay before exit.
   
   }
}
