#include "LocalDataWidgeet.h"
#include <QRegularExpressionValidator>
#include "OperateSqliteConfig.h"
#include "PortConfigDoc.h"
#include "OperatePromptInfo.h"
#include "PortConfigDoc.h"
#include "DeviceDetailCfg.h"
#include "OpeStyleSheet.h"
#include "BgiMpsEvent.h"
#include "BgiMasterApp.h"
#include "RobotOutputParaUtil.h"
#include "InfoPromptWidget.h"
#include "CommonColors.h"

LocalDataWidgeet::LocalDataWidgeet(QWidget* parent)
	: QMainWindow(parent)
	, m_bNewCreate(false)
	, m_bShowAllBatchID(false)
	, m_bAlreadyLoadAllCfg(false)
	, m_bAutoChangeBatchID(false)
	, m_strLastBatchID("")
	, m_bScanCode(false)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	Qt::WindowFlags windowFlag = windowFlags() & ~Qt::WindowMaximizeButtonHint;
	setWindowFlags(windowFlag);
	LimitLineEditInput();
	InitWidgets();

	//TestCode();
}

LocalDataWidgeet::~LocalDataWidgeet()
{}

void LocalDataWidgeet::ReloadBarcodeCtrDetail()
{
	QString tmpBarcode = ui.comboBoxBarcode->currentText();
	
	ui.comboBoxBarcode->clear();
	ui.comboBoxBatchID->clear();
	QStringList lastBatchIDList;
	for (GeneralParaType::SqlCfgData sqlCfgData : m_vecSqlCfgData)
	{
		ui.comboBoxBarcode->addItem(sqlCfgData.strBarCode);
		if (!lastBatchIDList.contains( sqlCfgData.strBatchID))
		{
			ui.comboBoxBatchID->addItem(sqlCfgData.strBatchID);
			lastBatchIDList.append(sqlCfgData.strBatchID);
		}
	}
	if (m_bShowAllBatchID)
	{
		ui.comboBoxBatchID->addItem("Show All");
		m_bShowAllBatchID = false;
	}
	if(lastBatchIDList.size()>0)
		m_strLastBatchID = lastBatchIDList.at(0);
	if (m_bScanCode)
	{
		ui.comboBoxBarcode->setCurrentText(tmpBarcode);
		m_bScanCode = false;
	}
	//加载当前barcode的配置到界面
	On_Barcode_Changed();
}

void LocalDataWidgeet::LoadSqlCfg()
{
	m_vecSqlCfgData.clear();
	if (!OperateSqliteConfig::Instance()->QureyAllCfgData(m_vecSqlCfgData))
		return;
	ReloadBarcodeCtrDetail();
	m_bAlreadyLoadAllCfg = true;
}

void LocalDataWidgeet::CreateToolBar()
{
	m_mainToolBar = new QToolBar(this);
	QWidget* spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_mainToolBar->addWidget(spacer);
	addToolBar(Qt::TopToolBarArea, m_mainToolBar);
	QAction* about = new QAction("重新加载本地配置", this);
	about->setObjectName("obj_Reload");
	about->setStatusTip("点击后重新加载修改后的本地配置文件");

	/*QList<QAction*> actionList = m_mainToolBar->actions();
	for (QAction* act : actionList)
	{
		if (act->objectName() == "obj_Reload")
		{
			act->setWhatsThis("点击后重新加载修改后的本地配置文件");
		}
	}*/
	m_mainToolBar->addSeparator();
	QFont* font = new QFont();
	font->setPixelSize(16);
	m_mainToolBar->setFont(*font);
	m_mainToolBar->addAction(about);
	m_mainToolBar->addSeparator();
	connect(about, &QAction::triggered, this, [this] {on_LocalDataWidgeet_customContextMenuRequested(); });
	//m_mainToolBar->setAllowedAreas(Qt::RightToolBarArea);
	m_mainToolBar->setMovable(false);

}

void LocalDataWidgeet::on_LocalDataWidgeet_customContextMenuRequested()
{
	PortConfigDoc::Instance()->ReadConfigIni();
	DeviceDetailCfg::Instance()->ReadConfigIni();
	emit SignalUpdatePosCombox();
}

