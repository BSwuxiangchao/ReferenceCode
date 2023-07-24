/*
* @Abrief UDP
* @note 多线程网络通讯
* @version 1.0
* @author mosongjin 2022-03-18
*/

#ifndef _BGI_UDP_SOCKET_H_
#define _BGI_UDP_SOCKET_H_


#include <qobject.h>
#include <qthread.h>
#include "BgiLock.h"
#include <QtNetwork/qudpsocket.h>


class  BgiUdpSocket : public QObject
{
	Q_OBJECT
public:
	BgiUdpSocket();
	BgiUdpSocket(QString ip, int port);
	virtual ~BgiUdpSocket();

	void SetUdpSocketPara(QString ip, int port);

	bool Open();
	bool Close();
	void Wirte(QByteArray buf);
	void Read(QByteArray& buf);

signals:
	void SignalConnect();
	void SendBuf(QByteArray buf);
	void WirteBuf(QByteArray buf);

private slots:
	void SlotConnect();
	void SlotWriteData(QByteArray buf);
	void SlotConnectSuc();
	void SlotConnectFail();
	void SlotReadData();
	void SlotShowError(QAbstractSocket::SocketError socket_error);

private:
	QThread* m_pThread;
	QUdpSocket* m_pUdpSocket;
	
	bool m_bUdpConnect;
	QString m_IpAdress;
	int m_iNetPort;

	QByteArray m_ReadBuf;

	BgiLock m_BgiLock;

};

#endif 


