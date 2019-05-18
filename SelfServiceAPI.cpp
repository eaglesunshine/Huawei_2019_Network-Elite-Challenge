#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "SelfServiceAPI.h"
#include "SelfServiceLib.h"

/**
 * 用分隔符解析字符串
 * 说明：
 * 提供了带分隔符字符串解析能力，没有考虑一些异常，考生可以根据实际需要进行修改完善
 *
 * @param src
 *     待解析的字符串
 *
 * @param sep
 *     分隔符
 *
 * @param destArray
 *     解析后的数据
 *
 * @param filtEmpty
 *     如果数据为空不存储，
 *     例如src为!4!9
 *     filtEmpty为true是结果为{"4","9"}
 *     filtEmpty为false时结果为{"","4","9"}
 */
void splitToArray(const std::string &src, const std::string &sep, std::vector<std::string> &destArray, bool filtEmpty = false)
{
	if (src.empty())
	{
		return;
	}

	std::string::size_type posBegin = 0;
	std::string::size_type posEnd   = src.find(sep);
	std::string value;
	while (posEnd != std::string::npos)
	{
		value = src.substr(posBegin, posEnd - posBegin);
		if (!(filtEmpty && value.empty()))
		{
			destArray.push_back(value);
		}
		posBegin = posEnd + 1;
		posEnd = src.find(sep, posBegin);
	}

	value = src.substr(posBegin, src.size() - posBegin);
	if (!(filtEmpty && value.empty()))
	{
		destArray.push_back(value);
	}
}

/**
 * 号码解析：把文件中读取到的一条号码记录解析成一个结构体
 * 说明：
 *    提供了一个号码记录的基本解析能力，没有考虑一些异常，考生可以根据实际需要进行修改完善
 *
 * @param record
 *    号码文件存储数据
 *
 * @param number
 *    解析后的号码结构
 *
 * @return 返回值
 *    E001：解析成功
 *    E004：号码记录格式错误
 */
RET_ERRORCODE parseNumber(const char* record, Number& number)
{
	if ((NULL == record) || (strlen(record) == 0))
	{
		return E001;
	}

	std::string numberRecord(record);
	std::vector<std::string> recordArray;

	std::string::size_type posBegin = 0;
	std::string::size_type posEnd   = numberRecord.find("\r\n");

	//去掉回车换行
	if (posEnd != std::string::npos)
	{
		numberRecord = numberRecord.substr(0, posEnd);
	}
	else
	{
		//去掉换号
		posEnd = numberRecord.find("\n");
		if (posEnd != std::string::npos)
		{
			numberRecord = numberRecord.substr(0, posEnd);
		}
	}

	//获取|分隔符分割的数据
	splitToArray(numberRecord, "|", recordArray);
    
	//合法数据只有6个字段
	if (recordArray.size() != 6)
	{
		return E004;
	}
	
	strcpy_s(number.id, recordArray[0].c_str());
	number.price = (float)atof(recordArray[1].c_str());
	strcpy_s(number.brand, recordArray[2].c_str());
	number.status = atoi(recordArray[3].c_str());
	strcpy_s(number.customer, recordArray[4].c_str());
	number.time   = atoi(recordArray[5].c_str());
	
	return E001;
}

/**
 * 初始化：使用初始化指令让系统重新完成初始化，各条记录（如号码记录）清空，时间状态初始化等。
 * @return 初始化结果
 *    E000:方法未实现
 *    E001:初始化成功
 */
RET_ERRORCODE initSys()
{
	Record.clear();

	initTimeSeconds();

	return E001;
	//return E000;
}

/**
 * 加载号码：读取numbers.txt格式的数据文件，加载所有号码信息到系统中。
 * 注意：
 *    1、需要考虑文件不存在的异常场景；
 *    2、需要考虑文件格式不正确的异常场景；
 *    3、需要考虑文件中号码重复，后加载的号码信息，覆盖先加载的号码信息。
 *
 * @param fileName
 *    配置文件名（路径:支持绝对路径、相对路径）
 *
 * @return 加载结果
 *    E000:方法未实现
 *    E001:成功
 *    E002:入参输入异常
 *    E003:文件不存在
 *    E004:文件格式不正确
 */
