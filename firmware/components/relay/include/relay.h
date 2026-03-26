#pragma once
#include <stdbool.h>

void relay_init(void);
void relay_set(int channel, bool on);
bool relay_get(int channel);
void relay_toggle(int channel);