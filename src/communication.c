/*
 * communication.c
 *
 *  Created on: 27.08.2017
 *      Author: marce
 */
#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "inc/timers.h"
#include "inc/communication.h"

#define COM_UART		USART0				///< UART for communication
#define COM_CLOCK		cmuClock_USART0	///< UART clock
#define COM_LOCATION	USART_ROUTE_LOCATION_LOC5	///< TX RX route
#define COM_TX_PORT		gpioPortC			///< Port for TX
#define COM_TX_PIN		0					///< Pin for TX
#define COM_RX_PORT		gpioPortC			///< Port for RX
#define COM_RX_PIN		1					///< Pin for RX
#define COM_CTS_PORT		gpioPortC			///< Port for TX
#define COM_CTS_PIN		2					///< Pin for TX
#define COM_RTS_PORT		gpioPortC			///< Port for RX
#define COM_RTS_PIN		3					///< Pin for RX
#define COM_BUF_SIZE 256

bool COM_RX_Available_Flag = false;			///< received data is available
char COM_RX_Data[COM_BUF_SIZE] = "";		///< buffer for received data
bool COM_TX_Busy_Flag = false;				///< busy with sending
char COM_TX_Data[COM_BUF_SIZE] = "";		///< buffer for data to be sent

// string buffers and indices for TX and RX
char TX_buf[COM_BUF_SIZE] = "";				///< transmit buffer
char RX_buf[COM_BUF_SIZE] = "";				///< receive buffer
uint8_t TX_index = 0;						///< transmit buffer index
uint8_t RX_index = 0;						///< receive buffer index

volatile struct circularBuffer
{
  uint8_t  data[COM_BUF_SIZE];  /* data buffer */
  uint32_t rdI;               /* read index */
  uint32_t wrI;               /* write index */
  uint32_t pendingBytes;      /* count of how many bytes are not yet handled */
  bool     overflow;          /* buffer overflow indicator */
} rxBuf, txBuf = { {0}, 0, 0, 0, false };