void LocalDataWidgeet::SetStyleSheet()
{
	// 设置样式
	OpeStyleSheet styleSheet;
	QString strStyle;
	bool bRet = styleSheet.ReadUICtrlsStyleSheetByFileName(strStyle, "QPushbutton-NofontNoRadius");
	if (bRet)
	{
		ui.pushButtonCreateBar->setStyleSheet(strStyle);
		ui.pushButton_DelBarcode->setStyleSheet(strStyle);
		ui.pushButton_FindBarcode->setStyleSheet(strStyle);
		ui.pushButtonSaveCfg->setStyleSheet(strStyle);
		ui.pushButton_ScanCode->setStyleSheet(strStyle);
	}
}

void LocalDataWidgeet::InitWidgets()
{
	CreateToolBar();
	SetStyleSheet();
	OperateSqliteConfig::Instance()->CreateCfgTable();
	LoadSqlCfg();
	
	connect(ui.pushButtonCreateBar, SIGNAL(clicked()), this, SLOT(On_Create_Clicked()));
	connect(ui.pushButton_DelBarcode, SIGNAL(clicked()), this, SLOT(On_Delete_Clicked()));
	connect(ui.pushButton_FindBarcode, SIGNAL(clicked()), this, SLOT(On_SearchFor_Clicked()));
	connect(ui.comboBoxBarcode, &QComboBox::activated, this, &LocalDataWidgeet::On_Barcode_Changed);
	connect(ui.comboBoxBatchID, &QComboBox::activated, this, &LocalDataWidgeet::On_BatchID_Changed);
	connect(ui.pushButtonSaveCfg, SIGNAL(clicked()), this, SLOT(On_SaveCfg_Clicked()));
	connect(ui.pushButton_ScanCode, SIGNAL(clicked()), this, SLOT(On_ScanCode_Clicked()));

	if (m_vecSqlCfgData.size() <= 0)
		return;
	//查询一条最近时间保存的记录，显示到配置界面
	GeneralParaType::SqlCfgData sqlCfgData;
	OperateSqliteConfig::Instance()->QueryRecentCfgData(sqlCfgData);
	SetUIControlText(sqlCfgData);
}

void LocalDataWidgeet::ClearUIControlText()
{
	//ui.comboBoxBarcode->setCurrentText("");
	//ui.comboBoxBatchID->setCurrentText("");
	ui.lineEditRead1Length->setText("");
	ui.lineEditRead2Length->setText("");
	ui.lineEditBarcode1Length->setText("");
	ui.lineEditBarcode2Length->setText("");
	ui.lineEditTotalCycle->setText("");
	ui.lineEditBiochemCycle->setText("");
	ui.lineEditImagerCycle->setText("");
	ui.lineEditReagentKit->setText("");
	ui.lineEditBarcodePatn->setText("");
	ui.comboBoxReference->setCurrentText("");
	ui.comboBoxSequencingType->setCurrentText("");
	ui.comboBoxDevice->setCurrentText("");
}

void LocalDataWidgeet::SetUIControlText(GeneralParaType::SqlCfgData sqlCfgData)
{
	if (sqlCfgData.strBatchID != m_strLastBatchID)
	{
		m_bAutoChangeBatchID = true;
		m_strLastBatchID = sqlCfgData.strBatchID;
	}

	ui.comboBoxBarcode->setCurrentText(sqlCfgData.strBarCode);
	ui.comboBoxBatchID->setCurrentText(sqlCfgData.strBatchID);
	ui.lineEditRead1Length->setText(QString("%1").arg(sqlCfgData.strTableDesc.iR1));
	ui.lineEditRead2Length->setText(QString("%1").arg(sqlCfgData.strTableDesc.iR2));
	ui.lineEditBarcode1Length->setText(QString("%1").arg(sqlCfgData.strTableDesc.iB1));
	ui.lineEditBarcode2Length->setText(QString("%1").arg(sqlCfgData.strTableDesc.iB2));
	ui.lineEditTotalCycle->setText(QString("%1").arg(sqlCfgData.strTableDesc.iAll));
	ui.lineEditBiochemCycle->setText(QString("%1").arg(sqlCfgData.strTableDesc.iBiochemistryTimes));
	ui.lineEditImagerCycle->setText(QString("%1").arg(sqlCfgData.strTableDesc.iPhotographTime));
	ui.lineEditReagentKit->setText(sqlCfgData.strTableDesc.strReagentKit);
	ui.lineEditBarcodePatn->setText(sqlCfgData.strTableDesc.strBarCodePah);
	ui.comboBoxReference->setCurrentText(sqlCfgData.strTableDesc.strReference);
	ui.comboBoxSequencingType->setCurrentText(sqlCfgData.strTableDesc.strSequencingMode);
	ui.comboBoxDevice->setCurrentText(sqlCfgData.strTableDesc.strImagerNo);
}

