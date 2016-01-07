// PUSHOVER ---------------->>
/*
  Expansion of Yún HTTP Client example http://arduino.cc/en/Tutorial/HttpClient
  Pushover service > https://pushover.net
 */

#include <Bridge.h>
#include <YunClient.h>

// Pushover settings - create an account to get Token & Key
char pushoversite[] = "api.pushover.net";
char apitoken[] = "your30characterapitokenhere123";
char userkey [] = "your30characteruserkeygoeshere";

int length;

// Initialize the client library
YunClient client;

// CAPACITANCE ---------------->>
#include <CapacitiveSensor.h>
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
int val;
boolean capState = false;
boolean caliState = false;

long start;
long total1;

// variable to hold sensor value
int calVal;
// variable to calibrate low value
int calL = 10;
// variable to calibrate high value
int calH = 30;

// LED pin - use to show status of events
const int ledReady = 8;
const int ledSend = 9;

// Messages to send & emoticons. Make these as saccharine as you like! 
// Add emoticons using Hex Codes > http://apps.timwhitlock.info/emoji/tables/unicode 
// -------------------------->>

char *think[]={"Counting the hours until he can be with you \xE2\x8F\xB3", "He's thinking of you \xF0\x9F\x98\x8D","There's nowhere else on this world that he'd rather be than beside you right now \xE2\x9B\xB5\xF0\x9F\x8C\xB4", "He touched your face!", "F'k em all. Let's run off and live in a yurt!"};
long thinking;


void setup() {
  // Bridge takes about two seconds to start up
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
  
  Serial.println("hello");
  
  pinMode(ledReady, OUTPUT);
  pinMode(ledSend, OUTPUT);

  digitalWrite(ledReady, HIGH);
  digitalWrite(ledSend, HIGH);
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);    
  
  // calibrate background value - do not touch sensor!
  Serial.println("CALIBRATE....");
  long currentMillis = millis();
  long interval = 5000;  
  while (millis() < 5000) {
    Serial.println(millis());
   total1 =  cs_4_2.capacitiveSensor(300);
      // record the maximum sensor value
      calVal = (int)total1;
      if (calVal > calH) {
        calH = calVal;
      }
      // record the minimum sensor value
      if (calVal < calL) {
        calL = calVal;
      }
    Serial.print("cal: ");
    Serial.print(calL);        // check on performance in milliseconds
    Serial.print("\t");        // tab character for debug window spacing
    Serial.println(calH);      // print sensor output 1
    }

 // while (!Serial); // wait for a serial connection
   Serial.println("CALIBRATED");
   digitalWrite(ledSend, LOW);
   digitalWrite(ledReady, HIGH);
}



void loop() {
   if (!capState){
     cap();
     delay(100);                             // arbitrary delay to limit data to serial port 
   } else {
     thinking = random(sizeof(think)/sizeof(char*));
     Serial.println(think[thinking]);
     pushover(think[thinking],0);  
     capState = false; 
     
     const int oneMinute = (1000 * 60);
     const int coolit = oneMinute * 5;
     delay(coolit);
   }
   
}


void cap(){
    start = millis();
   total1 =  cs_4_2.capacitiveSensor(300);

    val = (int)total1;
    //Serial.println(val);

   if(val > (calH + 50)){
    Serial.println("ON!");
    digitalWrite(ledSend, HIGH);
    digitalWrite(ledReady, LOW);

    capState = true; // PUSH!
   
   } else {
    digitalWrite(ledSend, LOW);
    digitalWrite(ledReady, HIGH);
   }

}



byte pushover(char *pushovermessage, int priority)
{
  String message = pushovermessage;

  length = 113 + message.length();
// more info on parameters https://pushover.net/api 
  if(client.connect(pushoversite,80))
  {
    client.println("POST /1/messages.json HTTP/1.1");
    client.println("Host: api.pushover.net");
    client.println("Connection: close\r\nContent-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.print(length);
    client.println("\r\n");;
    client.print("token=");
    client.print(apitoken);
    client.print("&user=");
    client.print(userkey);
    client.print("&device=work"); // << TEST DEVICE
    client.print("&title=iMissYou");
    client.print("&sound=bike");
    client.print("&message=");
    client.print(message);
    client.print("&priority=");
    client.print(priority);
    client.print("&retry=60");
    client.print("&expire=3600");
    
    while(client.connected())  
    {
      while(client.available())
      {
        char ch = client.read();
        Serial.write(ch);
      }
    }
    client.stop();
  }  
}