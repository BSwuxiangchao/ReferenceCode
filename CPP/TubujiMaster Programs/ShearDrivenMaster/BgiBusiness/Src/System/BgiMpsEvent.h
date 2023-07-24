/*
* @Abrief 自定义事件
* @note 高通量合成仪自定义事件，用作对象间的自定义事件信息传递
* @version 1.0
* @author mosongjin 2021-09-29
*/

#ifndef _BGI_MPS_EVENT_H_
#define _BGI_MPS_EVENT_H_

#include "BusinessExternDef.h"
#include <QEvent>
#include <QVariant>
#include <QVariantHash>
#include <qstring.h>

// UI事件类型定义
#define BGI_MPS_EVENT_TYPE (QEvent::User + 10000)

class BGIBUSINE_CLASS_EXPORT BgiMpsEvent : public QEvent
{
public:
	BgiMpsEvent();
	BgiMpsEvent(const QString& key);
	BgiMpsEvent(const QString& key, const QVariantHash& params);
	BgiMpsEvent(const QString& key, const QString& paramKey, const QVariant& var);
	virtual ~BgiMpsEvent();

public:
	QString Key();
	void SetKey(const QString& key);

	QString From();
	void SetFrom(const QString& from);

	QString To();
	void SetTo(const QString& to);

	QString Action();
	void SetAction(const QString& action);

	const QVariantHash& Params();
	void SetParams(const QVariantHash& params);
	void AddParams(const QString& key, const QVariant& var);

	bool BeHandled();
	void AddHandled();

	static bool IsBgiMpsEvent(QEvent* event);

public:
	unsigned short HandledCount;
private:
	QString m_key;
	QString m_from;
	QString m_to;
	QString m_action;
	QVariantHash m_params;

	static QMap<QString, QEvent::Type> m_BgiMpsEventIdMap;
};



#endif//_BGI_MPS_EVENT_H_
