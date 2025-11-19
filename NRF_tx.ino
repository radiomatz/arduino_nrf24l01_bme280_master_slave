/* NRF Sender */


#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

#include <Wire.h>
#include <Adafruit_BME280.h>

const uint16_t this_node = 00;   // Address of our node in Octal format
const uint16_t other_node = 01;  // Address of the other node in Octal format

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);  // Network uses that radio

RF24NetworkHeader header(other_node);

struct payload_t {  // Structure of our payload
  float pressure;
  float humidity;
  float tempc;
  char text[8];
} payload;

Adafruit_BME280 bme280;  // use I2C interface
Adafruit_Sensor *bme_temp = bme280.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme280.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme280.getHumiditySensor();
sensors_event_t temp_event, pressure_event, humidity_event;
bool i_bme280 = false;


void setup(void) {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F(__FILE__));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }

  radio.setChannel(90);
  // radio.setRadiation(RF24_PA_MAX, RF24_250KBPS);

  network.begin(/*node address*/ this_node);

  printf_begin();        // needed for RF24* libs' internal printf() calls
  radio.printDetails();  // requires printf supporterial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(F(__FILE__));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);

  printf_begin();        // needed for RF24* libs' internal printf() calls
  radio.printDetails();  // requires printf support

  if (!bme280.begin(0x76)) {
    if ( Serial ) Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    i_bme280 = false;
  } else {
    i_bme280 = true;
  }


}


void loop() {

  if (i_bme280) {
    bme_temp->getEvent(&temp_event);
    bme_pressure->getEvent(&pressure_event);
    bme_humidity->getEvent(&humidity_event);
    // if ( Serial ) Serial.println(F("BME280 Werte ok"));

  } else {
    payload.tempc = payload.humidity = payload.pressure = 0;
    if ( Serial ) Serial.println(F("no BME280 working!"));
  }

  network.update();  // Check the network regularly

  if (network.available()) {  // Is there anything ready for us?
    int nbytes = network.read(header, &payload, sizeof(payload));
    Serial.print("have read bytes");
    Serial.println(nbytes);
  
    if ( !strcmp(payload.text, "hw?") ) {

      strcpy(payload.text, "BME280");
      payload.tempc = temp_event.temperature;
      payload.humidity = humidity_event.relative_humidity;
      payload.pressure = pressure_event.pressure / pow(1.0 - 500.0 / 44330.0, 5.255);

      if ( Serial ) Serial.print(F("Sending:"));
      print_payload();
      RF24NetworkHeader sheader(/*to node*/ other_node);
      bool ok = network.write(sheader, &payload, sizeof(struct payload_t));
      if ( Serial ) Serial.print(F(" - "));
      if ( Serial ) Serial.println(ok ? F("ok") : F("failed"));
    }
    delay(1000);
  }
}

void print_payload() {
  if ( Serial ) {
    Serial.print(payload.text);
    Serial.print(F(" "));
    Serial.print(payload.tempc, 1);
    Serial.print(F("°C "));
    Serial.print(payload.humidity, 0);
    Serial.print(F("% "));
    Serial.print(payload.pressure, 0);
    Serial.print(F("hPa "));
    Serial.print(payload.text);
    Serial.print(F(" "));
  }
}