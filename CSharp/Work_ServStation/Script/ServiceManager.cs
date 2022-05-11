using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Runtime.Serialization;
using System.Net;
using System;
using System.Text;
using System.Linq;
using Newtonsoft.Json;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;
using System.Text.RegularExpressions;

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
    public static string ObjectToJSON<T>(T obj,int spacing = 4)
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

        return result = ConvertJsonString(result, spacing);
    }

    /// <summary>
    /// 格式化json格式的字符串
    /// </summary>
    /// <param name="str">未格式化的json字符串</param>
    /// <param name="spacing">格式化的空格个数</param>
    /// <returns>格式化的json格式字符串</returns>
    private static string ConvertJsonString(string str, int spacing = 4)
    {
        //格式化json字符串
        JsonSerializer serializer = new JsonSerializer();
        TextReader tr = new StringReader(str);
        JsonTextReader jtr = new JsonTextReader(tr);
        object obj = serializer.Deserialize(jtr);
        if (obj != null)
        {
            StringWriter textWriter = new StringWriter();
            JsonTextWriter jsonWriter = new JsonTextWriter(textWriter)
            {
                Formatting = Formatting.Indented,
                Indentation = spacing,
                IndentChar = ' '
            };
            serializer.Serialize(jsonWriter, obj);
            return textWriter.ToString();
        }
        else
        {
            return str;
        }
    }
}

#region videoPath
[DataContract]
public class VideosPath
{
    [DataMember(Order = 0)]
    public string []foldPath;

    [DataMember(Order = 1)]
    public Dictionary<string, List<string>> foldDic = new Dictionary<string, List<string>>();

    [DataMember(Order = 2)]
    public string mfilter = "*.mp4";
    public VideosPath(string []path)
    {
        foldPath = path;
        mfilter = "*.mp4";
    }

    public void addFold(string foldName,string filter = "*.mp4")
    {
        mfilter = filter;
        string subfoldPath = foldPath[0] + "\\" + foldName;
        if(Directory.Exists(subfoldPath))
        {
            var files = Directory.GetFiles(subfoldPath, filter);
            var videoList = files.ToList<string>();
            foldDic.Add(foldName, videoList);
        }
        else
        {
            foldDic.Add(foldName, null);
        }

    }

    public void checkVideos()
    {
        var dic = new Dictionary<string, List<string>>();
        foreach(var pair in foldDic)
        {
            string folder = foldPath[0] + "\\" + pair.Key;
            //string key = pair.Key;
            if (!Directory.Exists(folder))//判断视频文件夹存在否，不存在则删除
            {
                //foldDic.Remove(pair.Key);
                Debug.LogWarning("not found video folder :" + folder);
                continue;
            }
            Debug.LogWarning("found video folder :" + folder);
            var files = Directory.GetFiles(folder, mfilter);
            var videoList = files.ToList<string>();
            dic.Add(pair.Key, videoList);
        }
        foldDic = dic;
    }


}
#endregion

#region MV params config
[DataContract]
public class MvTagName
{
    [DataMember(Order = 0)]
    public int tag = 0;

    [DataMember(Order = 1)]
    public string name = "其他";
    public MvTagName(int Tag,string Name)
    {
        tag = Tag;
        name = Name;
    }

    public MvTagName(int Tag)
    {
        if(!tagDic.ContainsKey(Tag))
        {
            tag = 0;
            name = tagDic[0];
        }
        tag = Tag;
        name = tagDic[Tag];
    }

    public MvTagName(string Name)
    {
        if (!tagDic.ContainsValue(Name))
        {
            DicIndex += 1;
            tagDic.Add(DicIndex, Name);
            tag = DicIndex;
            name = Name;
        }
        tag = tagDic.Where(q=>q.Value==Name).Select(q=>q.Key).First();
        name = Name;
    }

    static private int DicIndex = 6;
    public static Dictionary<int, string> tagDic = new Dictionary<int, string>
    {
        {0,"其他" },
        {1,"流行" },
        {2,"古典" },
        {3,"摇滚" },
        {4,"民谣" },
        {5,"爵士" },
        {6,"布鲁斯" }
    };
}

//[DataContract]
//public class MvTag
//{
//    [DataMember(Order = 0)]
//    public MvTagName Others = new MvTagName(0,"其他");//其他类型
//
//    [DataMember(Order = 1)]
//    public MvTagName pop = new MvTagName(1, "流行");//流行
//
//    [DataMember(Order = 2)]
//    public MvTagName classical = new MvTagName(2, "古典");//古典
//
//    [DataMember(Order = 3)]
//    public MvTagName rock = new MvTagName(3, "摇滚");//摇滚
//
//    [DataMember(Order = 4)]
//    public MvTagName ballad = new MvTagName(4, "民谣");//民谣
//
//    [DataMember(Order = 5)]
//    public MvTagName jazz = new MvTagName(5, "爵士");
//
//    [DataMember(Order = 6)]
//    public MvTagName blues = new MvTagName(6, "布鲁斯");
//
//}

[DataContract]
public class RescueTag
{

}

[DataContract]
public class ScreenProTag
{

}

[DataContract]
public class MvAttr
{
    [DataMember(Order = 0)]
    public string fileName;

    [DataMember(Order = 1)]
    public MvTagName[] tags;
    //public string path;

    public MvAttr(string FileName,MvTagName[] Tags)
    {
        fileName = FileName;
        tags = Tags;
    }

    public MvAttr(string FileName, string[] Tags)
    {
        fileName = FileName;
        List<MvTagName> tagList = new List<MvTagName>();
        foreach (string tag in Tags){
            tagList.Add(new MvTagName(tag));
        }
        tags = tagList.ToArray();
    }
}


[DataContract]
public class KaraokeCfg
{
    [DataMember(Order = 0)]
    public string mvVieoFolderPath= @"F:\Videos\MV";

    [DataMember(Order = 1)]
    public string[] TagsHave;

    private MvAttr[] mvVideos_;
    [DataMember(Order = 2)]
    public MvAttr[] mvVideos { 
        set {
            mvVideos_ = value;
            List<string> tagList = new List<string>();
            foreach (MvAttr mvAttr in mvVideos_)
            {
                foreach (MvTagName tagName in mvAttr.tags)
                {
                    //if (tagDic.ContainsKey(tagName.tag))
                    //    continue;
                    //tagDic.Add(tagName.tag, tagName.name);
                    if (!tagList.Contains(tagName.name))
                        tagList.Add(tagName.name);
                }
            }
            TagsHave = tagList.ToArray();
        }
        get
        {
            return mvVideos_;
        }
    }

    public KaraokeCfg(MvAttr[] MvVideos)
    {
        mvVideos = MvVideos;
        //Dictionary<int, string> tagDic = new Dictionary<int, string>();
        //List<string> tagList = new List<string>();
        //foreach(MvAttr mvAttr in mvVideos)
        //{
        //    foreach(MvTagName tagName in mvAttr.tags)
        //    {
        //        //if (tagDic.ContainsKey(tagName.tag))
        //        //    continue;
        //        //tagDic.Add(tagName.tag, tagName.name);
        //        if (!tagList.Contains(tagName.name))
        //            tagList.Add(tagName.name);
        //    }
        //}
        //TagsHave = tagList.ToArray();
    }
}

#endregion

#region UI service have

[DataContract]
public class Service
{
    //private string name_;
    [DataMember(Order = 0)]
    public string name;

    //private int payType_;
    [DataMember(Order = 1)]
    public int payType;//0：free,1:payment without time count,2:payment with time count


