/*
* @简    述：信号灯类
* @详    细：用于操作信号灯
* @继    承：无
* @头 文 件：DeviceWarningLights.h
* @实现文件：DeviceWarningLights.cpp
* @上一版本：1.0
* @作    者：wuxiangchao
* @创建时间：2023/6/16
* @当前版本：1.1
* @修改时间：无
*
*/
#pragma once
#include <QThread>
#include "RobotParaType.h"


class DeviceWarningLights : public QThread
{
public:
	DeviceWarningLights();
	~DeviceWarningLights();

	//信号灯 iColorType 0:灯灭  1：红  2：绿
	bool OperateWarningLights(RobotParaType::RobotLightColor ColorType);
	void StartRun(bool bNowStop=false, RobotParaType::RobotLightColor ColorType = RobotParaType::RobotLightColor::NONE,int iSingCount=0);
	void Stop();
	

protected:

	// 线程函数
	void run();
	bool Running();

private:
	bool m_bRunning;
	// 闪烁颜色
	RobotParaType::RobotLightColor m_lightColor;
	// 闪烁次数
	int m_iSingCount;
	// 是否立即关闭-用于需要让警告灯立即关闭的设置
	bool m_bNowStop;
	// 当前灭灯状态
	bool m_bMuteState;
	// 当前状态开始时间点（ms）
	long long m_iCurStateStartTime;
	// 是否循环闪烁
	bool m_bLoop;

};

