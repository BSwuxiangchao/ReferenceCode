/*
* @Abrief 自定义参数
* @note 自定义参数，某些同类型参数可以作为一个参数处理
* @version 1.0
* @author wuxiangchao 2023-02-24
*/
#pragma once

#include "BusinessExternDef.h"
#include <QMetaType>
#include <QList>

class BGIBUSINE_CLASS_EXPORT ImagerParaType
{
public:
    struct ParaDesc
    {
        int iConnect;

        bool operator ==(const ParaDesc& obj) const
        {
            if (obj.iConnect == this->iConnect)
                return true;
            return false;
        }
    };

    enum EndCycleModeEnum
    {
        R1R2None = 0,
        R1Only = 1,
        R2Only = 2,
        R1R2Both = 3
    };

	//光机配置参数
	struct ImagerConfigPara
	{
        int Lane;
        QString Reference;
        int BasecallMode;
        bool IsSaveRawImage;
        bool IsImageSimulated;
        int SettingTime;
        int CheckThetaMode;
        int Direction;
        int StartRow;
        int EndRow;
        int StartCol;
        int EndCol;
        bool IsExposureFileClean;
        QList<double> ExposureTime;
        QList<double> ExposureFactors;
        QString SaveFovString;
        QString EndCycleProcessMode;
        double VacuumSupplyThreshold;
        QString ExtraSettings;

        ImagerConfigPara()
		{
            Lane = 0;
            Reference = "";
            BasecallMode = 0;
            IsSaveRawImage = false;
            IsImageSimulated = false;
            SettingTime = 0;
            CheckThetaMode = 0;
            Direction = 0;
            StartRow = 0;
            EndRow = 0;
            StartCol = 0;
            EndCol = 0;
            IsExposureFileClean = false;
            ExposureTime.clear();
            ExposureFactors.clear();
            SaveFovString = "";
            EndCycleProcessMode = "";
            VacuumSupplyThreshold = 0;
            ExtraSettings = "";
		}
	};

    //光机拍照参数
    struct ImagerSetupPara
    {
        QString SlideID;
        QString ScriptName;
        QString ExperimentName;
        QString UserName;
        int ImagerID;

        int ZAFOffset;
        bool IsCalAFOffset;

        int Read1Len;
        int Read2Len;
        int Barcode1Len;
        int Barcode2Len;

        QString Reference;
        std::map<std::string, std::string> BioBarcode;

        int TotalCycle;
        int BiochemCycle;
        int ImagerCycle;

        int TolerancePattern;

        QString sequencingType;

        int FqBarcode1Length;
        int FqBarcode1StartPos;
        int FqSpeciesMismatch;
        int FqBarcode2Length;
        int FqBarcode2StartPos;
        int FqSpeciesMismatch2;

        int FilterMode;
        int WriteFastqMode;
        int BarcodeReverse;

        ImagerSetupPara()
        {
            SlideID = "";
            ScriptName = "";
            ExperimentName = "";
            UserName = "";
            ImagerID = 0;

            ZAFOffset = 0;
            IsCalAFOffset = false;

            Read1Len = 0;
            Read2Len = 0;
            Barcode1Len = 0;
            Barcode2Len = 0;

            Reference = "";
            BioBarcode.clear();

            TotalCycle = 0;
            BiochemCycle = 0;
            ImagerCycle = 0;

            TolerancePattern = 0;

            sequencingType = "";

            FqBarcode1Length = 0;
            FqBarcode1StartPos = 0;
            FqSpeciesMismatch = 0;
            FqBarcode2Length = 0;
            FqBarcode2StartPos = 0;
            FqSpeciesMismatch2 = 0;

            FilterMode = 0;
            WriteFastqMode = 0;
            BarcodeReverse = 0;
        }
    };

    //光机运行参数
    struct ImagerRunPara
    {
        QString SlideID;
        int CurrentRow;
        int CurrentCol;
        int CurrentCycle;
        int CurrentLane;
        int StartRow;
        int EndRow;
        int StartCol;
        int EndCol;
        int TotalLane;
        int ScanDirect;

        ImagerRunPara()
        {
            SlideID = "";
            CurrentRow = 0;
            CurrentCol = 0;
            CurrentCycle = 0;
            CurrentLane = 0;
            StartRow = 0;
            EndRow = 0;
            StartCol = 0;
            EndCol = 0;
            TotalLane = 0;
            ScanDirect = 0;
        }
    };

    //basecall内存信息
    struct BasecallRunPara
    {
        int Status;
        bool Reseq;
        QString SlideID;
        int CurrentCycle;
        int Lane;
        int StarRow;
        int EndRow;
        int StartCol;
        int EndCol;
        int TotalCycle;
        int StartCycle;
        int Read1Length;
        int Read2Length;
        int BarcodeLength;
        int FovNumMaxPerScan;
        int BarcodePos;

        BasecallRunPara()
        {
            Status = 0;
            Reseq = false;
            SlideID = "";
            CurrentCycle = 0;
            Lane = 0;
            StarRow = 0;
            EndRow = 0;
            StartCol = 0;
            EndCol = 0;
            TotalCycle = 0;
            StartCycle = 0;
            Read1Length = 0;
            Read2Length = 0;
            BarcodeLength = 0;
            FovNumMaxPerScan = 0;
            BarcodePos = 0;
        }
    };
};

Q_DECLARE_METATYPE(ImagerParaType::ImagerConfigPara);
