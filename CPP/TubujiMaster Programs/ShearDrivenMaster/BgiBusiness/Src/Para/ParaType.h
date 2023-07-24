/*
* @Abrief 常规的自定义参数
* @note 常规的自定义参数，用基本类型不好描述的参数
* @version 1.0
* @author mosongjin 2021-07-27
*/


#ifndef _GENERAL_PARA_TYPE_H_
#define _GENERAL_PARA_TYPE_H_

#include <string>
#include <math.h>
#include <vector>
#include <QMetaType>

//using namespace std;

enum InfoType
{
    All,			//所有
    GeneralType,	//通用
    RobotType,		//机械手
    CoaterType,		//涂布机
    ImagerType,		//光机
};

class GeneralParaType
{
public:
    //开关参数
    class SwitchType {
    public:
        enum Enum {
            Off,
            On
        };
    };


    //范围参数
    template<class T> struct Range
    {
        T Start;
        T End;

        bool operator==(const Range& obj) const {
            if ((typeid(T) == typeid(float)) || (typeid(T) == typeid(double)))
            {
                return (fabs(Start - (float)obj.Start) < 0.000001) && (fabs(End - (float)obj.End) < 0.000001);
            }
            else
            {
                return ((Start == obj.Start) && (End == obj.End));
            }
        }
    };

    enum UserType
    {
        Operator,
        Maintainer,
        Admin,
    };
    // 用户信息
    struct UserInfo
    {
        QString strName;
        UserType userType;
        QString password;
        UserInfo()
        {
            strName = "";
            userType = Operator;
            password = "";
        }
    };

    //工作状态
    struct WorkStatus
    {
        bool bNotStart;            //未开始
        bool bEnd;              //结束
        bool bPause;            //暂停
        bool bAbnormal;         //异常
        bool bManualEnd;        
        bool bAuto;

        WorkStatus()
        {
            bNotStart = true;
            bEnd = false;
            bPause = false;
            bAbnormal = false;
			bManualEnd = false;
            bAuto = false;
          
        }

        WorkStatus& operator=(const WorkStatus& element)
        {
            if (this != &element)
            {
				this->bNotStart = element.bNotStart;
                this->bEnd = element.bEnd;
                this->bPause = element.bPause;
                this->bAbnormal = element.bAbnormal;
				this->bManualEnd = element.bManualEnd;
                this->bAuto = element.bAuto;
                
            }
            return *this;
        }

        bool operator==(const WorkStatus& obj) const
        {
            if (this == &obj)
            {
                return true;
            }
            else
            {
                if (obj.bEnd != this->bEnd
					|| obj.bNotStart != this->bNotStart
                    || obj.bPause != this->bPause
					|| obj.bManualEnd != this->bManualEnd
                    || obj.bAbnormal != this->bAbnormal
                    || obj.bAuto != this->bAuto)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    };

    struct SequencingTableDesc
    {
        int iSerial;
        int iBiochemistryTimes;
        int iPhotographTime;
        int iR1;
        int iR2;
        int iB1;
        int iB2;
        int iAll;
        QString strChipNo;
        QString strImagerNo;
        QString strSequencingMode;
        QString strReagentKit;
        QString strReference;
        QString strBarCodePah;

        SequencingTableDesc()
        {
            iSerial = 0;
            iBiochemistryTimes = 0;
            iPhotographTime = 0;
            iR1 = 0;
            iR2 = 0;
            iB1 = 0;
            iB2 = 0;
            iAll = 0;
            strChipNo = "";
            strImagerNo = "";
            strSequencingMode = "";
            strReagentKit = "";
            strReference = "";
            strBarCodePah = "";
        }

        bool operator== (const SequencingTableDesc& obj)const
        {
            if (this->iSerial == obj.iSerial && 
                this->iBiochemistryTimes == obj.iBiochemistryTimes && 
                this->iPhotographTime == obj.iPhotographTime && 
                this->iR1 == obj.iR1 && 
                this->iR2 == obj.iR2 && 
                this->iB1 == obj.iB1 && 
                this->iB2 == obj.iB2 && 
                this->iAll == obj.iAll && 
                this->strChipNo == obj.strChipNo && 
                this->strImagerNo == obj.strImagerNo && 
                this->strSequencingMode == obj.strSequencingMode &&
                this->strReagentKit == obj.strReagentKit &&
                this->strReference == obj.strReference &&
                this->strBarCodePah == obj.strBarCodePah)
                return true;
            return false;
        }

        SequencingTableDesc operator= (const SequencingTableDesc& obj)
        {
            if (*this == obj)
                return *this;
            this->iSerial = obj.iSerial;
            this->iBiochemistryTimes = obj.iBiochemistryTimes;
            this->iPhotographTime = obj.iPhotographTime;
            this->iR1 = obj.iR1;
            this->iR2 = obj.iR2;
            this->iB1 = obj.iB1;
            this->iB2 = obj.iB2;
            this->iAll = obj.iAll;
            this->strChipNo = obj.strChipNo;
            this->strImagerNo = obj.strImagerNo;
            this->strSequencingMode = obj.strSequencingMode;
            this->strReagentKit = obj.strReagentKit;
            this->strReference = obj.strReference;
            this->strBarCodePah = obj.strBarCodePah;
            return *this;
        }

    };

    struct SqlCfgData
    {
        QString strBatchID;
        QString strBarCode;
        SequencingTableDesc strTableDesc;
        SqlCfgData()
        {
            strBatchID = "";
            strBarCode = "";
        }

    };

    struct sqlErrorData
    {
        QString ErrTime;
        QString Errtype;
        QString ErrContent;
        
        sqlErrorData()
        {
            ErrTime = "";
            Errtype = "";
            ErrContent = "";
        }
    };
/*
    //整数级可调参数
    template<class T> struct LevelValue
    {
        int Level;
        std::vector<T> List;

        bool operator==(const LevelValue& obj) const {
            if ((Level != obj.Level) || (List != obj.List))
                return false;
            else
                return true;
        }
    };
*/
};


Q_DECLARE_METATYPE(GeneralParaType::WorkStatus);
Q_DECLARE_METATYPE(GeneralParaType::SequencingTableDesc);

//整数级可调参数
template<class T> struct LevelValue
{
    int Level;
    std::vector<T> List;

    bool operator==(const LevelValue& obj) const {
        if ((Level != obj.Level) || (List != obj.List))
            return false;
        else
            return true;
    }
};



#endif
