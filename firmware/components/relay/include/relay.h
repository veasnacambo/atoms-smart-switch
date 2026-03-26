#pragma once

void relay_init(void);
void relay_set(int channel, int state);
int relay_get(int channel);
void relay_toggle(int channel);
