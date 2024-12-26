#ifndef AudioFiles_h
#define AudioFiles_h

#include "stdint.h"

extern const char firsttrack[];
extern const char secondtrack[];

void play_melody(uint8_t melody_number);
void stop_playing(void);

#endif