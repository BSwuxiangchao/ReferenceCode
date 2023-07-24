// Copyright 2016 Complete Genomics, Inc.  All Rights Reserved.
// Confidential and proprietary works of Complete Genomics, Inc.

#include "./Identity.ice"
#include "./StageRunMgrIPC.ice"

module BGI {
	module RPC {

		enum BasecallConnectState
		{
			bcsConnect,
			bcsDisconnect
		};

		enum ImagerServerState
		{
			disIdle,
			disReady,
			disImagerRun,
			disScanRunning,
			disSetupRun,
			disWriteFq,
			disRunTask
		};

		enum ImagerState
		{
			imsDisconnect,
			imsIdle,
			imsReset,
			imsGoHome,
			imsGoLoadingPos,
			imsGoWashPos,
			imsSwitchVacuum,
			imsPreInjectSRE,
			imsPreProcessing,
			imsSlideLeavedImager,
			imsStartRun,
			imsReady,
			imsScanning,
			imsSelfCheck
		};
		
		struct ImagerRunInfo
		{
			string SlideID;
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
		};

		//hefutao
		enum ImagerSlideLeavedReason
		{
		  LeavedReasonNormal,
		  LeavedReasonVacuumFailed,
		  LeavedReasonOthers
		};

	   ////DipSeq-->Imager error to clients
		enum ImagerErrorEnum 
		{
			None = 0,
		    
			BasecallConnect = 2000,
			BasecallDisconnect = 2001,
			ReimagingError = 2003,
			BasecallBusy = 2004,

			BasecallLostFovList = 2010,
			BasecallNotSetupRun = 2011,
			BasecallReadyToAcqTimeout = 2012,
			BasecallIceEXCEP = 2013,
			ServerStdEXCEP = 2014,
			ServerFatalEXCEP = 2015,
			ServerStatusEXCEP = 2016,
			ServerSequenceEXCEP = 2017,
			StageLibEXCEP = 2018,
			
			StageNotReady = 2050,
			CameraNotReady = 2051,

			ParamsError = 2100,
			DllError = 2101,

			Findlocation = 2201,
			ThetaCorrection = 2202,
			DetectBubbles = 2203,
			StageError = 2204,
			CameraError = 2205,
			LaserError = 2206,
			IOError = 2207,
			XLP6KPumpError = 2208,
			BasecallingError = 2209,
			ImagerStop = 2210,
			SeqDataValid = 2211,
			ConfigDataError = 2212,
			ScriptError = 2213,
			ChipPositionError = 2214,
			ScanRowColError = 2215,
			VacuumError = 2216,

			PrePrimeError = 2220,
			AutoFocusError = 2221,
			MemoryAllocationError = 2222,
			MemoryAllocationInvalid = 2223,
			StageParamError = 2224,
			HardWareSoloistError = 2225,
			HardWareAcsError = 2226,
			HardLiquidPumpError = 2227,
			SoftWareCamAppError = 2228,
			
			SquirtError = 2303,
			SpreadError = 2304,
			
			CameraErrorEXCEP = 3000,
			InsuffDiskSpaceEXCEP = 3001,
			InsuffMemSpaceEXCEP = 3002,
			RepeatedRunIdEXCEP = 3003,
			RunInfoErrorEXCEP = 3004,
			VariableErrorEXCEP = 3005,
			CycleErrorEXCEP = 3006,
			InvalidChipTypeEXCEP = 3007,
			GPUErrorEXCEP = 3100,
			SimuAcquireErrorEXCEP = 3101,
			StateErrorEXCEP = 3102,
			
			UnknownError = 9999,
			
			
			// For ChangChun BGI
			
			//软件错误
			ControllerOffLineError = 25000,
			CamOffLineError = 25010,
			ImagerStateError = 25020,					//OpticEngine status Error
	
			//参数配置错误
			OpticEngineParamError = 26000,				// Param Error
			SeverParamError = 26100,

			//吸附错误
			StartRunVaccumError = 21610,				//StartRun:Vaccum Error
			ScanRowsVaccumError = 22110,				//ScanRows:Vaccum Error
	
			//拍照准备
			SreFloodingSoloistError = 21010,   			//StartRun:SreFlooding:Soloist Error
			SreFloodingACSError = 21020,   				//StartRun:SreFlooding:Soloist ErrorACS Error
			SreFloodingLiquidPumpError = 21030,			//StartRun:SreFlooding LiquidPump DC Switch

