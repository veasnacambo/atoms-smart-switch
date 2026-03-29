#include "switch_input.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define SWITCH1_GPIO  5
#define SWITCH2_GPIO  6
#define DEBOUNCE_MS   50

typedef struct {
    bool last_raw;
    bool stable_state;
    int64_t last_change_ms;
} switch_state_t;

static switch_state_t sw[2];

static gpio_num_t switch_channel_to_gpio(int channel)
{
    switch (channel) {
        case 1: return SWITCH1_GPIO;
        case 2: return SWITCH2_GPIO;
        default: return GPIO_NUM_NC;
    }
}

static int64_t now_ms(void)
{
    return esp_timer_get_time() / 1000;
}

void switch_input_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SWITCH1_GPIO) | (1ULL << SWITCH2_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    for (int i = 0; i < 2; i++) {
        sw[i].last_raw = false;
        sw[i].stable_state = false;
        sw[i].last_change_ms = 0;
    }
}

bool switch_input_read_raw(int channel)
{
    gpio_num_t gpio = switch_channel_to_gpio(channel);
    if (gpio == GPIO_NUM_NC) {
        return false;
    }

    // active-low: pressed when connected to GND
    return gpio_get_level(gpio) == 0;
}

bool switch_input_was_pressed(int channel)
{
    if (channel < 1 || channel > 2) {
        return false;
    }

    int idx = channel - 1;
    bool raw = switch_input_read_raw(channel);
    int64_t t = now_ms();

    if (raw != sw[idx].last_raw) {
        sw[idx].last_raw = raw;
        sw[idx].last_change_ms = t;
    }

    if ((t - sw[idx].last_change_ms) >= DEBOUNCE_MS) {
        if (sw[idx].stable_state != raw) {
            sw[idx].stable_state = raw;

            if (raw) {
                return true;
            }
        }
    }

    return false;
}