#pragma once

/* Converts a time in milliseconds to a time in ticks. */
#define pdMS_TO_TICKS( xTimeInMs ) ( ( ( TickType_t ) ( xTimeInMs ) * configTICK_RATE_HZ ) / ( TickType_t ) 1000 )

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )
#define errQUEUE_EMPTY	( ( BaseType_t ) 0 )
#define errQUEUE_FULL	( ( BaseType_t ) 0 )