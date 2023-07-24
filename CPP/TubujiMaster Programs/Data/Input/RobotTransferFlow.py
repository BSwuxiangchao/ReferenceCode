from ctypes import *
import os
from pickle import TRUE
import sys
import time

#点位起点是否需要校验
gc_needCheckPos=True
#起点校验是否成功
gc_StartPosCheckOk=False

# region 加载动态库
# str = os.path.join(sys.prefix, 'Library', 'Release','EpsonRobotLibrary.dll')
# funtest = cdll.LoadLibrary(str)
# 加载动态库
Robot = cdll.LoadLibrary('BgiBusiness.dll')


# endregion

##########################################################功能函数 开始 #############################################
def String2Char(strInfo):
    return strInfo.encode('utf-8')


def PrintRecordData(strInfo, iLevel):
    Record = Robot.PrintRecordData
    # Record.argTypes=[POINTER(c_char_p)]
    Record(String2Char(strInfo), iLevel)


def ReturnValue():
    print(Robot.ReturnValue())


def TestMsgBox():
    PrintRecordData("Python 中的调用", 1)
    Robot.On_OutputIO(1)
    Robot.ShowMsgBox()
    PrintRecordData("Python 中的调用2", 1)
    ret = Robot.ReturnValue()
    PrintRecordData("Python 中的调用3", 1)
    PrintRecordData('返回值 %s' % ret, 1)

# 判断机械臂是否满足移动条件
def IsMotorReadyMove():
    ret = Robot.IsMotorReadyMove()
    if ret == 1:
            return 1
    return 0

# 判断机械臂是否意外停止
def IsMoveStopUnexpected():
    ret = Robot.IsMoveStopUnexpected()
    if ret == 1:
        return 1
    return 0

# 判断是否到达位置
def IsMoveToPositionSuccess(strPosSign, TotalCount=500):
    if 0 == IsMotorReadyMove():
        return 0
    if 1 == IsMoveStopUnexpected():
        return 0
    
    count = 0
    while count < TotalCount:
        if 0 == IsMotorReadyMove():
            return 0
        if 1 == IsMoveStopUnexpected():
            return 0
        ret = Robot.IsMoveToPositionSuccess(String2Char(strPosSign))
        if ret == 1:
            return 1
        count = count + 1
        #PrintRecordData('TotalCount %s' % TotalCount, 0)
        #PrintRecordData('Count %s' % count, 0)
        #time.sleep(0.001)
    return 0

'''
iType: 1==>go  2==>mov
'''
def SportPointsListUnit(pList,iType):
    tempList =[]
    tempList.extend(pList)
    for i in range(0,len(pList)):
        if(i<len(pList)-1):
            if(iType == 1):
                if(FromPoint2Point(pList[i],pList[i+1])):
                    gc_needCheckPos=True
                    tempList.pop(0)
                else:
                    break
            elif(iType == 2):
                if(FromPointMove2Point(pList[i],pList[i+1])):
                    gc_needCheckPos=True
                    tempList.pop(0)
                else:
                    break
    return tempList

# 根据点列表走点位（仅支持go指令）
def GoPointsList(pList):
    gc_needCheckPos=True                
    gc_StartPosCheckOk=False
    tempList =[]
    tempList.extend(pList)
    iOriginLen=len(pList)
    iTotal = 10
    iIndex = 0
    while iIndex < iTotal:
        tempList = SportPointsListUnit(tempList,1)
        if(len(tempList) == 1):
            return 1
        #第一个点第一次必须检测是否到位
        if(len(tempList) ==iOriginLen):
            if gc_StartPosCheckOk:
                gc_needCheckPos = False
        else:
            gc_needCheckPos=False                
        iIndex +=1
    return 0

            
# 根据点列表走点位（仅支持Move指令）
def MovePointsList(pList):
    gc_needCheckPos=True                
    gc_StartPosCheckOk=False
    tempList =[]
    tempList.extend(pList)
    iOriginLen=len(pList)
    iTotal = 10
    iIndex = 0
    while iIndex < iTotal:
        tempList = SportPointsListUnit(tempList,2)
        if(len(tempList) == 1):
            return 1
        #第一个点第一次必须检测是否到位
        if(len(tempList) ==iOriginLen):
            if gc_StartPosCheckOk:
                gc_needCheckPos = False
        else:
            gc_needCheckPos=False                
        iIndex +=1
    return 0


