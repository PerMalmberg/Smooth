#include <freertos/queue.h>

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t /*xQueue*/ )
{
    return 0;
}

QueueHandle_t xQueueGenericCreate( const UBaseType_t /*uxQueueLength*/, const UBaseType_t /*uxItemSize*/,
                                   const uint8_t /*ucQueueType*/ )
{
    return nullptr;
}

BaseType_t xQueueGenericSendFromISR( QueueHandle_t /*xQueue*/,
                                     const void* const /*pvItemToQueue*/,
                                     BaseType_t* const /*pxHigherPriorityTaskWoken*/,
                                     const BaseType_t /*xCopyPosition*/ )
{
    return pdPASS;
}

BaseType_t xQueueGiveFromISR( QueueHandle_t /*xQueue*/, BaseType_t* const /*pxHigherPriorityTaskWoken*/)
{
    return pdPASS;
}

BaseType_t xQueueReceiveFromISR( QueueHandle_t /*xQueue*/, void* const /*pvBuffer*/,
                                 BaseType_t* const /*pxHigherPriorityTaskWoken*/ )
{
    return pdFAIL;
}

BaseType_t xQueueGenericReceive( QueueHandle_t /*xQueue*/,
                                 void* const /*pvBuffer*/,
                                 TickType_t /*xTicksToWait*/,
                                 const BaseType_t /*xJustPeek*/ )
{
    return pdFAIL;
}
