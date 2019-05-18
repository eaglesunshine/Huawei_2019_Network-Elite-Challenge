#include "SelfServiceLib.h"

//时间初始化
void initTimeSeconds()
{
	currentTime = 0;
}

//当前系统时间流逝
void elapseTimeSeconds(int elapse)
{
	currentTime += elapse;
}

//获取当前系统时间，秒数
int getCurrentTimeSeconds()
{
	return currentTime;
}