void COM_Init(void) {
	CMU_ClockEnable(COM_CLOCK, true);		// Enable USART clock

	/* Configure the GPIOs */
	CMU_ClockEnable(cmuClock_GPIO, true);	// Enable clock for GPIO module
	GPIO_PinModeSet(COM_TX_PORT, COM_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(COM_RX_PORT, COM_RX_PIN, gpioModeInput, 0);
	// GPIO_PinModeSet(COM_RX_PORT, COM_RX_PIN, gpioModeInputPull, 1); 	// with pullup


	USART_Reset(COM_UART);
	/* change fields of USARTInit, if standard values don't fit */
	USART_InitAsync_TypeDef usartInit = USART_INITASYNC_DEFAULT;
	  usartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
	  usartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
	  usartInit.baudrate     = 230400;         /* Baud rate */
	  usartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
	  usartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
	  usartInit.parity       = usartNoParity;  /* Parity mode */
	  usartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
	  usartInit.mvdis        = false;          /* Disable majority voting */
	  usartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
	  usartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */
		USART_InitAsync(COM_UART, &usartInit);

	/* Enable TX and RX and route to GPIO pins */
	COM_UART->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | COM_LOCATION;


	/* Configure interrupts */
	  // USART_IntClear(COM_UART, USART_IF_RXDATAV);
	  // USART_IntEnable(COM_UART, USART_IF_RXDATAV);
	  // NVIC_ClearPendingIRQ(USART0_RX_IRQn);
	  // NVIC_ClearPendingIRQ(USART0_TX_IRQn);
	  // NVIC_EnableIRQ(USART0_RX_IRQn);
	  // NVIC_EnableIRQ(USART0_TX_IRQn);


	/* Configure Handshake */
	GPIO_PinModeSet(COM_CTS_PORT, COM_CTS_PIN, gpioModeInput, 0);
	GPIO_PinModeSet(COM_RTS_PORT, COM_RTS_PIN, gpioModePushPull, 0);
	GPIO_IntConfig(COM_CTS_PORT, COM_CTS_PIN, true, false, true);

	USART_Enable(COM_UART, usartEnable);
}

void COM_Flush_Buffers(void) {
	RX_buf[0] = '\0';
	RX_index = 0;
	COM_RX_Data[0] = '\0';
	COM_RX_Available_Flag = false;
	TX_buf[0] = '\0';
	TX_index = 0;
	COM_TX_Data[0] = '\0';
	COM_TX_Busy_Flag = false;
}

bool COM_RX_Available(void) {
	return COM_RX_Available_Flag;
}

void uartPutChar(uint8_t ch)
{
  /* Check if Tx queue has room for new data */
  if ((txBuf.pendingBytes + 1) > COM_BUF_SIZE)
  {
    /* Wait until there is room in queue */
    while ((txBuf.pendingBytes + 1) > COM_BUF_SIZE) ;
  }

  /* Copy ch into txBuffer */
  txBuf.data[txBuf.wrI] = ch;
  txBuf.wrI             = (txBuf.wrI + 1) % COM_BUF_SIZE;

  /* Increment pending byte counter */
  txBuf.pendingBytes++;

  /* Enable interrupt on USART TX Buffer*/
  USART_IntEnable(COM_UART, USART_IF_TXBL);
}

uint32_t COM_RX_GetData(uint8_t * dataPtr, uint32_t dataLen)
{
  uint32_t i = 0;

  /* Wait until the requested number of bytes are available */
  if (rxBuf.pendingBytes < dataLen)
  {
    while (rxBuf.pendingBytes < dataLen) ;
  }

  if (dataLen == 0)
  {
    dataLen = rxBuf.pendingBytes;
  }

  /* Copy data from Rx buffer to dataPtr */
  while (i < dataLen)
  {
    *(dataPtr + i) = rxBuf.data[rxBuf.rdI];
    rxBuf.rdI      = (rxBuf.rdI + 1) % COM_BUF_SIZE;
    i++;
  }

  /* Decrement pending byte counter */
  rxBuf.pendingBytes -= dataLen;

  return i;
}

bool COM_TX_Busy(void) {
	return COM_TX_Busy_Flag;
}

void COM_TX_PutData(int *dataPtr, uint32_t dataLen)
{
	GPIO->P[gpioPortC].DOUTSET = 1 << 3; // Handshake CTS Output High
	int i;
	for (i=0;i<dataLen;i++) {
		USART_Tx(COM_UART,dataPtr[i]);
	}
	GPIO->P[gpioPortC].DOUTCLR = 1 << 3; // Handshake CTS Output High
}

void UART0_RX_IRQHandler(void)
{
  /* Check for RX data valid interrupt */

  if (USART0->STATUS & USART_STATUS_RXDATAV)
  {

      /* Copy data into RX Buffer */
      uint8_t rxData = USART_Rx(COM_UART);
      rxBuf.data[rxBuf.wrI] = rxData;
      rxBuf.wrI             = (rxBuf.wrI + 1) % COM_BUF_SIZE;
      rxBuf.pendingBytes++;

      /* Flag Rx overflow */
      if (rxBuf.pendingBytes > COM_BUF_SIZE)
      {
        rxBuf.overflow = true;
      }

      /* Clear RXDATAV interrupt */
      USART_IntClear(USART0, USART_IF_RXDATAV);
    }
}

void UART0_TX_IRQHandler(void)
{
  /* Clear interrupt flags by reading them. */
  USART_IntGet(USART0);

  /* Check TX buffer level status */
  if (USART0->STATUS & USART_STATUS_TXBL)
  {
    if (txBuf.pendingBytes > 0)
    {
      /* Transmit pending character */
      USART_Tx(COM_UART, txBuf.data[txBuf.rdI]);
      txBuf.rdI = (txBuf.rdI + 1) % COM_BUF_SIZE;
      txBuf.pendingBytes--;
    }

    /* Disable Tx interrupt if no more bytes in queue */
    if (txBuf.pendingBytes == 0)
    {
      USART_IntDisable(COM_UART, USART_IF_TXBL);
    }
  }
}
