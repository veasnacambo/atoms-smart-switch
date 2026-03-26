#include "relay.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define RELAY1_GPIO  3
#define RELAY2_GPIO  4

static const char *TAG = "relay";
static bool relay_state[2] = {false, false};

static gpio_num_t relay_channel_to_gpio(int channel)
{
    switch (channel) {
        case 1: return RELAY1_GPIO;
        case 2: return RELAY2_GPIO;
        default: return GPIO_NUM_NC;
    }
}

void relay_init(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << RELAY1_GPIO) | (1ULL << RELAY2_GPIO),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    relay_set(1, false);
    relay_set(2, false);

    ESP_LOGI(TAG, "Relay initialized");
}

void relay_set(int channel, bool on)
{
    gpio_num_t gpio = relay_channel_to_gpio(channel);
    if (gpio == GPIO_NUM_NC) {
        ESP_LOGW(TAG, "Invalid relay channel: %d", channel);
        return;
    }

    relay_state[channel - 1] = on;

    // Change this if your relay is active-low
    gpio_set_level(gpio, on ? 1 : 0);

    ESP_LOGI(TAG, "Relay %d -> %s", channel, on ? "ON" : "OFF");
}

bool relay_get(int channel)
{
    if (channel < 1 || channel > 2) {
        return false;
    }
    return relay_state[channel - 1];
}

void relay_toggle(int channel)
{
    if (channel < 1 || channel > 2) {
        return;
    }
    relay_set(channel, !relay_state[channel - 1]);
}