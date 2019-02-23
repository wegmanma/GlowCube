/*
 * timers.h
 *
 *  Created on: 07.08.2017
 *      Author: marce
 */

#ifndef SRC_TIMERS_H_
#define SRC_TIMERS_H_


void timers_init(uint32_t value_top);

void ML_setLight(uint32_t red, uint32_t green, uint32_t blue, uint32_t white);
void ML_wait(uint32_t duration);

#endif /* SRC_TIMERS_H_ */