			SreSpreadingSoloistError = 21110,			//StartRun:Spreading:Soloist Error
			SreSpreadingACSError = 21120,     			//StartRun:Spreading:ACS Error

			LafOnSoloistError = 21210,					//StartRun:LafOn:Soloist Error

			RegistrationSoloistError = 21310,			//StartRun: Registration:Soloist Error 
			RegistrationACSError = 21320,				//StartRun:Registration:ACS Error
			RegistrationThetaError = 21302,				//StartRun:Registration:Theta Error
			RegistrationImgCalError = 21303,    		//StartRun:Registration:Caloffset Error
			RegistrationImgTimeoutError = 21352,		//StartRun:Registration:Get Picuture Error
	
			RegistrationSlidePosError = 21304,  		//StartRun:Registration:Read SlideID Error

			StartCycleACSError = 21420,       			//StartRun:StartCycle:ACS Error

			EndprocessCycleSoloistError = 21510,		//StartRun:EndprocessCycle:Soloist Error
			EndprocessCycleACSError = 21520, 			//StartRun:EndprocessCycle: ACS Error
	
			ReadyScanVaccumError=21640,					//StartRun:ReadVacuumValue:Vaccum Pump Error
	
			//扫描
			ScanRowSoloistError = 22010,	    		//ScanRow:Soloist Error(Recoverable)
			ScanRowSoloistDestroyError=22011,			//ScanRow:Soloist Destroy Error(Unrecoverable)
			ScanRowACSError = 22020,       				//ScanRow:ACS Error
			ScaningVaccumError = 22140,                 //ScanRow:Vaccum Pump Error

			//光机维护
			GoHomeSoloistError = 23010,     			//Command:GoHome:Soloist Error
			GoHomeASCError = 23020,       				//Command:GoHome: ACS Error   

			GotoLoadSoloistError = 23110,     			//Command:GotoLoad:Soloist Error
			GotoLoadASCError = 23120,		 			//Command:GotoLoad:ACS Error
			GotoLoadLogicError = 23154,         		//Command:GotoLoad:Logic Error

			VaccumOnOFFError = 23540 ,					//Command:Vaccum:Vaccum Pump Error

			WaterPipeCleanSoloistError = 23210,			//Command: WaterPipeClean:Soloist Error
			WaterPipeCleanACSError = 23220,          	//Command: WaterPipeClean:ACS Error
			WaterPipeCleanLiquidPumpError = 23230,		//Command: WaterPipeClean: Liquid Pump DC Switch

			CleanObjectiveLensSoloistError = 23310,		//Command:CleanDevie:Soloist Error
			CleanObjectiveLensACSError = 23320,		  	//Command:CleanDevie ACS Error 
			CleanObjectiveLensLiquidPumpError = 23330,	//Command:Clean Devie Error                      
			
			//LiquidPump DC Switch
			SREPipeCleanSoloistError = 23710,			//Command:SREPipeClean:Soloist Error
			SREPipeCleanACSError = 23720,				//Command:SREPipeClean:ACS Error 
			SREPipeCleanLiquidPumpError=23730,			//Command:SREPipeClean: Liquid Pump DC Switch
												  
			PreHeatingSoloistError = 23410,				//Command:PreHeating:Soloist Error
			PreHeatingACSError = 23420,        			//Command:PreHeating:ACS Error
			PreHeatingLogicError=23404,

			WasteLiquidError = 23660					//Command:WasterLiquid:Waster Pump Error
		};

		enum ImagerDataEnum
		{
			Vacuum,
			FlowcellInHolderTimeout
		};

		enum EndCycleProcessModeEnum
		{
			R1R2None = 0,
			R1Only = 1,
			R2Only = 2,
			R1R2Both = 3
		};

		enum SequencingTypeEnum
        {
            PE100 = 0,
            PE150 = 1,
            stLFR = 2,
            SE100 = 3,
            SE150 = 4,
			UMIDBarcode = 5,
			QuarterBarcode1 = 6,
			PreProcessing = 8,
			iDropATAC = 9,
			iDropRNA = 10,
			iDropRNANew = 11,
			Custom = 99
        };
	
