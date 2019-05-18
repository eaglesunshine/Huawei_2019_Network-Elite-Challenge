#ifndef _SelfServiceTerminal_API_H_
#define _SelfServiceTerminal_API_H_

#include <map>
#include <string>

//返回错误码
enum RET_ERRORCODE
{
	E000 = 1, //方法未实现        
	E001,     //处理成功            
	E002,     //入参输入异常          
	E003,     //文件不存在      
	E004,     //文件格式不正确          
	E005,     //号码不能重复锁定            
	E006,     //客户锁定超过2个号码          
	E007,     //锁定号码在系统中不存在              
	E008      //其他错误
};

//Number信息
struct Number
{
	Number()
	{
		id[0]       = 0;
		price       = 0;
		brand[0]    = 0;
		status      = 0;
		customer[0] = 0;
		time        = 0;
	}
	Number & operator = (const Number&r)
	{
		strcpy_s(id, r.id);
		strcpy_s(brand, r.brand);
		strcpy_s(customer, r.customer);
		time   = r.time;
		status = r.status;
		price  = r.price;
		return *this;
	}

	//手机号码：11位长度的阿拉伯数字
	char id[12];

	//预付金额 
	float price;

	//品牌 "0"：无品牌限制 
	char brand[10];

	//状态 0：未锁定 1：锁定 
    int status = 0;

	// 锁定号码的客户客户ID 
	char customer[256];

	//锁定号码到期时间：为锁定当前时间 + 1天时间(秒数) 
	int time;
};

//一天的秒数
const int ONE_DAY_SECONDS  = 24 * 3600;

/******************/
//考生可在此自定义数据结构
std::map<std::string, Number> Record;

/******************/


/**
 * 初始化：使用初始化指令让系统重新完成初始化，各条记录（如号码记录）清空，时间状态初始化等。
 * @return 初始化结果
 *    E000:方法未实现
 *    E001:初始化成功
 */
RET_ERRORCODE initSys();


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
RET_ERRORCODE loadNumbers(const char* fileName);


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
RET_ERRORCODE queryNumber(const Number& number, Number numbers[], int& count);

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
RET_ERRORCODE randomPick(const char* brand, const int randNumber, Number number[], int& count);


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
RET_ERRORCODE selfServicePick(const char* brand, const char* expression1, const char* expression2, Number number[], int& count);


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
RET_ERRORCODE lockNumber(const Number& number);


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
RET_ERRORCODE unLockNumber();


/*****************************************************************************
 函 数 名  : selfServiceTerminal_TestCase
 功能描述  : 提供给（选择函数实现方式的）考生的测试用例样例。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
 重要说明  : 选择函数实现方式的考生可以在本接口中调整或者增加测试用例。
			 选择命令方式的考生不需要关注本接口，也不要改动本接口。
*****************************************************************************/
void selfServiceTerminal_TestCase();


#endif
 