/*
 * globals.c
 *
 *  Created on: 07.08.2017
 *      Author: marce
 */
#include "em_cmu.h"
#include "inc/globals.h"
#include "em_device.h"
#include "em_chip.h"


/** ***************************************************************************
 * @brief
 *****************************************************************************/
void INIT_XOclocks() {
	// High frequency clock
	CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
	CMU_OscillatorEnable(cmuOsc_USHFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_USHFRCO);
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_1);
	CMU_ClockEnable(cmuClock_HFPER, true);
}

/*void ltostr(int32_t l, char *string) {
	const char digit[] = "0123456789";	// digit characters
	if (0 > l) {			// negative integer
		l *= -1;			// change sign
		*string++ = '-';	// output a '-' and move one digit to the right
	}
	int tmp = l;
	do {					// calculate number of digits in output string
		tmp = tmp / 10;		// next digit
		string++;			// move to the end of the output string
	} while (0 < tmp);		// do while handles also the special case l == 0
	*string-- = '\0';		// output end of string char and move to last digit
	do {					// begin with rightmost digit in output string
		*string-- = digit[l % 10];	// last digit and move to next digit left
		l = l / 10;			// remaining integer portion without last digits
	} while (0 < l);		// do while handles also the special case l == 0
}
*/