void LocalDataWidgeet::GetUIControlText(GeneralParaType::SqlCfgData & data)
{
	data.strBarCode = ui.comboBoxBarcode->currentText().trimmed();
	data.strBatchID = ui.comboBoxBatchID->currentText().trimmed();
	data.strTableDesc.iR1 = ui.lineEditRead1Length->text().toInt();
	data.strTableDesc.iR2 = ui.lineEditRead2Length->text().toInt();
	data.strTableDesc.iB1 = ui.lineEditBarcode1Length->text().toInt();
	data.strTableDesc.iB2 = ui.lineEditBarcode2Length->text().toInt();
	data.strTableDesc.iAll = ui.lineEditTotalCycle->text().toInt();
	data.strTableDesc.iBiochemistryTimes = ui.lineEditBiochemCycle->text().toInt();
	data.strTableDesc.iPhotographTime = ui.lineEditImagerCycle->text().toInt();
	data.strTableDesc.strReagentKit = ui.lineEditReagentKit->text();
	data.strTableDesc.strBarCodePah = ui.lineEditBarcodePatn->text();
	data.strTableDesc.strReference = ui.comboBoxReference->currentText();
	data.strTableDesc.strSequencingMode = ui.comboBoxSequencingType->currentText();
	data.strTableDesc.strImagerNo = ui.comboBoxDevice->currentText();
	data.strTableDesc.strChipNo = ui.comboBoxBarcode->currentText().trimmed();
}

void LocalDataWidgeet::LimitInputUnit(QString strPatern, QLineEdit* lineEdit, QComboBox* comboBox)
{
	QRegularExpression regExp(strPatern);
	QValidator* validator = new QRegularExpressionValidator(regExp, this);
	if (lineEdit != NULL)
		lineEdit->setValidator(validator);
	else if (comboBox != NULL)
		comboBox->setValidator(validator);
}

void LocalDataWidgeet::LimitLineEditInput()
{
	QString strPattern = "^\\+?[1-9][0-9]*$"; //非零正整数
	LimitInputUnit(strPattern, ui.lineEditRead1Length);
	LimitInputUnit(strPattern, ui.lineEditRead2Length);
	//LimitInputUnit(strPattern, NULL, ui.comboBoxBatchID);  //BatchID可以为字母
	//strPattern = "^\\+?[2-9][0-9]*$"; //大于等于2的整数
	
	strPattern = "^\\+?[0-9][0-9]*$"; //大于等于0的整数
	LimitInputUnit(strPattern, ui.lineEditBiochemCycle);
	LimitInputUnit(strPattern, ui.lineEditImagerCycle);
	LimitInputUnit(strPattern, ui.lineEditBarcode1Length);  //Barcode可以为0
	LimitInputUnit(strPattern, ui.lineEditBarcode2Length);

	connect(ui.lineEditRead1Length, &QLineEdit::textChanged, this, [=] {ChangeTotalLength(); });
	connect(ui.lineEditRead2Length, &QLineEdit::textChanged, this, [=] {ChangeTotalLength(); });
	connect(ui.lineEditBarcode1Length, &QLineEdit::textChanged, this, [=] {ChangeTotalLength(); });
	connect(ui.lineEditBarcode2Length, &QLineEdit::textChanged, this, [=] {ChangeTotalLength(); });
}

void LocalDataWidgeet::MsgBoxTool(QString strContext, int iType)
{
	_tagPromptData promptData;
	promptData.m_bModel = true;
	promptData.m_strTitle = "温馨提示";
	promptData.m_strContent = strContext;
	promptData.m_iBtnType = iType; // 无按钮
	promptData.m_colForeground = gc_colorYellow;
	OperatePromptInfo::AddPromptInfo(promptData);
}

void LocalDataWidgeet::ChangeTotalLength()
{
	int iTotal = ui.lineEditRead1Length->text().toInt() + ui.lineEditRead2Length->text().toInt() \
		+ ui.lineEditBarcode1Length->text().toInt() + ui.lineEditBarcode2Length->text().toInt();
	ui.lineEditTotalCycle->setText(QString("%1").arg(iTotal));
}

