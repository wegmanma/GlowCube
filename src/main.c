
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_core.h"
#include "em_usart.h"
#include "inc/timers.h"
#include "inc/globals.h"
#include "inc/communication.h"
#include "wifi_bglib.h"
#include "apitypes.h"

#define waittime 300


	volatile int prog_case = 1;		///< set with pushbutton 0 interrupt
	volatile int pressed = 0;
	volatile int press_cnt = 0;
	volatile int wifi_connect_cycle = 0;
	volatile int wifi_connect_count = 0;
	uint32_t counter = 0;
	uint32_t reconnect_counter = 0;


	int data_to_send[] = {0x8,0x4,0x6,0x1,0x2,0xD,0x4,0x1};
	int wifi_deactivate[] = {0x8,0x0,0x3,0x1};
	int wifi_activate[] = {0x8,0x0,0x3,0x0};
	int wifi_scan[] = {0x08,0x0a,0x03,0x14,0x09,0x7a,0x77,0x7a,0x2d,0x36,0x32,0x35,0x32,0x35};
	int wifi_passwd[] = {0x08,0x14,0x03,0x05,0x13,0x77,0x6e,0x78,0x72,0x2d,0x72,0x37,0x32,0x70,0x2d,0x75,0x76,0x79,0x7a,0x2d,0x74,0x69,0x39,0x74};
	int wifi_conn[] = {0x08,0x0a,0x03,0x07,0x09,0x7a,0x77,0x7a,0x2d,0x36,0x32,0x35,0x32,0x35};
	int wifi_endp[] = {0x08,0x03,0x04,0x02,0x40,0x9c,0x00};
	int data_get[] = {0x08,0x00,0x01,0x00};
	uint8_t inputbuffer[256] = {0};
	const uint32_t data_length 			= 8;
	const uint32_t acti_length           = 4;
    const uint32_t scan_length			= 14;
    const uint32_t pw_length			= 24;
    const uint32_t conn_length			= 14;
    const uint32_t endp_length			= 7;
	const uint32_t get_length           = 4;
	  uint32_t red, green, blue, max, cnt, min, cnt_slow ;



void ML_reconnect() {
	ML_setLight(0,0,0,0);
		while((wifi_connect_cycle == 0)&&(wifi_connect_count<10)) {// switch off wifi first for reset purposes
			wifi_connect_count++;
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_deactivate, acti_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x01)&& (inputbuffer[4]== 0x81)&& (inputbuffer[5]== 0x01))  { //already switched off
				wifi_connect_cycle = 1;
			} else if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x01)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00)) { // wait for switching off
				COM_RX_GetData(inputbuffer);
				if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x01)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00)) {
					wifi_connect_cycle = 1;
				}
			}
			  COM_Flush_Buffers();
		}
		//ML_setLight(45000,10000,0,0);
		if (wifi_connect_count <10) {
			wifi_connect_count = 0;
		}
		while((wifi_connect_cycle == 1)&&(wifi_connect_count<10)) { //activate WiFi
			wifi_connect_count++;
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_activate, acti_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x00)&& (inputbuffer[4]== 0x81)&& (inputbuffer[5]== 0x01))  { //already switched off
				wifi_connect_cycle = 2;
			} else if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x00)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00)) { // wait for switching off
				COM_RX_GetData(inputbuffer);
				if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x00)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00)) {
					wifi_connect_cycle = 2;
				}
			}
			  COM_Flush_Buffers();
		}
		if (wifi_connect_count <10) {
			wifi_connect_count = 0;
		}
		//ML_setLight(45000,30000,0,0);
		while((wifi_connect_cycle == 2)&&(wifi_connect_count<10)) { //scan for zwz...
			wifi_connect_count++;
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_scan, scan_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x02)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x14)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00))  { //already switched off
				wifi_connect_cycle = 3;
			}
			  COM_Flush_Buffers();
		}
		if (wifi_connect_count <10) {
			wifi_connect_count = 0;
		}
		//ML_setLight(45000,30000,0,0);
		while((wifi_connect_cycle == 3)&&(wifi_connect_count<10)) { //set PW
			wifi_connect_count++;
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_passwd, pw_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x01)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x05)&& (inputbuffer[4]== 0x00))  { // PW set
				wifi_connect_cycle = 4;
			}
			  COM_Flush_Buffers();
		}
		if (wifi_connect_count <10) {
			wifi_connect_count = 0;
		}
		//ML_setLight(45000,30000,0,0);
		while((wifi_connect_cycle == 4)&&(wifi_connect_count<10)) { //connect
			wifi_connect_count++;
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_conn, conn_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x09)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x07)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00))  { //direct answer
				COM_RX_GetData(inputbuffer);
				if ((inputbuffer[0]== 0x88)&& (inputbuffer[1]== 0x08)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x05)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00))  { // connected!
					wifi_connect_cycle = 5;
				}
				wifi_connect_cycle = 5;
			}
			  COM_Flush_Buffers();
		}
		ML_wait(waittime);
		ML_wait(waittime);
		ML_wait(waittime);
		if (wifi_connect_count <10) {
			wifi_connect_count = 0;
		}


		ML_wait(waittime);




}