# 从一个点到另一个点（go指令）
def FromPoint2Point(StrCurPos, strNextPos, TotalCount=300):
    CurPos = StrCurPos
    NextPos = strNextPos
    if(gc_needCheckPos):
        if (IsMoveToPositionSuccess(CurPos, 10)):
            gc_StartPosCheckOk=True
            Robot.Execute_Go(String2Char(NextPos))
        else:
            PrintRecordData('Robot未到达CurPos %s' % CurPos, 1)
            return 0
    else:
        Robot.Execute_Go(String2Char(NextPos))
    if (IsMoveToPositionSuccess(NextPos, TotalCount)):
        return 1
    else: 
        PrintRecordData('Robot未到达NextPos %s' % NextPos, 1)
        return 0


# 从一个点到另一个点（Move指令）
def FromPointMove2Point(StrCurPos, strNextPos, TotalCount=300):
    CurPos = StrCurPos
    NextPos = strNextPos
    if gc_needCheckPos:
        if (IsMoveToPositionSuccess(CurPos, 10)):
            gc_StartPosCheckOk=True
            Robot.Execute_Move(String2Char(NextPos))
        else:
            PrintRecordData('Robot未到达CurPos %s' % CurPos, 1)
            return 0
    else:
        Robot.Execute_Move(String2Char(NextPos))
    if (IsMoveToPositionSuccess(NextPos, TotalCount)):
        return 1
    else:
       PrintRecordData('Robot未到达NextPos %s' % NextPos, 1)
       return 0


# 从AnyWhere到一个点
def AnyWhere2OnePoint(strNextPos):
    iTotal = 3
    iIndex = 0
    while iIndex < iTotal:
        Robot.Execute_Go(String2Char(strNextPos))
        if (IsMoveToPositionSuccess(strNextPos)):
            return 1
        else:
            PrintRecordData('Robot未到达 %s' % strNextPos, 1)
            iIndex += 1
    return 0

def CloseGripper():
    ret = Robot.CloseGripper()
    if(ret == 1):
        return 1
    else:
        PrintRecordData("关闭夹手失败", 1)
        return 0

def OpenGripper():
    ret = Robot.OpenGripper()
    if(ret == 1):
        return 1
    else:
        PrintRecordData("打开夹手失败", 1)
        return 0

#判断夹手打开是否成功
def IsOpenGripperSuccess():
    count = 0
    while count < 5:
        ret = Robot.IsOpenGripperSuccess()
        if(ret == 1):
            return 1
        count = count + 1
    return 0        
        
    
##########################################################功能函数 结束 #############################################

# <editor-fold desc="折叠后要显示的内容,PS:在pycharm里有效">
##########################################################全局变量 开始 #############################################
#brief describe: [gc_TferGet1 == TferWait ]   [gc_CoaterPut1 == CoaterWait]  [gc_ImagerPut1 == ImagerWait]
#region  点位名称全局变量
gc_home="HomePos"
gc_TferPut1="TferPut1"
gc_TferPut2="TferPut2"
gc_TferWait="TferGet1" 
gc_TferGet2="TferGet2"
gc_TferGet3="TferGet3"
gc_TferGet4="TferGet4"
gc_DryR1="DryR1"
gc_DryR2="DryR2"
gc_DrySafety="DrySafety"
gc_WindWait="WindWait"
gc_WindIn="WindIn"
gc_CoaterSafety="CoaterSafety"
gc_CoaterWait="CoaterPut1"  
gc_CoaterPut2="CoaterPut2"
gc_CoaterPut3="CoaterPut3"
gc_CoaterPut4="CoaterPut4"
gc_CoaterGet1="CoaterGet1"
gc_CoaterGet2="CoaterGet2"
gc_CoaterDry="CoaterDry"
gc_ImagerWait="ImagerPut1"  
gc_ImagerPut2="ImagerPut2"
gc_ImagerPut3="ImagerPut3"
gc_ImagerPut4="ImagerPut4"
gc_ImagerGet1="ImagerGet1"
gc_ImagerGet2="ImagerGet2"
gc_ImagerSafetyUp="ImagerSafetyUp"
gc_ImagerSafetyDown="ImagerSafety"
gc_ImagerDry="ImagerDry"
#endregion
##########################################################全局变量 开始 #############################################
# </editor-fold>

