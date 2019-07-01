#define USE_ARDUINO_INTERRUPTS true    
#define DEBUG true
#define SSID ""     // "WiFiname"
#define PASS "" // "password"
#define IP "184.106.153.149"      // thingspeak.com ip
#include <SoftwareSerial.h>
#include <Timer.h>
#include <PulseSensorPlayground.h>    // Includes the PulseSensorPlayground Library.   
Timer t;

PulseSensorPlayground pulseSensor;

String msg = "GET https://api.thingspeak.com/update?api_key=NNYL4SFNR94JGO45&field1=0&field2=0&field3=0"; 

SoftwareSerial esp8266(10,11);

//Variables
const int PulseWire = A0;       
const int LED13 = 13;          
int Threshold = 550;           //for heart rate sensor
float myTemp;
float myBPM;
String BPM;
String temp;
int error1;
int panic;
int raw_myTemp;
float Voltage;
float tempC;
void setup()
{
 
  Serial.begin(115200); 
  esp8266.begin(115200);
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   


   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  Serial.println("AT");
esp8266.println("AT");

  delay(3000);
  char ok[]="OK";
  if(esp8266.find(ok))
  {
    connectWiFi();
  }
  t.every(10000, getReadings);
   t.every(10000, updateInfo);
}

void loop()
{
  panic_button();
start: //label
    error1=0;
   t.update();
    //Resend if transmission is not completed
    if (error1==1)
    {
      goto start; //go to label "start"
    } 
 delay(4000);
}

void updateInfo()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  char error[]="Error";
  if(esp8266.find(error))
  {
    return;
  }
  cmd = msg ;
  cmd += "&field1=";    //field 1 for BPM
  cmd += BPM;
  cmd += "&field2=";  //field 2 for temperature
  cmd += temp;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find('>'))
  {
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
    //Resend...
    error1=1;
  }
}

boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  char ok[]="OK";
  if(esp8266.find(ok))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void getReadings(){
  raw_myTemp = analogRead(A1);
  Voltage = (raw_myTemp / 1023.0) * 5000; // 5000 to get millivots.
  tempC = Voltage * 0.1; 
  myTemp = (tempC * 1.8) + 32; // conver to F
   Serial.println("temp reading");
  Serial.println(myTemp);
   myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 
if (pulseSensor.sawStartOfBeat()) { 
  Serial.println("pulse reading");// Constantly test to see if "a beat happened". 
Serial.println(myBPM);                        // Prints the value inside of myBPM. 
}

  delay(40);            
    char buffer1[10];
     char buffer2[10];
    BPM = dtostrf(myBPM, 4, 1, buffer1);
    temp = dtostrf(myTemp, 4, 1, buffer2);  
  }

void panic_button(){
  panic = digitalRead(8);
    if(panic == HIGH){
     Serial.println("panic reading"); 
    Serial.println(panic);
    
      String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(3000);
  char error[]="Error";
  if(esp8266.find(error))
  {
    return;
  }
  Serial.println("panic");
  cmd = msg ;
  cmd += "&field3=";    
  cmd += panic;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find('>'))
  {
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    esp8266.println("AT+CIPCLOSE");
    //Resend...
    error1=1;
  }
}
}
