#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "SelfServiceAPI.h"
#include "SelfServiceLib.h"

/**
 * �÷ָ��������ַ���
 * ˵����
 * �ṩ�˴��ָ����ַ�������������û�п���һЩ�쳣���������Ը���ʵ����Ҫ�����޸�����
 *
 * @param src
 *     ���������ַ���
 *
 * @param sep
 *     �ָ���
 *
 * @param destArray
 *     �����������
 *
 * @param filtEmpty
 *     �������Ϊ�ղ��洢��
 *     ����srcΪ!4!9
 *     filtEmptyΪtrue�ǽ��Ϊ{"4","9"}
 *     filtEmptyΪfalseʱ���Ϊ{"","4","9"}
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
 * ������������ļ��ж�ȡ����һ�������¼������һ���ṹ��
 * ˵����
 *    �ṩ��һ�������¼�Ļ�������������û�п���һЩ�쳣���������Ը���ʵ����Ҫ�����޸�����
 *
 * @param record
 *    �����ļ��洢����
 *
 * @param number
 *    ������ĺ���ṹ
 *
 * @return ����ֵ
 *    E001�������ɹ�
 *    E004�������¼��ʽ����
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

	//ȥ���س�����
	if (posEnd != std::string::npos)
	{
		numberRecord = numberRecord.substr(0, posEnd);
	}
	else
	{
		//ȥ������
		posEnd = numberRecord.find("\n");
		if (posEnd != std::string::npos)
		{
			numberRecord = numberRecord.substr(0, posEnd);
		}
	}

	//��ȡ|�ָ����ָ������
	splitToArray(numberRecord, "|", recordArray);
    
	//�Ϸ�����ֻ��6���ֶ�
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
 * ��ʼ����ʹ�ó�ʼ��ָ����ϵͳ������ɳ�ʼ����������¼��������¼����գ�ʱ��״̬��ʼ���ȡ�
 * @return ��ʼ�����
 *    E000:����δʵ��
 *    E001:��ʼ���ɹ�
 */
RET_ERRORCODE initSys()
{
	Record.clear();

	initTimeSeconds();

	return E001;
	//return E000;
}

/**
 * ���غ��룺��ȡnumbers.txt��ʽ�������ļ����������к�����Ϣ��ϵͳ�С�
 * ע�⣺
 *    1����Ҫ�����ļ������ڵ��쳣������
 *    2����Ҫ�����ļ���ʽ����ȷ���쳣������
 *    3����Ҫ�����ļ��к����ظ�������صĺ�����Ϣ�������ȼ��صĺ�����Ϣ��
 *
 * @param fileName
 *    �����ļ�����·��:֧�־���·�������·����
 *
 * @return ���ؽ��
 *    E000:����δʵ��
 *    E001:�ɹ�
 *    E002:��������쳣
 *    E003:�ļ�������
 *    E004:�ļ���ʽ����ȷ
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
 * ��ѯ��������Ϣ��ѯ��
 * Ҫ��֧�ְ������ѯ���۸��ѯ��Ʒ�Ʋ�ѯ��״̬��ѯ��ʱ���ѯ
 *
 * @param number
 *    ������Ϣ��֧���������
 *    1��idΪ�ձ�ʾƥ�����к��룬����ƥ��ָ������
 *    2��priceΪ0��ʾƥ�����м۸񣬷���ƥ��ָ���۸����
 *    3��dateΪ0��ʾƥ������ʱ�䣬����ƥ��ָ��ʱ�����
 *    4��brandΪ�ձ�ʾƥ������Ʒ�ƣ�����ƥ��ָ��Ʒ�ƺ������Ʒ�ƺ���
 *
 * @param numbers
 *    �����б������С��������
 *
 * @param count
 *    �������������Ϊ number�����С�����Ϊѡȡ���ĺ��������������������ʱ����ʧ��
 *
 * @return ��ѯ���
 *    E000:����δʵ��
 *    E001:�ɹ�
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
 * ���ѡ�ţ�����Ʒ�ƣ����ѡȡδ��Ԥ���ĺ��룬���ͻ�ѡ��
 *
 * @param brand
 *    Ʒ�����ƣ�����Ʒ�Ƶĺ��룬������������Ʒ�ƣ�
 *    �ò���������Ϊ��
 *
 * @param randomNum
 *    �����ȡ�ĺ������
 *
 * @param number
 *    �����б������С��������
 *
 * @param count
 *    �������������Ϊ number�����С�����Ϊѡȡ���ĺ��������������������ʱ����ʧ��
 *
 * @return ѡ�Ž��
 *    E000:����δʵ��
 *    E001:�ɹ�
 *    E008:��������
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
 * ����ѡ�ţ�����ѡ�ű��ʽ����������������δ��Ԥ���ĺ����б�
 *
 * @param brand
 *   Ʒ�����ƣ�����Ʒ�Ƶĺ��룬������������Ʒ�ƣ�
 *   �ò���������Ϊ��
 *
 * @param expression1
 *   ѡ�ű��ʽ1 
 *   ֧�̶ֹ�����ѡ�ţ��磺13900000168
 *   ֧��*ͨ���ѡ�ţ��磺139****1314
 *   ����Ϊ�ձ�ʾ�Ժ����ʽ������
 *
 * @param expression2
 *   ѡ�ű��ʽ2 
 *   ֧��β�Ų�Ϊĳһ�����ֵ�ѡ�ţ��磺 !4������β��Ϊ4�ĺ��룻 !3!4������β��Ϊ3��4�ĺ��룬!�����9��
 *   ����ֵΪ�ձ�ʾ��β�������ơ�
 *   ֧��brand��expression1��expression2ͬʱ�в������������selfServicePick("2", "**7********", "!3!4")
 *
 * @param number
 *   �����б������С�������򣬿ձ�ʾû��ƥ��������ĺ���
 *
 * @param count
 *   �������������Ϊ number�����С�����Ϊѡȡ���ĺ��������������������ʱ����ʧ��
 *
 * @return ѡ�Ž��
 *   E000:����δʵ��
 *   E001:�ɹ�
 *   E008:��������
 */
