#include "web_server.h"
#include "control_logic.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <stdlib.h>
#include <stdio.h>

static const char *TAG = "web_server";
static httpd_handle_t server = NULL;


// Handle: "/"
// Send main HTML page with auto-refresh UI
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<link rel='icon' href='data:,'>"
        "<title>Atoms Smart Switch</title>"

        "<style>"
        "body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; }"
        ".card { margin: 20px auto; padding: 20px; width: 260px; border: 1px solid #ccc; border-radius: 12px; }"
        ".state { font-size: 22px; font-weight: bold; margin: 10px 0; }"
        "button { width: 180px; height: 55px; font-size: 18px; border: none; border-radius: 10px; color: white; }"
        ".on { background-color: green; }"
        ".off { background-color: red; }"
        "</style>"

        "<script>"
        // Update button and state text from current value
        "function updateUI(ch, state) {"
        "  const stateText = document.getElementById('state' + ch);"
        "  const btn = document.getElementById('btn' + ch);"
        "  if (state) {"
        "    stateText.innerText = 'ON';"
        "    btn.innerText = 'Turn OFF';"
        "    btn.className = 'on';"
        "  } else {"
        "    stateText.innerText = 'OFF';"
        "    btn.innerText = 'Turn ON';"
        "    btn.className = 'off';"
        "  }"
        "}"

        // Read latest channel state from ESP32
        "function refreshStatus() {"
        "  fetch('/status')"
        "    .then(response => response.json())"
        "    .then(data => {"
        "      updateUI(1, data.ch1);"
        "      updateUI(2, data.ch2);"
        "    })"
        "    .catch(err => console.log('Status error:', err));"
        "}"

        // Toggle selected channel
        "function toggleChannel(ch) {"
        "  fetch('/toggle?ch=' + ch)"
        "    .then(response => response.text())"
        "    .then(data => {"
        "      refreshStatus();"
        "    })"
        "    .catch(err => console.log('Toggle error:', err));"
        "}"

        // Start auto refresh when page loads
        "window.onload = function() {"
        "  refreshStatus();"
        "  setInterval(refreshStatus, 1000);"
        "};"
        "</script>"
        "</head>"

        "<body>"
        "<h2>Atoms Smart Switch</h2>"

        "<div class='card'>"
        "<h3>Channel 1</h3>"
        "<div id='state1' class='state'>--</div>"
        "<button id='btn1' class='off' onclick='toggleChannel(1)'>Loading...</button>"
        "</div>"

        "<div class='card'>"
        "<h3>Channel 2</h3>"
        "<div id='state2' class='state'>--</div>"
        "<button id='btn2' class='off' onclick='toggleChannel(2)'>Loading...</button>"
        "</div>"

        "</body>"
        "</html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}



static int get_channel_from_query(httpd_req_t *req)
{
    char buf[64];
    char param[8];

    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK) {
        if (httpd_query_key_value(buf, "ch", param, sizeof(param)) == ESP_OK) {
            return atoi(param);
        }
    }

    return -1;
}

// Handle: /toggle?ch=1
// Toggle selected channel, then return OK
static esp_err_t toggle_handler(httpd_req_t *req)
{
    int ch = get_channel_from_query(req);

    // Check valid channel
    if (ch >= 1 && ch <= 2) {
        control_logic_toggle_channel(ch);
    }

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

// Return current state as JSON
// Example: {"ch1":1,"ch2":0}
static esp_err_t status_get_handler(httpd_req_t *req)
{
    char json[64];

    snprintf(json, sizeof(json),
             "{\"ch1\":%d,\"ch2\":%d}",
             control_logic_get_channel(1),
             control_logic_get_channel(2));

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

//-------------------
static esp_err_t on_handler(httpd_req_t *req)
{
    int ch = get_channel_from_query(req);
    control_logic_set_channel(ch, true);

    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

static esp_err_t off_handler(httpd_req_t *req)
{
    int ch = get_channel_from_query(req);
    control_logic_set_channel(ch, false);

    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}


//-------------------


void web_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "HTTP server started");

        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t toggle = {
            .uri = "/toggle",
            .method = HTTP_GET,
            .handler = toggle_handler,
            .user_ctx = NULL
        };

        httpd_uri_t status = {
            .uri = "/status",
            .method = HTTP_GET,
            .handler = status_get_handler,
            .user_ctx = NULL
        };

        //---------------------------

        httpd_uri_t on_uri = {
            .uri = "/on",
            .method = HTTP_GET,
            .handler = on_handler,
            .user_ctx = NULL
        };

        httpd_uri_t off_uri = {
            .uri = "/off",
            .method = HTTP_GET,
            .handler = off_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &on_uri);
        httpd_register_uri_handler(server, &off_uri);

        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &toggle));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &status));

        ESP_LOGI(TAG, "Registered URI: /");
        ESP_LOGI(TAG, "Registered URI: /toggle");
        ESP_LOGI(TAG, "Registered URI: /status");
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}