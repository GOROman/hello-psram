#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void memory_info() {
    size_t free_heap_size = esp_get_free_heap_size();
    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    printf("PSRAM       : %d (%dKB)\n", psram_size, psram_size/1024);
    printf("Heap  (Free): %d (%dKB)\n", free_heap_size, free_heap_size/1024);
}

void app_main(void) {
    
    memory_info();

    size_t alloc_size = 1024*1024*4;    // 4MB確保したい

    uint8_t* heap = (uint8_t*)malloc(alloc_size);
    if (heap == NULL) {
        printf("Failed to allocate memory\n");
    } else {
        printf("Allocated %dKB\n", alloc_size/1024);
    }

    // 適当に書き込む
    for (size_t i = 0; i < alloc_size; i++) {
        heap[i] = 0x55;
    }

    memory_info();

    free(heap);
}

