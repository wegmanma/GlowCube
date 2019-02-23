/*
 * communication.h
 *
 *  Created on: 27.08.2017
 *      Author: marce
 */

#ifndef SRC_INC_COMMUNICATION_H_
#define SRC_INC_COMMUNICATION_H_
#include <string.h>

void COM_Init(void);
void COM_Flush_Buffers(void);
bool COM_RX_Available(void);
void uartPutChar(uint8_t ch);
uint32_t COM_RX_GetData(uint8_t * dataPtr);
bool COM_TX_Busy(void);
void COM_TX_PutData(int * dataPtr, uint32_t dataLen);
void UART0_RX_IRQHandler(void);
void UART0_TX_IRQHandler(void);

#endif /* SRC_INC_COMMUNICATION_H_ */
