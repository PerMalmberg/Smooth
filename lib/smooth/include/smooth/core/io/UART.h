

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/uart.h>
#pragma GCC diagnostic pop

#include <driver/gpio.h>

namespace smooth::core::io
{

  class UART
  {
  public:
    UART(uart_port_t port, int baud_rate, uart_word_length_t data_bits, uart_parity_t parity, uart_stop_bits_t stop_bits, uart_hw_flowcontrol_t flow_control, gpio_num_t tx_pin, gpio_num_t rx_pin, gpio_num_t rts_pin, gpio_num_t cts_pin, uart_mode_t mode = UART_MODE_UART);

    virtual ~UART();

    bool get_word_length(uart_word_length_t &data_bit);

    bool set_word_length(const uart_word_length_t &data_bit);

    bool get_stop_bits(uart_stop_bits_t &stop_bits);

    bool set_stop_bits(const uart_stop_bits_t &stop_bits);

    bool get_parity(uart_parity_t &parity);

    bool set_parity(const uart_parity_t &parity);

    bool get_baud_rate(uint32_t &baud_rate);

    bool set_baud_rate(uint32_t baud_rate);

    bool set_line_inverse(uint32_t inverse_mask);

    bool get_hw_flow_ctrl(uart_hw_flowcontrol_t &flow_control);

    bool set_hw_flow_ctrl(uart_hw_flowcontrol_t flow_control, uint8_t rx_thresh);

    bool clear_intr_status(uint32_t clr_mask);

    bool enable_intr_mask(uint32_t enable_mask);

    bool disable_intr_mask(uint32_t disable_mask);

    bool enable_rx_intr();

    bool disable_rx_intr();

    bool disable_tx_intr();

    bool enable_tx_intr(int enable, int thresh);

    bool isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, uart_isr_handle_t *handle);

    bool isr_free();

    bool set_pin(gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts);

    bool set_rts(gpio_num_t rts);

    bool set_dtr(gpio_num_t dtr);

    bool set_tx_idle_num(uint16_t idle_num);

    bool param_config(const uart_config_t *uart_config);

    bool intr_config(uart_intr_config_t *cfg);

    bool wait_tx_done(TickType_t ticks_to_wait);

    bool tx_chars(const char *buffer, uint32_t len);

    bool write_bytes(const void *buffer, size_t size);

    bool write_bytes_with_break(const void *buffer, size_t size, int brk_len);

    bool read_bytes(void *buffer, size_t size, TickType_t ticks_to_wait);

    bool flush();

    bool get_buffered_data_len(size_t &size);

    bool get_tx_buffer_free_size(size_t &size);

    bool disable_pattern_det_intr();

    bool enable_pattern_det_baud_intr(char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle);

    int pattern_pop_pos();

    int pattern_get_pos();

    bool pattern_queue_reset(int queue_len);

    bool set_mode(uart_mode_t mode);

    bool set_rx_full_threshold(int threshold);

    bool set_tx_empty_threshold(int threshold);

    bool set_rx_timeout(const uint8_t timeout);

    bool get_collision_flag(bool &flag);

    bool get_wakeup_threshold(int &threshold);

    bool set_wakeup_threshold(int threshold);

    bool wait_tx_idle_polling();

    bool set_loop_back(bool enable);

    void set_always_rx_timeout(bool enable);

  private:
    uart_port_t port_;

    uart_config_t config_;

    void log_error(const std::string msg, esp_err_t err);
  };

} // namespace smooth::core::io