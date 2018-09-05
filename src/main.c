
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
	volatile int press_cnt = 0;
	volatile int lamp_on = 1;
	volatile int long_press = 0;
	volatile int long_press_cnt = 0;
	int data_to_send[31] = {0x8,0x4,0x6,0x1,0x2,0xD,0x4,0x1};
    const uint32_t data_length           = 8;

int main(void)
{
  /* Chip errata */
  CHIP_Init();


    INIT_XOclocks();
    CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(gpioPortA, 10, gpioModeInputPullFilter , 1);

	GPIO_IntConfig(gpioPortA, 10, true, true, true);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	timers_init(27000); // Initialize Timers for Setpoint CCs
	COM_Init(); // Initialize Communications
	GPIO->P[gpioPortC].DOUTSET = 1 << 3; // Handshake CTS Output High


    COM_TX_PutData(data_to_send, data_length);


  /* Infinite loop */
  while (1) {
	  if (lamp_on == 1) {

			switch(prog_case) {
				case 1: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 100);

					break;
				}
				case 2: {
					ML_setLight(0, 100);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 3: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 100);
					ML_setLight(3, 0);
					ML_setLight(4, 100);
					//
					break;
				}
				case 4: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 100);
					ML_setLight(4, 0);
					break;
				}
				case 5: {
					ML_setLight(0, 0);
					ML_setLight(1, 100);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 6: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 100);
					ML_setLight(3, 100);
					ML_setLight(4, 0);
					break;
				}
				case 7: {
					ML_setLight(0, 0);
					ML_setLight(1, 100);
					ML_setLight(2, 100);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 8: {
					ML_setLight(0, 0);
					ML_setLight(1, 100);
					ML_setLight(2, 0);
					ML_setLight(3, 100);
					ML_setLight(4, 0);
					break;
				}
				case 9: {
					ML_setLight(0, 30);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 10: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 100);
					break;
				}
				case 11: {
					ML_setLight(0, 40);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 12: {
					ML_setLight(0, 50);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 13: {
					ML_setLight(0, 70);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 14: {
					ML_setLight(0, 80);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 15: {
					ML_setLight(0, 90);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				case 16: {
					ML_setLight(0, 100);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
				default: {
					ML_setLight(0, 0);
					ML_setLight(1, 0);
					ML_setLight(2, 0);
					ML_setLight(3, 0);
					ML_setLight(4, 0);
					break;
				}
			}
	  } else {
			ML_setLight(0, 0);
			ML_setLight(1, 0);
			ML_setLight(2, 0);
			ML_setLight(3, 0);
			ML_setLight(4, 0);
			break;
		}

  }
}

void GPIO_EVEN_IRQHandler(void) {
	if ((GPIO->IF & (1 << 10))) {	// check is IRQ flag is set
		GPIO->IFC = (1 << 10);
		GPIO->IEN = ~(1 << 10);
		pressed = 1;
		if ( GPIO_PinInGet(gpioPortA, 10) == 0) {

		} else {
			if (long_press == 1) {
				if (lamp_on == 1) lamp_on = 0;
				else lamp_on = 1;
				long_press = 0;
				long_press_cnt = 0;
			}
		}


	} else if ((GPIO->IF & (1 << 2))) {
		// data_from_Module

		GPIO->IFC = (1 << 2);
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
	if (long_press == 1) {
		long_press_cnt++;
		if (press_cnt == 1500) {
			long_press = 0;
			long_press_cnt = 0;

			if (prog_case == 16) {
				prog_case = 1;
			} else {
				prog_case++;

		}
		}
	}
}