int main(void)

{
  /* Chip errata */
  CHIP_Init();

  	  uint32_t i = 0;
    INIT_XOclocks();
    CMU_ClockEnable(cmuClock_GPIO, true);
	timers_init(55000); // Initialize Timers for Setpoint CCs
	GPIO_PinModeSet(gpioPortA, 10, gpioModeInputPullFilter , 1);

	GPIO_IntConfig(gpioPortA, 10, true, true, true);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	COM_Init(); // Initialize Communications
	COM_RX_GetData(inputbuffer);

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
	  ML_setLight(min,min,min,0); // dark right from start
	  COM_Flush_Buffers();
	  ML_setLight(45000,0,0,0); // red if ready
	  COM_RX_GetData(inputbuffer);
	  if ((inputbuffer[0]== 0x88)&& (inputbuffer[2]== 0x01)&& (inputbuffer[3]== 0x00)) {
	  }
	  ML_reconnect();
	  wifi_connect_count = 0;
	  if (wifi_connect_cycle == 5) {
			ML_setLight(30000,30000,0,0);
	  }
	  ML_wait(waittime);
	  ML_wait(waittime);
		while((wifi_connect_cycle == 5)&&(wifi_connect_count<10)) { //connect
			wifi_connect_count++;
			ML_wait(waittime);
			ML_wait(waittime);
			COM_Flush_Buffers();
			COM_TX_PutData(wifi_endp, endp_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x03)&& (inputbuffer[2]== 0x04)&& (inputbuffer[3]== 0x02)&& (inputbuffer[4]== 0x00)&& (inputbuffer[5]== 0x00))  { //direct answer
				wifi_connect_cycle = 6;
			}
			  COM_Flush_Buffers();
		}



				if (wifi_connect_cycle == 6) {
					ML_setLight(0,30000,0,0);
				}
				else {
					ML_setLight(45000,0,0,0);
				}
				ML_wait(waittime);
				ML_setLight(0,0,0,0);
				COM_Flush_Buffers();
  while (1) {
	  if (COM_RX_Available()) {
		  ML_setLight(45000,0,0,0);
		  COM_RX_GetData(inputbuffer);
		  COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x88)&& (inputbuffer[2]== 0x03)&& (inputbuffer[3]== 0x06)&& (inputbuffer[4]== 0x96)&& (inputbuffer[5]== 0x01)&& (inputbuffer[6]== 0x00))  { //direct answer
				ML_reconnect();
			} else if ((inputbuffer[0]== 0x88)&& (inputbuffer[1]== 0x06)&& (inputbuffer[2]== 0x05)&& (inputbuffer[3]== 0x01)) {
				ML_setLight(inputbuffer[6]*100,inputbuffer[7]*100,inputbuffer[8]*100,inputbuffer[9]*100);
			}
			for (i=0;i<256;i++) {
				inputbuffer[i]=0;
			}

	  }
	  if (reconnect_counter >= 10000000) {
		  reconnect_counter = 0;
			COM_Flush_Buffers();
			ML_wait(waittime);
			COM_TX_PutData(data_get, get_length);
			COM_RX_GetData(inputbuffer);
			COM_Flush_Buffers();
			if ((inputbuffer[0]== 0x08)&& (inputbuffer[1]== 0x00)&& (inputbuffer[14]== 0x01)) {
				ML_reconnect();
			}
	  }

  }


}





void GPIO_EVEN_IRQHandler(void) {
	if ((GPIO->IF & (1 << 10))) {	// check is IRQ flag is set
		GPIO->IFC = (1 << 10);
		if ( GPIO_PinInGet(gpioPortA, 10) == 0) {
			COM_TX_PutData(data_get, get_length);
		}
	}
}
void TIMER0_IRQHandler(void) {

	TIMER0->IFC = TIMER_IFC_OF;				// clear overflow interrupt flag
	counter++;
	reconnect_counter++;
	if (pressed == 1) {
		press_cnt++;
		if (press_cnt == 80) {
			GPIO->IEN = (1 << 10);
			pressed = 0;
		    press_cnt = 0;
		}
	}

}
