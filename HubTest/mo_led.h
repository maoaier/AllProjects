/** @file
 *  @brief �豸��״̬
 *  @author �����
 *  @date 2017-04-25
 *  @version 0.1
 */
#ifndef __MO_LED_H__
#define __MO_LED_H__


class Mo_Led
{
public:
	Mo_Led();
	~Mo_Led();
	//������ȷ��LED��
	void firstLed();
	void twoLed();
	void threeLed();


	void setCorrectLed();
	//���ô����LED��
	void setErrorLed();

	void setEndLight();

	void setOpenAll();

	void setAllBlack();
protected:
	void SetRunningLed(bool on);
	void SetCommLed(bool on);
	void SetAlarmLed(bool on);

protected:
	int m_runState;
};
#endif
