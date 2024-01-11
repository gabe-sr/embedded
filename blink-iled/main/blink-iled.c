#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define BLINK_LED 2

static uint8_t s_led_state = 0;
void app_main(void)
{
    char *targetTaskName = pcTaskGetName(NULL);

    ESP_LOGI(targetTaskName, "Hello, ESP32 starting up!\n");
    
    gpio_reset_pin(BLINK_LED);
    gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

    while(1)
    {
        s_led_state = !s_led_state;
        ESP_LOGI(targetTaskName, "Setting led %s", s_led_state == true ? "ON":"OFF");    
        gpio_set_level(BLINK_LED, s_led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
