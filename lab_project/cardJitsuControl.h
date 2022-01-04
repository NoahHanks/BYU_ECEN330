#ifndef CARDJITSUCONTROL_H_
#define CARDJITSUCONTROL_H_

#include <stdbool.h>
#include <stdint.h>

// Used to init the state machine. Always provided though it may not be
// necessary.
void cardJitsuControl_init();

// Standard tick function.
void cardJitsuControl_tick();

// If you want to stop the game after a mistake, check this function after each
// tick.
bool cardJitsuControl_isGameOver();

// Use this to set the sequence length. This the sequence set by this function
// will only be recognized when the controller passes through its init state.
void cardJitsuControl_setSequenceLength(uint16_t length);

#endif /* CARDJITSUCONTROL_H_ */