#include "relay.h"

void relay_init(void) {}
void relay_set(int channel, int state) { (void)channel; (void)state; }
int relay_get(int channel) { (void)channel; return 0; }
void relay_toggle(int channel) { (void)channel; }
