#include "DeviceWarningLights.h"
#include "MasterDevices.h"
#include "EpsonRobotDriver.h"
#include <QDateTime>

const int gc_gapTime = 500;

DeviceWarningLights::DeviceWarningLights()
	:m_bRunning(false)
{
	// 闪烁颜色
	m_lightColor = RobotParaType::RobotLightColor::NONE;
	// 闪烁次数
	m_iSingCount = 0;
	// 是否立即关闭-用于需要让警告灯立即关闭的设置
	m_bNowStop = false;
	// 当前灭灯状态
	m_bMuteState = true;
	// 当前状态开始时间点（ms）
	m_iCurStateStartTime = 0;
	m_bLoop = false;
}

DeviceWarningLights::~DeviceWarningLights()
{

}

void DeviceWarningLights::StartRun(bool bNowStop, RobotParaType::RobotLightColor ColorType, int iSingCount)
{
	if (m_bRunning)
	{
		return;
	}
	m_bNowStop = bNowStop;
	m_lightColor = ColorType;
	m_iSingCount = iSingCount;
	if (iSingCount == 0)
		m_bLoop = true;
	else
		m_bLoop = false;
	this->start();  // 启动线程
}
void DeviceWarningLights::Stop()
{
	m_bRunning = false;
}

bool DeviceWarningLights::Running()
{
	// 需要运行
	if (m_bNowStop)
	{
		// 关闭
		OperateWarningLights(RobotParaType::RobotLightColor::NONE);
		return false;
	}

	if (m_iSingCount >= 0)
	{
		qint64 iCurMs = QDateTime::currentMSecsSinceEpoch();
		if (m_bMuteState)
		{
			// 当前灭灯中，到时间就亮灯
			if (iCurMs - m_iCurStateStartTime > gc_gapTime)
			{
				// 开启
				OperateWarningLights(m_lightColor);
				m_iCurStateStartTime = iCurMs;
				m_bMuteState = false;
			}
		}
		else
		{
			// 当前亮灯中，到时间就关闭
			if (iCurMs - m_iCurStateStartTime > gc_gapTime)
			{
				// 灭灯
				OperateWarningLights(RobotParaType::RobotLightColor::NONE);
				m_iCurStateStartTime = iCurMs;
				m_bMuteState = true;
				if(!m_bLoop)
					m_iSingCount--;  
			}
		}
	}

	if (m_bLoop)
		return true;

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

// 线程函数
void DeviceWarningLights::run()
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

	OperateWarningLights(RobotParaType::RobotLightColor::NONE);
}

bool DeviceWarningLights::OperateWarningLights(RobotParaType::RobotLightColor ColorType)
{
	return MasterDevices::Instance()->GetEpsonRobotDriver()->OperateWaringLights(ColorType);
}