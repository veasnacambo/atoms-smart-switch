#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"
#include "switch_input.h"

void app_main(void)
{
    relay_init();
    switch_input_init();

    while (1) {
        if (switch_input_was_pressed(1)) {
            relay_toggle(1);
        }

        if (switch_input_was_pressed(2)) {
            relay_toggle(2);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}