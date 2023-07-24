#pragma once
#include<qmetatype.h>
#include"qstring.h"


struct SortingUiElement
{
public:
	bool NetWorkStatus;
	//光纤识别到的芯片数量
	int OpticalFiberDistinguishCnt;
	//首段芯片距离过近的芯片数量(首段回流吹气阀吹气的数量)
	int ExceedEffectiveSpacingChipCnt;
	//触发首段回流吹气阀吹气的数量
	int TriggerBlowingFirstCnt;
	//通知相机拍照的芯片数据
	int TriggerPhotoCnt;
	//相机识别到的有效芯片数量
	int CameraDistinguishCnt;
	//相机未能正确识别到的有效芯片数量
	int CameraDistinguishInvalidCnt;
	//相机未识别到芯片的信息数量
	int ChipNoIdCnt;
	//相机同时识别到2张及以上的信息数量
	int ChipMoreIdCnt;
	//记录吹气位置的芯片总数量
	int BlowingTotalCnt;
	//记录吹气阀A吹气的数量
	int BlowingACnt;
	//记录吹气阀T吹气的数量
	int BlowingTCnt;
	//记录吹气阀C吹气的数量
	int BlowingCCnt;
	//记录吹气阀G吹气的数量
	int BlowingGCnt;
	int BlowingXCnt;
	//记录吹气阀Y吹气的数量
	int BlowingYCnt;

	//触发吹气的芯片总数量
	int TriggerBlowingTotalCnt;
	//触发吹气阀A吹气的数量
	int TriggerBlowingACnt;
	//触发吹气阀T吹气的数量
	int TriggerBlowingTCnt;
	//触发吹气阀C吹气的数量
	int TriggerBlowingCCnt;
	//触发吹气阀G吹气的数量
	int TriggerBlowingGCnt;
	//触发吹气阀X吹气的数量
	int TriggerBlowingXCnt;
	//触发吹气阀Y吹气的数量
	int TriggerBlowingYCnt;
	//末段光纤识别到的芯片数量
	int EndOpticalFiberDistinguishCnt;
	//实时position
	long RealPosition;


	SortingUiElement()
	{
		NetWorkStatus = false;
		OpticalFiberDistinguishCnt = 0;
		ExceedEffectiveSpacingChipCnt = 0;
		TriggerBlowingFirstCnt = 0;
		TriggerPhotoCnt = 0;
		CameraDistinguishCnt = 0;
		CameraDistinguishInvalidCnt = 0;
		ChipNoIdCnt = 0;
		ChipMoreIdCnt = 0;
		BlowingTotalCnt = 0;
		BlowingACnt = 0;
		BlowingTCnt = 0;
		BlowingCCnt = 0;
		BlowingGCnt = 0;
		BlowingXCnt = 0;
		BlowingYCnt = 0;
		TriggerBlowingTotalCnt = 0;
		TriggerBlowingACnt = 0;
		TriggerBlowingTCnt = 0;
		TriggerBlowingCCnt = 0;
		TriggerBlowingGCnt = 0;
		TriggerBlowingXCnt = 0;
		TriggerBlowingYCnt = 0;
		EndOpticalFiberDistinguishCnt = 0;
		RealPosition = 0;
	}

	SortingUiElement& operator=(const SortingUiElement& element)
	{
		if(this != &element)
		{
			this->NetWorkStatus = element.NetWorkStatus;
			this->OpticalFiberDistinguishCnt = element.OpticalFiberDistinguishCnt;
			this->ExceedEffectiveSpacingChipCnt = element.ExceedEffectiveSpacingChipCnt;
			this->TriggerBlowingFirstCnt = element.TriggerBlowingFirstCnt;
			this->TriggerPhotoCnt = element.TriggerPhotoCnt;
			this->CameraDistinguishCnt = element.CameraDistinguishCnt;
			this->CameraDistinguishInvalidCnt = element.CameraDistinguishInvalidCnt;
			this->ChipNoIdCnt = element.ChipNoIdCnt;
			this->ChipMoreIdCnt = element.ChipMoreIdCnt;
			this->BlowingTotalCnt = element.BlowingTotalCnt;
			this->BlowingACnt = element.BlowingACnt;
			this->BlowingTCnt = element.BlowingTCnt;
			this->BlowingCCnt = element.BlowingCCnt;
			this->BlowingGCnt = element.BlowingGCnt;
			this->BlowingXCnt = element.BlowingXCnt;
			this->BlowingYCnt = element.BlowingYCnt;
			this->TriggerBlowingTotalCnt = element.TriggerBlowingTotalCnt;
			this->TriggerBlowingACnt = element.TriggerBlowingACnt;
			this->TriggerBlowingTCnt = element.TriggerBlowingTCnt;
			this->TriggerBlowingCCnt = element.TriggerBlowingCCnt;
			this->TriggerBlowingGCnt = element.TriggerBlowingGCnt;
			this->TriggerBlowingXCnt = element.TriggerBlowingXCnt;
			this->TriggerBlowingYCnt = element.TriggerBlowingYCnt;
			this->EndOpticalFiberDistinguishCnt = element.EndOpticalFiberDistinguishCnt;
			this->RealPosition = element.RealPosition;
		}
		return *this;
	}
};