RET_ERRORCODE loadNumbers(const char* fileName)
{
	std::string filename=fileName;
	std::string temp1 = ".txt";
	char temp[5];
	int count = 0;
	for (int i = filename.length() - 4; i < filename.length(); i++)
	{
		temp[count] = (filename[i]);
		++count;
	}
	for (int i = 0; i < 4; i++)
		if (temp[i] != temp1[i])
			return E002;

	FILE *fp = NULL;
	fp=fopen(fileName, "r");
	if (fp==NULL)
		return E003;

	char buff[2550];

	while (!feof(fp))
	{
		fgets(buff, 255, (FILE*)fp);
		Number NUM;
		RET_ERRORCODE TT;
		TT=parseNumber(buff, NUM);
		if (TT == E004)
			return E004;
		std::string temp3 = NUM.id;
		Record.insert(make_pair(temp3, NUM));
	}
	return E001;


	//return E000;
}

/**
 * 查询：号码信息查询。
 * 要求：支持按号码查询、价格查询、品牌查询、状态查询、时间查询
 *
 * @param number
 *    号码信息：支持条件组合
 *    1、id为空表示匹配所有号码，否则匹配指定号码
 *    2、price为0表示匹配所有价格，否则匹配指定价格号码
 *    3、date为0表示匹配所有时间，否则匹配指定时间号码
 *    4、brand为空表示匹配所有品牌，否则匹配指定品牌号码或不限品牌号码
 *
 * @param numbers
 *    号码列表，号码从小到大排序
 *
 * @param count
 *    号码个数，输入为 number数组大小，输出为选取到的号码个数，数组容量不够时返回失败
 *
 * @return 查询结果
 *    E000:方法未实现
 *    E001:成功
 */
RET_ERRORCODE queryNumber(const Number& number, Number numbers[], int& count)
{
	std::string ID = number.id;
	if (Record.count(ID) == 1)
	{
		numbers[0] = Record[ID];
		count = 1;
		return E001;
	}
	if (Record.count(ID) == 0)
	{
		count = 0;
		return E001;
	}


	return E000;
}

/**
 * 随机选号：根据品牌，随机选取未被预定的号码，供客户选择
 *
 * @param brand
 *    品牌名称（不限品牌的号码，可以用于任意品牌）
 *    该参数不允许为空
 *
 * @param randomNum
 *    随机获取的号码个数
 *
 * @param number
 *    号码列表，号码从小到大排序
 *
 * @param count
 *    号码个数，输入为 number数组大小，输出为选取到的号码个数，数组容量不够时返回失败
 *
 * @return 选号结果
 *    E000:方法未实现
 *    E001:成功
 *    E008:其他错误
 */
RET_ERRORCODE randomPick(const char* brand, const int randNumber, Number number[], int& count)
{
	std::vector<Number> all;
	
	for (auto it = Record.begin(); it != Record.end(); it++)
	{
		if (it->second.brand[0] == *brand)
			all.push_back(it->second);
	}

	if ((int)all.size() <= randNumber)
	{
		count = (int)all.size();
		for (int i = 0; i < (int)all.size(); i++)
		{
			number[i] = all[i];
		}
		return E001;
	}

	if ((int)all.size() > randNumber)
	{
		count = randNumber;
		for (int i = 0; i < randNumber; i++)
		{
			number[i] = all[i];
		}
		return E001;
	}


	return E000;
}

/**
 * 自助选号：根据选号表达式，返回满足条件的未被预定的号码列表
 *
 * @param brand
 *   品牌名称（不限品牌的号码，可以用于任意品牌）
 *   该参数不允许为空
 *
 * @param expression1
 *   选号表达式1 
 *   支持固定号码选号，如：13900000168
 *   支持*通配符选号，如：139****1314
 *   参数为空表示对号码格式不限制
 *
 * @param expression2
 *   选号表达式2 
 *   支持尾号不为某一个数字的选号，如： !4即过滤尾号为4的号码； !3!4即过滤尾号为3和4的号码，!号最多9个
 *   参数值为空表示对尾号无限制。
 *   支持brand、expression1、expression2同时有参数的情况，如selfServicePick("2", "**7********", "!3!4")
 *
 * @param number
 *   号码列表，号码从小到大排序，空表示没有匹配的条件的号码
 *
 * @param count
 *   号码个数，输入为 number数组大小，输出为选取到的号码个数，数组容量不够时返回失败
 *
 * @return 选号结果
 *   E000:方法未实现
 *   E001:成功
 *   E008:其他错误
 */
RET_ERRORCODE selfServicePick(const char* brand, const char* expression1, const char* expression2, Number number[], int& count)
{
	
	return E000;
}

