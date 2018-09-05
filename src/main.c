
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_core.h"
#include "inc/timers.h"
#include "inc/globals.h"
#include "inc/communication.h"

	volatile int prog_case = 0;		///< set with pushbutton 0 interrupt
	volatile int pressed = 0;
	volatile int press_cnt = 0;

int main(void)
{
  /* Chip errata */
  CHIP_Init();


    INIT_XOclocks();

	GPIO_PinModeSet(gpioPortA, 10, gpioModeInputPullFilter , 1);

	GPIO_IntConfig(gpioPortA, 10, false, true, true);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	timers_init(100); // Initialize Timers for Setpoint CCs
	COM_Init(); // Initialize Communications





    GPIO->P[gpioPortC].DOUTCLR = 1 << 3; // Handshake CTS Output Low
    uint8_t data_to_send[8] = {0x8,0x4,0x6,0x1,0x2,0xD,0x4,0x1};
    const uint32_t data_length           = sizeof(data_to_send);


  /* Infinite loop */
  while (1) {
	  switch(prog_case) {
	  case 1: { // All
		  ML_setLight(0, 40);
		  ML_setLight(1, 40);
		  ML_setLight(2, 40);
		  ML_setLight(3, 40);
		  ML_setLight(4, 40);
		  break;
	  }
	  case 2: { // White
		  ML_setLight(0, 100);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 3: { // Red + Ball
		  ML_setLight(0, 0);
		  ML_setLight(1, 0);
		  ML_setLight(2, 100);
		  ML_setLight(3, 0);
		  ML_setLight(4, 100);
		  break;
	  }
	  case 4: { // Green
		  ML_setLight(0, 0);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 100);
		  ML_setLight(4, 0);
		  // COM_TX_PutData((uint8_t*) data_to_send, data_length);
		  break;
	  }
	  case 5: { // Blue
		  ML_setLight(0, 0);
		  ML_setLight(1, 100);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 6: { // Yellow
		  ML_setLight(0, 0);
		  ML_setLight(1, 0);
		  ML_setLight(2, 100);
		  ML_setLight(3, 100);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 7: { // Magenta
		  ML_setLight(0, 0);
		  ML_setLight(1, 100);
		  ML_setLight(2, 100);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 8: { // Aqua
		  ML_setLight(0, 0);
		  ML_setLight(1, 100);
		  ML_setLight(2, 0);
		  ML_setLight(3, 100);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 9: { // Dim White
		  ML_setLight(0, 30);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 10: { // Ball Only
		  ML_setLight(0, 0);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 100);
		  break;
	  }
	  case 11: { // Dim White 40%
		  ML_setLight(0, 40);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 12: { // Dim White 50%
		  ML_setLight(0, 50);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 13: { // Dim White 70%
		  ML_setLight(0, 70);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 14: { // Dim White 80%
		  ML_setLight(0, 80);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 15: {// Dim White 90%
		  ML_setLight(0, 90);
		  ML_setLight(1, 0);
		  ML_setLight(2, 0);
		  ML_setLight(3, 0);
		  ML_setLight(4, 0);
		  break;
	  }
	  case 16: { // White 1000%
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

  }
}

void GPIO_EVEN_IRQHandler(void) {
	if ((GPIO->IF & (1 << 10))) {	// check is IRQ flag is set

		pressed = 1;
		GPIO->IFC = (1 << 10);

	} else if ((GPIO->IF & (1 << 2))) {
		// data_from_Module
	}
}
void TIMER0_IRQHandler(void) {

	TIMER0->IFC = TIMER_IFC_OF;				// clear overflow interrupt flag
	if (pressed == 1) {
		press_cnt++;
		if (press_cnt == 1000) {
			if ( GPIO_PinInGet(gpioPortA, 10) == 0) {
				if (prog_case == 16) {
					prog_case = 0;
				} else {
					prog_case++;
				}
			}
			pressed = 0;
		    press_cnt = 0;
		}
	}
}
