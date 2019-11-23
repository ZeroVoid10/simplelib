/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		nrf_comm.c
 * Description:		NRF
 * Author:			ZeroVoid
 * Version:			0.2
 * Data:			2019/11/08 Fri 21:06
 *******************************************************************************/
#include "nrf_comm.h"
#ifdef SL_NRF_COMM

#ifdef SL_CMD
#include "cmd.h"
#endif // SL_CMD

#include <string.h>

/*******************************************************************************
 * NRF Communication Val
 *******************************************************************************/

/* Public Val -----------------------------------------------------*/
uint8_t nrf_tx_buffer[99] = {0};
uint8_t nrf_tx_cnt = 0;
uint8_t nrf_tx_remainder = 0;

/* Private Val -----------------------------------------------------*/
NRF_COMM_STATE nrf_flow_state = NRF_IDLE;
static uint8_t _nrf_lock = 0;
static uint8_t cnt = 0;
DEFINE_KFIFO(nrf_tx_fifo, nrf_pck, NRF_COMM_TX_PCK_FIFO_SIZE);
DEFINE_KFIFO(nrf_rx_fifo, nrf_pck, NRF_COMM_RX_PCK_FIFO_SIZE);


/*******************************************************************************
 * NRF Communication Static Private Function Declaration
 *******************************************************************************/

static void _nrf_receive_callback(uint8_t *data, int len);
static inline void nrf_lock(void);
static inline void nrf_unlock(void);
#ifdef SL_NRF_COMM_CMD
static void nrf_comm_cmd(NRF_Handle *handle);
#endif // SL_NRF_COMM_CMD


/*******************************************************************************
 * NRF Communication API Definition
 *******************************************************************************/

/**
 * @brief	NRF 基于协议发送函数
 * @param   data        发送数据
 * @param   len         数据长度
 * @param   data_deal   NRF_UART / NRF_CAN / NRF_SPI 组合
 *          e.g. NRF_UART | NRF_CAN
 * @note    使用前设置to 和 from
 */
void nrf_comm_send(uint8_t *data, int len) {
    // nrf_handle.nrf_data_from = NRF_SPI;
    // nrf_handle.nrf_data_to = data_deal;
    nrf_tx_cnt = len/(32 - NRF_PCK_HEADER_SIZE) + 1;
    nrf_tx_data[1] = nrf_tx_cnt;
    nrf_tx_remainder = len%(32 - NRF_PCK_HEADER_SIZE);
    for (cnt = 0; cnt < nrf_tx_cnt - 1; cnt++) {
        nrf_lock();
        memcpy(nrf_tx_data + NRF_PCK_HEADER_SIZE, nrf_tx_buffer + NRF_PCK_SIZE*cnt, NRF_PCK_SIZE);
        _nrf_comm_send(nrf_tx_data, 32);
        while(_nrf_lock);
    }
    nrf_lock();
    memcpy(nrf_tx_data + NRF_PCK_HEADER_SIZE, nrf_tx_buffer + NRF_PCK_SIZE*cnt, nrf_tx_remainder);
    _nrf_comm_send(nrf_tx_data, NRF_PCK_HEADER_SIZE + nrf_tx_remainder);
    while(_nrf_lock);
}

// FIXME: ZeroVoid	2019/11/22	 NRF 连续发送 接收过程卡死问题
void nrf_main(void) {
    switch(nrf_flow_state) {

    case NRF_COMM_SEND:
        // _nrf_comm_send(nrf_handle.tx_data, nrf_handle.tx_len);
        nrf_comm_send(nrf_tx_buffer, nrf_handle.tx_len);
        nrf_flow_state = NRF_IDLE;
        break;

    case NRF_RX_CALLBACK:
        nrf_flow_state = NRF_IDLE;
		if (nrf_read_rx_data(nrf_rx_data, &nrf_handle.rx_len, NULL) >= 0) {
            _nrf_receive_callback(nrf_handle.rx_data, nrf_handle.rx_len);
        };
        break;
    case NRF_TX_CALLBACK:
        nrf_send_callback(nrf_handle.tx_data, nrf_handle.tx_len);
        memset(nrf_tx_data, 0, 32);
        nrf_flow_state = NRF_IDLE;
        break;
    case NRF_MAX_RT_CALLBACK:
        nrf_max_rt_callback(nrf_handle.tx_data, nrf_handle.tx_len);
        #ifdef SL_NRF_PC
        uprintf("[NRF] Max Retry\r\n");
        #endif // SL_NRF_PC
        memset(nrf_tx_data, 0, 32);
        nrf_flow_state = NRF_IDLE;
        break;
    
    default:
        break;
    }
}


__weak void nrf_spi_receive_callback(uint8_t *data, int len) {}
__weak void nrf_can_receive_callback(uint8_t *data, int len) {}
__weak void nrf_uart_receive_callback(uint8_t *data, int len) {}
__weak void nrf_receive_callback(uint8_t *data, int len) {}
__weak void nrf_send_callback(uint8_t *data, int len) {}
__weak void nrf_max_rt_callback(uint8_t *data, int len) {}

/*******************************************************************************
 * NRF Communication Private Function Definition
 *******************************************************************************/

/**
 * @brief	NRF GPIO IRQ Funcion
 * @note	Reload week nrf_irq_hanle in @nrf24l01.c
 */
