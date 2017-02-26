#include "TW/TinyWireS/TinyWireS.h"



#define I2C_SLAVE_ADDRESS 0x26 // Address of the slave

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t out;

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS); // join i2c network
  //TinyWireS.onReceive(receiveEvent); // not using this
  TinyWireS.onRequest(requestEvent); 
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  out = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  TinyWireS_stop_check();

}

// Gets called when the ATtiny recieves an i2c message
void requestEvent() {
  if(out == 0) {
    out = 1;
    digitalWrite(4, HIGH);
  } else {
    out = 0;
    digitalWrite(4, LOW);
  }

  TinyWireS.send(out);
}

