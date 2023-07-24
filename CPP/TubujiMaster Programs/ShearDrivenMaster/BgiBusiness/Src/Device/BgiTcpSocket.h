/*
* @Abrief TCP/IP
* @note 多线程网络通讯
* @version 1.0
* @author wuxiangchao 2023-02-20
*/

#ifndef _BGI_TCP_SOCKET_H_
#define _BGI_TCP_SOCKET_H_

#include <QColor>
#include <qobject.h>
#include <QtNetwork\qtcpsocket.h>
#include <qthread.h>
#include "BgiLock.h"


class  BgiTcpSocket : public QObject
{
	Q_OBJECT
public:
	BgiTcpSocket();
	BgiTcpSocket(QString ip, int port);
	virtual ~BgiTcpSocket();

	void SetTcpSocketPara(QString ip, int port);

	bool Open();
	bool Close();
	void Write(QByteArray buf);
	void DelayWrite(QByteArray buf);
	void Read(QByteArray& buf);
	void CleanBuf();
signals:
	void SignalConnect();
	void SendBuf(QByteArray buf);
	void WirteBuf(QByteArray buf);
	void SignalConnectStatus(bool bSuccess);

public slots:
	void SlotMasterCloseRobotConnect();

private slots:
	void SlotConnect();
	void SlotWriteData(QByteArray buf);
	void SlotConnectSuc();
	void SlotConnectFail();
	void SlotReadData();
	void SlotShowError(QAbstractSocket::SocketError socket_error);

private:
	QThread* m_pThread;
	QTcpSocket* m_pTcpSocket;

	bool m_bTcpConnect;
	QString m_IpAdress;
	int m_iNetPort;

	QByteArray m_ReadBuf;

	BgiLock m_BgiLock;
	int m_iConnectFailCount;
};

#endif 


