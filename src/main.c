
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_core.h"
#include "inc/timers.h"
#include "inc/globals.h"
#include "inc/communication.h"
#include "wifi_bglib.h"
#include "apitypes.h"

	volatile int prog_case = 1;		///< set with pushbutton 0 interrupt
	volatile int pressed = 0;
	volatile int wifi_seq = 0;
	volatile int press_cnt = 0;
	volatile int lamp_on = 1;
	volatile int long_press = 0;
	volatile int long_press_cnt = 0;
	int data_to_send[] = {0x8,0x4,0x6,0x1,0x2,0xD,0x4,0x1};
	int wifi_activate[] = {0x8,0x0,0x3,0x0};
	int wifi_scan[] = {0x08,0x0f,0x03,0x03,0x00,0x0d,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d};
	int wifi_passwd[] = {0x08,0x14,0x03,0x05,0x13,0x77,0x6e,0x78,0x72,0x2d,0x72,0x37,0x32,0x70,0x2d,0x75,0x76,0x79,0x7a,0x2d,0x74,0x69,0x39,0x74};
	int wifi_conn[] = {0x08,0x0a,0x03,0x07,0x09,0x7a,0x77,0x7a,0x2d,0x36,0x32,0x35,0x32,0x35};
	int wifi_endp[] = {0x08,0x03,0x04,0x02,0x40,0x9c,0x00};
	int data_get[] = {0x08,0x00,0x01,0x00};
	uint8_t inputbuffer[25] = {0};
	const uint32_t data_length 			= 8;
	const uint32_t acti_length           = 4;
    const uint32_t scan_length			= 19;
    const uint32_t pw_length			= 24;
    const uint32_t conn_length			= 14;
    const uint32_t endp_length			= 7;
	const uint32_t get_length           = 4;
	  uint32_t red, green, blue, max, cnt, min, cnt_slow;


int main(void)

{
  /* Chip errata */
  CHIP_Init();


    INIT_XOclocks();
    CMU_ClockEnable(cmuClock_GPIO, true);
	timers_init(55000); // Initialize Timers for Setpoint CCs
	GPIO_PinModeSet(gpioPortA, 10, gpioModeInputPullFilter , 1);

	GPIO_IntConfig(gpioPortA, 10, true, true, true);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	COM_Init(); // Initialize Communications

	int i;

	  max = 45000;
	  min = 0;
	  red = min;
	  green = min;
	  blue = min;
	  cnt = 0;
	  cnt_slow = 0;

	  green=max;
	  blue = min;
	  red = max;
	  ML_setLight(red,green,blue);

    COM_TX_PutData(data_to_send, data_length);
	for  (i=0; i<100; i++){
	}
	  green=max;
	  blue = min;
	  red = min;
	  ML_setLight(red,green,blue);
    COM_TX_PutData(wifi_activate, acti_length);
	for  (i=0; i<100; i++){
	}
	  green=max;
	  blue = max;
	  red = min;
	  ML_setLight(red,green,blue);
    COM_TX_PutData(wifi_scan, scan_length);
	for  (i=0; i<100; i++){
	}
	  green=min;
	  blue = max;
	  red = min;
	  ML_setLight(red,green,blue);
    COM_TX_PutData(wifi_passwd, pw_length);
	for  (i=0; i<100; i++){
	}
	  green=min;
	  blue = max;
	  red = max;
	  ML_setLight(red,green,blue);
    COM_TX_PutData(wifi_conn, conn_length);
	for  (i=0; i<100; i++){

	}
	  green=min;
	  blue = min;
	  red = max;
	  ML_setLight(red,green,blue);
	COM_TX_PutData(wifi_endp, endp_length);


  /* Infinite loop */
  while (1) {
		if (cnt < 25) {
			  cnt++;
		} else {
			  cnt = 0;
		}

		if (cnt == 0) {
			if (cnt_slow < 50) {
				  cnt_slow++;
			} else {
				  cnt_slow = 0;
				  COM_TX_PutData(data_get, get_length);
				  COM_RX_GetData(inputbuffer,24);
				  if (inputbuffer[14]== 0) {
					  green=max;
					  blue = min;
					  red = min;
				  } else if (inputbuffer[14]== 1) {
					  green= min;
					  blue = min;
					  red = max;
				  } else {
					  green=min;
					  blue = max;
					  red = min;
				  }
				  ML_setLight(red,green,blue);
			}
		}

		switch(prog_case) {
		case 0: ML_setLight(red,green,blue);
				break;
		case 1:
			  if (cnt == 0) {
			  	if ((red == max) && (blue == min) && (green < max)) {
			  			  green++;
			  		  }
			  		  else if ((green == max) && (blue == min) && (red > min)) {
			  			  red--;
			  		  }
			  		  else if ((green == max) && (red == min) && (blue < max)) {
			  			  blue++;
			  		  }
			  		  else if ((blue == max) && (red == min) && (green > min)) {
			  			  green--;
			  		  }
			  		  else if ((blue == max) && (green == min) && (red < max)) {
			  			  red++;
			  		  }
			  		  else if ((red == max) && (green == min) && (blue > min)) {
			  			  blue--;
			  		  }
			  		ML_setLight(red,green,blue);
			  		break;
			  	  }
		default: break;
		  	  }

		}

	 //COM_TX_PutData(data_to_send, data_length);

}

void GPIO_EVEN_IRQHandler(void) {
	if ((GPIO->IF & (1 << 10))) {	// check is IRQ flag is set
		GPIO->IFC = (1 << 10);
		GPIO->IEN = ~(1 << 10);
		if ( GPIO_PinInGet(gpioPortA, 10) == 0) {
			if (pressed == 0) {
				if (prog_case < 1) {
					prog_case++;
				}
				else {
					prog_case = 0;
				}
			}
			pressed = 1;
		}


	} else if ((GPIO->IF & (1 << 2))) {
		// data_from_Module
		GPIO->IFC = (1 << 2);
		GPIO->IEN = ~(1 << 2);
		if ( GPIO_PinInGet(gpioPortA, 2) == 0) {

		}
	}
}
void TIMER0_IRQHandler(void) {

	TIMER0->IFC = TIMER_IFC_OF;				// clear overflow interrupt flag
	if (pressed == 1) {
		press_cnt++;
		if (press_cnt == 80) {
			GPIO->IEN = (1 << 10);
			pressed = 0;
		    press_cnt = 0;
		}
	}

}
