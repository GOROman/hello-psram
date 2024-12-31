#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    while (1) {
        size_t free_heap_size = esp_get_free_heap_size();
        
        printf("Heap (Free): %d (%dKB)\n", free_heap_size, free_heap_size/1024);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

