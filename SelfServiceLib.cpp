#include "SelfServiceLib.h"

//ʱ���ʼ��
void initTimeSeconds()
{
	currentTime = 0;
}

//��ǰϵͳʱ������
void elapseTimeSeconds(int elapse)
{
	currentTime += elapse;
}

//��ȡ��ǰϵͳʱ�䣬����
int getCurrentTimeSeconds()
{
	return currentTime;
}

