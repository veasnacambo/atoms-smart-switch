#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "relay.h"
#include "switch_input.h"
#include "control_logic.h"
#include "wifi_manager.h"
#include "web_server.h"

void app_main(void)
{
    relay_init();
    switch_input_init();
    control_logic_init();
    wifi_manager_init();
    web_server_start();

    while (1) {
        if (switch_input_was_pressed(1)) {
            control_logic_toggle_channel(1);
        }

        if (switch_input_was_pressed(2)) {
            control_logic_toggle_channel(2);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}