#include "ImagerControlMgrIPCClient.h"
#include "PortConfigDoc.h"
#include "ImagerControlMgrIPCCallbackI.h"
#include <QDateTime>
#include "ImagerDriver.h"
#include "BgiLogger.h"

#define CATCHIMAGEREXCEPTION(x)\
    catch (const ::BGI::RPC::BGIRemoteException& ex)\
	{\
		QString str = "Imager " + (QString)x + " BGIRemoteException: " + QString::fromStdString(ex.message);\
		BGI_ERROR(str.toStdString().c_str());\
		return false;\
	}\
	catch (const ::Ice::Exception& ex)\
	{\
		QString str = "Imager " + (QString)x + " Ice::Exception: " + (QString)(ex.what());\
		BGI_ERROR(str.toStdString().c_str());\
		return false;\
	}\
	catch (const ::std::exception& ex)\
	{\
		QString str = "Imager " + (QString)x + " std::exception: " + (QString)(ex.what());\
		BGI_ERROR(str.toStdString().c_str());\
		return false;\
	}\
	catch (...)\
	{\
		QString str = "Imager " + (QString)x + " unknown exception";\
		BGI_ERROR(str.toStdString().c_str());\
		return false;\
	}

ImagerControlMgrIPCClient::ImagerControlMgrIPCClient(ImagerDriver* imagerDriver)
	: m_ImagerDriver(imagerDriver)
{
	PortConfigDoc::Instance()->GetPortValue("ImagerIP", m_ImagerIP);
	PortConfigDoc::Instance()->GetPortValue("ImagerPort", m_ImagerPort);
	PortConfigDoc::Instance()->GetPortValue("ImagerCfg/ImagerCallbackPort", m_ImagerCallbackPort);

    Ice::InitializationData initData = Ice::InitializationData();
    initData.properties = Ice::createProperties();
    initData.properties->setProperty("Ice.Retry.Intervals", "0 5 10 15");
    initData.properties->setProperty("Ice.Warn.UnusedProperties", "1");
    initData.properties->setProperty("Ice.Warn.Host", m_ImagerIP.toStdString());
    initData.properties->setProperty("Ice.MessageSizeMax", "102400");
    initData.properties->setProperty("Filesystem.MaxFileSize", "102400");
    initData.properties->setProperty("Ice.ThreadPool.Client.Size", "10");
    initData.properties->setProperty("Ice.ThreadPool.Client.SizeMax", "40");

	m_IceHolder = Ice::initialize(initData);
}

ImagerControlMgrIPCClient::~ImagerControlMgrIPCClient()
{}

bool ImagerControlMgrIPCClient::Connect()
{
    try
    {
        //StageRunMgrIPC:tcp -t 1000 -p 12321 -h 172.16.232.145
        QString connectString = "StageRunMgrIPC:tcp -t 1000 -p " + m_ImagerPort + " -h " + m_ImagerIP;
        auto base = m_IceHolder->stringToProxy(connectString.toStdString());
        m_ImagerPrx = Ice::checkedCast<ImagerControlMgrIPCPrx>(base);
        if (!m_ImagerPrx)
        {
            //WriteLog("Invalid proxy");
        }
        else
        {
            //WriteLog("connect sucess");
        }

        //StageRunMgrIPCCallback:tcp -t 1000 -p 12322 -h 172.16.232.145
        connectString = "StageRunMgrIPCCallback:tcp -t 1000 -p " + m_ImagerCallbackPort + " -h " + m_ImagerIP;
        auto base2 = m_IceHolder->stringToProxy(connectString.toStdString());
        m_ImagerCallbackPrx = Ice::checkedCast<ImagerControlMgrIPCCallbackPrx>(base2);
        if (!m_ImagerCallbackPrx)
        {
            //WriteLog("Invalid CB proxy");
        }
        else
        {
            //WriteLog("connect CB sucess");
        }

        auto adapter = m_IceHolder->createObjectAdapter("");
        auto ident = Ice::Identity();
        ident.name = QString("Master-" + m_ImagerIP + "-" + QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())).toStdString();
        ident.category = "";
        m_pImagerCallback = QSharedPointer<ImagerControlMgrIPCCallbackI>(new ImagerControlMgrIPCCallbackI(m_ImagerDriver));
        adapter->add(m_pImagerCallback.data(), ident);
        adapter->activate();
        m_ImagerCallbackPrx->ice_getConnection()->setAdapter(adapter);
        m_ImagerCallbackPrx->AddClient(ident);

        BGI_MESSAGE("Imager Connect succeed.");

        return true;
    }
    CATCHIMAGEREXCEPTION("Connect");
}