		enum PreProcessingEnum
		{
			WasteLiquidExtract = 1,
			WaterPipeClean = 2,
			PreHeating = 3,
			ObjectiveLensClean = 4,
			PreSpreadingSRE = 5,
			SelfChecking = 6,
		};

		sequence<double> ExposureTimeArray;
		sequence<double> ExposureFactorArray;
		struct ImagerConfigData
		{
			int Lane;
			string Reference;

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
			ExposureTimeArray ExposureTime;
			ExposureFactorArray ExposureFactors;
			string SaveFovString;
			EndCycleProcessModeEnum EndCycleProcessMode;

			double VacuumSupplyThreshold;

			string ExtraSettings;
		};

		dictionary<string, string> SpeciesBarcodesDatas;
		struct ImagerSetupInfo
		{
			string SlideID;
			string ScriptName;			
			string ExperimentName;
			string UserName;
			int ImagerID;

			int ZAFOffset;
			bool IsCalAFOffset;

			int Read1Len;
			int Read2Len;
			int Barcode1Len;
			int Barcode2Len;

			string Reference;
			SpeciesBarcodesDatas BioBarcode;

			int TotalCycle;
			int BiochemCycle;
			int ImagerCycle;

			int TolerancePattern;

			string sequencingType;

			int FqBarcode1Length;
			int FqBarcode1StartPos;
			int FqSpeciesMismatch;
			int FqBarcode2Length;
			int FqBarcode2StartPos;	
			int FqSpeciesMismatch2;

			int FilterMode;			//0:no filte 	1：filte
			int WriteFastqMode; 		//default 0=fovC*fovR fovs; 1=signal fov; 2=fovList fovs;			
			int BarcodeReverse;		//Reverse
		};
		
		enum BasecallStatusEnum
		{
            INACTIVE = 0,
            ACTIVE,
            ACQUIREWAIT,
            ACQUIRING,
            ACQUIRECOMPLETE,
            PROCESSINGCOMPLETE,
            DEAD
		};

		struct RunInformation
		{
			BasecallStatusEnum Status;
			bool Reseq;
			string SlideID;
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
		};
					
		sequence<RunInformation> RunInformationArray;			
		interface ImagerControlMgrIPC extends StageRunMgrIPC
		{
			bool ImagerSetup(ImagerSetupInfo info);
			bool Reset();
			bool SetupRun(ImagerSetupInfo info);//linlilong
			bool ReleaseRun(ImagerSetupInfo info);//linlilong
			bool GoHome();
			bool GoLoadingPos();
			bool GoWashPos();
			bool IsAtLoadPosition();
			bool SwitchVacuum(bool valveStatus, bool pumpStatus);//linlilong
			bool GetVacuumStatus(double thresholdValueMin, double thresholdValueMax, int timeoutSec);//linlilong
			bool PreInjectSRE();

			bool PreProcessing(PreProcessingEnum type, int timeoutSec);
			bool ImagerConfigSet(ImagerConfigData config);
			ImagerConfigData ImagerConfigGet();	
			void SlideLeavedImager( ImagerSlideLeavedReason reason );//Hefutao	
			
			RunInformationArray GetAllRunInformations();
			void GetRunInformation(string slideID, out ImagerSetupInfo runInfo);
			bool GetBasecallInformation(string slideID);
			bool EndCycle(bool reSeq);
			
			bool StartRunScript(string scriptName);

			// add at 2020/01/12 by bxl
			bool GetBasecallFlowcellConsumptionDiskSpace(out double value);
			bool GetBasecallRemoteDiskSpace(out double value); //using sync.mode=1 && sync.uploadPath
			bool GetBasecallLocalDiskSpace(out double value); //using CalFilePath
			bool GetParam(string name, out string value);
			bool SetParam(string name, string value, bool saveToDisk);
			void GetVersions(out string bcsVer, out string sbcVer, out string imagerVer);
		};

		interface ImagerControlMgrIPCCallback extends StageRunMgrIPCCallback
		{
			void SendImagerError(ImagerErrorEnum error);
			void SendImagerRunInfo(ImagerRunInfo info);
			void SendImagerTaskEnd(ImagerSetupInfo info, ImagerErrorEnum error);
			void SendData(string key,string value);
			int  SendHeartbeat(int count);
			void SendStatus(BasecallConnectState StateBasecall, ImagerServerState StateServer, ImagerState StateImager);
		};
	};
};
