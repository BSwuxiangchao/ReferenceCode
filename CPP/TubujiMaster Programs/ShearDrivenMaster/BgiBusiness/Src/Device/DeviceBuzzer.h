/*
* @简    述：蜂鸣器类
* @详    细：用于操作蜂鸣器
* @继    承：无
* @头 文 件：DeviceBuzzer.h
* @实现文件：DeviceBuzzer.cpp
* @上一版本：1.0
* @作    者：wuxiangchao
* @创建时间：2023/4/12
* @当前版本：1.1
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件
#include <QThread>
// 包含项目文件

// 包含局部头文件

// 放置前置引用


class DeviceBuzzer : public QThread
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	DeviceBuzzer();
	~DeviceBuzzer();
	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作
	void StartRun();
	void Stop();

	// 设置蜂鸣器的卡号和IO口
	void SetWorkPara(int iCardNo, int iPort);
	// 设置蜂鸣器工作参数
	void SetBuzzerWorkPara(bool bNowStop = false, int iSingCount = 1, int iSingMs = 200,int iMuteMs = 200);

	// 这里放置属性的存取

	// 这里放置类的状态查询

protected:

	// 线程函数
	void run();
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：运行蜂鸣器
 * 时    间：2022/10/3 11:43
 * 入    参：无
 * 出    参：无
 * 返    回：true-设备正在运行中，false-设备已经运行结束可以退出
 ************************************* end-XCW *****************************************/
	bool Running();

protected:
	int m_iCardNo;
	int m_iPort;

	// 鸣响次数
	int m_iSingCount;
	// 每次鸣响时长（ms）
	int m_iSingMs;
	// 每次静音时长（ms）
	int m_iMuteMs;
	// 是否立即关闭-用于需要让蜂鸣器立即关闭的设置
	bool m_bNowStop;
	// 当前静音状态
	bool m_bMuteState;
	// 当前状态开始时间点（ms）
	long long m_iCurStateStartTime;

	// 线程
	bool m_bRunning;
	
private:
};

