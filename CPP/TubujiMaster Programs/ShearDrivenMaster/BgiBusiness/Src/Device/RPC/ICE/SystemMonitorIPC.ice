// Copyright 2016 Complete Genomics, Inc.  All Rights Reserved.
// Confidential and proprietary works of Complete Genomics, Inc.

#include "./Identity.ice"
#include "./CommonIPC.ice"

module BGI {
	module RPC {

		enum AtomicValueTypeEnum
		{
			TypeDouble,
			TypeFloat,
			TypeInt,
			TypeLong,
			TypeString,
			TypeBool,
		};
		
		struct AtomicValue
		{
			string ValueName;
			AtomicValueTypeEnum ValueType;
			string MeasurementDT;
			double ValueD;
			float ValueF;
			int ValueI;
			long ValueL;
			string ValueS;
			bool ValueB;
			bool IsNull;
		};

		enum PowerOptionsEnum
		{
			ShutDown,
			Reboot,
		};

		interface SystemMonitorIPC {
			int GetHeartbeat();
			void PowerControl(PowerOptionsEnum powerOptions);
		};

		interface SystemMonitorIPCCallback
		{
			void AddClient(Ice::Identity ident);

			void SendAtomicValue(AtomicValue val);
		};
	};
};
