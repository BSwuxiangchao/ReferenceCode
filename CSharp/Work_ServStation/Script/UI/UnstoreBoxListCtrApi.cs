using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnstoreBoxListCtrApi : MonoBehaviour
{
    public int curBoxListType_ = 1;
    public static class BoxListType
    {
        public static readonly int None = 0;
        //一级
        public static readonly int Service = 1;
        //二级
        public static readonly int GoodsType = 2;
        public static readonly int OutDoor = 3;
        public static readonly int WaterElec = 4;
        public static readonly int Shower = 5;
        public static readonly int Rescue = 6;
        public static readonly int Kitchen = 7;
        public static readonly int Amusement = 8;
        //三级
        public static readonly int Food = 9;
        public static readonly int Drink = 10;
        public static readonly int AgriP = 11;

    }

    public virtual void SetBoxType(int type, string serviceName = "服务列表")
    {

    }
    public virtual int GetBoxType()
    {
        return 0;
    }
    public virtual void moveFront(int cnt = 1)
    {

    }

    public virtual void ShakeAct(int index = 3, bool isShake = true)
    {

    }

    public virtual string GetCurrentGoodsID()
    {
        return null;
    }

    public virtual string GetCurrentGoodsName(int num)
    {
        return null;
    }

    public virtual void initState()
    {

    }
}
