// Copyright 2016-2018 Complete Genomics, Inc.  All Rights Reserved.
// Confidential and proprietary works of Complete Genomics, Inc.


#include "./Identity.ice"
#include "./CommonIPC.ice"

module BGI 
{
    module BaseCallRPC 
    {
        enum CameraType
        {
            LumeneraCamera,
            CoaxlinkGrabber,
            SaperaLTCameraLink,
            Andor,
            CoaxEuresysViewworks
        };

        enum ScanDir
        {
            Forward,
            Reverse,
            LineIn
        };

        ["cs:attribute:System.Flags"]
        enum DownLoadResult
        {
            None				=0,
            CameraOneLostOne	=2,
            CameraOneLostTwo    =4,
            CameraTwoLostOne	=8,
            CameraTwoLostTwo	=16,
			CameraThreeLostOne	=32,
			CameraFourLostOne	=64,
            Completed			=256,
        };

        enum ImageTransferColor
        {			
            FourColor1, 
            FourColor2, 
            FourColor3, 
            FourColor4, 
            TwoColor1, 
            TwoColor2
        };

        ["cs:attribute:System.Flags"]
        enum ImageTransferType
        {
            Discard             =1,
            BaseCall            =2,
            SaveToDisk          =4,
            CoarseTheta         =8,
            TransferImage       =16,
            RegField            =32,
            ImgMetrics          =64,
            EngineerImage       =128
        };

        struct Point3D
        {
            double x;
            double y;
            double z;
        };

        class ImageInformation
        {
            ImageTransferType imgType;
            ImageTransferColor imgColor;
            int cameraIndex;
            bool isReceived;
            int index;
        };

        ["clr:generic:List"]
        sequence<ImageInformation> ImageList;

        struct FOVInformation
        {
            int row;
            int col;
            int fovTypeId;
            int fovQuadrantId;  
            //Point3D targetCoord;  
            //Point3D actualCoord;
            int imagesPerFOV;       // image count per fov
            ImageList images;

            //TODO add more field
        
        };

        ["clr:generic:List"]
        sequence<FOVInformation> FovList;

        struct CycleInfo
        {
            string slideId;
            int lane;
            int cycle;
            int rowNums;
            int colNums;
            string scanType;
            int imageBitPerPixel;	//bitPerPixel of saving tif image
            int fovTimeoutMs;
        };
        
        struct CameraSetting
        {
            int cameraIndex;
            int bitsPerPixel;
            float pixelSize;
            int xResolution;
            int yResolution;
            int xOffset;
            int yOffset;
            int exposureTime;
            float gain;
            float gamma;
            CameraType cameraType;
            int exposureNum;
            bool horizFlip;
            bool vertFlip;
            float blackLevel;
            bool useTDI;
            ScanDir scanDirection;
            float triggerRescalerRate;

            //for camera link only
            string serverName;
            int resourceIndex;
            string configFilePath;
        };

        struct RegResults
        {
            int regStatus;			// 0-good, 1-can't cal Center, 2-CoarseTheta result is bad, 3-can't cal del, 4-del result is bad
            float pitch;			// um
            float rotation;			// rad
            float residRotation;
            int npoint;
            float horizOffset;		// pixels
            float vertOffset;		// pixels
            int rowID;
            int colID;
            float horizDelSNR;
            float vertDelSNR;
        };

        struct ImageMetricsResults
        {
            float focusScore;
        };
        
        sequence<CameraSetting> CameraSettings;

        sequence<byte> ImageData;

        interface BaseCallImageMgrIPCCallback
        {
            // Used by client to send register for callbacks
            void AddClient(Ice::Identity ident);

            void SendHeartbeat(int count);

            //return download result
            void DownLoadComplete(DownLoadResult downLoadResult);
            
            //return image data to client for engineering UI 
            void SendImage(ImageData imgData, string imgInfoJSON);

            //Return the Registration Results
            void SendCoarseThetaResult(FOVInformation fovInfo, RegResults regResult);

            void SendImageMetrics(FOVInformation fovInfo, ImageMetricsResults imgMetrics);
            
        };

        interface BaseCallImageMgrIPC 
        {
            void SetCameraConfig(CameraSettings cameraSettings) 
                        throws BGI::RPC::BGIRemoteException;

            void CalibrateDarkFrame(CameraSettings cameraSettings, string slide, int cycleNum) 
                        throws BGI::RPC::BGIRemoteException;
            
            void UseDarkFrameCalibration(bool use)
                        throws BGI::RPC::BGIRemoteException;

            bool IsReadyToAcquire();
            void StartAcquire(FovList fovList);	
            void StopAcquire();
            void CompleteAcquire();

            void StartCycle(CycleInfo cycleInfo);
            void EndCycle();

			void SendHeartbeat(int count);
            string GetVersion();
        };

    };
};