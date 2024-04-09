#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <RTClib.h>

// Definition der WLAN-Zugangsdaten und MQTT-Konfiguration
#define ssid "FibreBox_X6-0E6AC7" // WLAN SSID
#define password "YCX7JATHRQRMYNPURM" // WLAN Passwort
#define mqtt_server "192.168.1.19" // MQTT Broker Adresse

// MQTT Topics für verschiedene Steuerungen und Status
#define switch_topic "esp8266/switch" // Topic für den Pumpenschalter
#define light_topic "esp8266/light" // Topic für den Lichtschalter
#define status_topic "esp8266/status" // Topic für den Status
#define inTopic "esp8266/output" // Topic für den Input
#define outTopic "esp8266/sensor" // Topic für den Output

// Definition der Pins für Sensoren und Aktoren
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
  setup_wifi(); // WLAN-Verbindung herstellen
  client.setServer(mqtt_server, 1884); // MQTT-Broker konfigurieren
  setup_rtc(); // RTC (Echtzeituhr) initialisieren
  client.setCallback(callback); // MQTT-Callback-Funktion setzen
  lcd.init(); // LCD initialisieren
  lcd.backlight(); // LCD-Hintergrundbeleuchtung einschalten
  pinMode(soilMoisturePin, INPUT); // Bodenfeuchtesensor als Eingang festlegen
  pinMode(lightRelay, OUTPUT); // Relais für Licht als Ausgang festlegen
  pinMode(pumpRelay, OUTPUT); // Relais für Pumpe als Ausgang festlegen
  pinMode(trigPin, OUTPUT); // Ultraschallsensor Trig-Pin als Ausgang festlegen
  pinMode(echoPin, INPUT); // Ultraschallsensor Echo-Pin als Eingang festlegen
  pinMode(fan1Pin, OUTPUT); // Lüfter 1 als Ausgang festlegen
  pinMode(fan2Pin, OUTPUT); // Lüfter 2 als Ausgang festlegen
  digitalWrite(lightRelay, HIGH); // Licht-Relais ausschalten
  digitalWrite(pumpRelay, HIGH); // Pumpen-Relais ausschalten
}

void loop() {
  if (!client.connected()) {
    reconnect(); // Bei Verbindungsverlust mit MQTT-Broker erneut verbinden
  }
  client.loop(); // MQTT-Client im Loop verarbeiten

  // Überprüfung der Counter-Werte für die verschiedenen Aktionen
  switch (counter) {
    case 5:
      soilMoisture(); // Bodenfeuchte überprüfen
      break;
    case 10:
      check_waterlevel(); // Wasserstand überprüfen
      break;
    case 15:
      light(); // Lichtsteuerung basierend auf Uhrzeit
      break;
    case 20:
      pump(); // Pumpensteuerung basierend auf Bodenfeuchte
      break;
    case 25:
      check_fans(); // Lüftersteuerung basierend auf aktueller Minute
      break;
    case 30:
      setup_rtc(); // RTC erneut initialisieren (optional)
      break;
    default:
      delay(1000);
      break;
  }
  counter++;
  if (counter > 30) {
    counter = 0; // Counter zurücksetzen, um den Zyklus zu wiederholen
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

  // Verarbeitung eingehender Nachrichten basierend auf dem Topic
  if (String(topic) == inTopic) {
    Serial.print("Ändere Output zu ");
    if (messageTemp == "true") {
      Serial.println("an");
      digitalWrite(pumpRelay, LOW); // Pumpe einschalten
      client.publish(switch_topic, "AN"); // Status veröffentlichen
      delay(200);
    } else if (messageTemp == "false") {
      Serial.println("aus");
      digitalWrite(pumpRelay, HIGH); // Pumpe ausschalten
      client.publish(switch_topic, "AUS"); // Status veröffentlichen
      delay(200);
    }
  } else if (String(topic) == light_topic) {
    Serial.print("Ändere Licht zu ");
    if (messageTemp == "true") {
      Serial.println("an");
      digitalWrite(lightRelay, LOW); // Licht einschalten
      client.publish(switch_topic, "Licht AN"); // Status veröffentlichen
      delay(200);
    } else if (messageTemp == "false") {
      Serial.println("aus");
      digitalWrite(lightRelay, HIGH); // Licht ausschalten
      client.publish(switch_topic, "Licht AUS"); // Status veröffentlichen
      delay(200);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Versuche MQTT Verbindung...");
    if (client.connect("SmartGardenClient")) {
      Serial.println("verbunden");
      client.publish(status_topic, "Aktiv"); // Status veröffentlichen
      client.subscribe(inTopic); // Auf Input-Topic abonnieren
      client.subscribe(light_topic); // Auf Licht-Topic abonnieren
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
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Zeit setzen
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
  char gauge[10];
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
    digitalWrite(lightRelay, LOW); // Licht einschalten während der Nacht
  } else {
    digitalWrite(lightRelay, HIGH); // Licht ausschalten während des Tages
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
    digitalWrite(pumpRelay, LOW); // Pumpe einschalten
    delay(10000);
    digitalWrite(pumpRelay, HIGH); // Pumpe ausschalten
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
  if (currentMinute % 120 == 0) { // Alle 2 Stunden Lüfter einschalten
    digitalWrite(fan1Pin, HIGH);
    digitalWrite(fan2Pin, HIGH);
    delay(60000);
    digitalWrite(fan1Pin, LOW);
    digitalWrite(fan2Pin, LOW);
  }
}

