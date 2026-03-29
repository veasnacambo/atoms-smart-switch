#pragma once

#include <stdbool.h>

void switch_input_init(void);
bool switch_input_read_raw(int channel);
bool switch_input_was_pressed(int channel);