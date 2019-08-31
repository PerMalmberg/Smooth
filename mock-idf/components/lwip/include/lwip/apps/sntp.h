#pragma once

#include <lwip/arch.h>

#define SNTP_OPMODE_POLL            0
#define SNTP_OPMODE_LISTENONLY      1

void sntp_setoperatingmode(u8_t operating_mode);

void sntp_init();

void sntp_stop();

void sntp_setservername(u8_t idx, const char* server);