Q_DECLARE_METATYPE(SortingUiElement)


struct DeviceDistance
{
public:
	long CameraDistance;
	long DistanceA;
	long DistanceT;
	long DistanceC;
	long DistanceG;
	long DistanceX;
	long DistanceY;
	long DistanceFirst;
	long DistanceEnd;
	long DistanceOk;
	long DistanceNg;
	long CameraEffectiveRange;
	long BlowingEffectiveRange;
	long ChipEffectiveSpacing;

	DeviceDistance()
	{
		CameraDistance = 0;
		DistanceA = 0;
		DistanceT = 0;
		DistanceC = 0;
		DistanceG = 0;
		DistanceX = 0;
		DistanceY = 0;
		DistanceFirst = 0;
		DistanceEnd = 0;
		DistanceOk = 0;
		DistanceNg = 0;
		CameraEffectiveRange = 0;
		BlowingEffectiveRange = 0;
		ChipEffectiveSpacing = 0;
	}

	DeviceDistance& operator=(const DeviceDistance& element)
	{
		if (this != &element)
		{
			this->CameraDistance = element.CameraDistance;
			this->DistanceA = element.DistanceA;
			this->DistanceT = element.DistanceT;
			this->DistanceC = element.DistanceC;
			this->DistanceG = element.DistanceG;
			this->DistanceX = element.DistanceX;
			this->DistanceY = element.DistanceY;
			this->DistanceEnd = element.DistanceEnd;
			this->DistanceNg = element.DistanceNg;
			this->DistanceOk = element.DistanceOk;
			this->DistanceFirst = element.DistanceFirst;
			this->CameraEffectiveRange = element.CameraEffectiveRange;
			this->BlowingEffectiveRange = element.BlowingEffectiveRange;
			this->ChipEffectiveSpacing = element.ChipEffectiveSpacing;
		}
		return *this;
	}

};


struct NetWorkAddress
{
public:
	QString Ip;
	int Port;

	NetWorkAddress()
	{
		Ip = QString("192.168.1.1");
		Port = 8500;
	}

	NetWorkAddress& operator=(const NetWorkAddress& element)
	{
		if (this != &element)
		{
			this->Ip = element.Ip;
			this->Port = element.Port;
		}
		return *this;
	}
};

struct ControlCardPara
{
public:
	// 运动方向
	int Dir;
	//X\Y\Z轴方向
	int Axis;
	//控制卡卡号
	int CardNo;
	//振盘
	int MaterialTran;
	//光纤的输入口
	int FibreOptical;
	int EndFibreOptical;
	//相机输入口
	int CameraA;
	//ATCG吹气阀
	int BlowingA;
	int BlowingT;
	int BlowingC;
	int BlowingG;
	int BlowingX;
	int BlowingY;
	int BlowingOk;
	int BlowingNg;
	int BlowingEnd;
	int BlowingFirst;
	//前回流真空
	int BackFlowStartVac;
	//后回流真空
	int BackFlowEndVac;
	int BackFlowDep;
	//震盘的输入口
	int ShockDisk;
	//分拣输送料仓
	int SortingTransferBin;
	//ATX碱基料仓门
	int AtxBaseBin;
	//YCG碱基料仓门
	int YcgBaseBin;
	//运动的起始速度
	double MinVel;
	//运动的最大速度
	double MaxVel;
	//总加速时间
	double Tacc;
	//总减速时间
	double Tdec;
	//相机间隔关闭时间:ms
	int CaremaCloseInterTime;
	//吹气阀间隔关闭时间：ms
	int BlowingCloseInterTime;
	//圆振下料吹气阀
	int CircularBlanking;
	//输送料仓下料吹气阀
	int TransferBinBlanking;