RET_ERRORCODE selfServicePick(const char* brand, const char* expression1, const char* expression2, Number number[], int& count)
{
	
	return E000;
}

/**
 * ���������ݺ�����Ϣ���Ժ�����Դ����������Ԥ������
 * Ҫ��
 * 1��һ���ͻ��������2�����룻
 * 2��ֻ�ܶԺ���״̬Ϊ0��δ�������ĺ�����������������޸�״̬Ϊ1������������
 * 3���������뵽��ʱ��=��ǰ����ʱ��+��������ʱ����㣨1�죩��ʱ��Ϊ������
 *
 * @param number
 *      ����
 * @return �������
 *      E000:����δʵ��
 *      E001:�ɹ�
 *      E005:���벻���ظ�����
 *      E006:�ͻ�����������������
 *      E007:����������ϵͳ�в�����
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
 * ����������ʱ���Ԥ�����뵽��ʱ��δ����ĺ�����Դ���н������������ͻ�ѡ��
 * Ҫ��
 * 1����ǰʱ������뵽��ʱ�����1�������ж�Ϊ��Ҫ�����ĺ���
 * 2���޸ĺ���״̬Ϊ0��δ��������
 * 3���޸��������뵽��ʱ��Ϊ0��
 *
 * @return �������
 *      E000:����δʵ��
 *      E001:�ɹ�
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
 �� �� ��  : selfServiceTerminal_TestCase
 ��������  : �ṩ����ѡ����ʵ�ַ�ʽ�ģ������Ĳ�������������
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
 ��Ҫ˵��  : ѡ����ʵ�ַ�ʽ�Ŀ��������ڱ��ӿ��е����������Ӳ���������
			 ѡ�����ʽ�Ŀ�������Ҫ��ע���ӿڣ�Ҳ��Ҫ�Ķ����ӿڡ�
*****************************************************************************/
void selfServiceTerminal_TestCase()
{
	//1 ϵͳ��ʼ��
	RET_ERRORCODE ret = initSys();
	assert(ret ==  E001);

    //2 �����������
	ret = loadNumbers("../output/template/numbers.txt");
	assert(ret == E001);

	//3 �ļ���У��
	ret = loadNumbers("");
	assert(ret == E002);

	//4 �����ʽУ��
	ret = loadNumbers("../output/template/numbers_err.txt");
	assert(ret == E004);

	//5 �����ѯ
	Number inNumber;
	strcpy_s(inNumber.id, "13900005899");
	Number number[10];
	int numberCount = 10;
	ret = queryNumber(inNumber, number, numberCount);
	assert(ret == E001);
	assert(numberCount == 1);

	//6 ���ѡ��
	int randNumber = 3;
	Number selectNumber[10];
	int selectNumberCount = 10;
	ret = randomPick("2", randNumber, selectNumber, selectNumberCount);
	assert(ret == E001);
	assert(selectNumberCount == 3);

	//7 ����ѡ�Ź���
	Number selfNumber[10];
	int selfNumberCount = 10;
	ret = selfServicePick("2", "", "", selfNumber, selfNumberCount);
	assert(ret == E001);
	assert(selfNumberCount == 9);
		
	//8 ����ѡ�Ź���
	ret = selfServicePick("2", "**7********", "!3!4", selfNumber, selfNumberCount);
	assert(ret == E001);
	assert(selfNumberCount == 2);

	//9 ��������
	ret = lockNumber(inNumber);
	assert(ret == E001);

	numberCount = 10;
	inNumber.status = 1;
	ret = queryNumber(inNumber, number, numberCount);
	assert(ret == E001);
	assert(numberCount == 1);
	assert(number[0].status == 1);
	assert(number[0].time == ONE_DAY_SECONDS);

	//10 ����ԤԼ����
	ret = initSys();
	assert(ret == E001);

	ret = loadNumbers("../output/template/numbers_unlock.txt");
	assert(ret == E001);

	//ϵͳʱ�����
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

