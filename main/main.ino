#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "SSID"; // Enter your WiFi name - phone hotssid. esp32 and laptop connected to the same hotssid. using WPA3 - personal
const char *password = "password";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.196.147"; // check terminal `ifconfig` on your laptop. this connect them through local network. 
const char *topic = "led-update";
const char *mqtt_username = "node1";
const char *mqtt_password = "qwerty";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int distanceTrigger = 10;
const int frequencyLimit = 20;

#define LED 2 // build in LED pin
#define TRIG_PIN 23 // ESP32 pin GPIO23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 22 // ESP32 pin GPIO22 connected to Ultrasonic Sensor's ECHO pin

int frequency = 10;
float duration_us, distance_cm;

unsigned long StartTime = millis(); // in milliseconds
float timeBetweenPulses = 1000 / frequency; // in milliseconds
void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 
 connectWifi();
 connectMQTTBroker();
 setupPin();
}


void loop() {
  // client.loop();
   // generate 10-microsecond pulse to TRIG pin

  readDistance();
  if(distance_cm < distanceTrigger){
    // update frequency
    if(frequency < frequencyLimit){
      frequency += 1;
      client.publish(topic, String(frequency).c_str());
    }
    Serial.println("13520055 : " + String(frequency) + " Hz");
    timeBetweenPulses = 1000 / frequency;

    // print the value to Serial Monitor
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    // debounce
    while(distance_cm < distanceTrigger){
      readDistance();
      delay(500);
    }
    Serial.print("debounce over, distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
  }

  runLed();
}


// SETUP HELPER
void connectWifi(){
// connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     Serial.println("Connecting to WiFi..");
     delay(2000);
 }
 Serial.println("Connected to the WiFi network");
}
void connectMQTTBroker(){
//connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
// publish and subscribe
 client.publish(topic, "node connected");
}

void setupPin(){
  pinMode(LED, OUTPUT);
    // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);
  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
}


// LOOP HELPER
void readDistance(){
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
    // calculate the distance
  distance_cm = 0.017 * duration_us;
}

void runLed(){
// run LED based on frequency
  unsigned long currentTime = millis();
  if (currentTime - StartTime >= timeBetweenPulses) {
    StartTime = currentTime;
    digitalWrite(LED, !digitalRead(LED));

  }
}