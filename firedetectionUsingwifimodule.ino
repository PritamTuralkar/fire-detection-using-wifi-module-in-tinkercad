#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(5,6,8,9,10,11);
//SoftwareSerial SerCommESP8266(8,9); 

int redled = 2;
int greenled = 3;
int buzzer = 4;
int firesensor = A0;
int gassensor = A1;
float temp;
float vout;
float vout1;
int gas=0;
//int gasthre =400;

String ssid     = "Simulator Wifi";  // SSID to connect to
String password = ""; // Our virtual wifi has no password (so dont do your banking stuff on this network)
String host     = "api.thingspeak.com"; // Open Weather Map API
const int httpPort   = 80;
String uri     = "/update?api_key=6P10S3VA6BKUCA0E&field1=";

int setupESP8266(void) {
  // Start our ESP8266 Serial Communication
  Serial.begin(115200);   // Serial connection over USB to computer
  Serial.println("AT");   // Serial connection on Tx / Rx port to ESP8266
  delay(1);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 1;
   Serial.println("ESP8266 OK!!!");
    
  // Connect to Circuits Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(1);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 2;
  Serial.println("Connected to WiFi!!!");
  
  // Open TCP connection to the host:
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(10);        // Wait a little for the ESP to respond
  if (!Serial.find("OK")) return 3;
  Serial.println("ESP8266 Connected to server!!!") ;
  
  return 0;
}

void anydata(void) {
  
  vout=analogRead(A0);
  vout1=(vout/1023)*5000;
  temp=(vout1-500)/10;
 
  gas = analogRead(A1);
  
  // Construct our HTTP call
  String httpPacket = "GET " + uri + String(temp)+ "&field2="+ String(gas) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length = httpPacket.length();
  
  // Send our message length
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(1); // Wait a little for the ESP to respond if (!Serial.find(">")) return -1;

  // Send our http request
  Serial.print(httpPacket);
  delay(1); // Wait a little for the ESP to respond
  if (!Serial.find("SEND OK\r\n")) return;
  
  
}





void setup() {
  
  setupESP8266();
   
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(firesensor, INPUT);
  pinMode(gassensor, INPUT);
  //Serial.begin(9600);
  //SerCommESP8266.begin(9600);
  lcd.begin(16,3);
               
}

void loop() {
  
  anydata();
  
  delay(1000);
  
  vout=analogRead(A0);
  vout1=(vout/1023)*5000;
  temp=(vout1-500)/10;
  
   //int firesensorvalue= analogRead(firesensor);
  int gassensorvalue= analogRead(gassensor);
  //Serial.print(firesensorvalue);
  //Serial.print(gassensorvalue);
   
  if(gassensorvalue >=400 || temp>=80)
  {
    digitalWrite(redled, HIGH);
    digitalWrite(greenled, LOW);
    tone(buzzer,1000,10000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("t=");
    lcd.print (temp);
    lcd.print(" / ");
    lcd.print("s=");
    lcd.print (gassensorvalue);
    lcd.setCursor(0,1);
    lcd.print("ALERT !!!!!");
    delay(100);
    
  }
  else
  {
    digitalWrite(redled, LOW);
    digitalWrite(greenled, HIGH);
    noTone(buzzer);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("t=");
    lcd.print (temp);
    lcd.print(" / ");
    lcd.print("s=");
    lcd.print (gassensorvalue);
    lcd.setCursor(0,1);
    lcd.print("SAFE");
    delay(100);
   
    
  }
  
  Serial.print("In Degree =");  
  Serial.print(" "); 
  Serial.print(temp);
  Serial.print("\t");
  Serial.print("GasSensor =");
  Serial.print(" "); 
  Serial.print(gassensorvalue);
  Serial.println();
  
  delay(100);
}

