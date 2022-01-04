

#include <stdint.h>
#include <stdio.h>
#include <xparameters.h>
#include "cardJitsuDisplay.h"
#include "cardJitsuControl.h"

#include "config.h"
#include "display.h"

#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"

#include "utils.h"

/////////////////////////////////////////
#define RUN_PROGRAM MILESTONE_1
// #define RUN_PROGRAM MILESTONE_2
// #define RUN_PROGRAM MILESTONE_3
// #define RUN_PROGRAM MILESTONE_4

// If nothing is uncommented above, run milestone 4
#ifndef RUN_PROGRAM
#define RUN_PROGRAM MILESTONE_4
#endif

// Compute the timer clock freq.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
// Compute timer load value.
#define TIMER_LOAD_VALUE ((CONFIG_TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define INTERRUPTS_PER_SECOND (1.0 / CONFIG_TIMER_PERIOD)
#define TOTAL_SECONDS 90
#define MAX_INTERRUPT_COUNT (INTERRUPTS_PER_SECOND * TOTAL_SECONDS)

/****************************** RUN_BUTTON_HANDLER_TEST ****************/
#if RUN_PROGRAM == MILESTONE_1
static void test_init() {
  leds_init(true);
  printf("Running the buttonHandler test.\n");
}

void tickAll() {
//   buttonHandler_tick();
//   bhTester_tick();
cardJitsuControl_tick();
}
#endif

int main(){
    printf("Hello World!");
    display_init();
    display_fillScreen(DISPLAY_BLACK);
//     cardJitsu_drawCardOutline(DISPLAY_RED, 1, 1, false);
//     cardJitsu_drawCardOutline(DISPLAY_BLUE, 2, 1, false);
//     cardJitsu_drawCardOutline(DISPLAY_WHITE, 3, 1, false);

//     cardJitsu_eraseAllCards();

//     cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_ONE, 0, false);
//     cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_TWO, 0, false);
//     cardJitsu_drawCardOutline(DISPLAY_YELLOW, CARD_THREE, 0, false);

//     cardJitsu_drawCardOutline(DISPLAY_RED, CARD_ONE, 1, false);
//     cardJitsu_drawElement(CARD_ONE_X, PLAYER_CARD_Y, 1, FIRE, false);

//     cardJitsu_drawCardOutline(DISPLAY_BLUE, CARD_TWO, 1, false);
//     cardJitsu_drawElement(CARD_TWO_X, PLAYER_CARD_Y, 1, WATER, false);

//     cardJitsu_drawCardOutline(DISPLAY_WHITE, CARD_THREE, 1, false);
//     cardJitsu_drawElement(CARD_THREE_X, PLAYER_CARD_Y, 1, SNOW, false);

// //////////////////////////////
//     cardJitsu_drawElement(50, 55, 3, WATER, false);
//     cardJitsu_drawElement(170, 55, 3, FIRE, false);  
while (1) {  
tickAll();
}
interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();



}

// All programs share the same main.
// Differences are limited to test_init() and isr_function().
int main2() {
  test_init(); // Program specific.
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();
  // Keep track of your personal interrupt count. Want to make sure that you
  // don't miss any interrupts.
  int32_t personalInterruptCount = 0;
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  while (1) {
    if (interrupts_isrFlagGlobal) {
      // Count ticks.
      personalInterruptCount++;
      tickAll();
      interrupts_isrFlagGlobal = 0;
      if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
    }
  }
  interrupts_disableArmInts();
  printf("isr invocation count: %d\n", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n", personalInterruptCount);
  return 0;
}

// Interrupt routine
void isr_function() {
  // Empty for flag method (flag set elsewhere)
}