//判断芯片编号在数据库中是否已经存在
bool LocalDataWidgeet::IsBarcodeCfgExist()
{
	bool bExist = false;
	QString strBarCode = ui.comboBoxBarcode->currentText().trimmed();;
	QList<GeneralParaType::SqlCfgData> vecSqlCfgData;
	if (m_bAlreadyLoadAllCfg)
		vecSqlCfgData = m_vecSqlCfgData;
	else
		OperateSqliteConfig::Instance()->QureyAllCfgData(vecSqlCfgData);

	for (GeneralParaType::SqlCfgData iter : vecSqlCfgData)
	{
		if (iter.strBarCode == strBarCode)
		{
			bExist = true;
			break;
		}
	}
	return bExist;
}

bool LocalDataWidgeet::IsBarcodeAndBatchIDIllegal()
{
	QString strBarCode = ui.comboBoxBarcode->currentText();
	QString strBatchID = ui.comboBoxBatchID->currentText();
	if (strBarCode == gc_ScanErrTip || strBarCode == gc_RescanTip)
		return false;
	return true;
}

void LocalDataWidgeet::On_Create_Clicked()
{
	if (ui.comboBoxBarcode->currentText().trimmed() == "" || ui.comboBoxBatchID->currentText().trimmed() == "")
	{
		MsgBoxTool("Barcode or BatchID can not be Empty!!!", 1);
		return ;
	}
	if (IsBarcodeCfgExist())
	{
		MsgBoxTool("Barcode Already Exist !!!", 1);
		return;
	}
	if (!IsBarcodeAndBatchIDIllegal())
	{
		MsgBoxTool("Can't Create this illegal Code", 1);
		return;
	}
	GeneralParaType::SqlCfgData data;
	GetUIControlText(data);
	OperateSqliteConfig::Instance()->WriteCfgData(data);
	LoadSqlCfg();
}

void LocalDataWidgeet::On_Delete_Clicked()
{
	QString strBarCode = ui.comboBoxBarcode->currentText().trimmed();
	QString strBatchID = ui.comboBoxBatchID->currentText().trimmed();
	if (strBarCode == "" && strBatchID == "")
		return;
	
	InfoPromptWidget infoWidget;
	BUTTON_STATE ret = infoWidget.Show("删除确认", "确定要删除芯片信息吗？", true, 3, Qt::red, gc_colorLightBlue);
	if (BUTTON_STATE::SURE != ret)
	{
		return;
	}

	if (strBarCode == "" && strBatchID != "")
		OperateSqliteConfig::Instance()->DelteCfgDataBystrBatchID(strBatchID);
	else
		OperateSqliteConfig::Instance()->DelteCfgDataBystrBarcode(strBarCode);

	LoadSqlCfg();
	//界面设为默认值
	GeneralParaType::SqlCfgData sqlCfgData;
	SetUIControlText(sqlCfgData);
}

void LocalDataWidgeet::On_SearchFor_Clicked()
{
	if (ui.comboBoxBarcode->currentText().trimmed() == "" && ui.comboBoxBatchID->currentText().trimmed() == "")
		LoadSqlCfg();
	else
	{
		QString strBarCode = ui.comboBoxBarcode->currentText().trimmed();
		QString strBatchID = ui.comboBoxBatchID->currentText().trimmed();
		QList<GeneralParaType::SqlCfgData> vecSqlCfgData;
		OperateSqliteConfig::Instance()->QureyCfgDataByKeyWords(strBarCode, strBatchID, vecSqlCfgData);
		m_vecSqlCfgData = vecSqlCfgData;
		ReloadBarcodeCtrDetail();
		m_bAlreadyLoadAllCfg = false;
	}
}

void LocalDataWidgeet::On_Barcode_Changed()
{
	if (m_vecSqlCfgData.size() == 0)
	{
		ClearUIControlText();
		return;
	}
	GeneralParaType::SqlCfgData sqlCfgData = m_vecSqlCfgData.at(0);
	for (GeneralParaType::SqlCfgData iter : m_vecSqlCfgData)
	{
		if (iter.strBarCode == ui.comboBoxBarcode->currentText())
			sqlCfgData = iter;
	}
	SetUIControlText(sqlCfgData);
}

