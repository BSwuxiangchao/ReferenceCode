#pragma once

#include <QObject>

class StageRunMgrIPCClient  : public QObject
{
	Q_OBJECT

public:
	StageRunMgrIPCClient(QObject *parent);
	~StageRunMgrIPCClient();
};
