#pragma once

#include <driver/sdmmc_types.h>
#include <sdmmc_types.h>
#include <cstdio>

void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card);
