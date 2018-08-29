#include "mo_led.h"
#include <stdio.h>

using namespace std;

#define MODE_HEARTBEAT		1 //!< heartbeat operation
#define MODE_GPIO			2 //!< gpio operation
#define MODE_TIMER			3 //!< timer operation
#define MODE_NONE			4 //!< none operation
#define BLINKY_TIME			500 /*500ms*/
#define RUNNING_LED_NAME    "led-run"
#define WARNING_LED_NAME    "led-warning"
#define REMOTE_LED_NAME     "led-remote"

#define GPIO_4G_STA     	89   /*PC25*/
#define GPIO_MODE_OUTPUT   	1

#define LED_RUN_NORMAL  	0
#define LED_RUN_BLUETH  	1

#ifdef __cplusplus
extern "C"
{
	// libled.aº¯Êý
	int gpio_set_mode(char* dev_name, int mode);
	int gpio_set_timer_delay(char* dev_name, int delay_on, int delay_off);
	int gpio_set_onoff(char* dev_name, int on_off);

	int gpio_register(size_t gpio, int mode);
	int gpio_unregister(size_t gpio);
	int gpio_set_value(size_t gpio, int value);
	int gpio_get_value(size_t gpio);
}
#endif

Mo_Led::Mo_Led()
{
	gpio_register(GPIO_4G_STA, GPIO_MODE_OUTPUT);
//	SetRunningLed(true);
//	SetAlarmLed(true);
//	SetCommLed(true);
	m_runState = LED_RUN_NORMAL;
}

Mo_Led::~Mo_Led()
{
	gpio_unregister(GPIO_4G_STA);
}

void Mo_Led::setEndLight()
{
	SetRunningLed(true);
	SetAlarmLed(false);
	SetCommLed(false);
}

void Mo_Led::setOpenAll()
{
	SetRunningLed(true);
	SetAlarmLed(true);
	SetCommLed(true);
}


void Mo_Led::setCorrectLed()
{
	SetRunningLed(true);
	SetAlarmLed(false);
	SetCommLed(true);
}

void Mo_Led::setErrorLed()
{
	SetRunningLed(false);
	SetAlarmLed(true);
	SetCommLed(false);
}

void Mo_Led::setAllBlack()
{
	SetRunningLed(false);
	SetAlarmLed(false);
	SetCommLed(false);
}

void Mo_Led::SetRunningLed(bool on)
{
	char ledName[50] = { 0 };
	sprintf(ledName, "%s", RUNNING_LED_NAME);
	gpio_set_mode(ledName, MODE_GPIO);
	gpio_set_onoff(ledName, on ? 1 : 0);
}

void Mo_Led::SetAlarmLed(bool on)
{
	char ledName[50] = { 0 };
	sprintf(ledName, "%s", WARNING_LED_NAME);
	gpio_set_mode(ledName, MODE_GPIO);
	gpio_set_onoff(ledName, on ? 1 : 0);
}

void Mo_Led::SetCommLed(bool on)
{
	gpio_set_value(GPIO_4G_STA, on ? 0 : 1);
}

