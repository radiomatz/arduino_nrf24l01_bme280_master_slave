/* NRF Receiver */

#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

const uint16_t this_node = 01;   // Address of our node in Octal format
const uint16_t other_node = 00;  // Address of the other node in Octal format

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);      // Network uses that radio

RF24NetworkHeader header;  // If so, grab it and print it out

struct payload_t {  // Structure of our payload
  float pressure;
  float humidity;
  float tempc;
  char text[8];
} payload;


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
  radio.printDetails();  // requires printf support
}

void loop(void) {
  memset(&payload, 0, sizeof(payload));
  strcpy(payload.text, "hw?");
  header.to_node = other_node;
  header.from_node = this_node;

  network.update();  // Check the network regularly
  bool ok = network.write(header, &payload, sizeof(payload));
  if ( !ok ) {
    Serial.println(F("sendig hw? failed"));
    delay(3000);
  }
  else {
    delay(350);
  }

  network.update();              // Check the network regularly
  while (network.available()) {  // Is there anything ready for us?
    uint16_t payloadSize = network.peek(header); 
    network.read(header, &payload, payloadSize);
    print_payload();
    Serial.println();
    delay(5000);
  }
  delay(50);
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