#pragma once

typedef void (*intr_handler_t)(void *arg);

typedef struct intr_handle_data_t intr_handle_data_t;
typedef intr_handle_data_t* intr_handle_t ;