#pragma once

#include <driver/gpio.h>

#define SDMMC_HOST_DEFAULT() {}

typedef struct {
    gpio_num_t gpio_cd;     ///< GPIO number of card detect signal
    gpio_num_t gpio_wp;     ///< GPIO number of write protect signal
    uint8_t width;          ///< Bus width used by the slot (might be less than the max width supported)
    uint32_t flags;         ///< Features used by this slot
} sdmmc_slot_config_t;

#define SDMMC_SLOT_CONFIG_DEFAULT() {}
