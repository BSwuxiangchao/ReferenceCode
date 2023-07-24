/*
* @Abrief Eposn机械臂参数类型
* @note Eposn机械臂参数类型
* @version 1.0
* @author wuxiangchao 2023-2-10
*/
#pragma once
#include "BusinessExternDef.h"
#include "GeneralTool.h"
#include <QMetaType>
#include "GeneralTool.h"
#include <string>
#include <QStringList>

class  BGIBUSINE_CLASS_EXPORT RobotParaType
{
public :
	enum RobotLightColor
	{
		NONE,
		RED,
		GREEN,
		YELLO,
	};

	struct RobotPoint
	{
		float f_X;
		float f_Y;
		float f_Z;
		float f_U;
		float f_V;
		float f_W;
		QString strPointName;
		RobotPoint()
		{
			f_X = 0;
			f_Y = 0;
			f_Z = 0;
			f_U = 0;
			f_V = 0;
			f_W = 0;
			strPointName = "None";
		}

		bool operator ==(const RobotPoint& obj)const
		{
			if (this->f_X == obj.f_X && this->f_Y == obj.f_Y && this->f_Z == obj.f_Z
				&& this->f_U == obj.f_U && this->f_V == obj.f_V && this->f_W == obj.f_W &&this->strPointName == obj.strPointName)
				return true;
			return false;
		}

		RobotPoint& operator =(const RobotPoint& obj)
		{
			if (*this == obj)
				return  *this;
			this->f_X = obj.f_X;
			this->f_Y = obj.f_Y;
			this->f_Z = obj.f_Z;
			this->f_U = obj.f_U;
			this->f_V = obj.f_V;
			this->f_W = obj.f_W;
			this->strPointName = obj.strPointName;
			return *this;
		}
	};

	struct RobotSportMode
	{
		QString strMode;
		int iType;
		int iDirection;
		int iStep;
		RobotSportMode()
		{
			strMode = "Go";
			//@iType 0:X 1:Y 2:Z 3:U 4:V 5:W
			iType = 0;
			//@iDirection 0:负向  1：正向
			iDirection = 0;
			iStep = 10;
		}
	};

	//工作状态
	struct RobotStatus
	{
		int iErrorCode;
		unsigned int iConnect;	//连接 0=》断开 1=》连接成功
		unsigned int iEstopOn;	//急停 0=》关 1=》开启
		unsigned int iSafetyOn;	//安全防护 0=》关闭 1=》开启
		unsigned int iMotorOn;	//电机是否开启 0=》关闭  1=》开启
		unsigned int iPower;	//电机运行功率 0=》低功率  1=》高功率
		unsigned int iSafetySpace;	//工作安全空间 0=》不安全  1=》安全

		RobotStatus()
		{
			iErrorCode = 0;
			iConnect = 0;
			iEstopOn = 0;
			iSafetyOn = 0;
			iMotorOn = 0;
			iPower = 0;
			iSafetySpace = 0;
		}

		bool operator==(const RobotStatus& obj)const
		{
			if (obj.iErrorCode == this->iErrorCode && obj.iConnect == this->iConnect && obj.iEstopOn == this->iEstopOn
				&& obj.iSafetyOn == this->iSafetyOn && obj.iMotorOn == this->iMotorOn && obj.iPower == this->iPower && obj.iSafetySpace == this->iSafetySpace)
				return true;
			return false;
		}

		RobotStatus& operator=(const RobotStatus& obj)
		{
			if (obj == *this)
				;
			else
			{
				this->iErrorCode = obj.iErrorCode;
				this->iConnect = obj.iConnect;
				this->iEstopOn = obj.iEstopOn;
				this->iSafetyOn = obj.iSafetyOn;
				this->iMotorOn = obj.iMotorOn;
				this->iPower = obj.iPower;
				this->iSafetySpace = obj.iSafetySpace;
			}
			return *this;
		}
	};

	//设置参数
	struct RobotPara
	{
		int iPower;
		int iPTPSpeed;
		int iPTPAccel;
		int iCPSpeed;
		int iCPAccel;
		int iRSpeed;
		int iRAccel;
		int iMotorNo;

		RobotPara()
		{
			iPower = 0;
			iPTPSpeed = 0;
			iPTPAccel = 0;
			iCPSpeed = 0;
			iCPAccel = 0;
			iRSpeed = 0;
			iRAccel = 0;
			iMotorNo = 0;
		}

		bool operator == (const RobotPara& obj) const
		{
			if (obj.iPower == this->iPower && obj.iPTPSpeed == this->iPTPSpeed && obj.iPTPAccel == this->iPTPAccel&& obj.iCPSpeed == this->iCPSpeed 
				&& obj.iCPAccel == this->iCPAccel && obj.iRSpeed == this->iRSpeed && obj.iRAccel == this->iRAccel &&obj.iMotorNo == this->iMotorNo)
				return true;
			return false;
		}

		RobotPara& operator =(const RobotPara& obj)
		{
			if (obj == *this)
				return *this;
			this->iPower = obj.iPower;
			this->iPTPSpeed = obj.iPTPSpeed;
			this->iPTPAccel = obj.iPTPAccel;
			this->iCPSpeed = obj.iCPSpeed;
			this->iCPAccel = obj.iCPAccel;
			this->iRSpeed = obj.iRSpeed;
			this->iRAccel = obj.iRAccel;
			this->iMotorNo = obj.iMotorNo;
			return *this;
		}
	};
};

Q_DECLARE_METATYPE(RobotParaType::RobotPoint);
Q_DECLARE_METATYPE(RobotParaType::RobotStatus);
Q_DECLARE_METATYPE(RobotParaType::RobotPara);