bool ImagerControlMgrIPCClient::GoLoadingPos()
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->GoLoadingPos();
    }
    CATCHIMAGEREXCEPTION("GoLoadingPos");
}

bool ImagerControlMgrIPCClient::IsAtLoadPosition()
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->IsAtLoadPosition();
    }
    CATCHIMAGEREXCEPTION("IsAtLoadPosition");
}

bool ImagerControlMgrIPCClient::GoHome()
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->GoHome();
    }
    CATCHIMAGEREXCEPTION("GoHome");
}

bool ImagerControlMgrIPCClient::SwitchVacuum(bool valveStatus, bool pumpStatus)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        //double value1 = 0;
        //m_ImagerPrx->GetBasecallFlowcellConsumptionDiskSpace(value1);
        //double value2 = 0;
        //m_ImagerPrx->GetBasecallRemoteDiskSpace(value2);
        //double value3 = 0;
        //m_ImagerPrx->GetBasecallLocalDiskSpace(value3);

        //std::string bcsVer = "";
        //std::string sbcVer = "";
        //std::string imagerVer = "";
        //m_ImagerPrx->GetVersions(bcsVer, sbcVer, imagerVer);

        //::BGI::RPC::RunInformationArray array = m_ImagerPrx->GetAllRunInformations();

        //bool bRet1 = m_ImagerPrx->GetBasecallInformation("FP200009001");
        //bool bRet2 = m_ImagerPrx->GetBasecallInformation("FP200009002");
        //bool bRet3 = m_ImagerPrx->GetBasecallInformation("FP200009003");

        return m_ImagerPrx->SwitchVacuum(valveStatus, pumpStatus);
    }
    CATCHIMAGEREXCEPTION("SwitchVacuum");
}

bool ImagerControlMgrIPCClient::Reset()
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->Reset();
    }
    CATCHIMAGEREXCEPTION("Reset");
}

bool ImagerControlMgrIPCClient::ImagerConfigGet(ImagerParaType::ImagerConfigPara& data)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        BGI::RPC::ImagerConfigData dataOri = m_ImagerPrx->ImagerConfigGet();
        data = SwitchImagerConfigData(dataOri);
        return true;
    }
    CATCHIMAGEREXCEPTION("ImagerConfigGet");
}

bool ImagerControlMgrIPCClient::ImagerConfigSet(const ImagerParaType::ImagerConfigPara& data)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->ImagerConfigSet(SwitchImagerConfigData(data));
    }
    CATCHIMAGEREXCEPTION("ImagerConfigSet");
}