    //private string servicePanel_;
    [DataMember(Order = 2)]
    public string servicePanel;

    //private string cmd_;
    [DataMember(Order = 3)]
    public string cmd;

    public Service(string Name, string Cmd, string ServicePanel=null,int PayType = 0)
    {
        name = Name;
        cmd = Cmd;
        servicePanel = ServicePanel;
        payType = PayType;
    }
}

#endregion

#region message type for dispatch
[DataContract]
public class MessageType
{
    [DataMember(Order = 0)]
    public string sender;

    [DataMember(Order = 1)]
    public string senderMessage;

    [DataMember(Order = 2)]
    public string recipient;

    [DataMember(Order = 3)]
    public string dispatchMesage;

    public MessageType(string sender_,string senderMessage_,
        string recipient_,string dispatchMessage_)
    {
        sender = sender_;
        senderMessage = senderMessage_;
        recipient = recipient_;
        dispatchMesage = dispatchMessage_;
    }
}
#endregion

[DataContract]
public class NetCfg
{
    [DataMember(Order = 0)]
    public NetBase.NetEnd webServer = new NetBase.NetEnd("127.0.0.1",9091);

    [DataMember(Order = 1)]
    public NetBase.NetEnd tcpServer = new NetBase.NetEnd("127.0.0.1", 8081);
}

[DataContract]
public class DeviceInfo
{
    [DataMember(Order = 0)]
    public string equipmentID;

    [DataMember(Order = 1)]
    public string userID;

    [DataMember(Order = 2)]
    public string equipmentTAG;

    [DataMember(Order = 3)]
    public string scrnSoftName = "audacity.exe";

    [DataMember(Order = 4)]
    public string scrnSoftPath = @"E:\1toolsInstall\Audacity\audacity.exe";

    [DataMember(Order = 5)]
    public int waitPayTime = 60 * 1000;

    [DataMember(Order = 6)]
    public string backGroudPic = "";

    [DataMember(Order = 7)]
    public string ServiceListFolder = "";

    [DataMember(Order = 8)]
    public string productListFolder = "";

    [DataMember(Order = 9)]
    public bool isLocalVoice = true;

    [DataMember(Order = 10)]
    public int waitConnectTime = 60 * 1000;

    public DeviceInfo(string equipmentId,string userId,string equipmentTag)
    {
        equipmentID = equipmentId;
        userID = userId;
        equipmentTAG = equipmentTag;
    }
}

[DataContract]
public class CfgJson
{
    [DataMember(Order = 0)]
    public NetCfg netcfg = new NetCfg();

    [DataMember(Order = 1)]
    public DeviceInfo deviceinfo = new DeviceInfo("100007", "YYSB", "YYSB");

    [DataMember(Order = 2)]
    public KaraokeCfg karaokeCfg = new KaraokeCfg(new MvAttr[] {
        new MvAttr("宇多田光0.mp4", new string[] { "流行","布鲁斯","JPOP","日语","美声" })
    });

    //提供的服务列表
    [DataMember(Order = 3)]
    public List<Service> serviceList = new List<Service>();

    //转发消息列表
    [DataMember(Order = 4)]
    public List<MessageType> dispatchMessList = new List<MessageType>();

    //人物动作视频字典
    [DataMember(Order = 5)]
    public VideosPath charactorVideos = new VideosPath(new string[]{@"F:\1.2UnityDev\WRSDActionVideos"});

    //广告视频字典
    [DataMember(Order = 6)]
    public VideosPath adverVideos = new VideosPath(new string[] { @"F:\1.2UnityDev\WRSDAdvVideos" });

    public CfgJson() {
        createServiceList();
        createDispatchList();
        createVideoDic();
    }


    public CfgJson(string cfgPath)
    {
        lock(this)
        {
            string cfgstr = null;
            CfgJson cj = new CfgJson();
            if (File.Exists(cfgPath))
            {
                cfgstr = File.ReadAllText(cfgPath);
                cj = JsonHelper.JsonToObject<CfgJson>(cfgstr);
                //karaokeCfg = cj.karaokeCfg;
                set(cj);
                //File.WriteAllText(cfgPath, cfgstr);
            }
            charactorVideos.checkVideos();
            adverVideos.checkVideos();
            get(cj);
            cfgstr = JsonHelper.ObjectToJSON<CfgJson>(cj);
            File.WriteAllText(cfgPath, cfgstr);
        }
    }

    public void set(CfgJson kcf)
    {
        karaokeCfg = kcf.karaokeCfg;
        serviceList = kcf.serviceList;
        dispatchMessList = kcf.dispatchMessList;
        charactorVideos = kcf.charactorVideos;
        adverVideos = kcf.adverVideos;
        netcfg = kcf.netcfg;
        deviceinfo = kcf.deviceinfo;
    }

    public void get(CfgJson kcf)
    {
        kcf.karaokeCfg = karaokeCfg;
        kcf.serviceList = serviceList;
        kcf.dispatchMessList = dispatchMessList;
        kcf.charactorVideos = charactorVideos;
        kcf.adverVideos = adverVideos;
        kcf.netcfg = netcfg;
        kcf.deviceinfo = deviceinfo;
    }

    /// <summary>
    /// 创建服务列表，和触发服务的对应的关键字
    /// </summary>
    private void createServiceList()
    {
        //Service toMainUIService = new Service("主界面", "toMain");//响应
        //Service screenProService = new Service("投屏", "screenPro", "ScreenProPanel", 1);//计时收费
        addService("返回主界面", "toMain");////响应返回主界面请求
        addService("向上滑动", "toleftBtn");////响应向上滑动请求
        addService("向下滑动", "toleftBtn");////响应向下滑动请求
        addService("向左滑动", "toleftBtn");////响应向左滑动请求
        addService("向右滑动", "toRightBtn");////响应向右滑动请求
        addService("确认选择", "select");////响应确认滑动请求

        addService("请求投屏", "screenPro", "ScreenProPanel", 1);////投屏计时收费服务
        addService("投屏结束", "close");
        addService("进入卡乐影院", "film", null, 1);////看电影计时收费服务
        addService("进入卡乐唱吧", "karaoke", null, 1);////唱歌计时收费服务

        addService("应急知识服务", "Emgcyknowledge", null, 0);//应急知识服务(免费)
        addService("本地地图服务", "LocalMap", null, 0);//本地地图服务(免费)
        addService("救援电话服务", "EmgcyPhone", null, 1);//看电影计时收费服务
        addService("紧急呼叫服务", "EmgcyCall", null, 1);//唱歌计时收费服务
    }

    private void addService(string Name, string Cmd, string ServicePanel = null, int PayType = 0)
    {
        Service tempService = new Service(Name, Cmd, ServicePanel, PayType);
        serviceList.Add(tempService);
    }

    /// <summary>
    /// 创建通讯转发列表
    /// </summary>
    private void createDispatchList()
    {
        addDispatchMes("IMG_AI","ComeIn", "VOICE_AI", "startgame");//客户光临
        addDispatchMes("IMG_AI", "Leave", "VOICE_AI", "endgame");//客户离开
        addDispatchMes("IMG_AI", "Regular", "VOICE_AI", "IDRegularCustomer");//熟客光临
        addDispatchMes("HTTP_SERVER", "PayScs", "VOICE_AI", "paymentsuccess");//支付成功
        addDispatchMes("HTTP_SERVER", "PayFailed", "VOICE_AI", "payedfailed");//支付失败
        addDispatchMes("UI", "PayCancel", "VOICE_AI", "cancelpayment");//支付取消
        addDispatchMes("TCP_SERVER", "KaraokeOver", "VOICE_AI", "karaokeOver");//结束K歌
        addDispatchMes("WEB_CLIENT", "ScreenOver", "VOICE_AI", "screenOver");//结束投屏
    }

