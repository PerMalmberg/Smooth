/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <sstream>

#include "smooth/core/io/UART.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::io
{
  constexpr const char* log_tag = "UART";

  UART::UART(uart_port_t port, int baud_rate, uart_word_length_t data_bits, uart_parity_t parity, uart_stop_bits_t stop_bits, uart_hw_flowcontrol_t flow_control, gpio_num_t tx_pin, gpio_num_t rx_pin, gpio_num_t rts_pin, gpio_num_t cts_pin, uart_mode_t mode)
      : port_(port)
  {
    this->config_ = uart_config_t{
        .baud_rate = baud_rate,
        .data_bits = data_bits,
        .parity = parity,
        .stop_bits = stop_bits,
        .flow_ctrl = flow_control,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(this->port_, 1024, 0, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_param_config(this->port_, &this->config_));

    ESP_ERROR_CHECK(uart_set_pin(this->port_, tx_pin, rx_pin, rts_pin, cts_pin));

    ESP_ERROR_CHECK(uart_set_mode(this->port_, mode));
  }

  UART::~UART()
  {
    ESP_ERROR_CHECK(uart_driver_delete(this->port_));
  }

  bool UART::get_word_length(uart_word_length_t &data_bit)
  {
    const auto res = uart_get_word_length(this->port_, &data_bit);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get word length", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_word_length(const uart_word_length_t &data_bit)
  {
    auto res = uart_set_word_length(this->port_, data_bit);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set word length", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_stop_bits(uart_stop_bits_t &stop_bits)
  {
    const auto res = uart_get_stop_bits(this->port_, &stop_bits);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get stop bits", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_stop_bits(const uart_stop_bits_t &stop_bits)
  {
    const auto res = uart_set_stop_bits(this->port_, stop_bits);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set stop bits", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_parity(uart_parity_t &parity)
  {
    const auto res = uart_get_parity(this->port_, &parity);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get parity", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_parity(const uart_parity_t parity)
  {
    const auto res = uart_set_parity(this->port_, parity);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set parity", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_baud_rate(uint32_t &baud_rate)
  {
    const auto res = uart_get_baudrate(this->port_, &baud_rate);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get baud rate", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_baud_rate(uint32_t baud_rate)
  {
    const auto res = uart_set_baudrate(this->port_, baud_rate);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set baud rate", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_line_inverse(uint32_t inverse_mask)
  {
    const auto res = uart_set_line_inverse(this->port_, inverse_mask);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set line inverse", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_hw_flow_ctrl(uart_hw_flowcontrol_t &flow_control)
  {
    const auto res = uart_get_hw_flow_ctrl(this->port_, &flow_control);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get hw flow control", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_hw_flow_ctrl(uart_hw_flowcontrol_t flow_control, uint8_t rx_thresh)
  {
    const auto res = uart_set_hw_flow_ctrl(this->port_, flow_control, rx_thresh);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set hw flow control", res);
    }
    return ESP_OK == res;
  }

  bool UART::clear_intr_status(uint32_t clr_mask)
  {
    const auto res = uart_clear_intr_status(this->port_, clr_mask);
    if(res != ESP_OK)
    {
      this->log_error("Failed to clear interrupt status", res);
    }
    return ESP_OK == res;    
  }

  bool UART::enable_intr_mask(uint32_t enable_mask)
  {
    const auto res = uart_enable_intr_mask(this->port_, enable_mask);
    if(res != ESP_OK)
    {
      this->log_error("Failed to enable interrupt mask", res);
    }
    return ESP_OK == res;
  }

  bool UART::disable_intr_mask(uint32_t disable_mask)
  {
    const auto res = uart_disable_intr_mask(this->port_, disable_mask);
    if(res != ESP_OK)
    {
      this->log_error("Failed to disable interrupt mask", res);
    }
    return ESP_OK == res;
  }

  bool UART::enable_rx_intr()
  {
    const auto res = uart_enable_rx_intr(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to enable rx interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::disable_rx_intr()
  {
    const auto res = uart_disable_rx_intr(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to disable rx interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::disable_tx_intr()
  {
    const auto res = uart_disable_tx_intr(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to disable tx interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::enable_tx_intr(int enable, int threshold)
  {
    const auto res = uart_enable_tx_intr(this->port_, enable, threshold);
    if(res != ESP_OK)
    {
      this->log_error("Failed to enable tx interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, uart_isr_handle_t *handle)
  {
    const auto res = uart_isr_register(this->port_, fn, arg, intr_alloc_flags, handle);
    if(res != ESP_OK)
    {
      this->log_error("Failed to register interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::isr_free()
  {
    const auto res = uart_isr_free(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to free interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_pin(gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts)
  {
    const auto res = uart_set_pin(this->port_, tx, rx, rts, cts);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set pin", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_rts(gpio_num_t rts)
  {
    const auto res = uart_set_rts(this->port_, rts);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set rts", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_dtr(gpio_num_t dtr)
  {
    const auto res = uart_set_dtr(this->port_, dtr);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set dtr", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_tx_idle_num(uint16_t idle_num)
  {
    const auto res = uart_set_tx_idle_num(this->port_, idle_num);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set tx idle num", res);
    }
    return ESP_OK == res;
  }

  bool UART::param_config(const uart_config_t *uart_config)
  {
    const auto res = uart_param_config(this->port_, uart_config);
    if(res != ESP_OK)
    {
      this->log_error("Failed to configure uart", res);
    }
    return ESP_OK == res;
  }

  bool UART::intr_config(uart_intr_config_t *cfg)
  {
    const auto res = uart_intr_config(this->port_, cfg);
    if(res != ESP_OK)
    {
      this->log_error("Failed to configure interrupt", res);
    }
    return ESP_OK == res;
  }

  bool UART::wait_tx_done(TickType_t ticks_to_wait)
  {
    const auto res = uart_wait_tx_done(this->port_, ticks_to_wait);
    if(res != ESP_OK)
    {
      this->log_error("Failed to wait for tx done", res);
    }
    return ESP_OK == res;
  }

  bool UART::tx_chars(const char *buffer, uint32_t len, std::size_t &written)
  {
    const auto res = uart_tx_chars(this->port_, buffer, len);
    if(res != ESP_OK)
    {
      this->log_error("Failed to tx chars", res);
    }
    else if(res > 0)
    {
      written = static_cast<std::size_t>(res);
    }
    return ESP_OK == res;
  }

  bool UART::write_bytes(const void* buffer, size_t length, std::size_t &written)
  {
    const auto res = uart_write_bytes(this->port_, buffer, length);
    if(res != ESP_OK)
    {
      this->log_error("Failed to write bytes", res);
    }
    else if(res > 0)
    {
      written = static_cast<std::size_t>(res);
    }
    return ESP_OK == res;
  }

  bool UART::write_bytes_with_break(const void* buffer, size_t length, int brk_len, std::size_t &written)
  {
    const auto res = uart_write_bytes_with_break(this->port_, buffer, length, brk_len);
    if(res != ESP_OK)
    {
      this->log_error("Failed to write bytes with break", res);
    }
    else if(res > 0)
    {
      written = static_cast<std::size_t>(res);
    }
    return ESP_OK == res;
  }

  bool UART::read_bytes(void *buffer, size_t length, TickType_t ticks_to_wait)
  {
    const auto res = uart_read_bytes(this->port_, buffer, length, ticks_to_wait);
    if(res != ESP_OK)
    {
      this->log_error("Failed to read bytes", res);
    }
    return ESP_OK == res;
  }

  bool UART::flush()
  {
    const auto res = uart_flush(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to flush", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_buffered_data_len(size_t &size)
  {
    const auto res = uart_get_buffered_data_len(this->port_, &size);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get buffered data len", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_tx_buffer_free_size(size_t &size)
  {
    const auto res = uart_get_tx_buffer_free_size(this->port_, &size);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get tx buffer free size", res);
    }
    return ESP_OK == res;
  }

  bool UART::disable_pattern_det_intr()
  {
    const auto res = uart_disable_pattern_det_intr(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to disable pattern det intr", res);
    }
    return ESP_OK == res;
  }

  bool UART::enable_pattern_det_baud_intr(char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle)
  {
    const auto res = uart_enable_pattern_det_baud_intr(this->port_, pattern_chr, chr_num, chr_tout, post_idle, pre_idle);
    if(res != ESP_OK)
    {
      this->log_error("Failed to enable pattern det baud intr", res);
    }
    return ESP_OK == res;
  }

  int UART::pattern_pop_pos()
  {
    return uart_pattern_pop_pos(this->port_);
  }

  int UART::pattern_get_pos()
  {
    return uart_pattern_get_pos(this->port_);
  }

  bool UART::pattern_queue_reset(int queue_len)
  {
    const auto res = uart_pattern_queue_reset(this->port_, queue_len);
    if(res != ESP_OK)
    {
      this->log_error("Failed to reset pattern queue", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_mode(uart_mode_t mode)
  {
    const auto res = uart_set_mode(this->port_, mode);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set mode", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_rx_full_threshold(int threshold)
  {
    const auto res = uart_set_rx_full_threshold(this->port_, threshold);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set rx full threshold", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_tx_empty_threshold(int threshold)
  {
    const auto res = uart_set_tx_empty_threshold(this->port_, threshold);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set tx empty threshold", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_rx_timeout(const uint8_t timeout)
  {
    const auto res = uart_set_rx_timeout(this->port_, timeout);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set rx timeout", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_collision_flag(bool &flag)
  {
    const auto res = uart_get_collision_flag(this->port_, &flag);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get collision flag", res);
    }
    return ESP_OK == res;
  }

  bool UART::get_wakeup_threshold(int &threshold)
  {
    const auto res = uart_get_wakeup_threshold(this->port_, &threshold);
    if(res != ESP_OK)
    {
      this->log_error("Failed to get wakeup threshold", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_wakeup_threshold(int threshold)
  {
    const auto res = uart_set_wakeup_threshold(this->port_, threshold);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set wakeup threshold", res);
    }
    return ESP_OK == res;
  }

  bool UART::wait_tx_idle_polling()
  {
    const auto res = uart_wait_tx_idle_polling(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to wait for tx idle polling", res);
    }
    return ESP_OK == res;
  }

  bool UART::set_loop_back(bool enable)
  {
    const auto res = uart_set_loop_back(this->port_, enable);
    if(res != ESP_OK)
    {
      this->log_error("Failed to set loop back", res);
    }
    return ESP_OK == res;
  }

  void UART::set_always_rx_timeout(bool enable)
  {
    uart_set_always_rx_timeout(this->port_, enable);
  }

  bool UART::driver_install(int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags)
  {
    const auto res = uart_driver_install(this->port_, rx_buffer_size, tx_buffer_size, queue_size, uart_queue, intr_alloc_flags);
    if(res != ESP_OK)
    {
      this->log_error("Failed to install driver", res);
    }
    return ESP_OK == res;
  }
    
  bool UART::driver_delete()
  {
    const auto res = uart_driver_delete(this->port_);
    if(res != ESP_OK)
    {
      this->log_error("Failed to delete driver", res);
    }
    return ESP_OK == res;
  }
    
  bool UART::is_driver_installed() const
  {
    return uart_is_driver_installed(this->port_);
  }

  void UART::log_error(const std::string msg, esp_err_t err)
  {
    std::stringstream ss;
    ss << "UART Error for port '" << static_cast<int>(port_) << "', " << msg << ": " << esp_err_to_name(err);
  }

} // namespace smooth::core::io