ImagerParaType::ImagerConfigPara ImagerControlMgrIPCClient::SwitchImagerConfigData(const BGI::RPC::ImagerConfigData& dataOri)
{
    ImagerParaType::ImagerConfigPara data;

    data.Lane = dataOri.Lane;
    data.Reference = QString::fromStdString(dataOri.Reference);
    data.BasecallMode = dataOri.BasecallMode;
    data.IsSaveRawImage = dataOri.IsSaveRawImage;
    data.IsImageSimulated = dataOri.IsImageSimulated;
    data.SettingTime = dataOri.SettingTime;
    data.CheckThetaMode = dataOri.CheckThetaMode;
    data.Direction = dataOri.Direction;
    data.StartRow = dataOri.StartRow;
    data.EndRow = dataOri.EndRow;
    data.StartCol = dataOri.StartCol;
    data.EndCol = dataOri.EndCol;
    data.IsExposureFileClean = dataOri.IsExposureFileClean;
    foreach(auto d, dataOri.ExposureTime)
    {
        data.ExposureTime.append(d);
    }
    foreach(auto d, dataOri.ExposureFactors)
    {
        data.ExposureFactors.append(d);
    }
    data.SaveFovString = QString::fromStdString(dataOri.SaveFovString);
    switch (dataOri.EndCycleProcessMode)
    {
    case EndCycleProcessModeEnum::R1R2None:
        data.EndCycleProcessMode = "R1R2None";
        break;
    case EndCycleProcessModeEnum::R1Only:
        data.EndCycleProcessMode = "R1Only";
        break;
    case EndCycleProcessModeEnum::R2Only:
        data.EndCycleProcessMode = "R2Only";
        break;
    case EndCycleProcessModeEnum::R1R2Both:
        data.EndCycleProcessMode = "R1R2Both";
        break;
    default:
        data.EndCycleProcessMode = "";
        break;
    }
    data.VacuumSupplyThreshold = dataOri.VacuumSupplyThreshold;
    data.ExtraSettings = QString::fromStdString(dataOri.ExtraSettings);

    return data;
}

BGI::RPC::ImagerConfigData ImagerControlMgrIPCClient::SwitchImagerConfigData(const ImagerParaType::ImagerConfigPara& dataOri)
{
    BGI::RPC::ImagerConfigData data;

    data.Lane = dataOri.Lane;
    data.Reference = dataOri.Reference.toStdString();
    data.BasecallMode = dataOri.BasecallMode;
    data.IsSaveRawImage = dataOri.IsSaveRawImage;
    data.IsImageSimulated = dataOri.IsImageSimulated;
    data.SettingTime = dataOri.SettingTime;
    data.CheckThetaMode = dataOri.CheckThetaMode;
    data.Direction = dataOri.Direction;
    data.StartRow = dataOri.StartRow;
    data.EndRow = dataOri.EndRow;
    data.StartCol = dataOri.StartCol;
    data.EndCol = dataOri.EndCol;
    data.IsExposureFileClean = dataOri.IsExposureFileClean;
    data.ExposureTime.clear();
    foreach(auto d, dataOri.ExposureTime)
    {
        data.ExposureTime.push_back(d);
    }
    data.ExposureFactors.clear();
    foreach(auto d, dataOri.ExposureFactors)
    {
        data.ExposureFactors.push_back(d);
    }
    data.SaveFovString = dataOri.SaveFovString.toStdString();
    if (dataOri.EndCycleProcessMode == "R1Only")
    {
        data.EndCycleProcessMode = EndCycleProcessModeEnum::R1Only;
    }
    else if (dataOri.EndCycleProcessMode == "R2Only")
    {
        data.EndCycleProcessMode = EndCycleProcessModeEnum::R2Only;
    }
    else if (dataOri.EndCycleProcessMode == "R1R2Both")
    {
        data.EndCycleProcessMode = EndCycleProcessModeEnum::R1R2Both;
    }
    else
    {
        data.EndCycleProcessMode = EndCycleProcessModeEnum::R1R2None;
    }
    data.VacuumSupplyThreshold = dataOri.VacuumSupplyThreshold;
    data.ExtraSettings = dataOri.ExtraSettings.toStdString();

    return data;
}

bool ImagerControlMgrIPCClient::GetAllRunInformations(QList<ImagerParaType::BasecallRunPara>& infos)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        ::BGI::RPC::RunInformationArray array = m_ImagerPrx->GetAllRunInformations();
        infos.clear();
        foreach(auto info, array)
        {
            infos.append(SwitchBasecallRunData(info));
        }
        return true;
    }
    CATCHIMAGEREXCEPTION("GetAllRunInformations");
}

