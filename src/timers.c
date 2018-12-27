#include "em_gpio.h"
#include "em_timer.h"
#include "em_cmu.h"

void timers_init(uint32_t value_top) {
	GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0); //all back to wiredAnd
	GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 9, gpioModePushPull, 0);
	CMU_ClockEnable(cmuClock_TIMER0, true);
	// CMU_ClockEnable(cmuClock_TIMER1, true);
	CMU_ClockEnable(cmuClock_TIMER2, true);
	/* load default values for general TIMER configuration */




	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	TIMER_TopSet(TIMER0, value_top);
	TIMER_TopSet(TIMER2, value_top);
	/* load default values for compare/capture channel configuration */
	TIMER_InitCC_TypeDef timerInitCC = TIMER_INITCC_DEFAULT;

	timerInitCC.mode = timerCCModePWM;            // configure as PWM channel

	TIMER_InitCC(TIMER0, 2, &timerInitCC);  // CC channel 2 is use
	TIMER_InitCC(TIMER0, 1, &timerInitCC);  // CC channel 1 is used
	TIMER_InitCC(TIMER0, 0, &timerInitCC);  // CC channel 0 is used
	TIMER_InitCC(TIMER2, 1, &timerInitCC);  // CC channel 1 is used
	TIMER_InitCC(TIMER2, 0, &timerInitCC);  // CC channel 0 is used
	TIMER_CompareSet(TIMER0, 2, 20);        // CC value defines PWM active time - Start Pulse
	TIMER_CompareSet(TIMER0, 1, 20);  // CC value defines PWM active time - PWM1
	TIMER_CompareSet(TIMER0, 0, 20);  // CC value defines PWM active time - PWM2
	TIMER_CompareSet(TIMER2, 1, 20);  // CC value defines PWM active time - PWM1
	TIMER_CompareSet(TIMER2, 0, 20);  // CC value defines PWM active time - PWM2
	 /* route output to location #3 and enable output CC0 */

    TIMER0 ->ROUTE |= (TIMER_ROUTE_LOCATION_LOC0 | TIMER_ROUTE_CC2PEN);
    TIMER0 ->ROUTE |= (TIMER_ROUTE_LOCATION_LOC0 | TIMER_ROUTE_CC1PEN);
    TIMER0 ->ROUTE |= (TIMER_ROUTE_LOCATION_LOC0 | TIMER_ROUTE_CC0PEN);

    TIMER2 ->ROUTE |= (TIMER_ROUTE_LOCATION_LOC0 | TIMER_ROUTE_CC1PEN);
    TIMER2 ->ROUTE |= (TIMER_ROUTE_LOCATION_LOC0 | TIMER_ROUTE_CC0PEN);

	TIMER_Init(TIMER0, &timerInit);                           // init and start the timer
	TIMER_Init(TIMER2, &timerInit);

    NVIC_ClearPendingIRQ(TIMER0_IRQn);               // clear pending timer interrupts
    TIMER_IntEnable(TIMER0, TIMER_IF_OF);          // enable timer overflow interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);
}
void ML_setLight(uint32_t red, uint32_t green, uint32_t blue){

	if ((red==0)||(blue==0)||(green==0)) {
		TIMER_CompareSet(TIMER0, 0, 0);
	} else {
		TIMER_CompareSet(TIMER0, 0, 0);// TIMER_CompareSet(TIMER0, 1, (red+green+blue+blue)>>2);
	}

	TIMER_CompareSet(TIMER2, 0, green); // green led
	TIMER_CompareSet(TIMER0, 1, blue); // blue led
	TIMER_CompareSet(TIMER2, 1, red); // red ball
	TIMER_CompareSet(TIMER0, 2, red); //red led
}