	//转盘起始速度（转/分）
	unsigned int TurnTableSpeedMin;
	//转盘最大速度（转/分）
	unsigned int TurnTableSpeedMax;
	//转盘半径（毫米）
	unsigned int TurnTableRadius;
	//转盘分辨率（1圈对应脉冲数）
	unsigned int TurnTableResolution;

	ControlCardPara()
	{
		Dir = 1;
		Axis = 0;
		CardNo = 0;
		MaterialTran = 17;
		FibreOptical = 1;
		EndFibreOptical = 8;
		CameraA = 6;
		BlowingA = 2;
		BlowingT = 3;
		BlowingC = 4;
		BlowingG = 5;
		BlowingX = 9;
		BlowingY = 10;
		BlowingNg = 12;
		BlowingOk = 13;
		BlowingEnd = 20;
		BlowingFirst = 11;
		BackFlowStartVac = 14 ;
		BackFlowEndVac = 15 ;
		BackFlowDep = 16;

		ShockDisk = 7;
		SortingTransferBin = 0;
		AtxBaseBin = 39;
		YcgBaseBin = 23;
		MinVel = 1000;
		MaxVel = 80000;
		Tacc = 1;
		Tdec = 1;
		CaremaCloseInterTime = 5000;
		BlowingCloseInterTime = 5000;
		CircularBlanking = 40;
		TransferBinBlanking = 41;

		TurnTableSpeedMin = 1;
		TurnTableSpeedMax = 1;
		TurnTableRadius = 300;
		TurnTableResolution = 204800;

	}

	ControlCardPara& operator=(const ControlCardPara& element)
	{
		if (this != &element)
		{
			this->Dir = element.Dir;
			this->Axis = element.Axis;
			this->CardNo = element.CardNo;
			this->MaterialTran = element.MaterialTran;
			this->FibreOptical = element.FibreOptical;
			this->EndFibreOptical = element.EndFibreOptical;
			this->CameraA = element.CameraA;
			this->BlowingA = element.BlowingA;
			this->BlowingT = element.BlowingT;
			this->BlowingC = element.BlowingC;
			this->BlowingG = element.BlowingG;
			this->BlowingX = element.BlowingX;
			this->BlowingY = element.BlowingY;
			this->BlowingNg = element.BlowingNg;
			this->BlowingOk = element.BlowingOk;
			this->BlowingEnd = element.BlowingEnd;
			this->BlowingFirst = element.BlowingFirst;
			this->BackFlowStartVac = element.BackFlowStartVac;
			this->BackFlowEndVac = element.BackFlowEndVac;
			this->BackFlowDep = element.BackFlowDep;
			this->ShockDisk = element.ShockDisk;
			this->SortingTransferBin = element.SortingTransferBin;
			this->AtxBaseBin = element.AtxBaseBin;
			this->YcgBaseBin = element.YcgBaseBin;
			this->MinVel = element.MinVel;
			this->MaxVel = element.MaxVel;
			this->Tacc = element.Tacc;
			this->Tdec = element.Tdec;
			this->CaremaCloseInterTime = element.CaremaCloseInterTime;
			this->BlowingCloseInterTime = element.BlowingCloseInterTime;
			this->CircularBlanking = element.CircularBlanking;
			this->TransferBinBlanking = element.TransferBinBlanking;

			this->TurnTableSpeedMin = element.TurnTableSpeedMin;
			this->TurnTableSpeedMax = element.TurnTableSpeedMax;
			this->TurnTableRadius = element.TurnTableRadius;
			this->TurnTableResolution = element.TurnTableResolution;
		}
		return *this;
	}
};

//上传线程参数
struct UploadDataPara
{
public:
	int UploadInterTimeMs;

	UploadDataPara()
	{
		UploadInterTimeMs = 1;
	}

	UploadDataPara& operator=(const UploadDataPara& element)
	{
		if (this != &element)
		{
			this->UploadInterTimeMs = element.UploadInterTimeMs;
		}
		return *this;
	}
};


Q_DECLARE_METATYPE(ControlCardPara)