bool ImagerControlMgrIPCClient::GetRunInformation(const QString& slideID, int& startCycle)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        std::string myId = slideID.toStdString();
        ::BGI::RPC::RunInformationArray array = m_ImagerPrx->GetAllRunInformations();
        foreach(auto info, array)
        {
            if (info.SlideID == myId)
            {
                startCycle = info.StartCycle;
                return true;
            }
        }
        return false;

        /*BGI::RPC::ImagerSetupInfo runInfo;
        m_ImagerPrx->GetRunInformation(slideID.toStdString(), runInfo);
        if (!runInfo.SlideID.empty())
        {
            info = SwitchImagerSetupData(runInfo);
            return true;
        }
        else
        {
            return false;
        }*/
    }
    CATCHIMAGEREXCEPTION("GetRunInformation");
}

ImagerParaType::BasecallRunPara ImagerControlMgrIPCClient::SwitchBasecallRunData(const BGI::RPC::RunInformation& dataOri)
{
    ImagerParaType::BasecallRunPara data;

    data.Status = (int)dataOri.Status;
    data.Reseq = dataOri.Reseq;
    data.SlideID = QString::fromStdString(dataOri.SlideID);
    data.CurrentCycle = dataOri.CurrentCycle;
    data.Lane = dataOri.Lane;
    data.StarRow = dataOri.StarRow;
    data.EndRow = dataOri.EndRow;
    data.StartCol = dataOri.StartCol;
    data.EndCol = dataOri.EndCol;
    data.TotalCycle = dataOri.TotalCycle;
    data.StartCycle = dataOri.StartCycle;
    data.Read1Length = dataOri.Read1Length;
    data.Read2Length = dataOri.Read2Length;
    data.BarcodeLength = dataOri.BarcodeLength;
    data.FovNumMaxPerScan = dataOri.FovNumMaxPerScan;
    data.BarcodePos = dataOri.BarcodePos;

    return data;
}

bool ImagerControlMgrIPCClient::ImagerSetup(const ImagerParaType::ImagerSetupPara& info)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->ImagerSetup(SwitchImagerSetupData(info));
    }
    CATCHIMAGEREXCEPTION("ImagerSetup");
}

bool ImagerControlMgrIPCClient::SetupRun(const ImagerParaType::ImagerSetupPara& info)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->SetupRun(SwitchImagerSetupData(info));
    }
    CATCHIMAGEREXCEPTION("SetupRun");
}

bool ImagerControlMgrIPCClient::ReleaseRun(const ImagerParaType::ImagerSetupPara& info)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        return m_ImagerPrx->ReleaseRun(SwitchImagerSetupData(info));
    }
    CATCHIMAGEREXCEPTION("ReleaseRun");
}

bool ImagerControlMgrIPCClient::StartRun()
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        m_ImagerPrx->StartRun();
        return true;
    }
    CATCHIMAGEREXCEPTION("StartRun");
}

bool ImagerControlMgrIPCClient::GetVersions(QString& bcsVer, QString& sbcVer, QString& imagerVer)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        std::string bcs = "";
        std::string sbc = ""; 
        std::string imager = "";
        m_ImagerPrx->GetVersions(bcs, sbc, imager);

        bcsVer = QString::fromStdString(bcs);
        sbcVer = QString::fromStdString(sbc);
        imagerVer = QString::fromStdString(imager);
        return true;
    }
    CATCHIMAGEREXCEPTION("GetVersions");
}

bool ImagerControlMgrIPCClient::GetDiskSpace(double& consumptionValue, double& availableValue)
{
    if (m_ImagerPrx == NULL)
    {
        return false;
    }

    try
    {
        bool bRet1 = m_ImagerPrx->GetBasecallFlowcellConsumptionDiskSpace(consumptionValue);
        bool bRet2 = m_ImagerPrx->GetBasecallLocalDiskSpace(availableValue);
        return bRet1 && bRet2;
    }
    CATCHIMAGEREXCEPTION("GetDiskSpace");
}