void LocalDataWidgeet::On_BatchID_Changed()
{
	if (m_bAutoChangeBatchID)
	{
		m_bAutoChangeBatchID = false;
		return;
	}

	//不存在则清空界面
	/*if (!IsBarcodeCfgExist())
	{
		ClearUIControlText();
		return;
	}*/
	QString strBatchID = ui.comboBoxBatchID->currentText().trimmed();
	if (strBatchID == "Show All")
	{
		LoadSqlCfg();
	}
	else
	{
		m_strLastBatchID = strBatchID;
		m_bShowAllBatchID = true;
		QList<GeneralParaType::SqlCfgData> vecSqlCfgData;
		OperateSqliteConfig::Instance()->QureyCfgDataByKeyWords("", strBatchID, vecSqlCfgData);
		m_vecSqlCfgData = vecSqlCfgData;
		ReloadBarcodeCtrDetail();
		m_bAlreadyLoadAllCfg = false;
	}
	if (m_vecSqlCfgData.size() > 0)
	{
		GeneralParaType::SqlCfgData sqlCfgData = m_vecSqlCfgData.at(0);
		SetUIControlText(sqlCfgData);
	}
}

void LocalDataWidgeet::On_SaveCfg_Clicked()
{
	if (!IsBarcodeCfgExist())
	{
		MsgBoxTool("芯片编号未在数据库查到，请先创建 !!!", 1);
		return;
	}
	GeneralParaType::SqlCfgData data;
	GetUIControlText(data);
	OperateSqliteConfig::Instance()->WriteCfgData(data);
	emit SignalSaveLocalcfg(data.strTableDesc);
	close();
}

void LocalDataWidgeet::On_ScanCode_Clicked()
{
	ui.pushButton_ScanCode->setEnabled(false);
	ui.comboBoxBarcode->setFocus();
	ui.comboBoxBarcode->setCurrentText("");
	RobotOutputParaUtil::SetScannerInfo("");
	BgiMpsEvent* p_event = new BgiMpsEvent("Driver_Event_Scanner");
	p_event->SetAction("StartReadCode");
	BgiMasterApp* app = BgiMasterApp::Instance();
	QApplication::postEvent(app, p_event);
	QString strCodeInfo = "";
	RobotOutputParaUtil::GetScannerInfo(strCodeInfo);
	ui.comboBoxBarcode->setCurrentText(strCodeInfo);
	ui.pushButton_ScanCode->setEnabled(true);
	//不存在则清空界面
	if (!IsBarcodeCfgExist())
	{
		ClearUIControlText();
		ui.comboBoxBatchID->setCurrentText("");
		return;
	}
	if (!m_bAlreadyLoadAllCfg)
	{
		m_bScanCode = true;
		LoadSqlCfg();
	}
	else
		On_Barcode_Changed();
}

void LocalDataWidgeet::SetSaveBtnState(bool bShow)
{
	ui.pushButtonSaveCfg->setEnabled(bShow);
}

void LocalDataWidgeet::SlotUpdateCycle(GeneralParaType::SequencingTableDesc tableDesc)
{
	//从数据库中更新
	m_vecSqlCfgData.clear();
	if (OperateSqliteConfig::Instance()->QureyAllCfgData(m_vecSqlCfgData))
	{
		m_bAlreadyLoadAllCfg = true;
		//如果更新的是当前芯片，刷新界面显示
		if (tableDesc.strChipNo == ui.comboBoxBarcode->currentText())
		{
			On_Barcode_Changed();
		}
	}
}

void LocalDataWidgeet::TestCode()
{
	GeneralParaType::SqlCfgData data1;

	OperateSqliteConfig::Instance()->QueryRecentCfgData(data1);

	data1.strBarCode = "bbbbb";
	data1.strBatchID = "PPefw";
	OperateSqliteConfig::Instance()->WriteCfgData(data1);

	QList<GeneralParaType::SqlCfgData> vecSqlCfgData;
	OperateSqliteConfig::Instance()->QureyAllCfgData(vecSqlCfgData);

	OperateSqliteConfig::Instance()->QureyCfgDataByKeyWords("aaaa", "", vecSqlCfgData);
	OperateSqliteConfig::Instance()->QureyCfgDataByKeyWords("aaaa", "bbbbb", vecSqlCfgData);
	OperateSqliteConfig::Instance()->QureyCfgDataByKeyWords("", "Pefw", vecSqlCfgData);

	//OperateSqliteConfig::Instance()->DelteCfgDataBystrBarcode("bbbbb");
}