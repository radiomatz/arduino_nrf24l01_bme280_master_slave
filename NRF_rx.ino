/* NRF Receiver */

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>


RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 01;   // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 00;  // Address of the other node in Octal format

RF24NetworkHeader header;  // If so, grab it and print it out

struct payload_t {  // Structure of our payload
  float pressure;
  float humidity;
  float temp;
  float tempc;
  char text[20];
} payload;


void setup(void) {
  Serial.begin(57600);
  while (!Serial) {
    delay(150);
  }
  delay(500);
  Serial.println(F(__FILE__));

  while (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    delay(1000);
  }
  Serial.println(F("Radio started ok."));

  radio.setChannel(90);
  radio.setRadiation(RF24_PA_MAX, RF24_250KBPS);

  network.begin(/*node address*/ this_node);
}

void loop(void) {

  network.update();  // Check the network regularly
  strcpy(payload.text, "hw?");
  header.to_node = other_node;
  header.from_node = this_node;

  bool ok = network.write(header, &payload, sizeof(payload));
  if ( !ok )
    Serial.println(F("sendig hw? failed"));

  delay(500);

  network.update();              // Check the network regularly
  while (network.available()) {  // Is there anything ready for us?
    network.read(header, &payload, sizeof(payload));
    Serial.print(F("Received:"));
    print_payload();
    Serial.println();
  }
  delay(10000);
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