// Copyright 2016 Complete Genomics, Inc.  All Rights Reserved.
// Confidential and proprietary works of Complete Genomics, Inc.

#include "./Identity.ice"
#include "./CommonIPC.ice"

module BGI {
	module RPC {
		struct SoftwareVer
		{
			int Major;
			int Minor;
			int Revision;
			int Build;
		};

		sequence<string> Scriptlist;

		interface ZebraSchedulerIPC {
			int GetHeartbeat();
			SoftwareVer GetVersion();
			string GetInstrumentName();
			Scriptlist GetScriptNames();
			void OverrideKitRTBarcode(string barcode);
			void OverrideKitCBarcode(string barcode);
		};

		interface ZebraSchedulerIPCCallback
		{
			void AddClient(Ice::Identity ident);
		};
	};
};