/**
 * 锁定：根据号码信息，对号码资源进行锁定（预定）。
 * 要求：
 * 1、一个客户最多锁定2个号码；
 * 2、只能对号码状态为0（未锁定）的号码进行锁定，并且修改状态为1（已锁定）；
 * 3、锁定号码到期时间=当前锁定时间+锁定保留时间计算（1天），时间为秒数。
 *
 * @param number
 *      号码
 * @return 锁定结果
 *      E000:方法未实现
 *      E001:成功
 *      E005:号码不能重复锁定
 *      E006:客户锁定超过两个号码
 *      E007:锁定号码在系统中不存在
 */
RET_ERRORCODE lockNumber(const Number& number)
{
	std::string ID = number.id;
	if (Record.count(ID) == 0)
		return E007;

	if (Record.count(ID) == 1)
	{
		if (Record[ID].status == 1)
			return E005;
		if (Record[ID].status == 0)
		{
			Record[ID].status = 1;
			Record[ID].time = getCurrentTimeSeconds() + ONE_DAY_SECONDS;
			for (int i = 0; i < 12; i++)
				Record[ID].customer[i] = number.id[i];
			return E001;
		}
	}

	return E000;
}

/**
 * 解锁：根据时间对预定号码到期时间未处理的号码资源进行解锁，供其他客户选择。
 * 要求：
 * 1、当前时间减号码到期时间大于1天的则可判断为需要解锁的号码
 * 2、修改号码状态为0（未锁定）；
 * 3、修改锁定号码到期时间为0。
 *
 * @return 解锁结果
 *      E000:方法未实现
 *      E001:成功
 */
RET_ERRORCODE unLockNumber()
{
	for (auto it = Record.begin(); it != Record.end(); it++)
	{
		if (it->second.time < getCurrentTimeSeconds())
		{
			it->second.status = 0;
			it->second.time = 0;
		}
	}

	return E001;
	//return E000;
}


/*****************************************************************************
 函 数 名  : selfServiceTerminal_TestCase
 功能描述  : 提供给（选择函数实现方式的）考生的测试用例样例。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
 重要说明  : 选择函数实现方式的考生可以在本接口中调整或者增加测试用例。
			 选择命令方式的考生不需要关注本接口，也不要改动本接口。
*****************************************************************************/
void selfServiceTerminal_TestCase()
{
	//1 系统初始化
	RET_ERRORCODE ret = initSys();
	assert(ret ==  E001);

    //2 正常号码加载
	ret = loadNumbers("../output/template/numbers.txt");
	assert(ret == E001);

	//3 文件名校验
	ret = loadNumbers("");
	assert(ret == E002);

	//4 号码格式校验
	ret = loadNumbers("../output/template/numbers_err.txt");
	assert(ret == E004);

	//5 号码查询
	Number inNumber;
	strcpy_s(inNumber.id, "13900005899");
	Number number[10];
	int numberCount = 10;
	ret = queryNumber(inNumber, number, numberCount);
	assert(ret == E001);
	assert(numberCount == 1);

	//6 随机选号
	int randNumber = 3;
	Number selectNumber[10];
	int selectNumberCount = 10;
	ret = randomPick("2", randNumber, selectNumber, selectNumberCount);
	assert(ret == E001);
	assert(selectNumberCount == 3);

	//7 自助选号规则
	Number selfNumber[10];
	int selfNumberCount = 10;
	ret = selfServicePick("2", "", "", selfNumber, selfNumberCount);
	assert(ret == E001);
	assert(selfNumberCount == 9);
		
	//8 自助选号规则
	ret = selfServicePick("2", "**7********", "!3!4", selfNumber, selfNumberCount);
	assert(ret == E001);
	assert(selfNumberCount == 2);

	//9 锁定号码
	ret = lockNumber(inNumber);
	assert(ret == E001);

	numberCount = 10;
	inNumber.status = 1;
	ret = queryNumber(inNumber, number, numberCount);
	assert(ret == E001);
	assert(numberCount == 1);
	assert(number[0].status == 1);
	assert(number[0].time == ONE_DAY_SECONDS);

	//10 解锁预约号码
	ret = initSys();
	assert(ret == E001);

	ret = loadNumbers("../output/template/numbers_unlock.txt");
	assert(ret == E001);

	//系统时间调整
	elapseTimeSeconds(86400);
	
	ret = unLockNumber();
	assert(ret == E001);

	inNumber.id[0]    = 0;
	inNumber.status   = 0;
	inNumber.brand[0] = 0;
	numberCount       = 10;
	
	ret = queryNumber(inNumber, number, numberCount);
	assert(ret == E001);
	assert(numberCount == 1);
	for (int i = 0; i < numberCount; ++i)
	{
		assert(number[i].status == 0);
		assert(number[i].time   == 0);
	}
}