    private void addDispatchMes(string sender_, string senderMessage_,
        string recipient_, string dispatchMessage_)
    {
        MessageType tempMessage = new MessageType(sender_, senderMessage_,
            recipient_, dispatchMessage_);
        dispatchMessList.Add(tempMessage);
    }

    /// <summary>
    /// 创建默认参数（人物的动作视频，广告视频等）
    /// </summary>
    private void createVideoDic()
    {
        charactorVideos.addFold("WRSD_A基本动作");
        charactorVideos.addFold("WRSD_B说话类");
        charactorVideos.addFold("WRSD_C转场动画");
        charactorVideos.addFold("WRSD_D招呼类");
        charactorVideos.addFold("WRSD_E疑问类");
        charactorVideos.addFold("WRSD_F可爱类");
        charactorVideos.addFold("WRSD_G特殊动作");

        adverVideos.addFold("可乐");
    }
}

public class ServiceManager : MonoBehaviour
{
    #region members
    const string cfgFilr = "param.cfg";
    public static CfgJson jsonParam = new CfgJson(cfgFilr);

    public string netAttachComponentName = "LipSyncInterface";
    private NetBase ServerApiScript = null;//websocketserver
    private HttpClient httpClient = null;
    private Tcpclient tcpclient = null;

    public bool isUseMenuCtr = true;
    public string menuCtrAttachName = "Canvas";
    private MenuControl menuCtr = null;
    private UnstoreMenuCtr unstoreCtr = null;

    private AutoResetEvent payOKEvent_;//handle QR pay event
    private bool payscreenOK_ = false;
    private bool paykaraokeOK_ = false;
    private AutoResetEvent screenConnectEvent_;
    private bool isConnect_ = false;

    private AutoResetEvent ReShowYNEvent_;//继续或取消
    private int ReShowYNTime = jsonParam.deviceinfo.waitPayTime+5000; //继续或取消等待时间

    private string curServiceIndex = "";
    
    #endregion
    // Start is called before the first frame update
    void Start()
    {
        payOKEvent_ = new AutoResetEvent(true);//
        ReShowYNEvent_ = new AutoResetEvent(true);//
        screenConnectEvent_ = new AutoResetEvent(true);//

        //jsonParam = new CfgJson(cfgFilr);
        if (ServerApiScript == null)
        {
            ServerApiScript = GameObject.Find(netAttachComponentName)?.GetComponent<WSServer>();
        }
        if (ServerApiScript != null)
        {
            //register websocketserver message handlers
            ServerApiScript.run(jsonParam.netcfg.webServer.ip, jsonParam.netcfg.webServer.port);
            ServerApiScript.vtuberVocieDelegate += this.vtuberVoiceHandle;
            ServerApiScript.customerVocieDelegate += this.customerVoiceHandle;
            ServerApiScript.customerImgDelegate += this.customerImgHandle;
            ServerApiScript.scrnProStrDelegate += this.scrnProStrHandle;
            ServerApiScript.msgDelegate += this.msgReceiveHandle; 
            
        }
        if(httpClient == null)
        {
            httpClient = GameObject.Find(netAttachComponentName).GetComponent<HttpClient>();
        }
        if(httpClient != null)
        {
            //register httpClient message handlers
        }
        if(tcpclient == null)
        {
            tcpclient = GameObject.Find(netAttachComponentName).GetComponent<Tcpclient>();
        }
        if(tcpclient != null)
        {
            tcpclient.run(jsonParam.netcfg.tcpServer.ip, jsonParam.netcfg.tcpServer.port);
            //register tcpclient message handlers
            tcpclient.msgPayDelegate += this.msgPayHandler;
            Task.Run(() =>
            {
                Task.Delay(1000);
                tcpclient.login(jsonParam.deviceinfo.userID);
            });
        }
        if(!isUseMenuCtr)
        {
            return;
        }
        if (menuCtr == null)
        {
            menuCtr = GameObject.Find(menuCtrAttachName)?.GetComponent<MenuControl>();
        }
        if(menuCtr != null)
        {
            menuCtr.mesDispatch_ += this. mesUIDispatchHandle;
            menuCtr.getInfoAction += GetInfoeHandle;
            menuCtr.getQRCodeAction += GetQRCodeHandle;
            menuCtr.karaokeAction += OnStartKaroke;
            menuCtr.screenProAction += OnStartScreenProj;
            menuCtr.localMapAction += this.OnRescueService;
        }
        if(unstoreCtr == null)
        {
            unstoreCtr = GameObject.Find(menuCtrAttachName)?.GetComponent<UnstoreMenuCtr>();
        }
        if(unstoreCtr !=null)
        {
            unstoreCtr.mesDispatch_ += this.mesUIDispatchHandle;
            unstoreCtr.getInfoAction += GetInfoeHandle;
            unstoreCtr.getQRCodeAction += GetQRCodeHandle;
            unstoreCtr.dealServiceAction += OnDealServiceFlow;

            unstoreCtr.buyGoodsFlowAction += OnBuyGoodsFlow;
            unstoreCtr.askKTVAction += OnStartKaraokeFlow;
            unstoreCtr.askScreenProAction += OnStartScreenProFlow;
        }
    }


    #region serviceHandle

