#include <SoftwareSerial.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 7
#define MoistureSensor1 A3
#define MoistureSensor2 A5
#define LightSensor 8
#define Pump 4
#define RX 10
#define TX 11
//Setting the Wi-Fi username and password
String AP = "Abc";       
String PASS = "123123123"; 
//Setting Thingspeak API and fields
String API = "7V8D9F7MA7ZQCTMJ";
String HOST = "api.thingspeak.com";
String PORT = "80";
String dhtTem = "field1";
String dhtHum = "field5";
String Lig = "field2";
String S1 = "field3";
String S2 = "field4";

int countTrueCommand;
int countTimeCommand; 
boolean found = false;
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
String response="";
// Load url into thingspeak and parse the result
void sendAT(String command,const int timeout){
  response="";
  int copy=0;
  esp8266.print(command);
  long int time=millis();
  while((time+timeout)>millis()){
    while(esp8266.available()){
      char c = esp8266.read();
      if(isAlpha(c))
        copy=0;
      if(copy)
        response +=c;
      if(c==':')
        copy=1;

    }
  }
}

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  pinMode(6, OUTPUT); // Setting output pin for the suction pump
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  Serial.println("Connected");
}
void loop() {
  updateThinkspeak(getTempr(),getLight(),getSm1(),getSm2()); //Push data to ThingSpeak cloud
  pullData();
  pullData2();
}
String getTempr(){
  int chk = DHT.read11(DHT11_PIN); 
  int temp = DHT.temperature;
  int hum = DHT.humidity;
  return String(temp)+String(hum); 
}
String getLight(){
  int lightVal = 1 - digitalRead(LightSensor);
  return String(lightVal);  
}

String getSm1(){
  int soilMoist1 = analogRead(MoistureSensor1);
  soilMoist1= 100 - map(soilMoist1,260,1020,0,100); // Map the value between 0 to 100
  return String(soilMoist1); 
}

String getSm2(){
  int soilMoist2 = analogRead(MoistureSensor2);
  soilMoist2= 100 - map(soilMoist2,325,1020,0,100);
  return String(soilMoist2);
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

void updateThinkspeak(String Temperature,String Light,String Moist1,String Moist2){
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",2000);
  delay(2000);
  String Humidity;
  // Parsing the DHT11 string return by getTempr function
  Humidity = String(int(Temperature[2]-48)*10 + int(Temperature[3]-48)); 
  Temperature=String(int(Temperature[0]-48)*10+int(Temperature[1]-48));
   
  String cmdlen;
  String cmd = "GET /update?api_key=7V8D9F7MA7ZQCTMJ&field1="+Temperature+"&field5="+Humidity+"&field2="+Light+"&field3="+Moist1+"&field4="+Moist2+"\r\n";
  cmdlen=cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd);
  sendAT("AT+CIPCLOSE\r\n",2000);
  Serial.println("Send a batch of values");
  Serial.println(Temperature);
  Serial.println(Humidity);
  Serial.println(Light);
  Serial.println(Moist1);
  Serial.println(Moist2);
  
  delay(5000);  
}

// Function to pull data from thingspeak action channel
void pullData(){
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",2000);
  delay(2000);
  String cmdlen;
  String fieldname="field1";
  String cmd="GET /channels/613980/field/"+fieldname+"/last"+"\r\n";
  cmdlen=cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd);
  sendAT("AT+CIPCLOSE\r\n",2000);

  Serial.println("Response from API ");
  int cloudResponse1 = response.toInt();
  // Pump action based on Matlab Analysis
  Serial.print("Water Action");
  Serial.println(cloudResponse1);
  if (cloudResponse1>1){
      Serial.println("Fire the Pump");
      digitalWrite(6,HIGH);
      delay(10000);
      digitalWrite(6,LOW);
  }
  else
    digitalWrite(6,LOW);
  delay(5000);
}
void pullData2(){
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n",2000);
  delay(2000);

  String cmdlen;
  String fieldname="field2";
  String cmd="GET /channels/613980/field/"+fieldname+"/last"+"\r\n";
  cmdlen=cmd.length();
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd);
  sendAT("AT+CIPCLOSE\r\n",2000);
  Serial.println("Response from API ");
  String cloudResponse2 = response;
  Serial.print("Action Code");
  Serial.println(cloudResponse2);
  
  // Parsing action code returned by ThingSpeak
  int len = cloudResponse2.length();
  for (int i=0;i<len;i++){
    if (cloudResponse2[i]=='0')
        Serial.println("Everything is Fine");
    if (cloudResponse2[i]=='1')
        Serial.println("Light Too Bright!!!");
    if (cloudResponse2[i]=='2')
        Serial.println("Humidity is too low!!!");
    if (cloudResponse2[i]=='3')
        Serial.println("Too Cold!!!");
    if (cloudResponse2[i]=='4')
        Serial.println("Too Hot!!!");
  }
  
  delay(5000);
}
