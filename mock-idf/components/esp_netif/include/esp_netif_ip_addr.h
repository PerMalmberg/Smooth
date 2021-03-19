#pragma once

struct esp_ip6_addr {
    uint32_t addr[4];
    uint8_t zone;
};

struct esp_ip4_addr {
    uint32_t addr;
};

typedef struct esp_ip4_addr esp_ip4_addr_t;

typedef struct esp_ip6_addr esp_ip6_addr_t;
