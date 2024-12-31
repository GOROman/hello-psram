#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_system.h"
#include "esp_cache.h"
#include <esp_log.h>
#include "esp_mmu_map.h"

#define PSRAM_BASE_ADDR 0x3F800000

// MMU で PSRAMを論理アドレスへマッピングする
uint32_t* map_psram_to_virtual_memory( size_t map_size ) {
    const char* TAG = "PSRAM_MMU";

    // 物理アドレス（PSRAM）
    esp_paddr_t physical_base = PSRAM_BASE_ADDR;  // PSRAMの物理アドレス

    // 仮想アドレスの出力用ポインタ
    void* virtual_base = NULL;

    // MMUマッピングを設定
    printf("\n\tesp_err_t err = esp_mmu_map(physical_base, map_size, MMU_TARGET_PSRAM0, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE, 0, &virtual_base);\n");
    esp_err_t err = esp_mmu_map(physical_base, map_size, MMU_TARGET_PSRAM0, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE, 0, &virtual_base);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to map PSRAM: %s", esp_err_to_name(err));
        return NULL;
    }

    ESP_LOGI(TAG, "PSRAM successfully mapped to virtual memory at %p.", virtual_base);

    return (uint32_t*)virtual_base;
}


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

//    printf("\nheap_caps_print_heap_info(MALLOC_CAP_SPIRAM)\n");
//    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);

#if CONFIG_SPIRAM_USE_MEMMAP
    size_t map_size = 1024*1024*8;
    uint32_t* psram_addr = map_psram_to_virtual_memory( map_size );

    printf("\n\tesp_mmu_map_dump_mapped_blocks(stdout)\n");
    esp_mmu_map_dump_mapped_blocks(stdout);
    // 4MB 書き込む
    for (int i = 0; i < map_size/sizeof(uint32_t); i++) {
        psram_addr[i] = (uint32_t)&psram_addr[i];
        if (i % 0x4000 == 0) {
            esp_paddr_t paddr;
            mmu_target_t target;
        psram_addr[i] = (uint32_t)paddr;
            esp_mmu_vaddr_to_paddr(&psram_addr[i], &paddr, &target);
           esp_cache_msync(&psram_addr[0], 1024*8, ESP_CACHE_MSYNC_FLAG_INVALIDATE);
            printf("[W:0x%08lX] = 0x%08lX P:0x%08lX\n", (uint32_t)&psram_addr[i], psram_addr[i], (uint32_t)paddr);
        }
    }

    // 4MB 読み込む
    for (int i = 0; i < map_size/sizeof(uint32_t); i++) {
        if (i % 0x4000 == 0) {
            printf("[R:0x%08lX] = 0x%08lX\n", (uint32_t)&psram_addr[i], psram_addr[i]);
        }
    }
#endif
    size_t alloc_size = 1024*8000;//1024*1024*7 + 1024*512;//1024*1024*2;

    // PSRAM(SPIRAM)を明示して確保する場合
    printf("---------------------- Allocating %dKB from PSRAM...", alloc_size/1024);
    uint8_t* psram = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_SPIRAM);
    if (psram == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("OK! %p\n", psram);
        if (esp_ptr_external_ram(psram)) {
            printf("このメモリはPSRAMだよーん！\n");
        } else {
            printf("このメモリは内蔵RAMだよーん！\n");
        }
    }


    memory_info();

    // 普通にmallocする
        printf("---------------------- Allocating %dKB from malloc...", alloc_size/1024);
    uint8_t* heap = (uint8_t*)malloc(alloc_size);
    if (heap == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("OK! %p\n", heap);
        if (esp_ptr_external_ram(heap)) {
            printf("このメモリはPSRAMだよーん！\n");
        } else {
            printf("このメモリは内蔵RAMだよーん！\n");
        }
    }

    memory_info();

    // 普通にmallocする
    // menuconfig の "Maximum malloc() size, in bytes, to always put in internal memory" で決まる
    alloc_size = 1024*32;  // 32KB
    printf("---------------------- Allocating %dKB from malloc...", alloc_size/1024);
    uint8_t* heap2 = (uint8_t*)malloc(alloc_size);
    if (heap2 == NULL) {
        printf("ぬるぽ\n");
    } else {
        printf("OK! %p\n", heap2);
        if (esp_ptr_external_ram(heap2)) {
            printf("このメモリはPSRAMだよーん！\n");
        } else {
            printf("このメモリは内蔵RAMだよーん！\n");
        }
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
        if (esp_ptr_external_ram(heap3)) {
            printf("このメモリはPSRAMだよーん！\n");
        } else {
            printf("このメモリは内蔵RAMだよーん！\n");
        }
    }

    memory_info();

    printf("------------------------ Free allocated memory\n");
    heap_caps_free(psram);
    free(heap3);
    free(heap2);
    free(heap);

    memory_info();
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

