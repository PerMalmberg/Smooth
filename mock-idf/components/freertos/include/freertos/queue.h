#pragma once

#include "portmacro.h"
#include "projdefs.h"

#define queueQUEUE_TYPE_BASE                ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_SET                 ( ( uint8_t ) 0U )
#define queueQUEUE_TYPE_MUTEX               ( ( uint8_t ) 1U )
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE  ( ( uint8_t ) 2U )
#define queueQUEUE_TYPE_BINARY_SEMAPHORE    ( ( uint8_t ) 3U )
#define queueQUEUE_TYPE_RECURSIVE_MUTEX     ( ( uint8_t ) 4U )

#define queueSEND_TO_BACK       ( ( BaseType_t ) 0 )
#define queueSEND_TO_FRONT      ( ( BaseType_t ) 1 )
#define queueOVERWRITE          ( ( BaseType_t ) 2 )

typedef void* QueueHandle_t;

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue );

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength,
                                   const UBaseType_t uxItemSize,
                                   const uint8_t ucQueueType );

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue,
                                     const void* const pvItemToQueue,
                                     BaseType_t* const pxHigherPriorityTaskWoken,
                                     const BaseType_t xCopyPosition );

BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t* const pxHigherPriorityTaskWoken );

BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void* const pvBuffer,
                                 BaseType_t* const pxHigherPriorityTaskWoken );

#define xQueueCreate( uxQueueLength, \
                      uxItemSize ) xQueueGenericCreate( ( uxQueueLength ), ( uxItemSize ), ( queueQUEUE_TYPE_BASE ) )

#define xQueueSendToBackFromISR( xQueue, pvItemToQueue, \
                                 pxHigherPriorityTaskWoken ) \
    xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )

#define xQueueReceive( xQueue, pvBuffer, \
                       xTicksToWait ) xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdFALSE )

BaseType_t xQueueGenericReceive( QueueHandle_t xQueue,
                                 void* const pvBuffer,
                                 TickType_t xTicksToWait,
                                 const BaseType_t xJustPeek );