    #region 1.1测试消息转发
    private void mesUIDispatchHandle(string recipient, string mes)
    {
        MessageType mestmp = new MessageType("", "", recipient, mes);
        messageDispatch(mestmp);
    }
    #endregion
    #region 1.2测试接受内容
    private void msgReceiveHandle(string mes)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(msgReceiveCoroutine(mes));
    }

    IEnumerator msgReceiveCoroutine(string msg)
    {
        //处理消息显示
        if(menuCtr != null)
            menuCtr.ShowMsgDebug(msg);
        yield return null;
    }
    #endregion
    #region 1.3 测试和后端及总控通讯
    private void GetQRCodeHandle()
    {
        //to do 进入卡乐唱吧
        //payOKEvent_.Reset();
        //to do 请求投屏
        //to do 购物
        //1.http获取项目信息
        var proinfo = httpClient?.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http获取二维码路径
        var qrInfo = httpClient?.getQRCode(proinfo["data"]["egoodList"][0]["id"].ToString());

        if (unstoreCtr != null)
            unstoreCtr.OnShowQRCode(qrInfo.msg);

        menuCtr?.OnShowQRCode(qrInfo.msg);
   
    }

    private void GetInfoeHandle()
    {
        //to do 进入卡乐唱吧
        //payOKEvent_.Reset();
        //to do 请求投屏
        //1.http获取项目信息
        var proinfo = httpClient?.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http获取二维码路径
        //var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[0].id);

        //menuCtr.OnShowQRCode(qrInfo.msg);
    }
    #endregion

    #region 2.投屏及图像语音消息处理
    private void msgPayHandler(NetBase.MsgPay mp)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(msgPayCoroutine(mp));
    }

    private IEnumerator msgPayCoroutine(NetBase.MsgPay mp)
    {
        if (mp.instruct == "100001")//付费成功开锁
        {
            if(mp.goodNo == "0" && mp.type == "1")//K歌，支付成功标志
            {
                Debug.LogWarning("K歌支付成功");
                paykaraokeOK_ = true;
                payOKEvent_.Set();
                OnPayKaraokeOKHandle();
            }
            if (mp.goodNo == "1" && mp.type == "1")//投屏，支付成功标志
            {
                Debug.LogWarning("投屏支付成功");
                payscreenOK_ = true;
                payOKEvent_.Set();
                OnPayScreenOKHandle();
            }

            //if (mp.goodNo == "2" && mp.type == "1")//应急救援
            //{
            //    //payOKEvent_.Set();
            //    OnRescueService();
            //}
        }
        if (mp.instruct == "100002")//关锁
        {
            Debug.LogWarning("100002指令触发");
            if(mp.goodNo == "1")//投屏费用耗尽关闭投屏
            {
                //to do 结束投屏
                //OnCloseScreenProj();
                closeExe(jsonParam.deviceinfo.scrnSoftName);//关屏
                MessageType mes = getMessageType("ScreenOver");//告诉语音投屏结束
                messageDispatch(mes);//告诉语音投屏结束
                if (unstoreCtr != null && unstoreCtr.getCurrentUIState() != PanelsCtrlApi.UIState.RESCUE_UI)
                {
                    unstoreCtr?.OnToMainUI();
                }
            }
        }
        if (mp.instruct == "100006")//结束租用，暂不会接受此类型信号
        {
            // go to main UI;
            if (mp.goodNo == "0" && mp.type == "1")//结束K歌成功
            {
                //if (menuCtr != null)
                //menuCtr?.OnToMainUI();
                Debug.LogWarning("结束K歌");
                menuCtr?.OnStartBtn();
                unstoreCtr?.OnToMainUI();
                //unstoreCtr?.OnToMainUI();
                MessageType mes = getMessageType("KaraokeOver");//结束K歌消息
                messageDispatch(mes);//处理事件转发，例如人来，人走
            }
            if(mp.goodNo == "2")
            {
                Debug.LogWarning("应急救援结束");
                menuCtr?.OnStartBtn();
                unstoreCtr?.OnToMainUI();
            }
        }
        if(mp.instruct == "100008")
        {
            Debug.LogWarning("应急救援触发");
            OnRescueService();
            OnRescueFlow();
        }
        yield return null;
    }

    private void scrnProHandle(NetBase.ScreenProjection sp)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(scrnProCoroutine(sp));
    }

    IEnumerator scrnProCoroutine(NetBase.ScreenProjection sp)
    {
        Service currentService = getService(sp.ScreenPro);
        serviceHandle(currentService);//处理和界面交互的服务，列如向上向下，选择

        yield return null;
    }

    private void scrnProStrHandle(string sp)
    {
        Debug.LogWarning("receive  screen soft message:"+sp);
        if(sp.Contains("start"))
        {
            screenConnectEvent_.Reset();
            isConnect_ = false;
            Debug.LogWarning("投屏软件启动");
            Task.Run(() => {
            if(!screenConnectEvent_.WaitOne(jsonParam.deviceinfo.waitConnectTime))
                {
                    if(!isConnect_)
                    {
                        Debug.LogWarning("超时未连接关闭投屏");
                        unstoreCtr?.OnToMainUI();
                        OnCloseScreenProj();
                    }
                }
            });
        }
        if(sp.Contains("connect"))
        {
            isConnect_ = true;
            screenConnectEvent_.Set();
            Debug.LogWarning("投屏连接");
        }
        if (!sp.Contains("close"))
            return;

        Debug.LogWarning("投屏关闭信号");
        UnityMainThreadDispatcher.Instance().Enqueue(scrnProStrCoroutine(sp));
    }

    IEnumerator scrnProStrCoroutine(string sp)
    {
        Service currentService = getService("close");
        serviceHandle(currentService);//处理和界面交互的服务，列如向上向下，选择
        yield return null;
    }
    private void customerImgHandle(NetBase.PeopleImg pi)
    {
        //Service currentService = getService(pi.FACE_DATA.action);
        //serviceHandle(currentService);
        UnityMainThreadDispatcher.Instance().Enqueue(customerImgCoroutine(pi));
    }

    IEnumerator customerImgCoroutine(NetBase.PeopleImg pi)
    {
        Service currentService = getService(pi.FACE_DATA.action);
        bool isIgnore = false;
        if(unstoreCtr!=null&&unstoreCtr.IsIgnoredAI())
        {
            if (!(currentService.name == "向左滑动" || currentService.name == "向右滑动"))
            {
                isIgnore = true;
            }
        }
        if (!isIgnore)
        {
            serviceHandle(currentService);//处理和界面交互的服务，列如向上向下，选择
            MessageType mes = getMessageType(pi.FACE_DATA.action);
            messageDispatch(mes);//处理事件转发，例如人来，人走
        }
        Debug.LogWarning("Ignore the img AI message!");
        yield return null;
    }

    private void customerVoiceHandle(NetBase.PeopleVoice pv)
    {
        if (unstoreCtr != null && unstoreCtr.IsIgnoredAI())
        {
            Debug.LogWarning("Ignore the customerVoice message!");
            return;
        }
        //throw new NotImplementedException();
        //Service customerSay = new Service("顾客说", "customer");
        //serviceHandle(customerSay, pv.VOICE_DATA.TEXT.word);//先显示说话内容
        //
        //Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        //serviceHandle(currentService);
        UnityMainThreadDispatcher.Instance().Enqueue(customerVoiceCoroutine(pv));
    }

    IEnumerator customerVoiceCoroutine(NetBase.PeopleVoice pv)
    {
        //throw new NotImplementedException();
        //Service customerSay = new Service("顾客说", "customerSpeak");
        //serviceHandle(customerSay, pv.VOICE_DATA.TEXT.word);//先显示说话内容

        Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        serviceHandle(currentService, pv.VOICE_DATA.TEXT.word);
        yield return null;
    }

    private void vtuberVoiceHandle(NetBase.PeopleVoice pv)
    {
        if (unstoreCtr != null && unstoreCtr.IsIgnoredAI())
        {
            Debug.LogWarning("Ignore the vtuberVoice message!");
            return;
        }
        //throw new NotImplementedException();
        //Service vtuberSay = new Service("小卡说", "xiaoka");
        //serviceHandle(vtuberSay, pv.VOICE_DATA.TEXT.word, pv.VOICE_DATA.VOICE_PATH);//先显示说话内容
        //
        //Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        //serviceHandle(currentService);
        UnityMainThreadDispatcher.Instance().Enqueue(vtuberVoiceCoroutine(pv));
    }

    private IEnumerator vtuberVoiceCoroutine(NetBase.PeopleVoice pv)
    {
        //throw new NotImplementedException();

        //Service vtuberSay = new Service("小卡说", "aiSpeak");
        //serviceHandle(vtuberSay, pv.VOICE_DATA.TEXT.word, pv.VOICE_DATA.VOICE_PATH);//先显示说话内容

        Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        serviceHandle(currentService, pv.VOICE_DATA.TEXT.word, pv.VOICE_DATA.VOICE_PATH);
        yield return null;
    }
    #endregion
    private MessageType getMessageType(string senderCmd)
    {
        if (jsonParam.dispatchMessList == null)
            return null;
        MessageType mes = null;
        foreach (var mesTmp in jsonParam.dispatchMessList)
        {
            if (mesTmp.senderMessage == senderCmd)
            {
                mes = mesTmp;
            }
        }
        return mes;
    }

    private void messageDispatch(MessageType mes)
    {
        if (mes == null)
            return;

        if (mes.recipient == "IMG_AI" || mes.recipient == "VOICE_AI")
            ServerApiScript.sendMessage(mes.recipient, mes.dispatchMesage);

        //if(mes.senderMessage == "ComeIn")
        //{
        //    unstoreCtr?.SetCurVIPanelName(PanelsCtrlApi.ShoppingPanel);
        //}
        if (mes.senderMessage == "Leave")
        {
            if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.APPLoadPanel)
                unstoreCtr?.OnLeave();
        }
    }

    private Service getService(string cmd)
    {
        Service service = null;
        foreach(var serviceTmp in jsonParam.serviceList)
        {
            if(serviceTmp.cmd == cmd)
            {
                service = serviceTmp;
            }
        }
        return service;
    }

    private void openExe(string path)
    {
        if(!System.IO.File.Exists(path))
        {
            Debug.LogError("cannot found such file at :" + path);
            return;
        }
        var process = new System.Diagnostics.Process();
        process.StartInfo.FileName = path;
        process.StartInfo.Arguments = null;
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.RedirectStandardInput = false;
        process.StartInfo.RedirectStandardOutput = false;
        process.StartInfo.RedirectStandardError = false;
        process.StartInfo.CreateNoWindow = false;
        process.Start();//启动
    }

    private void closeExe(string name)
    {
        var p = System.Diagnostics.Process.GetProcesses();
        try
        {
            foreach (var pt in p)
            {
                if (name.Contains(pt.ProcessName))
                {
                    pt.Kill();
                }
            }
        }
        catch(Exception ex)
        {
            Debug.LogError("close " + name + " with exception:" + ex.Message);
        }
    }

    private void serviceHandle(Service service,string info = null,string path = null)
    {
        if (service == null)
            return;
        #region 1.基本对话动作交互服务
        if (service.name == "开始")
        {
            //to do 返回主界面
            //menuCtr.openMainUIPanel();
            menuCtr?.OnStartBtn();
            unstoreCtr?.OnStartBtn();
        }
        if (service.name == "停止")
        {
            //to do 返回主界面
            //menuCtr.openMainUIPanel();
            //if (menuCtr != null)
            menuCtr?.OnStopBtn();
            unstoreCtr?.OnStopBtn();
        }
        if (service.name == "返回主界面")
        {
            //to do 返回主界面
            //menuCtr.openMainUIPanel();
            menuCtr?.OnToMainUI();
            unstoreCtr?.OnToMainUI();
        }
        #region gesture Service
        if (service.name == "向左滑动")
        {
            //to do 向左滑动,向前选择
            //if (menuCtr != null)
            menuCtr?.OnPreBtn();
            unstoreCtr?.OnLeftGestureHandle();
        }
        if (service.name == "向右滑动")
        {
            //to do 向右滑动，向后选择
            menuCtr?.OnNextBtn();
            unstoreCtr?.OnRrightGestureHandle();
        }
        if (service.name == "确认选择")
        {
            //to do 确认选择
            menuCtr?.OnSelect();
            unstoreCtr?.OnSureGestureHandle();
        }
        #endregion
        if (service.name == "顾客说")
        {
            //to do 顾客说话
            //if (menuCtr != null)
            menuCtr?.OnCustomerSay(info);
            unstoreCtr?.OnCustomerSay(info);
        }
        if (service.name == "小卡说")
        {
            //to do 小卡说话
            menuCtr?.OnXiaokaSay(info, path);
            unstoreCtr?.OnXiaokaSay(info, path);
        }

        if(service.name == "打招呼")
        {
            // to  do greet sombody and speak
            unstoreCtr?.OnGreetBtn();
            //unstoreCtr?.SetLastVIPanelName(PanelsCtrlApi.ShoppingPanel);
        }
        #endregion

        #region 2.收费服务(大屏项目，投屏和卡乐唱吧)
        if (service.name == "请求投屏")
        {
            //OnStartScreenProj();
            //OnStartKaraokeFlow(num, goodsName);
            OnStartScreenProFlow(-1, "投屏");
        }
        if (service.name == "进入卡乐影院")
        {
            //to do 进入卡乐影院
        }
        if (service.name == "进入卡乐唱吧")
        {
            //OnStartKaroke();
            OnStartKaraokeFlow(-1, "K歌");
        }
        if(service.name == "投屏结束")
        {
            //to do 结束投屏
            OnCloseScreenProj();
            unstoreCtr?.OnToMainUI();
        }
        #endregion

        #region 3.应急服务(大屏项目)
        if(service.name == "应急救援")
        {
            //notice ui to display rescue panel;
            OnRescueService();
            OnRescueFlow();
        }
        if (service.name == "应急知识服务")
        {
            //to do 进入卡乐影院
        }
        if (service.name == "本地地图服务")
        {
            OnRescueService();
            OnRescueFlow();
        }
        if (service.name == "救援电话服务")
        {
            //to do 顾客说话
        }
        if (service.name == "紧急呼叫服务")
        {
            //to do 小卡说话
        }
        #endregion

        #region 4.无人商店项目(购物和趣味互动)
        if(service.name == "购物或互动")
        {
            // notice ui display shopping panel,跳转到购物或界面
            unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
            unstoreCtr?.ResetStage();
        }
        if (service.name == "购物")
        {
            // notice ui display food panel,跳转到食品饮料界面
            //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ShoppingPanel)
            //unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.FOOD_DRINK);
            if(unstoreCtr!=null && unstoreCtr.GetCurChoosePanelName()==PanelsCtrlApi.ShoppingPanel)
            {
                unstoreCtr?.OnServiceBtn();
            }
        }
        //if (service.name == "食品")
        //{
        //    //notice ui to display boxlist panel for food
        //    //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.FoodPanel)
        //    unstoreCtr?.OnProductBtn();
        //}
        //if (service.name == "饮料")
        //{
        //    //notice ui to display boxlist panel for drinking
        //    //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.FoodPanel)//服务
        //        unstoreCtr?.OnServiceBtn();
        //}
        if (service.name == "商品购买")
        {
            // to do showing yes or no panel
            //unstoreCtr?.OnShowUIBtnState(UnstorePanelCtr.UIBtnState.YES_NO);
            if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.BoxListPanel)
            {
                //旋转处理
                
                unstoreCtr?.OnMoveToTop(info);
                OnSetDealServiceIndex(info);
            }
        }
        if (service.name == "是否继续")
        {
            //notice ui to display YesOrNo panel
            if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.BoxListPanel)
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
        }
        if (service.name == "是继续")
        {
            //to do select
            // to do get qr code and purchase
            //OnBuyGoodsFlow();
            // notice ui display food panel,跳转到食品饮料界面

            var LastVIname = unstoreCtr?.GetLastVIPanelName();

            if (LastVIname == PanelsCtrlApi.YesOrNoPanel)
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
            if(LastVIname == PanelsCtrlApi.BoxListPanel)
            {
                //var boxType = unstoreCtr?.GetBoxType();
                //if (boxType == UnstoreBoxListCtrApi.BoxListType.Service || boxType == UnstoreBoxListCtrApi.BoxListType.GoodsType)
                //{
                //    OnDealServiceFlow();
                //}
                //else
                //{
                //    OnBuyGoodsFlow();
                //}
                unstoreCtr?.OnSelect();
            }
            if(LastVIname == PanelsCtrlApi.OKroNotPanel)
            {
                unstoreCtr?.OnDownLoadAPP();
            }

            //unstoreCtr?.OnSampleShopPlay();//播放选择动作
        }
        if (service.name == "否继续")
        {
            //to do cancel
            var LastVIname = unstoreCtr?.GetLastVIPanelName();

            if (LastVIname == PanelsCtrlApi.YesOrNoPanel || LastVIname == PanelsCtrlApi.BoxListPanel)
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
                //unstoreCtr?.OnToMainUI();
            if (LastVIname == PanelsCtrlApi.OKroNotPanel)
                unstoreCtr?.OnChatActShort();
        }
        if (service.name == "趣味互动")
        {
            if (unstoreCtr?.GetRandChaneStage() == service.name)
                unstoreCtr?.setCurRootPath(jsonParam.charactorVideos.foldPath[1]);
            unstoreCtr?.OnChatAct();
            unstoreCtr?.SetLastVIPanelName("趣味互动");
        }
        if (service.name == "互动长聊天1" )
        {
            if (unstoreCtr?.GetRandChaneStage() == service.name)
                unstoreCtr?.setCurRootPath(jsonParam.charactorVideos.foldPath[1]);
            //互动聊天（长）和变身萌宠（随机） 二选一
            if (unstoreCtr?.GetLastVIPanelName() == "趣味互动")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnRelyChat();
                unstoreCtr?.SetLastVIPanelName("互动长聊天1");
            }
        }
        if (service.name == "互动长聊天1b")
        {
            //互动聊天（长）和变身萌宠（随机） 二选一
            if (unstoreCtr?.GetLastVIPanelName() == "趣味互动")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnNoRelyChat();
                unstoreCtr?.SetLastVIPanelName("互动长聊天1b");
            }
        }
        if (service.name == "互动长聊天1c")
        {
            //互动聊天（长）和变身萌宠（随机） 二选一
            if (unstoreCtr?.GetLastVIPanelName() == "趣味互动")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnErrRelyChat();
                unstoreCtr?.SetLastVIPanelName("互动长聊天1c");
            }
        }
        if (service.name == "互动长聊天2")
        {
            if (unstoreCtr?.GetLastVIPanelName() == "互动长聊天1")
            {
                unstoreCtr?.OnChatActLong2();
                unstoreCtr?.SetLastVIPanelName("互动长聊天2");
            }
        }
        if(service.name == "K歌或投屏")
        {
            unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.KTVSCREEN);
        }
        if (service.name == "GAME")
        {
            var LastVIname = unstoreCtr?.GetLastVIPanelName();

            //if (LastVIname == "直播" || LastVIname == "短视频")
            //{
            //    unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.GAMECH);
            //}
            //if (LastVIname == PanelsCtrlApi.GameTitlePanel)
            {
                var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(4) - 0.45f);
                unstoreCtr?.OnShowGameTitle((int)randNum);
            }
        }
        //if (service.name == "GAME1")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.GameChoosePanel)
        //    {
        //        unstoreCtr?.OnShowGameTitle(1);
        //    }
        //}
        //if (service.name == "GAME2")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.GameChoosePanel)
        //    {
        //        unstoreCtr?.OnShowGameTitle(2);
        //    }
        //}
        if (service.name == "互动短聊天")
        {
            // 视频序列播放
            var LastVIname = unstoreCtr?.GetLastVIPanelName();
            if (LastVIname == PanelsCtrlApi.GameTitlePanel)
            {
                //unstoreCtr?.OnChatActShort();

                unstoreCtr?.SetLastVIPanelName("互动短聊天");
            }
        }
        if (service.name == "直播或视频")
        {
            //if (unstoreCtr?.GetLastVIPanelName() == "互动长聊天2")
            //{
            //    unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.ZHIBOCH);
            //}
            var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(3) - 0.45f);
            if((int)randNum ==0)
            {
                unstoreCtr?.OnZhiboVR();
                unstoreCtr?.SetLastVIPanelName("直播");
            }
            else if ((int)randNum == 1)
            {
                unstoreCtr?.OnShortVideo();
                unstoreCtr?.SetLastVIPanelName("短视频");
            }
            else if ((int)randNum == 2)
            {
                unstoreCtr?.OnVRhost();
                unstoreCtr?.SetLastVIPanelName("虚拟主持人");
            }
        }
        //if (service.name == "直播")
        //{
        //    //VR直播和主持人 直播二选一
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnZhiboVR();
        //        unstoreCtr?.SetLastVIPanelName("直播");
        //    }
        //}
        //if (service.name == "短视频")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnShortVideo();
        //        unstoreCtr?.SetLastVIPanelName("短视频");
        //    }
        //}
        //if (service.name == "虚拟主持人")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnVRhost();
        //        unstoreCtr?.SetLastVIPanelName("虚拟主持人");
        //    }
        //}

        //
        #endregion

    }

    private void OnSetDealServiceIndex(string ServiceNum = "4")
    {
        curServiceIndex = ServiceNum;
    }
    private void OnBuyGoodsFlow(string goodsNum = "4")
    {
        int num = 3;
        if (curServiceIndex != "")
            goodsNum = curServiceIndex;
        if (IsNumber(goodsNum))
            num = Convert.ToInt32(goodsNum) - 1;

        var goodsName = unstoreCtr?.OnGetGoodsName(num);
        if(goodsName.Contains("应急"))
        {
            OnRescueFlow();
            return;
        }
        if(goodsName.Contains("K歌"))
        {
            OnStartKaraokeFlow(num, goodsName);
            return;
        }
        if (goodsName.Contains("投屏"))
        {
            OnStartScreenProFlow(num, goodsName);
            return;
        }

        //to do 请求投屏
        //1.http获取项目信息
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        ////2.http获取二维码路径
        //
        ////var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[1].id);
        //var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        //unstoreCtr?.OnShakeGoods(num, 1.0f);

        NetBase.QRCodeInfo qrInfo = new NetBase.QRCodeInfo();
        if (proinfo != null)
            qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        else
        {
            qrInfo.msg = "网络或服务器异常";
            qrInfo.code = -1;
        }
        if (num >= 0)
            unstoreCtr?.OnShakeGoods(num, 1.0f);
        ReShowYNEvent_.Reset();
        Task.Run(() =>
        {
            if (!ReShowYNEvent_.WaitOne(1500))
            {
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
                unstoreCtr?.OnCharactorEn(false);
                unstoreCtr?.OnCloseBoxList(1.0f);
            }
        });
        Task.Run(() =>
        {
            if (qrInfo.code == 200&&(!ReShowYNEvent_.WaitOne(3000)))
            {
                unstoreCtr?.OnBuyGoodsChatAct();
                menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime / 1000);
                unstoreCtr?.OnShowQRCode(qrInfo.msg, goodsName, jsonParam.deviceinfo.waitPayTime / 1000);
            }
        });
        if (qrInfo.code != 200)
        {
            Task.Run(() =>
            {
                Thread.Sleep(2 * 1000);
                float errShowTime = 10;
                menuCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", errShowTime);
                unstoreCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", errShowTime);
                //Task.Run(() =>
                //{
                //    //Thread.Sleep(10*1000);
                //    OnBuyFaileFlow();
                //});
                Invoke("OnBuyFaileFlow", errShowTime);
            });
            return;
        }
        
        payOKEvent_.Reset();
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime))
            {
                //支付成功进入投屏
                //1.打开投屏软件
                Debug.LogWarning("Buy goods pay success!");
                // to do check out goods action
                //转发支付成功消息
                var mes = getMessageType("PayScs");
                messageDispatch(mes);

                //unstoreCtr?.OnOpenMainUIPanel();
                //unstoreCtr?.OnShowPayShopingResult();
            }
            else
            {
                //转发支付失败消息
                var mes = getMessageType("PayFailed");
                messageDispatch(mes);
                //支付超时，回到主界面
                //unstoreCtr?.OnShowPayShopingResult(); 
                OnBuyFaileFlow();
            }
            
        });
        //Task.Run(() =>
        //{
        //    if (!ReShowYNEvent_.WaitOne(ReShowYNTime))
        //    {
        //        menuCtr?.OnToMainUI();
        //        unstoreCtr?.OnCloseAllPanel();
        //        unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
        //    }
        //});
        curServiceIndex = "";
    }

    private void OnBuyFaileFlow()
    {
        menuCtr?.OnToMainUI();
        unstoreCtr?.OnCloseAllPanel();
        unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
    }

    private void OnDealServiceFlow(string goodsNum = "4")
    {
        int num = 3;
        if (curServiceIndex != "")
            goodsNum = curServiceIndex;
        if (IsNumber(goodsNum))
            num = Convert.ToInt32(goodsNum) - 1;

        unstoreCtr?.OnShakeGoods(num, 1.0f);
        var ServiceName = unstoreCtr?.OnGetGoodsName(num);

        ReShowYNEvent_.Reset();
        Task.Run(() =>
        {
            if (!ReShowYNEvent_.WaitOne(1500))
            {
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
                unstoreCtr?.OnCharactorEn(false);
                unstoreCtr?.OnCloseBoxList(1.0f);
            }

        });
        Task.Run(() =>
        {
            if (!ReShowYNEvent_.WaitOne(3000))
            {
                switch (ServiceName)
                {
                    case "户外商品":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.OutDoor, ServiceName);

                        break;
                    case "生活用品":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.GoodsType, ServiceName);

                        break;
                    case "水电服务":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.WaterElec, ServiceName);

                        break;
                    case "卫浴服务":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Shower, ServiceName);

                        break;
                    case "应急服务":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Rescue, ServiceName);

                        break;
                    case "智能厨房":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Kitchen, ServiceName);
                        
                        break;
                    case "食品":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Food, ServiceName);
                        break;
                    case "饮料":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Drink, ServiceName);
                        break;
                    case "农特产品":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.AgriP, ServiceName);
                        break;
                }
            }

        });
        
        curServiceIndex = "";
    }

    /// <summary>
    /// 判断字符串是否是数字
    /// </summary>
    public static bool IsNumber(string s)
    {
        if (string.IsNullOrWhiteSpace(s)) return false;
        const string pattern = "^[0-9]*$";
        Regex rx = new Regex(pattern);
        return rx.IsMatch(s);
     }

    private void OnStartScreenProFlow(int num,string goodsName)
    {

        //to do 请求投屏
        //1.http获取项目信息
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http获取二维码路径

        //var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[1].id);
        NetBase.QRCodeInfo qrInfo = new NetBase.QRCodeInfo();
        if (proinfo != null)
           qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        else
        {
            qrInfo.msg = null;
            qrInfo.code = -1;
        }
        if (num >= 0)
            unstoreCtr?.OnShakeGoods(num, 1.0f);
        ReShowYNEvent_.Reset();
        Task.Run(() =>
        {
            if (!ReShowYNEvent_.WaitOne(1500))
            {
                //3.隐藏商品
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
                unstoreCtr?.OnCharactorEn(false);
                unstoreCtr?.OnCloseBoxList(1.0f);
            }
        });
        int waitYN = 3000;
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && (!ReShowYNEvent_.WaitOne(waitYN)))
            {
                unstoreCtr?.OnBuyGoodsChatAct();
                menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime / 1000);
                unstoreCtr?.OnShowQRCode(qrInfo.msg, goodsName, 10.0f, jsonParam.deviceinfo.waitPayTime / 1000);
            }

        });
        if (qrInfo.code != 200)
        {
            menuCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
            unstoreCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
            Invoke("OnPayFailedFlow", 10);
            return;
        }

        payOKEvent_.Reset();
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime+ waitYN))
            {
                //unstoreCtr?.OnCloseQRCode();
                ////支付成功进入投屏
                ////1.打开投屏软件
                //Debug.LogWarning("ScreeenPro pay success!");
                //// to do check out goods action
                ////转发支付成功消息
                //var mes = getMessageType("PayScs");
                //messageDispatch(mes);
                //OnOpenScreen();
                ////unstoreCtr?.OnOpenMainUIPanel();
                ////unstoreCtr?.OnShowPayShopingResult();
            }
            else
            {
                if (!payscreenOK_)
                {
                    ////转发支付失败消息
                    //var mes = getMessageType("PayFailed");
                    //messageDispatch(mes);
                    ////支付超时，进入聊天流程
                    ////unstoreCtr?.OnShowPayShopingResult();
                    //menuCtr?.OnToMainUI();
                    ////unstoreCtr?.OnCloseAllPanel();
                    ////unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
                    //unstoreCtr?.OnSwitchToGameChoosePanel();
                    OnPayFailedFlow();
                }
            }

        });

        curServiceIndex = "";
    }

    private void OnStartKaraokeFlow(int num, string goodsName)
    {

        //to do 请求投屏
        //1.http获取项目信息
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http获取二维码路径
        NetBase.QRCodeInfo qrInfo = new NetBase.QRCodeInfo();
        if (proinfo != null)
            qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][0]["id"].ToString());
        else
        {
            qrInfo.msg = null;
            qrInfo.code = -1;
        }
        if(num>=0)
            unstoreCtr?.OnShakeGoods(num, 1.0f);
        ReShowYNEvent_.Reset();
        Task.Run(() =>
        {
            if (!ReShowYNEvent_.WaitOne(1500))
            {
                //3.隐藏商品
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
                unstoreCtr?.OnCharactorEn(false);
                unstoreCtr?.OnCloseBoxList(1.0f);
            }
        });

        int waitYN = 3000;
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && (!ReShowYNEvent_.WaitOne(waitYN)))
            {
                unstoreCtr?.OnBuyGoodsChatAct();
                menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime / 1000);
                unstoreCtr?.OnShowQRCode(qrInfo.msg, goodsName,10.0f, jsonParam.deviceinfo.waitPayTime / 1000);
            }

        });
        if (qrInfo.code != 200)
        {
            menuCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
            unstoreCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
            Invoke("OnPayFailedFlow",10);
            return;
        }

        payOKEvent_.Reset();
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime+waitYN))
            {
                //unstoreCtr?.OnCloseQRCode();
                ////支付成功进入投屏
                //Debug.LogWarning("Karoke pay success!");
                //// to do check out goods action
                ////转发支付成功消息
                //var mes = getMessageType("PayScs");
                //messageDispatch(mes);
                //unstoreCtr?.OnShowKaraokePanel();
                ////unstoreCtr?.OnOpenMainUIPanel();
                ////unstoreCtr?.OnShowPayShopingResult();
            }
            else
            {
                if (!paykaraokeOK_)
                {
                    ////转发支付失败消息
                    //var mes = getMessageType("PayFailed");
                    //messageDispatch(mes);
                    ////支付超时，回到主界面
                    ////unstoreCtr?.OnShowPayShopingResult();
                    //menuCtr?.OnToMainUI();
                    ////unstoreCtr?.OnCloseAllPanel();
                    ////unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
                    //unstoreCtr?.OnSwitchToGameChoosePanel();
                    OnPayFailedFlow();
                }
            }
        });
        curServiceIndex = "";
    }

    private void OnPayFailedFlow()
    {
        //转发支付失败消息
        var mes = getMessageType("PayFailed");
        messageDispatch(mes);
        //支付超时，回到主界面
        //unstoreCtr?.OnShowPayShopingResult();
        menuCtr?.OnToMainUI();
        //unstoreCtr?.OnCloseAllPanel();
        //unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
        unstoreCtr?.OnSwitchToGameChoosePanel();
    }

    private void OnPayScreenOKHandle()
    {
        if(payscreenOK_)
        {
            unstoreCtr?.OnCloseQRCode();
            //支付成功进入投屏
            //1.打开投屏软件
            Debug.LogWarning("ScreeenPro pay success!");
            // to do check out goods action
            //转发支付成功消息
            var mes = getMessageType("PayScs");
            messageDispatch(mes);
            OnOpenScreen();
            payscreenOK_ = false;
        }
    }

    private void OnPayKaraokeOKHandle()
    {
        if(paykaraokeOK_)
        {
            unstoreCtr?.OnCloseQRCode();
            //支付成功进入投屏
            Debug.LogWarning("Karoke pay success!");
            // to do check out goods action
            //转发支付成功消息
            var mes = getMessageType("PayScs");
            messageDispatch(mes);
            unstoreCtr?.OnShowKaraokePanel();
            //unstoreCtr?.OnOpenMainUIPanel();
            //unstoreCtr?.OnShowPayShopingResult();
        }
    }

    private void OnRescueFlow()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnRescueServiceFlowCourtine());
    }

    private IEnumerator OnRescueServiceFlowCourtine()
    {
        if(unstoreCtr != null && unstoreCtr.getCurrentUIState() == PanelsCtrlApi.UIState.SCREENPRO_UI)
        {
            //1,如果处于投屏状态关闭投屏
            //OnCloseScreenProj();
            closeExe(jsonParam.deviceinfo.scrnSoftName);

            MessageType mes = getMessageType("ScreenOver");//告诉语音投屏结束
            messageDispatch(mes);//告诉语音投屏结束

            //sendCloseScreenToTcp();
            Invoke("sendCloseScreenToTcp", 1.0f);     
        }

        //2.打开应急地图
        unstoreCtr?.OnRescueService();
        try
        {
            MessageType mes = getMessageType("AskRescue");//告诉语音投屏结束
            messageDispatch(mes);//告诉语音投屏结束
        }
        catch (Exception ex)
        {
            Debug.LogError("send AskRescue Err with " + ex.Message);
        }
        yield return null;
    }

    private void OnCloseScreenProj()
    {
        //1.转发投屏结束消息
        NetBase.MsgPay msgConten = new NetBase.MsgPay(
            jsonParam.deviceinfo.equipmentID, "1", "100006");
        NetBase.SocketMessage sm = new NetBase.SocketMessage(
            jsonParam.deviceinfo.equipmentTAG,
            jsonParam.deviceinfo.userID,
            msgConten);
        string closeMsg = JsonHelper.ObjectToJSON<NetBase.SocketMessage>(sm);
        tcpclient.sendMessage(closeMsg);
        //2.关闭投屏软件
        closeExe(jsonParam.deviceinfo.scrnSoftName);

        MessageType mes = getMessageType("ScreenOver");//告诉语音投屏结束
        messageDispatch(mes);//告诉语音投屏结束
    }

    private void sendCloseScreenToTcp()
    {
        NetBase.MsgPay msgConten = new NetBase.MsgPay(
    jsonParam.deviceinfo.equipmentID, "1", "100006");
        NetBase.SocketMessage sm = new NetBase.SocketMessage(
            jsonParam.deviceinfo.equipmentTAG,
            jsonParam.deviceinfo.userID,
            msgConten);
        string closeMsg = JsonHelper.ObjectToJSON<NetBase.SocketMessage>(sm);
        tcpclient.sendMessage(closeMsg);
    }

    #region ServiceStationOldCode
    private void OnStartKaroke()
        {
            payOKEvent_.Reset();
            //to do 请求投屏
            //1.http获取项目信息
            var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
            //2.http获取二维码路径
            var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][0]["id"].ToString());
            if(qrInfo.code != 200)
            {
                //if (menuCtr != null)
                menuCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
                return;
            }
            menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime/1000);
            Task.Run(() =>
            {
    
                if (payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime))//等待支付成功
                {
                    //1.支付成功，等待K歌结束
                    Debug.LogWarning("Karoke pay success!");
                    var goodGoll = proinfo["data"]["egoodList"][0]["good"]["goodGoll"].ToObject<JObject>();
                    //if (menuCtr != null)
                    menuCtr?.OnShowPayResultPanel(goodGoll);
                    //转发支付成功消息
                    var mes = getMessageType("PayScs");
                    messageDispatch(mes);
                }
                else
                {
                    //2.支付超时，回到主界面
                    //转发支付失败消息
                    var mes = getMessageType("PayFailed");
                    messageDispatch(mes);
                    menuCtr?.OnToMainUI();
                }
            });
     }
    private void OnStartScreenProj()
    {
        payOKEvent_.Reset();
        //to do 请求投屏
        //1.http获取项目信息
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http获取二维码路径

        //var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[1].id);
        var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        if (qrInfo.code != 200)
        {
            menuCtr?.OnShowErro("获取二维码错误", qrInfo.msg, "请联系管理员", 10);
            return;
        }
        //if (menuCtr != null)
        menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime / 1000);
        Task.Run(() =>
        {
            if (payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime))
            {
                //支付成功进入投屏
                //1.打开投屏软件
                Debug.LogWarning("ScreeenPro pay success!");
                var goodGoll = proinfo["data"]["egoodList"][1]["good"]["goodGoll"].ToObject<JObject>();
                //转发支付成功消息
                var mes = getMessageType("PayScs");
                messageDispatch(mes);

                menuCtr?.OnShowPayResultPanel(goodGoll);
                openExe(jsonParam.deviceinfo.scrnSoftPath);
            }
            else
            {
                //支付超时，回到主界面
                //转发支付失败消息
                var mes = getMessageType("PayFailed");
                messageDispatch(mes);
                //if (menuCtr != null)
                menuCtr?.OnToMainUI();
            }
        });

    }
   
    private void OnRescueService()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnRescueServiceCourtine());
    }

    private IEnumerator OnRescueServiceCourtine()
    {

        menuCtr?.OnRescueService();

        try
        {
            MessageType mes = getMessageType("AskRescue");//告诉语音投屏结束
            messageDispatch(mes);//告诉语音投屏结束
        }
        catch (Exception ex)
        {
            Debug.LogError("send AskRescue Err with " + ex.Message);
        }
        yield return null;
    }

    private void OnOpenScreen()
    {
        //menuCtr?.OnShowScreenProPanel();
        //menuCtr?.OnShowProcessPanel();
        //var mes = getMessageType("PayScs");
        //messageDispatch(mes);
        //openExe(jsonParam.deviceinfo.scrnSoftPath);
        //Thread.Sleep(2 * 1000);
        //menuCtr?.OnCloseProcessPanel();
        //menuCtr?.OnShowScreenProPanel();
        UnityMainThreadDispatcher.Instance().Enqueue(OnOpenScreenCourtine());
    }

    IEnumerator OnOpenScreenCourtine()
    {
        //menuCtr?.OnShowScreenProPanel();
       // menuCtr?.OnShowProcessPanel();
        var mes = getMessageType("PayScs");
        messageDispatch(mes);
        openExe(jsonParam.deviceinfo.scrnSoftPath);
        Thread.Sleep(1 * 1000);
        //menuCtr?.OnCloseProcessPanel();
        //menuCtr?.OnShowScreenProPanel();
        unstoreCtr?.OnShowScreenProPanel();
        yield return null;
    }
    #endregion
#endregion
}
