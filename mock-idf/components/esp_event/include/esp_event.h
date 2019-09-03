#pragma once

// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_err.h"

#include "esp_event_base.h"


esp_err_t esp_event_handler_register(esp_event_base_t event_base,
                                     int32_t event_id,
                                     esp_event_handler_t event_handler,
                                     void* event_handler_arg);

/**
 * @brief Register an event handler to a specific loop.
 *
 * This function behaves in the same manner as esp_event_handler_register, except the additional
 * specification of the event loop to register the handler to.
 *
 * @param[in] event_loop the event loop to register this handler function to
 * @param[in] event_base the base id of the event to register the handler for
 * @param[in] event_id the id of the event to register the handler for
 * @param[in] event_handler the handler function which gets called when the event is dispatched
 * @param[in] event_handler_arg data, aside from event data, that is passed to the handler when it is called
 *
 * @note the event loop library does not maintain a copy of event_handler_arg, therefore the user should
 * ensure that event_handler_arg still points to a valid location by the time the handler gets called
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_register_with(esp_event_loop_handle_t event_loop,
                                          esp_event_base_t event_base,
                                          int32_t event_id,
                                          esp_event_handler_t event_handler,
                                          void* event_handler_arg);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function can be used to unregister a handler so that it no longer gets called during dispatch.
 * Handlers can be unregistered for either: (1) specific events, (2) all events of a certain event base,
 * or (3) all events known by the system event loop
 *
 *  - specific events: specify exact event_base and event_id
 *  - all events of a certain base: specify exact event_base and use ESP_EVENT_ANY_ID as the event_id
 *  - all events known by the loop: use ESP_EVENT_ANY_BASE for event_base and ESP_EVENT_ANY_ID as the event_id
 *
 * This function ignores unregistration of handlers that has not been previously registered.
 *
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return ESP_OK success
 * @return ESP_ERR_INVALID_ARG invalid combination of event base and event id
 * @return others fail
 */
esp_err_t esp_event_handler_unregister(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function behaves in the same manner as esp_event_handler_unregister, except the additional specification of
 * the event loop to unregister the handler with.
 *
 * @param[in] event_loop the event loop with which to unregister this handler function
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALID_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_unregister_with(esp_event_loop_handle_t event_loop,
                                            esp_event_base_t event_base,
                                            int32_t event_id,
                                            esp_event_handler_t event_handler);
