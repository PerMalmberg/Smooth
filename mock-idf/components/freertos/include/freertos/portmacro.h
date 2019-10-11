#pragma once

#include <cstdint>

#define portCHAR        int8_t
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        int32_t
#define portSHORT       int16_t
#define portSTACK_TYPE  uint8_t
#define portBASE_TYPE   int

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef unsigned portBASE_TYPE UBaseType_t;

typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
