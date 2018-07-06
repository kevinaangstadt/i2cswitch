#include "TW/TinyWireS/TinyWireS.h"



#define I2C_SLAVE_ADDRESS 0x26 // Address of the slave

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

// One register for the mux state
volatile uint8_t i2c_regs[]
{
  0
};

// Tracks the current register pointer position
volatile byte reg_position;
const byte reg_size = sizeof(i2c_regs);

volatile uint8_t out;

/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the 
 * send-buffer when using this callback
 */
void requestEvent()
{  
    // we have a special feature that will send back all the registers
    if(reg_position > reg_size) 
    {
      for(uint8_t i=0; i<reg_size; i++) {
        TinyWireS.send(i2c_regs[i]);
      }
    } else 
    {
      TinyWireS.send(i2c_regs[reg_position]);
      // Increment the reg position on each read, and loop back to zero
      reg_position++;
    }
    if (reg_position >= reg_size)
    {
        reg_position = 0;
    }
}

/**
 * The I2C data received -handler
 *
 * This needs to complete before the next incoming transaction (start, data, restart/stop) on the bus does
 * so be quick, set flags for long running tasks to be called from the mainloop instead of running them directly,
 */
void receiveEvent(uint8_t howMany)
{
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }
    if (howMany > TWI_RX_BUFFER_SIZE)
    {
        // Also insane number
        return;
    }

    reg_position = TinyWireS.receive();
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        return;
    }

    // check to make sure the register is sane
    if(reg_position >= reg_size) {
      // this is a ridiculous position; reset to 0
      reg_position = 0;
    }
    
    while(howMany--)
    {
        i2c_regs[reg_position] = TinyWireS.receive();
        reg_position++;
        if (reg_position >= reg_size)
        {
            reg_position = 0;
        }
    }

    
}

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS); // join i2c network
  TinyWireS.onReceive(receiveEvent);
  TinyWireS.onRequest(requestEvent); 
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  out = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  TinyWireS_stop_check();

  if(i2c_regs[0]) {
    digitalWrite(4, HIGH);
  } else {
    digitalWrite(4, LOW);
  }
}

