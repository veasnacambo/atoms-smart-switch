#include "control_logic.h"
#include "relay.h"
#include "esp_log.h"

#define MAX_CHANNELS 2

static const char *TAG = "control_logic";
static bool channel_state[MAX_CHANNELS] = {false, false};

static bool valid_channel(int channel)
{
    return (channel >= 1 && channel <= MAX_CHANNELS);
}

void control_logic_init(void)
{
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channel_state[i] = false;
        relay_set(i + 1, false);
    }

    ESP_LOGI(TAG, "Control logic initialized");
}

void control_logic_set_channel(int channel, bool on)
{
    if (!valid_channel(channel)) {
        ESP_LOGW(TAG, "Invalid channel: %d", channel);
        return;
    }

    channel_state[channel - 1] = on;
    relay_set(channel, on);

    ESP_LOGI(TAG, "Channel %d set to %s", channel, on ? "ON" : "OFF");
}

void control_logic_toggle_channel(int channel)
{
    if (!valid_channel(channel)) {
        ESP_LOGW(TAG, "Invalid channel: %d", channel);
        return;
    }

    control_logic_set_channel(channel, !channel_state[channel - 1]);
}

bool control_logic_get_channel(int channel)
{
    if (!valid_channel(channel)) {
        return false;
    }

    return channel_state[channel - 1];
}