##########################################################动作执行 开始 #############################################
# 从AnyWhere到Home
"""
Anywhere->gc_Home
"""
def Ctrl_AnyWhere2Home():
    NextPos = gc_home
    return AnyWhere2OnePoint(NextPos)


# 从AnyWhere到中转槽等待区
"""
AnyWhere->gc_TferWait
"""
def Ctrl_AnyWhere2TransferTankWait():
    NextPos = gc_TferWait 
    return AnyWhere2OnePoint(NextPos)


# 从AnyWhere到风刀口等待区
"""
AnyWhere->gc_WindWait
"""
def Ctrl_AnyWhere2WindBladeWait():
    NextPos = gc_WindWait
    return AnyWhere2OnePoint(NextPos)


# 从AnyWhere到涂布机生化等待区
"""
AnyWhere->gc_CoaterWait
"""
def Ctrl_AnyWhere2CoaterWait():
    NextPos = gc_CoaterWait
    return AnyWhere2OnePoint(NextPos)


# 从AnyWhere到光机拍照等待区
"""
AnyWhere->gc_ImagerWait
"""
def Ctrl_AnyWhere2ImagerWait():
    NextPos = gc_ImagerWait
    return AnyWhere2OnePoint(NextPos)


# 从AnyWhere到沥液区
"""
AnyWhere->gc_DryR2
"""
def Ctrl_AnyWhere2DryWait():
    NextPos = gc_DryR2
    return AnyWhere2OnePoint(NextPos)


# 从home到中转槽等待区
"""
[gc_home,gc_TferWait]
"""
def Ctrl_Home2TransferTankWait():
    PList = [gc_home,gc_TferWait]
    return GoPointsList(PList)


# 从Home到风刀口等待区
"""
[gc_home,gc_DryR1,gc_DrySafety,gc_WindWait]
"""
def Ctrl_Home2WindBladeWait():
    PList = [gc_home,gc_DryR1,gc_DrySafety,gc_WindWait]
    return GoPointsList(PList)



