using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using System;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Runtime.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public class NetBase : MonoBehaviour
{
    public class JsonHelper
    {
        /// <summary>
        /// Json转换成对象
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="jsonText"></param>
        /// <returns></returns>
        public static T JsonToObject<T>(string jsonText)
        {
            DataContractJsonSerializer s = new DataContractJsonSerializer(typeof(T));
            MemoryStream ms = new MemoryStream(Encoding.UTF8.GetBytes(jsonText));
            T obj = (T)s.ReadObject(ms);
            ms.Dispose();
            return obj;
        }

        /// <summary>
        /// 对象转换成JSON
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="obj"></param>
        /// <returns></returns>
        public static string ObjectToJSON<T>(T obj)
        {
            DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(T));
            string result = string.Empty;
            using (MemoryStream ms = new MemoryStream())
            {
                serializer.WriteObject(ms, obj);
                ms.Position = 0;
                using (StreamReader read = new StreamReader(ms))
                {
                    result = read.ReadToEnd();
                }
            }
            return result;
        }
    }

    [DataContract]
    public class Result
    {
        [DataMember(Order = 0)]
        public bool isOK;
    }

    [DataContract]
    public class NetEnd
    {
        [DataMember(Order = 0)]
        public string ip;

        [DataMember(Order = 1)]
        public int port;

        public NetEnd(string ip_,int port_)
        {
            ip = ip_;
            port = port_;
        }
    }

    #region 1.Voice
    [DataContract]
    public class VoiceText
    {
        [DataMember(Order = 0)]
        public string word { get; set; }

        [DataMember(Order = 1)]
        public string command { get; set; }
    }

    [DataContract]
    public class VoiceData
    {
        [DataMember(Order = 0)]
        public VoiceText TEXT { get; set; }

        // public List<float> VOICE_STRAM { get; set; }
        [DataMember(Order = 1)]
        //public List<float> VOICE_STRAM { get; set; }
        public string VOICE_PATH { get; set; }

        [DataMember(Order = 2)]
        public List<float> VOICE_STRAM { get; set; }
    }

    [DataContract]
    public class PeopleVoice
    {
        [DataMember(Order = 0)]
        public string VOICE_ATTR { get; set; }

        [DataMember(Order = 1)]
        public string TIME { get; set; }

        [DataMember(Order = 2)]
        public VoiceData VOICE_DATA { get; set; }
    }
    #endregion

    #region 2.IMG

    [DataContract]
    public class EulerAngle
    {
        [DataMember(Order = 0)]
        public float X = 0;

        [DataMember(Order = 1)]
        public float Y = 0;

        [DataMember(Order = 2)]
        public float Z = 0;
    }

    [DataContract]
    public class FaceParam
    {
        [DataMember(Order = 0)]
        public float eyeL = 0.0f;

        [DataMember(Order = 1)]
        public float eyeR = 0.0f;

        [DataMember(Order = 2)]
        public float mouth = 0.0f;
    }

    [DataContract]
    public class ImgDetail
    {
        [DataMember(Order = 0)]
        public FaceParam faceData;

        [DataMember(Order = 1)]
        public EulerAngle head;

        [DataMember(Order = 2)]
        public EulerAngle armL;

        [DataMember(Order = 3)]
        public EulerAngle fore_armL;

        [DataMember(Order = 4)]
        public EulerAngle handL;

        [DataMember(Order = 5)]
        public EulerAngle armR;

        [DataMember(Order = 6)]
        public EulerAngle fore_armR;

        [DataMember(Order = 7)]
        public EulerAngle handR;
    }

    [DataContract]
    public class VtuberImg
    {
        [DataMember(Order = 0)]
        public string IMG_ATTR;

        [DataMember(Order = 1)]
        public ImgDetail IMG_DATA;

        [DataMember(Order = 2)]
        public string TIME;
    }

    [DataContract]
    public class FaceDetail
    {
        [DataMember(Order = 0)]
        public string Name { get; set; }

        [DataMember(Order = 1)]
        public int age { get; set; }

        [DataMember(Order = 2)]
        public string gender { get; set; }

        [DataMember(Order = 3)]
        public string emotion { get; set; }

        [DataMember(Order = 4)]
        public string action { get; set; }

    }

    [DataContract]
    public class PeopleImg
    {
        [DataMember(Order = 0)]
        public string IMG_ATTR;

        [DataMember(Order = 1)]
        public FaceDetail FACE_DATA;

        [DataMember(Order = 2)]
        public string TIME;
    }    
    #endregion

    #region 3.1支付(Tcp)

    [DataContract]
    public class MsgPay
    {
        [DataMember(Order = 0)]
        public string equipNo;//设备编号

        [DataMember(Order = 1)]
        public string goodNo;//商品标识 0：K歌 1：投屏

        [DataMember(Order = 2)]
        public string instruct;//100001开锁，100002关锁，100006结束租赁结果信号

        [DataMember(Order = 3)]
        public string type;//指令代码 0：失败 1：成功

        [DataMember(Order = 4)]
        public string content;//业务数据

        public MsgPay(string equipNo_,string goodNo_,string instruct_,
            string type_ ="1",string content_ = null)
        {
            equipNo = equipNo_;
            goodNo = goodNo_;
            instruct = instruct_;
            type = type_;
            content = content_;
        }
    }

    [DataContract]
    public class MsgType
    {
        [DataMember(Order = 0)]
        public string msgType;//"0"登录，"1"心跳，"2"退出房间，"3"进入房间，"4"游戏开始，"5"游戏结束，"6"通用数据
    }

    [DataContract]
    public class HeartBeatMsg
    {
        [DataMember(Order = 0)]
        public string msgType;//"0"登录，"1"心跳，"2"退出房间，"3"进入房间，"4"游戏开始，"5"游戏结束，"6"通用数据

        [DataMember(Order = 1)]
        public string userId;//用户ID

        public HeartBeatMsg(string msg="1",string Id = "10000")
        {
            msgType = msg;
            userId = Id;
        }
    }

    [DataContract]
    public class LoginCmd
    {
        [DataMember(Order = 0)]
        public string msgType;

        [DataMember(Order = 1)]
        public string userId;

        public LoginCmd(string userID,string mt="0")
        {
            msgType = mt;
            userId = userID;
        }
    }

    [DataContract]
    public class LogInResult
    {
        [DataMember(Order = 0)]
        public string type;
    }

    [DataContract]
    public class LogInRcv
    {
        [DataMember(Order = 0)]
        public string msgType;

        [DataMember(Order = 1)]
        public LogInResult msgContent;
    }

    [DataContract]
    public class SocketMessage
    {
        [DataMember(Order = 0)]
        public string msgType;//"0"登录，"1"心跳，"2"退出房间，"3"进入房间，"4"游戏开始，"5"游戏结束，"6"通用数据

        [DataMember(Order = 3)]
        public string route;//设备标识

        [DataMember(Order = 1)]
        public string userId;//用户ID

        //[DataMember(Order = 3)]
        //public string userType;//用户类型 0：线上用户，1线下用户，2机械用户

        [DataMember(Order = 2)]
        public MsgPay msgContent;//消息内容

        public SocketMessage(string route_,string userId_,MsgPay mp,
            string msgType_="4")
        {
            msgType = msgType_;
            route = route_;
            userId = userId_;
            msgContent = mp;
        }
    }
    #endregion

    #region 3.2支付(http)

    [DataContract]
    public class GoodGoll
    {
        [DataMember(Order = 0)]
        public string id { set; get; }

        [DataMember(Order = 1)]
        public string searchValue { set; get; }

        [DataMember(Order = 2)]
        public string createBy { set; get; }

        [DataMember(Order = 3)]
        public string createTime { set; get; }

        [DataMember(Order = 4)]
        public string updateBy { set; get; }

        [DataMember(Order = 5)]
        public string updateTime { set; get; }

        [DataMember(Order = 6)]
        public string remark { set; get; }

        [DataMember(Order = 7)]
        public string delFlag { set; get; }

        [DataMember(Order = 8)]
        public string groupSql { set; get; }

        [DataMember(Order = 9)]
        public string tollTimeVal { set; get; }

        [DataMember(Order = 10)]
        public string tollTimeType { set; get; }

        [DataMember(Order = 11)]
        public string tollMaxTime { set; get; }

        [DataMember(Order = 12)]
        public float cost { set; get; }

        [DataMember(Order = 13)]
        public float tollAmount { set; get; }

        [DataMember(Order = 14)]
        public float deposit { set; get; }//租金

        [DataMember(Order = 15)]
        public float tollStandard { set; get; }//租借计费

        [DataMember(Order = 16)]
        public string closeRemark { set; get; }//结算说明

        [DataMember(Order = 17)]
        public string status { set; get; }

        [DataMember(Order = 18)]
        public string sort { set; get; }

        [DataMember(Order = 19)]
        public string remarks { set; get; }//说明
    }

    [DataContract]
    public class GoodInfo
    {
        [DataMember(Order = 0)]
        public string id { set; get; }

        [DataMember(Order = 1)]
        public string searchValue { set; get; }

        [DataMember(Order = 2)]
        public string createBy { set; get; }

        [DataMember(Order = 3)]
        public string createTime { set; get; }

        [DataMember(Order = 4)]
        public string updateBy { set; get; }

        [DataMember(Order = 5)]
        public string updateTime { set; get; }

        [DataMember(Order = 6)]
        public string remark { set; get; }

        [DataMember(Order = 7)]
        public string delFlag { set; get; }

        [DataMember(Order = 8)]
        public string groupSql { set; get; }

        [DataMember(Order = 9)]
        public string name { set; get; }

        [DataMember(Order = 10)]
        public string img { set; get; }

        [DataMember(Order = 11)]
        public string bgImg { set; get; }
        [DataMember(Order = 12)]
        public string bgImgMax { set; get; }
        [DataMember(Order = 13)]
        public string bgImgMiddle { set; get; }
        [DataMember(Order = 14)]
        public string bgImgMin { set; get; }
        [DataMember(Order = 15)]
        public string introduce { set; get; }
        [DataMember(Order = 16)]
        public string tollType { set; get; }
        [DataMember(Order = 17)]
        public string type { set; get; }
        [DataMember(Order = 18)]
        public string tollId { set; get; }
        [DataMember(Order = 19)]
        public float cost { set; get; }
        [DataMember(Order = 20)]
        public string status { set; get; }
        [DataMember(Order = 21)]
        public string sort { set; get; }
        [DataMember(Order = 22)]
        public string remarks { set; get; }

        [DataMember(Order = 23)]
        public GoodGoll goodGoll { set; get; }

        [DataMember(Order = 24)]
        public string showStatus { set; get; }
    }

    [DataContract]
    public class EgoodInfo
    {
        [DataMember(Order = 0)]
        public string id { set; get; }

        [DataMember(Order = 1)]
        public string searchValue { set; get; }

        [DataMember(Order = 2)]
        public string createBy { set; get; }

        [DataMember(Order = 3)]
        public string createTime { set; get; }

        [DataMember(Order = 4)]
        public string updateBy { set; get; }

        [DataMember(Order = 5)]
        public string updateTime { set; get; }

        [DataMember(Order = 6)]
        public string remark { set; get; }

        [DataMember(Order = 7)]
        public string delFlag { set; get; }

        [DataMember(Order = 8)]
        public string groupSql { set; get; }

        [DataMember(Order = 9)]
        public string name { set; get; }

        [DataMember(Order = 10)]
        public string workId { set; get; }

        [DataMember(Order = 11)]
        public string equipId { set; get; }

        [DataMember(Order = 12)]
        public string goodId { set; get; }

        [DataMember(Order = 13)]
        public string stock { set; get; }

        [DataMember(Order = 14)]
        public string status { set; get; }

        [DataMember(Order = 15)]
        public string sort { set; get; }

        [DataMember(Order = 16)]
        public string remarks { set; get; }

        [DataMember(Order = 17)]
        public string type { set; get; }

        [DataMember(Order = 18)]
        public GoodInfo good { set; get; }
    }

    [DataContract]
    public class EquipData
    {
        [DataMember(Order = 0)]
        public string work { set; get; }

        [DataMember(Order = 1)]
        public string equipNo { set; get; }

        [DataMember(Order = 2)]
        public string equip { set; get; }

        [DataMember(Order = 3)]
        public List<EgoodInfo> egoodList;
    }

    [DataContract]
    public class EquipInfo
    {
        [DataMember(Order = 0)]
        public string msg { set; get; }

        [DataMember(Order = 1)]
        public int code { set; get; }

        [DataMember(Order = 2)]
        public EquipData data { set; get; }
    }

    [DataContract]
    public class QRCodeInfo
    {
        [DataMember(Order = 0)]
        public string msg;//QR Code Path

        [DataMember(Order = 1)]
        public int code;
    }

    #endregion

    #region 4.投屏
    [DataContract]
    public class ScreenProjection
    {
        [DataMember(Order = 0)]
        public string ScreenPro;
    }
    #endregion
    public delegate void MsgDelegate(string msg);

    public delegate void VoiceDelegate(PeopleVoice pv);

    public delegate void ImgDelegate(PeopleImg pi);

    public delegate void VtuberDelegate(VtuberImg vi);

    public delegate void ScreenProDelegate(ScreenProjection sp);
    public delegate void ScreenProStrDelegate(string mes);

    public delegate void MsgPayDelegate(MsgPay mp);
    
    public MsgDelegate msgDelegate = null;
    public VoiceDelegate vtuberVocieDelegate = null;
    public VoiceDelegate customerVocieDelegate = null;
    public ImgDelegate customerImgDelegate = null;
    public VtuberDelegate vtuberDelegate = null;
    public ScreenProDelegate scrnProDelegate = null;
    public ScreenProStrDelegate scrnProStrDelegate = null;
    public MsgPayDelegate msgPayDelegate = null;

    protected string TCPEndNote = "$_";
    public virtual void sendMessage(string clientName, string message)
    {

    }

    public virtual void sendMessage(string message)
    {

    }

    public virtual void run(string ip_, int port_)
    {

    }

    protected virtual void processRequestBody(string bodyStr)
    {
        System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();
        stopwatch.Start(); //  开始监视代码运行时间
        if (bodyStr.Contains("IMG_ATTR"))
        {
            var jsObj = (JObject)JsonConvert.DeserializeObject(bodyStr);
            if(jsObj["IMG_ATTR"]?.ToString() == "faceAttr")
            {
                // to do img message reaction
                PeopleImg customerImg = JsonHelper.JsonToObject<PeopleImg>(bodyStr);
                if (customerImgDelegate != null)
                {
                    customerImgDelegate(customerImg);
                }
            }
            if (jsObj["IMG_ATTR"]?.ToString() == "imgAttr")
            {
                // to do img message reaction
                var customerImg = JsonHelper.JsonToObject<VtuberImg>(bodyStr);
                if (vtuberDelegate != null)
                {
                    vtuberDelegate(customerImg);
                }
            }
        }
        else if (bodyStr.Contains("customerAttr"))
        {
            // to do cutomer voice reaction
            //JavaScriptSerializer js = new JavaScriptSerializer();
            //PeopleVoice customerVoice = JsonConvert.DeserializeObject<PeopleVoice>(bodyStr);
            PeopleVoice customerVoice = JsonHelper.JsonToObject<PeopleVoice>(bodyStr);

            Debug.Log(customerVoice.TIME + "->" + customerVoice.VOICE_DATA.TEXT);
            // PeopleVoice vtuberVoice = JsonUtility.FromJson<PeopleVoice>(bodyStr);
            if (customerVocieDelegate != null)
            {
                customerVocieDelegate(customerVoice);
            }
        }
        else if (bodyStr.Contains("vtuberAttr"))
        {
            // to do vtuber voice reaction
            PeopleVoice vtuberVoice = JsonHelper.JsonToObject<PeopleVoice>(bodyStr);

            Debug.Log(vtuberVoice.TIME + "->" + vtuberVoice.VOICE_DATA.TEXT);
            if (vtuberVocieDelegate != null)
            {
                vtuberVocieDelegate(vtuberVoice);
            }
        }
        else if(bodyStr.Contains("ScreenPro"))
        {
            if (scrnProStrDelegate != null)
                scrnProStrDelegate(bodyStr);
            // to do screenprojection close 
            //ScreenProjection scrnData = JsonHelper.JsonToObject<ScreenProjection>(bodyStr);
            //Debug.Log(vtuberVoice.TIME + "->" + vtuberVoice.VOICE_DATA.TEXT);
            //if (scrnProDelegate != null)
            //{
            //    scrnProDelegate(scrnData);
            //}
        }

        stopwatch.Stop(); //  停止监视
        TimeSpan timespan = stopwatch.Elapsed; //  获取当前实例测量得出的总时间
        double milliseconds = timespan.TotalMilliseconds;  //  总毫秒数
        Debug.Log("Parese body total time : " + milliseconds);
    }
}
