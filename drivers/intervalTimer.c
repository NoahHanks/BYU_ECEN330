#include "intervalTimer.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdint.h>
#include <stdio.h>

#define TIMER0_BASE_ADDR XPAR_AXI_TIMER_0_BASEADDR
#define TIMER1_BASE_ADDR XPAR_AXI_TIMER_1_BASEADDR
#define TIMER2_BASE_ADDR XPAR_AXI_TIMER_2_BASEADDR

#define CLEAR_TIMER 0x0     // Sets timer to 0
#define TIMER1_TCSR0 0x0    // Control/Status Register 0
#define TIMER_UDT0 0x02     // Up/Down Count Timer0
#define TIMER1_TLR0 0x04    // Load Register 0
#define TIMER1_TCR0 0x08    // Timer/Counter Register 0
#define TIMER2_TCSR1 0x10   // Control/Status Register 1
#define TIMER2_TLR1 0x14    // Load Register 1
#define TIMER2_TCR1 0x18    // Timer/Counter Register 1
#define TIMER_LOAD 0x20     // Load on fifth bit
#define TIMER_ENT0 0x80     // ENT0 on eighth bit
#define TIMER_CASCADE 0x800 // Cascade on fifteenth bit

#define TIMER_CLOCK_FREQ XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ
#define TIMER_SHIFT 32 // Value to shift the 64 bit counter 32 bits to the left
#define MAX_COUNT 0x3  // Max number of timers

// Takes an integer to check if it is one of the three available values. If so,
// it then returns the address to that timer.
uint32_t getTimerAddress(uint32_t timerNumber) {
  // Takes the given timer number and checks if it is one of the three available
  // values. Fails if not a valid input.
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    return TIMER0_BASE_ADDR;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    return TIMER1_BASE_ADDR;
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    return TIMER2_BASE_ADDR;
  } else {
    return INTERVAL_TIMER_STATUS_FAIL;
  }
}

// Receives the input of the of the timer sumber then goes to get that timer
// address. It then reads the upper and lower parts of the register then
// combines the two to return the full value of the timer.
uint64_t getTimerValue(uint32_t timerNumber) {
  uint32_t timerAddress = getTimerAddress(timerNumber);
  uint32_t lowValue = Xil_In32(timerAddress + TIMER1_TCR0);
  uint64_t fullValue = Xil_In32(timerAddress + TIMER2_TCR1);
  fullValue = fullValue << TIMER_SHIFT;
  return (fullValue + lowValue);
}

// This takes in the address and the value that wants to be changed. In the line
// of bits, it is able to change one of the bits with that value.
void setBit(uint32_t timerAddress, uint32_t timerValue) {
  uint32_t timerRegister = Xil_In32(timerAddress);
  timerRegister = (timerRegister | timerValue);
  Xil_Out32(timerAddress, timerRegister);
}

// This takes in the address and the value that wants to be changed. In the line
// of bits, it is able to clear one of the bits to be a zero usually.
void clearBit(uint32_t timerAddress, uint32_t timerValue) {
  uint32_t timerRegister = Xil_In32(timerAddress);
  timerRegister = (timerRegister & ~timerValue);
  Xil_Out32(timerAddress, timerRegister);
}

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  uint32_t timerAddress = getTimerAddress(timerNumber);
  // Checks to make sure the address is not zero then goes to clear the timers
  // to initialize them.
  if (timerAddress != 0) {
    Xil_Out32(timerAddress + TIMER1_TCSR0, CLEAR_TIMER);
    Xil_Out32(timerAddress + TIMER2_TCSR1, CLEAR_TIMER);
    setBit(timerAddress + TIMER1_TCSR0, TIMER_CASCADE);
    clearBit(timerAddress + TIMER1_TCSR0, TIMER_UDT0);
    return INTERVAL_TIMER_STATUS_OK;
  } else {
    return INTERVAL_TIMER_STATUS_FAIL;
  }
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll() {
  // Goes through all the different timers then applies the function to
  // initialize them.
  for (uint32_t i = 0; i < MAX_COUNT; i++) {
    intervalTimer_init(i);
  }
  return INTERVAL_TIMER_STATUS_OK;
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  uint32_t timerAddress = getTimerAddress(timerNumber);
  setBit(timerAddress + TIMER1_TCSR0, TIMER_ENT0);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  uint32_t timerAddress = getTimerAddress(timerNumber);
  clearBit(timerAddress + TIMER1_TCSR0, TIMER_ENT0);
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
  uint32_t timerAddress = getTimerAddress(timerNumber);
  Xil_Out32(timerAddress + TIMER1_TLR0, CLEAR_TIMER);
  setBit(timerAddress + TIMER1_TCSR0, TIMER_LOAD);
  clearBit(timerAddress + TIMER1_TCSR0, TIMER_LOAD);

  Xil_Out32(timerAddress + TIMER1_TLR0, CLEAR_TIMER);
  setBit(timerAddress + TIMER2_TCSR1, TIMER_LOAD);
  clearBit(timerAddress + TIMER2_TCSR1, TIMER_LOAD);
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll() {
  for (uint32_t i = 0; i < MAX_COUNT; i++) {
    intervalTimer_reset(i);
  }
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll() {}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  return (getTimerValue(timerNumber) / (double)TIMER_CLOCK_FREQ);
}