# 从Home到涂布机生化等待区
"""
[gc_home,gc_DryR1,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
"""
def Ctrl_Home2CoaterWait():
    PList = [gc_home,gc_DryR1,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
    return GoPointsList(PList)


# 从Home到光机拍照等待区
"""
[gc_home,gc_DryR1,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
"""
def Ctrl_Home2ImagerWait():
    PList = [gc_home,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
    #PList = [gc_ImagerSafetyUp,gc_ImagerSafetyDown]
    return GoPointsList(PList)


# 从Home到沥液区
"""
[gc_home,gc_DryR1,gc_DryR2]
"""
def Ctrl_Home2DryWait():
    PList = [gc_home,gc_DryR1,gc_DryR2]
    return GoPointsList(PList)


# 从中转槽等待区到Home
"""
[gc_TferWait,gc_DryR1,gc_home]
"""
def Ctrl_TransferTankWait2Home():
    PList = [gc_TferWait,gc_DryR1,gc_home]
    return GoPointsList(PList)


# 从中转槽等待区到风刀口等待区
"""
gc_TferWait->gc_DryR1->gc_DryR2->停几秒->gc_DrySafety->gc_WindWait
"""
def Ctrl_TransferTankWait2WindBladeWait():
    PList = [gc_TferWait,gc_DryR1,gc_DryR2]
    if(not GoPointsList(PList)):
        return 0
    #停几秒钟
    PrintRecordData("停2秒钟", 0)
    time.sleep(2)

    PList = [gc_DryR2,gc_DrySafety,gc_WindWait]
    return GoPointsList(PList)


# 从中转槽等待区到涂布机等待区
"""
[gc_TferWait,gc_DryR1,gc_DryR2,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
"""
def Ctrl_TransferTankWait2CoaterWait():
    PList = [gc_TferWait,gc_DryR1,gc_DryR2,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
    return GoPointsList(PList)


# 从中转槽等待区到光机等待区
"""
[gc_TferWait,gc_DryR1,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
"""
def Ctrl_TransferTankWait2ImagerWait():
    PList = [gc_TferWait,gc_DryR1,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
    return GoPointsList(PList)


# 从中转槽等待区到沥液区
"""
[gc_TferWait,gc_DryR1,gc_DryR2]
"""
def Ctrl_TransferTankWait2DryWait():
    PList = [gc_TferWait,gc_DryR1,gc_DryR2]
    return GoPointsList(PList)


# 从风刀口到风刀口等待区
"""
[gc_WindIn,gc_WindWait]
"""
def Ctrl_WindBlade2WindBladeWait():
    PList = [gc_WindIn,gc_WindWait]
    return MovePointsList(PList)

# 从风刀口等待区到Home
"""
[gc_WindWait,gc_DrySafety,gc_DryR1,gc_home]
"""
def Ctrl_WindBladeWait2Home():
    PList = [gc_WindWait,gc_DrySafety,gc_DryR1,gc_home]
    return GoPointsList(PList)


# 从风刀口等待区到中转槽等待区
"""
[gc_WindWait,gc_DrySafety,gc_TferWait]
"""
def Ctrl_WindBladeWait2TransferTankWait():
    PList = [gc_WindWait,gc_DrySafety,gc_TferWait]
    return GoPointsList(PList)


# 从风刀口等待区到风刀口
"""
[gc_WindWait,gc_WindIn]
"""
def Ctrl_WindBladeWait2WindBlade():
    PList = [gc_WindWait,gc_WindIn]
    return MovePointsList(PList)


# 从风刀口等待区到涂布机生化等待区
"""
[gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
"""
def Ctrl_WindBladeWait2CoaterWait():
    PList = [gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
    return GoPointsList(PList)


# 从风刀口等待区到光机拍照等待区
"""
[gc_WindWait,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
"""
def Ctrl_WindBladeWait2ImagerWait():
    PList = [gc_WindWait,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
    return GoPointsList(PList)


# 从风刀口等待区到沥液区
"""
[gc_WindWait,gc_DrySafety,gc_DryR2]
"""
def Ctrl_WindBladeWait2DryWait():
    PList = [gc_WindWait,gc_DrySafety,gc_DryR2]
    return GoPointsList(PList)


# 从涂布机生化等待区到Home
"""
gc_CoaterWait->gc_CoaterSafety->gc_WindWait->gc_DrySafety->gc_home
"""
def Ctrl_CoaterWait2Home():
    PList = [gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_home]
    return GoPointsList(PList)


# 从涂布机生化等待区到中转槽等待区
"""
[gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_TferWait]
"""
def Ctrl_CoaterWait2TransferTankWait():
    PList = [gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_TferWait]
    return GoPointsList(PList)


# 从涂布机生化等待区到风刀口等待区
"""
[gc_CoaterWait,gc_CoaterSafety,gc_WindWait]
"""
def Ctrl_CoaterWait2WindBladeWait():
    PList = [gc_CoaterWait,gc_CoaterSafety,gc_WindWait]
    return GoPointsList(PList)


# 从涂布机生化等待区到光机拍照等待区
"""
[gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
"""
def Ctrl_CoaterWait2ImagerWait():
    PList = [gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
    return GoPointsList(PList)


# 从涂布机生化等待区到沥液区
"""
[gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_DryR2]
"""
def Ctrl_CoaterWait2DryWait():
    PList = [gc_CoaterWait,gc_CoaterSafety,gc_WindWait,gc_DrySafety,gc_DryR2]
    return GoPointsList(PList)

# 从涂布机生化等待区到生化沥液区
"""
[gc_CoaterWait,gc_CoaterDry]
"""
def Ctrl_CoaterWait2CoaterDry():
    PList = [gc_CoaterWait,gc_CoaterDry]
    return GoPointsList(PList)
    
    
# 从涂布机生化等待区到生化沥液区
"""
[gc_CoaterDry,gc_CoaterWait]
"""
def Ctrl_CoaterDry2CoaterWait():
    PList = [gc_CoaterDry,gc_CoaterWait]
    return GoPointsList(PList)
    
    
# 从光机拍照等待区到Home
"""
[gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_home]
"""
def Ctrl_ImagerWait2Home():
    PList = [gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_home]
    return GoPointsList(PList)


# 从光机拍照等待区到中转槽等待区
"""
[gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_TferWait]
"""
def Ctrl_ImagerWait2TransferTankWait():
    PList = [gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_TferWait]
    return GoPointsList(PList)


# 从光机拍照等待区到风刀口等待区
"""
[gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_WindWait]
"""
def Ctrl_ImagerWait2WindBladeWait():
    PList = [gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_WindWait]
    return GoPointsList(PList)


# 从光机拍照等待区到涂布机生化等待区
"""
[gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
"""
def Ctrl_ImagerWait2CoaterWait():
    PList = [gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
    return GoPointsList(PList)


# 从光机拍照等待区到沥液区
"""
[gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_DryR2]
"""
def Ctrl_ImagerWait2DryWait():
    PList = [gc_ImagerWait,gc_ImagerSafetyDown,gc_ImagerSafetyUp,gc_DrySafety,gc_DryR2]
    return GoPointsList(PList)


# 从光机等待区到光机沥液区
"""
[gc_ImagerWait,gc_ImagerDry]
"""
def Ctrl_ImagerWait2ImagerDry():
    PList = [gc_ImagerWait,gc_ImagerDry]
    return GoPointsList(PList)
    
    
# 从光机沥液区到光机拍照等待区
"""
[gc_ImagerDry,gc_ImagerWait]
"""
def Ctrl_ImagerDry2ImagerWait():
    PList = [gc_ImagerDry,gc_ImagerWait]
    return GoPointsList(PList)
    
    
# 从沥液区到Home
"""
[gc_DryR2,gc_home]
"""
def Ctrl_DryWait2Home():
    PList = [gc_DryR2,gc_home]
    return GoPointsList(PList)


# 从沥液区到中转槽等待区
"""
[gc_DryR2,gc_DryR1,gc_TferWait]
"""
def Ctrl_DryWait2TransferTankWait():
    PList = [gc_DryR2,gc_DryR1,gc_TferWait]
    return GoPointsList(PList)


# 从沥液区到风刀口等待区
"""
[gc_DryR2,gc_DrySafety,gc_WindWait]
"""
def Ctrl_DryWait2WindBladeWait():
    PList = [gc_DryR2,gc_DrySafety,gc_WindWait]
    return GoPointsList(PList)


# 从沥液区到涂布机生化等待区
"""
[gc_DryR2,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
"""
def Ctrl_DryWait2CoaterWait():
    PList = [gc_DryR2,gc_DrySafety,gc_WindWait,gc_CoaterSafety,gc_CoaterWait]
    return GoPointsList(PList)


# 从沥液区到光机拍照等待区
"""
[gc_DryR2,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
"""
def Ctrl_DryWait2ImagerWait():
    PList = [gc_DryR2,gc_DrySafety,gc_ImagerSafetyUp,gc_ImagerSafetyDown,gc_ImagerWait]
    return GoPointsList(PList)


##########################################################动作执行 结束 #############################################

##########################################################芯片操作 开始 #############################################
# 中转槽放芯片-进
"""
[gc_TferWait,gc_TferPut1,gc_TferPut2]
"""
def Ctrl_TransferTankPutChips_In():
    PList = [gc_TferWait,gc_TferPut1,gc_TferPut2]
    return MovePointsList(PList)

# 中转槽放芯片-出
"""
[gc_TferPut2,gc_TferPut1,gc_TferWait]
"""
def Ctrl_TransferTankPutChips_Out():
    PList = [gc_TferPut2,gc_TferPut1,gc_TferWait]
    return MovePointsList(PList)

# 中转槽取芯片-进
"""
[gc_TferWait,gc_TferGet2,gc_TferGet3]
"""
def Ctrl_TransferTankGetChips_In():
    if(not IsOpenGripperSuccess()):
        #开夹手
        if(not OpenGripper()):
            return 0
    PList = [gc_TferWait,gc_TferGet2,gc_TferGet3]
    return MovePointsList(PList)


# 中转槽取芯片-出
"""
[gc_TferGet3,gc_TferGet4,gc_TferWait]
"""
def Ctrl_TransferTankGetChips_Out():
    PList = [gc_TferGet3,gc_TferGet4,gc_TferWait]
    return MovePointsList(PList)


# 涂布机放芯片-进
"""
gc_CoaterWait->gc_CoaterPut2->gc_CoaterPut3->gc_CoaterPut4
"""
def Ctrl_CoaterPutChips_In():
    PList = [gc_CoaterWait,gc_CoaterPut2,gc_CoaterPut3,gc_CoaterPut4]
    return MovePointsList(PList)
   

# 涂布机放芯片-出
"""
gc_CoaterPut4->gc_CoaterGet2->gc_CoaterGet1->gc_CoaterWait
"""
def Ctrl_CoaterPutChips_Out():
    PList = [gc_CoaterPut4,gc_CoaterGet2,gc_CoaterGet1,gc_CoaterWait]
    return MovePointsList(PList)

# 涂布机取芯片-进
"""
gc_CoaterWait->gc_CoaterGet1->gc_CoaterGet2->gc_CoaterPut4
"""
def Ctrl_CoaterGetChips_In():
    PList = [gc_CoaterWait,gc_CoaterGet1,gc_CoaterGet2,gc_CoaterPut4]
    return MovePointsList(PList)
   

# 涂布机取芯片-出
"""
gc_CoaterPut4->gc_CoaterPut3->gc_CoaterPut2->gc_CoaterWait
"""
def Ctrl_CoaterGetChips_Out():
    PList = [gc_CoaterPut4,gc_CoaterPut3,gc_CoaterPut2,gc_CoaterWait]
    return MovePointsList(PList)


# 光机放芯片-进
"""
gc_ImagerWait->gc_ImagerPut2->gc_ImagerPut3->gc_ImagerPut4
"""
def Ctrl_ImagerPutChips_In():
    PList = [gc_ImagerWait,gc_ImagerPut2,gc_ImagerPut3,gc_ImagerPut4]
    return MovePointsList(PList)


# 光机放芯片-出
"""
[gc_ImagerPut4,gc_ImagerGet2,gc_ImagerGet1,gc_ImagerWait]
"""
def Ctrl_ImagerPutChips_Out():
    PList = [gc_ImagerPut4,gc_ImagerGet2,gc_ImagerGet1,gc_ImagerWait]
    return MovePointsList(PList)


# 光机取芯片-进
"""
[gc_ImagerWait,gc_ImagerGet1,gc_ImagerGet2,gc_ImagerPut4]
"""
def Ctrl_ImagerGetChips_In():
    if(not IsOpenGripperSuccess()):
        #开夹手
        if(not OpenGripper()):
            return 0
    PList = [gc_ImagerWait,gc_ImagerGet1,gc_ImagerGet2,gc_ImagerPut4]
    return MovePointsList(PList)


# 光机取芯片-出
"""
gc_ImagerPut4->gc_ImagerPut3->gc_ImagerPut2->gc_ImagerWait
"""
def Ctrl_ImagerGetChips_Out():
    PList = [gc_ImagerPut4,gc_ImagerPut3,gc_ImagerPut2,gc_ImagerWait]
    return MovePointsList(PList)

##########################################################芯片操作 结束 #############################################


if __name__ == "__main__":
    #ReturnValue()
    #TestMsgBox()
    print(gc_TferWait)
    pass