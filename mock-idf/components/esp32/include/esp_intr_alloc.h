#pragma once

typedef void (* intr_handler_t)(void* arg);

typedef struct intr_handle_data_t intr_handle_data_t;
typedef intr_handle_data_t* intr_handle_t;

#define ESP_INTR_FLAG_LEVEL1        (1<<1)  ///< Accept a Level 1 interrupt vector (lowest priority)
#define ESP_INTR_FLAG_LEVEL2        (1<<2)  ///< Accept a Level 2 interrupt vector
#define ESP_INTR_FLAG_LEVEL3        (1<<3)  ///< Accept a Level 3 interrupt vector
#define ESP_INTR_FLAG_LEVEL4        (1<<4)  ///< Accept a Level 4 interrupt vector
#define ESP_INTR_FLAG_LEVEL5        (1<<5)  ///< Accept a Level 5 interrupt vector
#define ESP_INTR_FLAG_LEVEL6        (1<<6)  ///< Accept a Level 6 interrupt vector
#define ESP_INTR_FLAG_NMI           (1<<7)  ///< Accept a Level 7 interrupt vector (highest priority)
#define ESP_INTR_FLAG_SHARED        (1<<8)  ///< Interrupt can be shared between ISRs
#define ESP_INTR_FLAG_EDGE          (1<<9)  ///< Edge-triggered interrupt
#define ESP_INTR_FLAG_IRAM          (1<<10) ///< ISR can be called if cache is disabled
#define ESP_INTR_FLAG_INTRDISABLED  (1<<11) ///< Return with this interrupt disabled

///< Low and medium prio interrupts. These can be handled in C.
#define ESP_INTR_FLAG_LOWMED    (ESP_INTR_FLAG_LEVEL1|ESP_INTR_FLAG_LEVEL2|ESP_INTR_FLAG_LEVEL3)

///< High level interrupts. Need to be handled in assembly.
#define ESP_INTR_FLAG_HIGH      (ESP_INTR_FLAG_LEVEL4|ESP_INTR_FLAG_LEVEL5|ESP_INTR_FLAG_LEVEL6|ESP_INTR_FLAG_NMI)
