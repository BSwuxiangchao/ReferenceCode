// Copyright 2016 Complete Genomics, Inc.  All Rights Reserved.
// Confidential and proprietary works of Complete Genomics, Inc.

#include "./Identity.ice"
#include "./CommonIPC.ice"

module BGI {
	module RPC {
		enum ScriptStateEnum 
		{
			Idle,			// Stage is idle.  Nothing happening.
			SetupRun,		// New run is being setup.  Receiving run parameters from client.
			ReadyToRun,		// New run is ready to start.  All neccessary pareamters received and valid.
			ScriptRunning,  // Script is running.
			ScriptPausing,  // Script is Pausing.  Pause command received and script is still running.
			ScriptPaused,   // Script is Paused.  Script waiting for new command.
			ScriptStopping, // script is Stopping.  Stop command received and script is still running.
			Unknown,        // Default unknown state for coding purposes only.
		};

		enum ScriptMessageSeverityEnum
		{
			Header,
			Verbose,
			Info,
			Warning,
			Error
		};
		enum MotorTypeEnum
		{
			SequenceNeedle,
			WashNeedle,
			FlowcellLoader
		};

		// Structure to keep track of current and recent script runs
		struct ScriptRunInfo
		{
		    int ScriptRunId;        // Unique script run ID on this instrument
			string ScriptFilename;  // Full path and filename of this script
			int ParentScriptRunId;  // ScriptRunId of the parent script that started this script
			string StartDate;       // DateTime the script was started to run
			string EndDate;         // DateTime the script Ended
			string StatusStr;       // Current or most recent status of script
			bool IsSuccess;         // Did the script complete without errors
		};

		// List of all scripts run on system since startup including current
		["clr:generic:List"]
		sequence<ScriptRunInfo> ScriptRunInfoList;
		
		enum DeviceCheckTypeEnum
		{
			InitCheck,//check device status when device inited
			BeforeRunningCheck,//Check device status before running 
			RunningCheck,  //Check device status when running
			IdleCheck, //Check device status when idle,like maintenc
			OtherCheck
		};

		//service message information include alarm and log
		struct ServiceMessageInfo
		{
			int MessageId;
			string InstructmentId;
			string FlowCellId;
			string UserName;
			string SoftwareVersion;
			ScriptMessageSeverityEnum MessageType;	
			string Errorcode;
			string Message;
			string Datetime;
			bool IsPublic;
		};

		struct IpAddr
		{
		   string IP;
		   int Port;
		};

		struct ScriptMessageInfo
		{
			int MessageId;
			ScriptMessageSeverityEnum MessageSeverity;
			string MessageDateTime;
			string MessageStr;
		};

		sequence<ScriptMessageInfo> MessageArray;

		//Alarm message information collection
		["clr:generic:List"]
		sequence<ServiceMessageInfo> AlarmMessageList;

		//Log message information collection
		["clr:generic:List"]
		sequence<ServiceMessageInfo> LogMessageList;

		//Clear message list
		["clr:generic:List"]
		sequence<int> ClearAlarmMessageIds;

		["clr:generic:List"]
		sequence<string>  SequenceTypes;

		["clr:generic:List"]
		sequence<string>  DataList;

		// QC Data
		struct QCDataValue
		{
			string QCMetricName;
			int cycleNum;
			float QCValue;
		};

		sequence<QCDataValue> QCDataValues;

		struct LoginedInfo
		{
			bool IsSuccess;
			string UserRole;
		};
		
		dictionary<string,string> ExperimentInfo;
		dictionary<string,string> RunInfo;
		dictionary<string,string> ParaDict;

		// Interface for RPC calls to Service from GUI
		interface StageRunMgrIPC 
		{
		    // Get the StageRunMgr heartbeat
			int GetHeartbeat() throws BGIRemoteException;

			// GUI calls to Service to step through setting up for a script run and managing that run.			
			// Tell StageRunMgr to start receiving run information
			void StartSetup() throws BGIRemoteException;
			void SaveSelectedScriptFileName(string strFileName) throws BGIRemoteException;
			void ScanFlowcellBarcode() throws BGIRemoteException;
			void ScanReagentKitBarcode() throws BGIRemoteException;
			void OverrideFlowcellBarcode(string barcode) throws BGIRemoteException;
			void SendReagentKit(string reagentKit) throws BGIRemoteException;

