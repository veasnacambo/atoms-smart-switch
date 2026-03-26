#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "relay.h"
#include "switch_input.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "app_logic.h"

void app_main(void)
{
    printf("Atoms Smart Switch V1 starting...\n");

    relay_init();
    switch_input_init();
    app_logic_init();
    wifi_manager_init();
    web_server_start();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
