#pragma once

#include <lwip/arch.h>
#include <esp_sntp.h>

#define SNTP_OPMODE_POLL            0
#define SNTP_OPMODE_LISTENONLY      1

void sntp_setoperatingmode(u8_t operating_mode);

void sntp_init();

void sntp_stop();

void sntp_setservername(u8_t idx, const char* server);

typedef void (* sntp_sync_time_cb_t)(struct timeval* tv);

void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback);
