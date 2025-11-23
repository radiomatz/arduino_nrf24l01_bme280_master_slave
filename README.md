# arduino_nrf24l01_bme280_master_slave
Two Arduino Nano with NRF24L01, one with BME280 are talking together

## How does it work
the receiver sends to the sender, which has the BME280 Sensor a command 'hw?', and this command lets the sender give the receiver the data for printing it out.<br/>
the advantage is: the sender does not unneccesary send any data, while the receiver does not listen (when you don't want to see the data all the time).

### Caveats:
each file has to be under the arduino tree, each in its directory:<br>
NRF_tx/NRF_tx.ino<br>
NRF_rx/NRF_rx.ino

### Questions: dm2hr@hmro.de 
vy 73 de Matthias, DM2HR
