#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* DEFINES */
// setting wifi credentials >TAKE OFF BEFORE COMMITING<
#define ESP_WIFI_SSID   CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS   CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAX_RETRY   5
#define WIFI_CONN_BIT   BIT0
#define WIFI_FAIL_BIT   BIT1

/* Global Variables */
// FreeRTOS ecent froup to signal connection status
static EventGroupHandle_t s_wifi_event_group;

// task tag
static const char *TAG = "wifi | STA";

// connection retry counter
static int s_retry_count = 0;

// this block must not be executed more than once unless
// the previous handle is deleted w/esp_netif_destroy
void init_core_wifi(){
    // initialize esp networking interface
    ESP_ERROR_CHECK(esp_netif_init());
    // initialize esp default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // create ESP-NETIF instance w/WIFI station config
    // attaches netif to wifi + registers wifi handlers to event loop
    esp_netif_create_default_wifi_sta();
    // set up wifi station with default config
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config);
}

void connect_wifi(esp_err_t *status){
    // create event loop group
    s_wifi_event_group = xEventGroupCreate();
    // initialize networking (wifi)
    init_core_wifi();

    esp_event_handler_instance_t wifi_handler_instance;
    // register handler/function for any wifi event
    // this is a catch-all just for this example
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_handler, NULL, &wifi_handler_instance));

    esp_event_handler_instance_t ip_acquired_instance;
    // register handler/function for ip acquisition event
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_handler, NULL, &ip_acquired_instance));

    // start wifi driver
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };


}

void app_main(void)
{
    // initialize default NVS partition, necessary to store necessary data (SSID, PASS, etc)
    esp_err_t ret = nvs_flash_init();
    if( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    // abort if ret in error state
    ESP_ERROR_CHECK(ret);

    // track connection status
    esp_err_t *conn_status = WIFI_FAIL_BIT;

    ESP_LOGI(TAG, "ESP WIFI connection test | STA mode");

    // connect wifi
    connect_wifi(conn_status);
}