			// Start the script
			void StartRun() throws BGIRemoteException;
			void PauseScript() throws BGIRemoteException;
			void ResumeScript() throws BGIRemoteException;
			void StopScript() throws BGIRemoteException;
			void AbortScript() throws BGIRemoteException;

			// Override the main script setup workflow (above) and run script now
			int RunScriptNow(string strFileName) throws BGIRemoteException;

			// Get Script Run Info
			ScriptRunInfo GetScriptRunInfo(int scriptRunId) throws BGIRemoteException;

			// GUI request for all Script Messages starting at the indicated Message ID.
			MessageArray GetScriptMessages(int startMessageId) throws BGIRemoteException;
			
			//Set up run experiment
			void SetupScriptFile(string msgType) throws BGIRemoteException;

			// QC Data
			QCDataValues GetQCValues(string qcMetricName, int startCycleNum) throws BGIRemoteException;

			//Send sample id (DNB id)
			void SendSampleId(string sampleId) throws BGIRemoteException;			

			ExperimentInfo GetExperimentInfo();

			// TODO: Why is this here?  Seems restrictive. Assumes one of each script file type? SA
			void DownloadScriptFile(out string workFlowScript,out string imageScript,out string biochemistryScript);

			void UIOperation(string message, string datetime,string errorcode);				

			//Get alarm messages
			AlarmMessageList GetAlarmMessages(int startMessageId);
			//Get log message
			LogMessageList GetLogMessages(int startMessageId);
			//Clear alarm message by id
			void ClearAlarmMessages(ClearAlarmMessageIds ids);
			//control the ReagentNeedle to down or up
			 void ReagentNeedle(bool UP);
			//Send experiment type
			void SendExperimentType(string experimentType);
			//set buzzer volume
			 void SetBuzzerVolume(int frenquency);
			 
			 SequenceTypes GetSequenceTypes(string sampleId);

			 bool SetClearData(DataList datas);
			 
			 void StartRunLength(int read1length,int read2length,int bardcodelength);

			 //setup run information: eg. read1Length,read2Length,barcodeLength,barcodePosition...
			 void SetupRunInfo(RunInfo runInfo);
			 void MotorMove(MotorTypeEnum motor, bool up);

			//TODO: The below interface are platform-independent.should remove to schedualer?
			//Authenicate
			LoginedInfo Authenicate(string userName, string password);
			// I18N data
			string GetLanguage();
			void SetLanguage(string language); //TODO if we don't change language dynamic,it will remove
			void SetBaseCallIpAddr(string ipAddr ,int port);
			void SetZLIMSIpAddr(string ipAddr ,int port);
			IpAddr GetZLIMSAddr();			 
			void SetCategoryParas(string category,ParaDict paraDict);
			string GetParaValue(string category,string paraName);
			void StartSelfCheck(DeviceCheckTypeEnum checkType);
		};

		// Callback interface for RPC calls from Service to all registered clients (GUI)
		interface StageRunMgrIPCCallback
		{
			// Used by client to send register for callbacks
			void AddClient(Ice::Identity ident);

			// Send from Service as needed
			void SendScriptName(string scriptName);

			// Action is for display in GUI only, and needs to be localized.
			// For some customers this will be generic like "Stage1" for IP security
			// but for other customers (internal) it may be details like Incorporation, MD, ETC.
			void SendActionName(string actionName);

			// This will send the entire Flowcell Barcode (all GS1 parts) to the GUI for display
			void SendFlowcellBarcode(string flowcellBarcode);

			// Current Cycle or Position number of the run
			void SendPositionNumber(int cycleNum);

			// Used to update the GUI progress
			void SendPcntDone(int pctDone);

			// Used to send the current State to the GUI for display purposes.
			// Note: This should be renamed as SendScriptState, not SendScriptStatus.
			void SendScriptStatus(ScriptStateEnum curState);

			// Send the most recent Script MessageId to the GUI so it knows it is out of date and
			// Can decide to request messages from Service as needed.
			void SendScriptMessageId(int messageId);

			//Send stage message to client
			void SendStageMessage(string msgType,string message);

			// Send one QC data value to the GUI for display
			void SendQCValue(QCDataValue qcValue);
			
			// Send QC data values to the GUI for display
			void SendQCValues(QCDataValues qcValues);

			// Send alarm message id
			void SendAlarmMessageId(int messageId);

			// Send log message id
			void SendLogMessageId(int messageId);

			// Send estimated remaining time to the GUI
			void SendRemainingTime(double remainingTimeMinute);

			void SendDataList(DataList datas);
		};
	};
};
