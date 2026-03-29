#pragma once
#include <stdbool.h>

void control_logic_init(void);

void control_logic_set_channel(int channel, bool on);
void control_logic_toggle_channel(int channel);
bool control_logic_get_channel(int channel);