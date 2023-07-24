#include "DeviceBuzzer.h"
#include <QTime>
#include "MasterDevices.h"
#include "EpsonRobotDriver.h"

DeviceBuzzer::DeviceBuzzer()
{
	m_iCardNo = 0;
	m_iPort = 0;

	// 鸣响次数
	m_iSingCount = 0;
	// 每次鸣响时长（ms）
	m_iSingMs = 0;
	// 每次静音时长（ms）
	m_iMuteMs = 0;
	// 当前静音状态
	m_bMuteState = true;
	// 当前状态保持时长（ms）
	m_iCurStateStartTime = 0;  // QDateTime::toMSecsSinceEpoch();
	m_bNowStop = false;

	m_bRunning = false;
}
DeviceBuzzer::~DeviceBuzzer()
{

}

bool DeviceBuzzer::Running()
{
	if (m_bNowStop)
	{
		// 关闭
		MasterDevices::Instance()->GetEpsonRobotDriver()->OperateBuzzer(false);
		return false;
	}
	
	// 需要运行
	if (m_iSingCount > 0)
	{
		qint64 iCurMs = QDateTime::currentMSecsSinceEpoch();
		if (m_bMuteState)
		{
			// 当前静音中，到时间就打开
			if (iCurMs - m_iCurStateStartTime > m_iMuteMs)
			{
				// 开启
				MasterDevices::Instance()->GetEpsonRobotDriver()->OperateBuzzer(true);
				m_iCurStateStartTime = iCurMs;
				m_bMuteState = false;
			}
		}
		else
		{
			// 当前鸣叫中，到时间就关闭
			if (iCurMs - m_iCurStateStartTime > m_iSingMs)
			{
				// 开启
				MasterDevices::Instance()->GetEpsonRobotDriver()->OperateBuzzer(true);
				m_iCurStateStartTime = iCurMs;
				m_bMuteState = true;
				m_iSingCount--;  // 鸣叫次数减1
			}
		}
	}
	
	if (m_iSingCount > 0)
	{
		return true;
	}
	else
	{
		m_bNowStop = true;
		return false;
	}
	return true;
}

// 设置蜂鸣器的卡号和IO口
void DeviceBuzzer::SetWorkPara(int iCardNo, int iPort)
{
	m_iCardNo = iCardNo;
	m_iPort = iPort;
}

// 设置蜂鸣器工作参数
void DeviceBuzzer::SetBuzzerWorkPara(bool bNowStop, int iSingCount, int iSingMs, int iMuteMs)
{
	m_iSingCount = iSingCount;
	m_iSingMs = iSingMs;
	m_iMuteMs = iMuteMs;
	m_bNowStop = bNowStop;
}

// 这里放置类的操作
void DeviceBuzzer::StartRun()
{
	if (m_bRunning)
	{
		return;
	}
	this->start();  // 启动线程
}
void DeviceBuzzer::Stop()
{
	m_bRunning = true;
}

// 线程函数
void DeviceBuzzer::run()
{
	m_bRunning = true;
	while (m_bRunning)
	{
		if (false == Running())
		{
			break;
		}
		QThread::msleep(1);
	}
	m_bRunning = false;
}
