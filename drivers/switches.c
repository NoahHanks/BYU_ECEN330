#include "switches.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define SWITCHES_OFFSET 0x4
#define SWITCHES_BASE_ADDRESS XPAR_SLIDE_SWITCHES_BASEADDR + SWITCHES_OFFSET
#define SWITCHES_INPUT 0xF
#define ALL_SWITCHES_ON 0xF

// Returns the current value of the switches from the GPIO register when using
// the offset.
int32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(SWITCHES_BASE_ADDRESS);
}

// Writes to the values of the switches using the GPIO register. Also uses
// the same offset then returns the value.
int32_t switches_writeGpioRegister(int32_t offset, int32_t value) {}

// Initializes the SWITCHES driver software and hardware. Returns one of the
// STATUS values defined above.
int32_t switches_init() {
  leds_init(0);
  Xil_Out32(SWITCHES_BASE_ADDRESS, SWITCHES_INPUT);
  return SWITCHES_INIT_STATUS_OK;
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read() { return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR); }

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void switches_runTest() {
  switches_init();
  uint32_t buttonState = 0;
  // Checks to see if all the switches are pressed or not.
  while (buttonState ^ ALL_SWITCHES_ON) {
    buttonState = switches_read();
    leds_write(buttonState & ALL_SWITCHES_ON);
  }
  leds_write(
      !buttonState); // Turns off all the leds when all the switches are on.
}