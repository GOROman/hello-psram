#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void memory_info() {
    size_t free_heap_size = esp_get_free_heap_size();

    size_t malloc_cap_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t malloc_cap_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t malloc_cap_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    size_t malloc_cap_dma = heap_caps_get_free_size(MALLOC_CAP_DMA);
    size_t malloc_cap_spi = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    printf("Free Heap        : %8d (%5dKB)\n", free_heap_size, free_heap_size/1024);
    printf("Heap Cap Internal: %8d (%5dKB)\n", malloc_cap_internal, malloc_cap_internal/1024);
    printf("Heap Cap 8bit    : %8d (%5dKB)\n", malloc_cap_8bit, malloc_cap_8bit/1024);
    printf("Heap Cap 32bit   : %8d (%5dKB)\n", malloc_cap_32bit, malloc_cap_32bit/1024);
    printf("Heap Cap DMA     : %8d (%5dKB)\n", malloc_cap_dma, malloc_cap_dma/1024);
    printf("Heap Cap SPI     : %8d (%5dKB)\n", malloc_cap_spi, malloc_cap_spi/1024);
}

void app_main(void) {
    size_t spiram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t internal_size = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    printf("SPIRAM(PSRAM) : %8d (%5dKB)\n", spiram_size, spiram_size/1024);
    printf("Internal      : %8d (%5dKB)\n", internal_size, internal_size/1024);
    
    memory_info();
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);

    size_t alloc_size = 1024*1024*2;

    // PSRAM(SPIRAM)を明示して確保する場合
    uint8_t* psram = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_SPIRAM);
    if (psram == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("---------------------- Allocated %dKB from PSRAM %p\n", alloc_size/1024, psram);
    }

    if (esp_ptr_external_ram(psram)) {
        printf("このメモリはPSRAMだよーん！\n");
    } else {
        printf("このメモリは内蔵RAMだよーん！\n");
    }

    memory_info();

    // 普通にmallocする
    uint8_t* heap = (uint8_t*)malloc(alloc_size);
    if (heap == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("---------------------- Allocated %dKB from malloc %p\n", alloc_size/1024, heap);
    }
    if (esp_ptr_external_ram(heap)) {
        printf("このメモリはPSRAMだよーん！\n");
    } else {
        printf("このメモリは内蔵RAMだよーん！\n");
    }

    memory_info();

    // 普通にmallocする
    // menuconfig の "Maximum malloc() size, in bytes, to always put in internal memory" で決まる
    alloc_size = 1024*32;  // 32KB
    uint8_t* heap2 = (uint8_t*)malloc(alloc_size);
    if (heap2 == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("---------------------- Allocated %dKB from malloc %p\n", alloc_size/1024, heap2);
    }
    if (esp_ptr_external_ram(heap2)) {
        printf("このメモリはPSRAMだよーん！\n");
    } else {
        printf("このメモリは内蔵RAMだよーん！\n");
    }

    memory_info();

    // 普通にmallocする
    alloc_size = 1024*1;  // 1KB
    uint8_t* heap3 = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_32BIT);
//    uint8_t* heap3 = (uint8_t*)malloc(alloc_size);
    if (heap3 == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("---------------------- Allocated %dKB from malloc %p\n", alloc_size/1024, heap3);
    }

    memory_info();

    printf("------------------------ Free allocated memory\n");
    heap_caps_free(psram);
    free(heap3);
    free(heap2);
    free(heap);

    memory_info();
}

