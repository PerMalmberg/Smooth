

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

    /// Get word length
    /// \param data_bit word length
    /// \return true on success, false on failure
    bool get_word_length(uart_word_length_t &data_bit);

    /// Set word length
    /// \param data_bit word length
    /// \return true on success, false on failure
    bool set_word_length(const uart_word_length_t &data_bit);

    /// Get stop bits
    /// \param stop_bits stop bits
    /// \return true on success, false on failure
    bool get_stop_bits(uart_stop_bits_t &stop_bits);

    /// Set stop bits
    /// \param stop_bits stop bits
    /// \return true on success, false on failure
    bool set_stop_bits(const uart_stop_bits_t &stop_bits);

    /// Get parity
    /// \param parity parity
    /// \return true on success, false on failure
    bool get_parity(uart_parity_t &parity);

    /// Set parity
    /// \param parity Parity
    /// \return true on success, false on failure
    bool set_parity(const uart_parity_t parity);

    /// Get baud rate
    /// \param baud_rate Reference to accept value of UART baud rate
    /// \return true on success, false on failure
    bool get_baud_rate(uint32_t &baud_rate);

    /// Set baud rate
    /// \param baud_rate baud rate
    /// \return true on success, false on failure
    bool set_baud_rate(uint32_t baud_rate);

    /// Set UART line inverse mode
    /// \param inverse_mask UART line inverse mode
    /// \return true on success, false on failure
    bool set_line_inverse(uint32_t inverse_mask);

    /// Get hardware flow control
    /// \param flow_control hardware flow control
    /// \return true on success, false on failure
    bool get_hw_flow_ctrl(uart_hw_flowcontrol_t &flow_control);

    /// Set hardware flow control
    /// \param flow_control Hardware flow control mode
    /// \param rx_thresh Threshold of Hardware RX flow control (0 ~ UART_FIFO_LEN).
    bool set_hw_flow_ctrl(uart_hw_flowcontrol_t flow_control, uint8_t rx_thresh);

    /// Clear UART interrupt status
    /// \param clr_mask Interrupt status to clear
    /// \return true on success, false on failure
    bool clear_intr_status(uint32_t clr_mask);

    /// Enable UART interrupt
    /// \param enable_mask Interrupt to enable
    /// \return true on success, false on failure
    bool enable_intr_mask(uint32_t enable_mask);

    /// Disable UART interrupt
    /// \param disable_mask Interrupt to disable
    /// \return true on success, false on failure
    bool disable_intr_mask(uint32_t disable_mask);

    /// Enable UART RX interrupt
    /// \return true on success, false on failure
    bool enable_rx_intr();

    /// Disable UART RX interrupt
    /// \return true on success, false on failure
    bool disable_rx_intr();

    /// Enable UART TX interrupt
    /// \param enable Enable or disable TX interrupt
    bool disable_tx_intr();

    /// Disable UART TX interrupt
    /// \param enable Enable or disable TX interrupt
    /// \param thresh Threshold of TX interrupt
    /// \return true on success, false on failure
    bool enable_tx_intr(int enable, int thresh);

    /// Register UART interrupt handler (ISR).
    /// \param fn Interrupt handler function.
    /// \param arg Parameter for interrupt handler function.
    /// \param intr_alloc_flags Flags used to allocate the interrupt. One (and only one) of ESP_INTR_FLAG_LOWMED or ESP_INTR_FLAG_HIGH must be set.
    /// \param handle Pointer to return handle. If non-NULL, a handle for the interrupt will be returned here.
    /// \return true on success, false on failure
    bool isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, uart_isr_handle_t *handle);

    /// Free UART interrupt handler registered by uart_isr_register. Must be called on the same core as uart_isr_register was called.
    /// \return true on success, false on failure
    bool isr_free();

    /// Assign signals of a UART peripheral to GPIO pins
    /// \param tx TX pin
    /// \param rx RX pin
    /// \param rts RTS pin
    /// \param cts CTS pin
    /// \return true on success, false on failure
    bool set_pin(gpio_num_t tx, gpio_num_t rx, gpio_num_t rts, gpio_num_t cts);

    /// Manually set RTS pin level
    /// \note UART must be configured with hardware flow control disabled.
    /// \param level 1: RTS output low (active); 0: RTS output high (block)
    /// \return true on success, false on failure
    bool set_rts(gpio_num_t level);

    /// Manually set the UART DTR pin level.
    /// \param dtr 1: DTR output low; 0: DTR output high
    /// \return true on success, false on failure
    bool set_dtr(gpio_num_t dtr);

    /// Set UART idle interval after tx FIFO is empty
    /// \param idle_num idle interval after tx FIFO is empty(unit: the time it takes to send one bit under current baudrate)
    /// \return true on success, false on failure
    bool set_tx_idle_num(uint16_t idle_num);

    /// Set UART configuration parameters.
    /// \param uart_config UART configuration parameters
    /// \return true on success, false on failure
    bool param_config(const uart_config_t *uart_config);

    /// Configure UART interrupts.
    /// \param cfg UART interrupt configuration parameters
    /// \return true on success, false on failure
    bool intr_config(uart_intr_config_t *cfg);

    /// Wait until UART TX FIFO is empty.
    /// \param ticks_to_wait Timeout, count in RTOS ticks
    /// \return true on success, false on failure
    bool wait_tx_done(TickType_t ticks_to_wait);

    /// Send data to the UART port from a given buffer and length.
    ///
    /// This function will not wait for enough space in TX FIFO. It will just fill the available TX FIFO and return when the FIFO is full.
    ///
    /// \note This function should only be used when UART TX buffer is not enabled.
    /// \param buffer Pointer to the buffer to send
    /// \param length Length of the buffer
    /// \param written Reference to the number of bytes written
    /// \return true on success, false on failure
    bool tx_chars(const char *buffer, uint32_t length, std::size_t &written);

    /// Send data to the UART port from a given buffer and length,
    ///
    /// If the UART driver's parameter 'tx_buffer_size' is set to zero:
    /// This function will not return until all the data have been sent out, or at least pushed into TX FIFO.
    ///
    /// Otherwise, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
    /// UART ISR will then move data from the ring buffer to TX FIFO gradually.
    ///
    /// \param buffer Pointer to the buffer to send
    /// \param length Length of the buffer
    /// \param written Reference to the number of bytes written
    /// \return true on success, false on failure
    bool write_bytes(const void *buffer, size_t length, std::size_t &written);

    /// Send data to the UART port from a given buffer and length
    /// If the UART driver's parameter 'tx_buffer_size' is set to zero:
    /// This function will not return until all the data and the break signal have been sent out.
    /// After all data is sent out, send a break signal.
    ///
    /// Otherwise, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
    /// UART ISR will then move data from the ring buffer to TX FIFO gradually.
    /// After all data sent out, send a break signal.
    ///
    /// \param buffer Pointer to the buffer to send
    /// \param length Length of the buffer
    /// \param brk_len Break signal duration(unit: the time it takes to send one bit at current baudrate)
    /// \param written Reference to the number of bytes written
    /// \return true on success, false on failure
    bool write_bytes_with_break(const void *buffer, size_t length, int brk_len, std::size_t &written);

    /// UART read bytes from UART buffer
    /// \param buffer Pointer to the buffer to read
    /// \param length Length of the buffer
    /// \param ticks_to_wait Timeout, count in RTOS ticks
    /// \return true on success, false on failure
    bool read_bytes(void *buffer, size_t length, TickType_t ticks_to_wait);

    /// Flush UART TX FIFO
    ///
    /// UART ring buffer flush. This will discard all data in the UART RX buffer
    /// \note Instead of waiting the data sent out, this function will clear UART rx buffer.
    /// In order to send all the data in tx FIFO, we can use uart_wait_tx_done function.
    /// \return true on success, false on failure
    bool flush();

    /// UART get RX ring buffer cached data length
    /// \param size Reference of size_t to accept cached data length
    /// \return true on success, false on failure
    bool get_buffered_data_len(size_t &size);

    /// UART get TX ring buffer free space size
    /// \param size Reference of size_t to accept the free space size
    /// \return true on success, false on failure
    bool get_tx_buffer_free_size(size_t &size);

    /// UART disable pattern detect function.
    ///
    /// Designed for applications like 'AT commands'.
    /// When the hardware detects a series of one same character, the interrupt will be triggered.
    /// \return true on success, false on failure
    bool disable_pattern_det_intr();

    /// UART enable pattern detect function.
    ///
    /// Designed for applications like 'AT commands'.
    /// When the hardware detect a series of one same character, the interrupt will be triggered.
    /// \param pattern_chr character of the pattern.
    /// \param chr_num number of the character, 8bit value.
    /// \param chr_tout timeout of the interval between each pattern characters, 16bit value, unit is the baud-rate cycle you configured.
    /// When the duration is more than this value, it will not take this data as at_cmd char.
    /// \param post_idle idle time after the last pattern character, 16bit value, unit is the baud-rate cycle you configured.
    /// When the duration is less than this value, it will not take the previous data as the last at_cmd char
    /// \param pre_idle idle time before the first pattern character, 16bit value, unit is the baud-rate cycle you configured.
    /// When the duration is less than this value, it will not take this data as the first at_cmd char.
    /// \return true on success, false on failure
    bool enable_pattern_det_baud_intr(char pattern_chr, uint8_t chr_num, int chr_tout, int post_idle, int pre_idle);

    /// Return the nearest detected pattern position in buffer.
    /// The positions of the detected pattern are saved in a queue,
    /// this function will dequeue the first pattern position and move the pointer to next pattern position.
    /// \note If the RX buffer is full and flow control is not enabled,
    /// the detected pattern may not be found in the rx buffer due to overflow.
    ///
    /// The following APIs will modify the pattern position info:
    /// uart_flush_input, uart_read_bytes, uart_driver_delete, uart_pop_pattern_pos
    /// It is the application's responsibility to ensure atomic access to the pattern queue and the rx data buffer
    /// when using pattern detect feature.
    /// \return the position of the pattern character in the ring buffer.
    int pattern_pop_pos();

    /// Return the nearest detected pattern position in buffer.
    /// The positions of the detected pattern are saved in a queue,
    /// This function do nothing to the queue.
    /// \note  If the RX buffer is full and flow control is not enabled,
    ///        the detected pattern may not be found in the rx buffer due to overflow.
    ///
    ///        The following APIs will modify the pattern position info:
    ///        flush_input, read_bytes, driver_delete, pop_pattern_pos
    ///        It is the application's responsibility to ensure atomic access to the pattern queue and the rx data buffer
    ///        when using pattern detect feature.
    /// \return the position of the pattern character in the ring buffer.
    int pattern_get_pos();

    /// Allocate a new memory with the given length to save record the detected pattern position in rx buffer.
    /// \param queue_len Max queue length for the detected pattern.
    /// If the queue length is not large enough, some pattern positions might be lost.
    /// Set this value to the maximum number of patterns that could be saved in data buffer at the same time.
    /// \return true on success, false on failure
    bool pattern_queue_reset(int queue_len);

    /// UART set communication mode
    /// \note This function must be executed after uart_driver_install(), when the driver object is initialized.
    /// \param mode UART communication mode
    /// \return true on success, false on failure
    bool set_mode(uart_mode_t mode);

    /// Set uart threshold value for RX fifo full
    /// \note If application is using higher baudrate and it is observed that bytes
    /// in hardware RX fifo are overwritten then this threshold can be reduced
    /// \param threshold Threshold value above which RX fifo full interrupt is generated
    /// \return true on success, false on failure
    bool set_rx_full_threshold(int threshold);

    /// Set uart threshold values for TX fifo empty
    /// \param threshold Threshold value below which TX fifo empty interrupt is generated
    /// \return true on success, false on failure
    bool set_tx_empty_threshold(int threshold);

    /// UART set threshold timeout for TOUT feature
    /// \param tout This parameter defines timeout threshold in uart symbol periods. The maximum value of threshold is 126.
    ///             tout_thresh = 1, defines TOUT interrupt timeout equal to transmission time of one symbol (~11 bit) on current baudrate.
    ///             If the time is expired the UART_RXFIFO_TOUT_INT interrupt is triggered. If tout_thresh == 0,
    ///             the TOUT feature is disabled.
    /// \return true on success, false on failure
    bool set_rx_timeout(const uint8_t timeout);

    /// Returns collision detection flag for RS485 mode
    /// Function returns the collision detection flag into variable pointed by collision_flag.
    /// collision_flag = true, if collision detected else it is equal to false.
    /// This function should be executed when actual transmission is completed (after uart_write_bytes()).
    /// \param flag Reference of bool to accept the collision flag
    /// \return true on success, false on failure
    bool get_collision_flag(bool &flag);

    /// Get the number of RX pin signal edges for light sleep wakeup.
    ///
    /// See description of set_wakeup_threshold for the explanation of UART
    /// wakeup feature.
    /// \param[out] out_wakeup_threshold  output, set to the current value of wakeup
    ///                                   threshold for the given UART.
    /// \return true on success, false on failure
    bool get_wakeup_threshold(int &threshold);

    /// Set the number of RX pin signal edges for light sleep wakeup
    ///
    /// UART can be used to wake up the system from light sleep. This feature works
    /// by counting the number of positive edges on RX pin and comparing the count to
    /// the threshold. When the count exceeds the threshold, system is woken up from
    /// light sleep. This function allows setting the threshold value.
    ///
    /// Stop bit and parity bits (if enabled) also contribute to the number of edges.
    /// For example, letter 'a' with ASCII code 97 is encoded as 0100001101 on the wire
    /// (with 8n1 configuration), start and stop bits included. This sequence has 3
    /// positive edges (transitions from 0 to 1). Therefore, to wake up the system
    /// when 'a' is sent, set wakeup_threshold=3.
    ///
    /// The character that triggers wakeup is not received by UART (i.e. it can not
    /// be obtained from UART FIFO). Depending on the baud rate, a few characters
    /// after that will also not be received. Note that when the chip enters and exits
    /// light sleep mode, APB frequency will be changing. To make sure that UART has
    /// correct baud rate all the time, select REF_TICK as UART clock source,
    /// by setting use_ref_tick field in uart_config_t to true.
    ///
    /// \note in ESP32, the wakeup signal can only be input via IO_MUX (i.e.
    ///       GPIO3 should be configured as function_1 to wake up UART0,
    ///       GPIO9 should be configured as function_5 to wake up UART1), UART2
    ///       does not support light sleep wakeup feature.
    ///
    /// \param uart_num  UART number, the max port number is (UART_NUM_MAX -1).
    /// \param wakeup_threshold  number of RX edges for light sleep wakeup, value is 3 .. 0x3ff.
    bool set_wakeup_threshold(int threshold);

    /// Wait until UART tx memory empty and the last char send ok (polling mode).
    /// \return true on success, false on failure
    bool wait_tx_idle_polling();

    /// Configure TX signal loop back to RX module, just for the test usage.
    /// \param enable Set true to enable the loop back function, else set it false.
    /// \return true on success, false on failure
    bool set_loop_back(bool enable);

    /// Configure behavior of UART RX timeout interrupt.
    ///
    /// When always_rx_timeout is true, timeout interrupt is triggered even if FIFO is full.
    /// This function can cause extra timeout interrupts triggered only to send the timeout event.
    /// Call this function only if you want to ensure timeout interrupt will always happen after a byte stream.
    /// \param enable Set to false enable the default behavior of timeout interrupt,
    ///               set it to true to always trigger timeout interrupt.
    /// \return true on success, false on failure
    void set_always_rx_timeout(bool enable);

  private:
    uart_port_t port_;

    uart_config_t config_;
    
    /// Install UART driver and set the UART to the default configuration.
    ///
    /// UART ISR handler will be attached to the same CPU core that this function is running on.
    ///
    /// \note Rx_buffer_size should be greater than UART_FIFO_LEN. Tx_buffer_size should be either zero or greater than UART_FIFO_LEN.
    /// 
    /// \param rx_buffer_size UART RX ring buffer size.
    /// \param tx_buffer_size UART TX ring buffer size.
    ///        If set to zero, driver will not use TX buffer, TX function will block task until all data have been sent out.
    /// \param queue_size UART event queue size/depth.
    /// \param uart_queue UART event queue handle (out param). On success, a new queue handle is written here to provide
    ///        access to UART events. If set to NULL, driver will not use an event queue.
    /// \param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
    ///        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info. Do not set ESP_INTR_FLAG_IRAM here
    ///        (the driver's ISR handler is not located in IRAM)
    bool driver_install(int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);
    
    /// Uninstall UART driver.
    /// \return true on success, false on failure
    bool driver_delete();
    
    /// Get the UART driver installed flag.
    /// \return true if the UART driver is installed, false if not.
    bool is_driver_installed() const;

    /// Log an error message.
    /// \param msg The error message
    /// \param err The error code
    void log_error(const std::string msg, esp_err_t err);
  };

} // namespace smooth::core::io