/*
 * RTC_internal.h
 *
 * Created: 26-02-2013 19:56:49
 *  Author: Jonas
 */ 


#ifndef RTC_INTERNAL_H_
#define RTC_INTERNAL_H_

unsigned int rtc_milliseconds;
unsigned char rtc_seconds;
unsigned char rtc_minutes;
unsigned char rtc_hours;
unsigned char rtc_days;

extern void rtc_init_internal(void);
void update_RTC (void);


#endif /* RTC_INTERNAL_H_ */