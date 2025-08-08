/* NRF Sender */
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#include <Wire.h>
#include <Adafruit_BME280.h>

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);  // Network uses that radio
RF24NetworkHeader header;

const uint16_t this_node = 00;   // Address of our node in Octal format
const uint16_t other_node = 01;  // Address of the other node in Octal format

struct payload_t {  // Structure of our payload
  float pressure;
  float humidity;
  float temp;
  float tempc;
  char text[20];
} payload;

Adafruit_BME280 bme280;  // use I2C interface
Adafruit_Sensor *bme_temp = bme280.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme280.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme280.getHumiditySensor();
sensors_event_t temp_event, pressure_event, humidity_event;
bool i_bme280 = false;

int i;

void setup(void) {

  Serial.begin(57600);
  while (!Serial) {
    delay(150);
  }
  delay(500);
  Serial.println(F(__FILE__));

  while (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    delay(125);
  }
  Serial.println(F("Radio started ok."));

  radio.setChannel(90);
  radio.setRadiation(RF24_PA_MAX, RF24_250KBPS);

  network.begin(/*node address*/ this_node);

  if (!bme280.begin(0x76)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
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

  } else {
    payload.temp = payload.tempc = payload.humidity = payload.pressure = 0;
    Serial.println(F("no BME280 working!"));
  }

  delay(500);

  network.update();  // Check the network regularly
  if (network.available()) {  // Is there anything ready for us?

    RF24NetworkHeader header;  // If so, grab it and print it out
    network.read(header, &payload, sizeof(payload));

    if ( !strcmp(payload.text, "hw?") ) {

      header.from_node = this_node;
      header.to_node = other_node;

      strcpy(payload.text, "BME280");
      payload.temp = 0;
      payload.tempc = temp_event.temperature;
      payload.humidity = humidity_event.relative_humidity;
      payload.pressure = pressure_event.pressure / pow(1.0 - 500.0 / 44330.0, 5.255);

      Serial.print(F("Sending:"));
      print_payload();
      bool ok = network.write(header, &payload, sizeof(payload));
      Serial.print(F(" - "));
      Serial.println(ok ? F("ok") : F("failed"));
    }
    delay(500);
  }
}

void print_payload() {
  Serial.print(payload.tempc, 1);
  Serial.print(F("°C "));
  Serial.print(payload.humidity, 0);
  Serial.print(F("% "));
  Serial.print(payload.pressure, 0);
  Serial.print(F("hPa "));
  Serial.print(payload.text);
  Serial.print(F(" "));
}