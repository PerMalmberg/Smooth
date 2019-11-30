#include "driver/spi_master.h"

esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t* bus_config, int dma_chan)
{
    return ESP_OK;
}

esp_err_t spi_bus_free(spi_host_device_t host)
{
    return ESP_OK;
}

esp_err_t spi_bus_add_device(spi_host_device_t host,
                             const spi_device_interface_config_t* dev_config,
                             spi_device_handle_t* handle)
{
    return ESP_OK;
}

esp_err_t spi_bus_remove_device(spi_device_handle_t handle)
{
    return ESP_OK;
}

esp_err_t spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t* trans_desc, TickType_t ticks_to_wait)
{
    return ESP_OK;
}

esp_err_t spi_device_get_trans_result(spi_device_handle_t handle,
                                      spi_transaction_t** trans_desc,
                                      TickType_t ticks_to_wait)
{
    return ESP_OK;
}

esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc)
{
    return ESP_OK;
}

esp_err_t spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc)
{
    return ESP_OK;
}