void nrf_irq_handle(void) {
	uint8_t status = _nrf_get_status();
	if (NRF_STATUS_GET_RX_DR(status)) {
		nrf_flow_state = NRF_RX_CALLBACK;
		rx_callback_cnt++;
		nrf_read_rx_data(nrf_rx_data, &nrf_handle.rx_len, NULL);
	} else if (NRF_STATUS_GET_TX_DS(status)) {
		nrf_flow_state = NRF_TX_CALLBACK;
		nrf_unlock();
		
		_nrf_clear_tx_irq();
		if (!tx_pipe0_addr_eq && nrf_handle.config->send_crc_ack) {
			_nrf_set_rx_addr(0, nrf_rx_addr[0], nrf_handle.nrf_addr_len);
		}
		_nrf_flush_tx();
		NRF_CE_DISABLE();
		_nrf_set_mode(NRF_PRX);
		nrf_spi_delay();
		NRF_CE_ENABLE();
	} else if (NRF_STATUS_GET_MAX_RT(status)) {
		nrf_flow_state = NRF_MAX_RT_CALLBACK;
		nrf_unlock();

		_nrf_clear_maxrt_irq();
		_nrf_flush_tx();
		NRF_CE_DISABLE();
		_nrf_set_mode(NRF_PRX);
		nrf_spi_delay();
		NRF_CE_ENABLE();
	}
}

void _nrf_comm_send(uint8_t *data, int len) {
    data[0] = (nrf_handle.nrf_data_from << 4) | nrf_handle.nrf_data_to;
    nrf_handle.tx_data = data;
    nrf_handle.tx_len = len;
    nrf_send_data(data, len);
}

/* Static Private Functions -----------------------------------------------------*/
static void _nrf_receive_callback(uint8_t *data, int len) {
    // uint8_t cnt = (data[0])
    uint8_t deal_method = (data[0] & 0x0F);
    uint8_t data_from = (data[0] & 0xF0);
    UNUSED(data_from);
    if (deal_method & NRF_UART) {
        #ifdef SL_CMD
        // uprintf("rx cnt %d\r\n", rx_callback_cnt);
        // uprintf_to(&huart1, (char*)(nrf_rx_data + NRF_PCK_HEADER_SIZE));
        pCMD_USART->gState = HAL_UART_STATE_READY;
        HAL_UART_Transmit_DMA(pCMD_USART, nrf_rx_data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
        while(pCMD_USART->hdmatx->State != HAL_DMA_STATE_READY);
        rx_callback_cnt = 0;
        // uprintf((char*)(nrf_rx_data_buffer + NRF_PCK_HEADER_SIZE));
        // pCMD_USART->gState = HAL_UART_STATE_READY;
        // HAL_UART_Transmit_DMA(pCMD_USART, nrf_rx_data + NRF_PCK_HEADER_SIZE, len);
        // while(pCMD_USART->hdmatx->State != HAL_DMA_STATE_READY);
        #endif // SL_CMD
        nrf_uart_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
    }
    if (deal_method & NRF_CAN) {
        nrf_can_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
    }
    if (deal_method & NRF_SPI) {
        #ifdef SL_NRF_COMM_CMD
        nrf_comm_cmd(&nrf_handle);
        #endif // SL_NRF_COMM_CMD
        nrf_spi_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
    }
    #ifdef IND_LED_Pin
    HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
    #endif // IND_LED_Pin
    nrf_receive_callback(nrf_handle.rx_data, nrf_handle.rx_len);
}

#ifdef SL_NRF_COMM_CMD
static void nrf_comm_cmd(NRF_Handle *handle) {
    uint8_t arg = handle->rx_data[NRF_PCK_HEADER_SIZE]&0x0F;
    uint8_t cmd = handle->rx_data[NRF_PCK_HEADER_SIZE] >> 4;
    char str_tmp[] = "Ping OK\r\n";
    switch (cmd) {
    case NRF_COMM_CMD_ALL_CAN:
        nrf_all_can_send = arg;
        break;
    
    case NRF_COMM_CMD_PING:
        memcpy(nrf_tx_data + NRF_PCK_HEADER_SIZE, str_tmp, 10);
        nrf_handle.nrf_data_from = NRF_SPI;
        nrf_handle.nrf_data_to = NRF_UART;
        _nrf_comm_send(nrf_tx_data, 12);
        HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
        break;
    
    default:
        break;
    }
}
#endif // SL_NRF_COMM_CMD


#ifdef SL_NRF_HW_CAN
void _can_rx_nrf_callback(uint32_t *id, can_msg *data) {
    nrf_handle.nrf_data_from = NRF_CAN;
    nrf_handle.nrf_data_to = NRF_UART | NRF_SPI;
    nrf_handle.tx_len = 12 + NRF_PCK_HEADER_SIZE;
    memcpy(nrf_handle.tx_data + NRF_PCK_HEADER_SIZE, id, 4);
    memcpy(nrf_handle.tx_data + NRF_PCK_HEADER_SIZE + 4, data, 8);
    nrf_lock();
    _nrf_comm_send(nrf_handle.tx_data, nrf_handle.tx_len);
    while(_nrf_lock);
}
#endif // SL_NRF_HW_CAN

static inline void nrf_lock(void) {
    _nrf_lock = 1;
}
static inline void nrf_unlock(void) {
    _nrf_lock = 0;
}


#endif // SL_NRF_COMM