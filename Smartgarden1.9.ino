#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <RTClib.h>

#define ssid "FibreBox_X6-0E6AC7" // WLAN SSID
#define password "YCX7JATHRQRMYNPURM" // WLAN Passwort
#define mqtt_server "192.168.1.19" // MQTT Broker Adresse

#define switch_topic "esp8266/switch" // Topic für den Pumpenschalter
#define light_topic "esp8266/light" // Topic für den Lichtschalter
#define status_topic "esp8266/status" // Topic für den Status
#define inTopic "esp8266/output" // Topic für den Input
#define outTopic "esp8266/sensor" // Topic für den Output

const int soilMoisturePin = A0; // Pin für den Bodenfeuchtesensor
const int lightRelay = D5; // Pin für das Relais der Pflanzenbeleuchtung
const int pumpRelay = D6;  // Pin für das Relais der Wasserpumpe
const int trigPin = D4;    // Pin für den Ultraschallsensor (Trig)
const int echoPin = D7;    // Pin für den Ultraschallsensor (Echo)
const int fan1Pin = D8;    // Pin für Lüfter 1
const int fan2Pin = D9;    // Pin für Lüfter 2

int SoilMoisturepercentage;
int counter = 0;

WiFiClient espClient;
PubSubClient client(espClient);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1884);
  setup_rtc();
  client.setCallback(callback);
  lcd.init();
  lcd.backlight();
  pinMode(soilMoisturePin, INPUT);
  pinMode(lightRelay, OUTPUT);
  pinMode(pumpRelay, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(fan1Pin, OUTPUT);
  pinMode(fan2Pin, OUTPUT);
  digitalWrite(lightRelay, HIGH);
  digitalWrite(pumpRelay, HIGH);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Überprüfung der Counter-Werte für die verschiedenen Aktionen
  switch (counter) {
    case 5:
      soilMoisture();
      break;
    case 10:
      check_waterlevel();
      break;
    case 15:
      light();
      break;
    case 20:
      pump();
      break;
    case 25:
      check_fans();
      break;
    case 30:
      setup_rtc();
      break;
    default:
      delay(1000);
      break;
  }
  counter++;
  if (counter > 30) {
    counter = 0;
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi verbunden");
  Serial.println("IP Adresse: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Nachricht empfangen auf Topic: ");
  Serial.println(topic);
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == inTopic) {
    Serial.print("Ändere Output zu ");
    if (messageTemp == "true") {
      Serial.println("an");
      digitalWrite(pumpRelay, LOW);
      client.publish(switch_topic, "AN");
      delay(200);
    } else if (messageTemp == "false") {
      Serial.println("aus");
      digitalWrite(pumpRelay, HIGH);
      client.publish(switch_topic, "AUS");
      delay(200);
    }
  } else if (String(topic) == light_topic) {
    Serial.print("Ändere Licht zu ");
    if (messageTemp == "true") {
      Serial.println("an");
      digitalWrite(lightRelay, LOW);
      client.publish(switch_topic, "Licht AN");
      delay(200);
    } else if (messageTemp == "false") {
      Serial.println("aus");
      digitalWrite(lightRelay, HIGH);
      client.publish(switch_topic, "Licht AUS");
      delay(200);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Versuche MQTT Verbindung...");
    if (client.connect("SmartGardenClient")) {
      Serial.println("verbunden");
      client.publish(status_topic, "Aktiv");
      client.subscribe(inTopic);
      client.subscribe(light_topic);
    } else {
      Serial.print("fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println(" versuche es erneut in 5 Sekunden");
      delay(5000);
    }
  }
}

void setup_rtc() {
  if (!rtc.begin()) {
    Serial.println("RTC nicht gefunden");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC verloren, setze die Zeit!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  DateTime now = rtc.now();
  Serial.print("Aktuelle Zeit: ");
  print2digits(now.hour());
  Serial.print(":");
  print2digits(now.minute());
  Serial.print(":");
  print2digits(now.second());
  Serial.print(" ");
  Serial.print(now.year(), DEC);
  Serial.print("/");
  print2digits(now.month());
  Serial.print("/");
  print2digits(now.day());
  Serial.println();
  delay(1000);
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}

void soilMoisture() {
  int sensorValue = analogRead(soilMoisturePin);
  int percentageSoilMoisture = map(sensorValue, 495, 696, 100, 0);
  client.publish(outTopic, String(percentageSoilMoisture).c_str());
  char gauge [10];
  itoa(percentageSoilMoisture, gauge, 10);
  client.publish(outTopic, gauge);
  delay(500);
  SoilMoisturepercentage = percentageSoilMoisture;
  delay(500);
}

void check_waterlevel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration / 2) * 0.0343;
  float waterLevel = 25 - distance;
  if (waterLevel < 0) waterLevel = 0;
  Serial.print("Entfernung: ");
  Serial.print(distance);
  Serial.println(" cm");
  client.publish("garden/water", String(waterLevel).c_str());
  if (waterLevel < 25) {
    client.publish("garden/water", "Wasser niedrig, bitte nachfüllen");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wasserstand");
    lcd.setCursor(0, 1);
    lcd.print("niedrig");
  }
}

void light() {
  DateTime now = rtc.now();
  int currentHour = now.hour();
  if (currentHour >= 18 || currentHour < 6) {
    digitalWrite(lightRelay, LOW);
  } else {
    digitalWrite(lightRelay, HIGH);
  }
}

void pump() {
  if (SoilMoisturepercentage >= 50) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bodenfeuchte");
    lcd.setCursor(0, 1);
    lcd.print("optimal");
    Serial.println("Stop");
  } else if (SoilMoisturepercentage <= 49 && SoilMoisturepercentage >= 30) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bodenfeuchte");
    lcd.setCursor(0, 1);
    lcd.print("genug");
    Serial.println("Genug");
  } else if (SoilMoisturepercentage < 30) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bodenfeuchte");
    lcd.setCursor(0, 1);
    lcd.print("zu trocken");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wasserpumpe");
    lcd.setCursor(0, 1);
    lcd.print("an");
    digitalWrite(pumpRelay, LOW);
    delay(10000);
    digitalWrite(pumpRelay, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wasserpumpe");
    lcd.setCursor(0, 1);
    lcd.print("aus");
    Serial.println("Ich bin durstig");
    delay(10000);
  }
}

void check_fans() {
  DateTime now = rtc.now();
  int currentMinute = now.minute();
  if (currentMinute % 120 == 0) {
    digitalWrite(fan1Pin, HIGH);
    digitalWrite(fan2Pin, HIGH);
    delay(60000);
    digitalWrite(fan1Pin, LOW);
    digitalWrite(fan2Pin, LOW);
  }
}
