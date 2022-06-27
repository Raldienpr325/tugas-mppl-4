#define BLYNK_PRINT SwSerial
#include <SoftwareSerial.h>
SoftwareSerial SwSerial(2, 3); // RX, TX
#include <BlynkSimpleStream.h>
#include <DHT.h>
#define BLYNK_TEMPLATE_ID "TMPLunVDF9KE"
#define BLYNK_DEVICE_NAME "UTS piranti C"

char auth[] = "6gedXzg-Im3escEuM6iKQb_eDTAbNBp3";

#define DHTPIN 13          // What digital pin we're connected to

#define DHTTYPE DHT11

#define kpa2atm 0.00986923267

DHT dht(DHTPIN, DHTTYPE);

WidgetLED ledblue(V3);
WidgetLED ledgreen(V4);
WidgetLED ledred(V5);

BlynkTimer timer;

int pressurePin = 1;

int val;
float pkPa; // pressure in pkPa
float pAtm; // pressure in pAtm

const int FAN = 9;
const int red = 10;
const int green = 11;
const int blue = 12;

int fanSpeed;

unsigned long time;

BLYNK_WRITE(V6){
  if(param.asInt() > 0){
    digitalWrite(FAN, HIGH);
  }
  else if (param.asInt() == 0){
    digitalWrite(FAN, 0);
  }
}

void sendSensor()
{
  val = analogRead(pressurePin);
  pkPa = ((float)val/(float)1023+0.095)/0.009;
  pAtm = kpa2atm*pkPa;
  
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int heatIndex = dht.computeHeatIndex(t, h, false);

  if (isnan(h) || isnan(t) || isnan(pkPa)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else if(t <= 20){
    digitalWrite(blue, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    ledblue.on();
    ledgreen.off();
    ledred.off();
    digitalWrite(FAN, 0);
    delay(1000);
  }
  else if(t >= 21 && t <= 35){
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(red, LOW);
    ledblue.off();
    ledgreen.on();
    ledred.off();
    digitalWrite(FAN, 32);
    delay(1000);
  }
  else if(t >= 36){
    digitalWrite(red, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(green, LOW);
    ledblue.off();
    ledgreen.off();
    ledred.on();
    digitalWrite(FAN, 255);
    delay(1000);
  }
  else{
    digitalWrite(blue, LOW);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    ledblue.off();
    ledgreen.off();
    ledred.off();
    //digitalWrite(FAN, LOW);
  }

  Blynk.virtualWrite(V0, heatIndex);
  Blynk.virtualWrite(V1, pkPa);
  Blynk.virtualWrite(V2, pAtm);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  SwSerial.begin(9600);
  Blynk.begin(Serial, auth);
  dht.begin();
  timer.setInterval(1000L, sendSensor);
  pinMode(FAN, OUTPUT);
}

void loop()
{
  Blynk.run();
  timer.run();
}