ImagerParaType::ImagerSetupPara ImagerControlMgrIPCClient::SwitchImagerSetupData(const BGI::RPC::ImagerSetupInfo& dataOri)
{
    ImagerParaType::ImagerSetupPara data;

    data.SlideID = QString::fromStdString(dataOri.SlideID);
    data.ScriptName = QString::fromStdString(dataOri.ScriptName);
    data.ExperimentName = QString::fromStdString(dataOri.ExperimentName);
    data.UserName = QString::fromStdString(dataOri.UserName);
    data.ImagerID = dataOri.ImagerID;

    data.ZAFOffset = dataOri.ZAFOffset;
    data.IsCalAFOffset = dataOri.IsCalAFOffset;

    data.Read1Len = dataOri.Read1Len;
    data.Read2Len = dataOri.Read2Len;
    data.Barcode1Len = dataOri.Barcode1Len;
    data.Barcode2Len = dataOri.Barcode2Len;

    data.Reference = QString::fromStdString(dataOri.Reference);
    data.BioBarcode = dataOri.BioBarcode;
    
    data.TotalCycle = dataOri.TotalCycle;
    data.BiochemCycle = dataOri.BiochemCycle;
    data.ImagerCycle = dataOri.ImagerCycle;

    data.TolerancePattern = dataOri.TolerancePattern;

    data.sequencingType = QString::fromStdString(dataOri.sequencingType);

    data.FqBarcode1Length = dataOri.FqBarcode1Length;
    data.FqBarcode1StartPos = dataOri.FqBarcode1StartPos;
    data.FqSpeciesMismatch = dataOri.FqSpeciesMismatch;
    data.FqBarcode2Length = dataOri.FqBarcode2Length;
    data.FqBarcode2StartPos = dataOri.FqBarcode2StartPos;
    data.FqSpeciesMismatch2 = dataOri.FqSpeciesMismatch2;

    data.FilterMode = dataOri.FilterMode;
    data.WriteFastqMode = dataOri.WriteFastqMode;
    data.BarcodeReverse = dataOri.BarcodeReverse;

    return data;
}

BGI::RPC::ImagerSetupInfo ImagerControlMgrIPCClient::SwitchImagerSetupData(const ImagerParaType::ImagerSetupPara& dataOri)
{
    BGI::RPC::ImagerSetupInfo data;

    data.SlideID = dataOri.SlideID.toStdString();
    data.ScriptName = dataOri.ScriptName.toStdString();
    data.ExperimentName = dataOri.ExperimentName.toStdString();
    data.UserName = dataOri.UserName.toStdString();
    data.ImagerID = dataOri.ImagerID;

    data.ZAFOffset = dataOri.ZAFOffset;
    data.IsCalAFOffset = dataOri.IsCalAFOffset;

    data.Read1Len = dataOri.Read1Len;
    data.Read2Len = dataOri.Read2Len;
    data.Barcode1Len = dataOri.Barcode1Len;
    data.Barcode2Len = dataOri.Barcode2Len;

    data.Reference = dataOri.Reference.toStdString();
    data.BioBarcode = dataOri.BioBarcode;

    data.TotalCycle = dataOri.TotalCycle;
    data.BiochemCycle = dataOri.BiochemCycle;
    data.ImagerCycle = dataOri.ImagerCycle;

    data.TolerancePattern = dataOri.TolerancePattern;

    data.sequencingType = dataOri.sequencingType.toStdString();

    data.FqBarcode1Length = dataOri.FqBarcode1Length;
    data.FqBarcode1StartPos = dataOri.FqBarcode1StartPos;
    data.FqSpeciesMismatch = dataOri.FqSpeciesMismatch;
    data.FqBarcode2Length = dataOri.FqBarcode2Length;
    data.FqBarcode2StartPos = dataOri.FqBarcode2StartPos;
    data.FqSpeciesMismatch2 = dataOri.FqSpeciesMismatch2;

    data.FilterMode = dataOri.FilterMode;
    data.WriteFastqMode = dataOri.WriteFastqMode;
    data.BarcodeReverse = dataOri.BarcodeReverse;

    return data;
}
