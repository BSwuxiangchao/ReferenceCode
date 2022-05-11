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
    /// Jsonת���ɶ���
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
    /// ����ת����JSON
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
    /// ��ʽ��json��ʽ���ַ���
    /// </summary>
    /// <param name="str">δ��ʽ����json�ַ���</param>
    /// <param name="spacing">��ʽ���Ŀո����</param>
    /// <returns>��ʽ����json��ʽ�ַ���</returns>
    private static string ConvertJsonString(string str, int spacing = 4)
    {
        //��ʽ��json�ַ���
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
            if (!Directory.Exists(folder))//�ж���Ƶ�ļ��д��ڷ񣬲�������ɾ��
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
    public string name = "����";
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
        {0,"����" },
        {1,"����" },
        {2,"�ŵ�" },
        {3,"ҡ��" },
        {4,"��ҥ" },
        {5,"��ʿ" },
        {6,"��³˹" }
    };
}

//[DataContract]
//public class MvTag
//{
//    [DataMember(Order = 0)]
//    public MvTagName Others = new MvTagName(0,"����");//��������
//
//    [DataMember(Order = 1)]
//    public MvTagName pop = new MvTagName(1, "����");//����
//
//    [DataMember(Order = 2)]
//    public MvTagName classical = new MvTagName(2, "�ŵ�");//�ŵ�
//
//    [DataMember(Order = 3)]
//    public MvTagName rock = new MvTagName(3, "ҡ��");//ҡ��
//
//    [DataMember(Order = 4)]
//    public MvTagName ballad = new MvTagName(4, "��ҥ");//��ҥ
//
//    [DataMember(Order = 5)]
//    public MvTagName jazz = new MvTagName(5, "��ʿ");
//
//    [DataMember(Order = 6)]
//    public MvTagName blues = new MvTagName(6, "��³˹");
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
    public int payType;//0��free,1:payment without time count,2:payment with time count


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
        new MvAttr("������0.mp4", new string[] { "����","��³˹","JPOP","����","����" })
    });

    //�ṩ�ķ����б�
    [DataMember(Order = 3)]
    public List<Service> serviceList = new List<Service>();

    //ת����Ϣ�б�
    [DataMember(Order = 4)]
    public List<MessageType> dispatchMessList = new List<MessageType>();

    //���ﶯ����Ƶ�ֵ�
    [DataMember(Order = 5)]
    public VideosPath charactorVideos = new VideosPath(new string[]{@"F:\1.2UnityDev\WRSDActionVideos"});

    //�����Ƶ�ֵ�
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
    /// ���������б��ʹ�������Ķ�Ӧ�Ĺؼ���
    /// </summary>
    private void createServiceList()
    {
        //Service toMainUIService = new Service("������", "toMain");//��Ӧ
        //Service screenProService = new Service("Ͷ��", "screenPro", "ScreenProPanel", 1);//��ʱ�շ�
        addService("����������", "toMain");////��Ӧ��������������
        addService("���ϻ���", "toleftBtn");////��Ӧ���ϻ�������
        addService("���»���", "toleftBtn");////��Ӧ���»�������
        addService("���󻬶�", "toleftBtn");////��Ӧ���󻬶�����
        addService("���һ���", "toRightBtn");////��Ӧ���һ�������
        addService("ȷ��ѡ��", "select");////��Ӧȷ�ϻ�������

        addService("����Ͷ��", "screenPro", "ScreenProPanel", 1);////Ͷ����ʱ�շѷ���
        addService("Ͷ������", "close");
        addService("���뿨��ӰԺ", "film", null, 1);////����Ӱ��ʱ�շѷ���
        addService("���뿨�ֳ���", "karaoke", null, 1);////�����ʱ�շѷ���

        addService("Ӧ��֪ʶ����", "Emgcyknowledge", null, 0);//Ӧ��֪ʶ����(���)
        addService("���ص�ͼ����", "LocalMap", null, 0);//���ص�ͼ����(���)
        addService("��Ԯ�绰����", "EmgcyPhone", null, 1);//����Ӱ��ʱ�շѷ���
        addService("�������з���", "EmgcyCall", null, 1);//�����ʱ�շѷ���
    }

    private void addService(string Name, string Cmd, string ServicePanel = null, int PayType = 0)
    {
        Service tempService = new Service(Name, Cmd, ServicePanel, PayType);
        serviceList.Add(tempService);
    }

    /// <summary>
    /// ����ͨѶת���б�
    /// </summary>
    private void createDispatchList()
    {
        addDispatchMes("IMG_AI","ComeIn", "VOICE_AI", "startgame");//�ͻ�����
        addDispatchMes("IMG_AI", "Leave", "VOICE_AI", "endgame");//�ͻ��뿪
        addDispatchMes("IMG_AI", "Regular", "VOICE_AI", "IDRegularCustomer");//��͹���
        addDispatchMes("HTTP_SERVER", "PayScs", "VOICE_AI", "paymentsuccess");//֧���ɹ�
        addDispatchMes("HTTP_SERVER", "PayFailed", "VOICE_AI", "payedfailed");//֧��ʧ��
        addDispatchMes("UI", "PayCancel", "VOICE_AI", "cancelpayment");//֧��ȡ��
        addDispatchMes("TCP_SERVER", "KaraokeOver", "VOICE_AI", "karaokeOver");//����K��
        addDispatchMes("WEB_CLIENT", "ScreenOver", "VOICE_AI", "screenOver");//����Ͷ��
    }

    private void addDispatchMes(string sender_, string senderMessage_,
        string recipient_, string dispatchMessage_)
    {
        MessageType tempMessage = new MessageType(sender_, senderMessage_,
            recipient_, dispatchMessage_);
        dispatchMessList.Add(tempMessage);
    }

    /// <summary>
    /// ����Ĭ�ϲ���������Ķ�����Ƶ�������Ƶ�ȣ�
    /// </summary>
    private void createVideoDic()
    {
        charactorVideos.addFold("WRSD_A��������");
        charactorVideos.addFold("WRSD_B˵����");
        charactorVideos.addFold("WRSD_Cת������");
        charactorVideos.addFold("WRSD_D�к���");
        charactorVideos.addFold("WRSD_E������");
        charactorVideos.addFold("WRSD_F�ɰ���");
        charactorVideos.addFold("WRSD_G���⶯��");

        adverVideos.addFold("����");
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

    private AutoResetEvent ReShowYNEvent_;//������ȡ��
    private int ReShowYNTime = jsonParam.deviceinfo.waitPayTime+5000; //������ȡ���ȴ�ʱ��

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

    #region 1.1������Ϣת��
    private void mesUIDispatchHandle(string recipient, string mes)
    {
        MessageType mestmp = new MessageType("", "", recipient, mes);
        messageDispatch(mestmp);
    }
    #endregion
    #region 1.2���Խ�������
    private void msgReceiveHandle(string mes)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(msgReceiveCoroutine(mes));
    }

    IEnumerator msgReceiveCoroutine(string msg)
    {
        //������Ϣ��ʾ
        if(menuCtr != null)
            menuCtr.ShowMsgDebug(msg);
        yield return null;
    }
    #endregion
    #region 1.3 ���Ժͺ�˼��ܿ�ͨѶ
    private void GetQRCodeHandle()
    {
        //to do ���뿨�ֳ���
        //payOKEvent_.Reset();
        //to do ����Ͷ��
        //to do ����
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient?.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http��ȡ��ά��·��
        var qrInfo = httpClient?.getQRCode(proinfo["data"]["egoodList"][0]["id"].ToString());

        if (unstoreCtr != null)
            unstoreCtr.OnShowQRCode(qrInfo.msg);

        menuCtr?.OnShowQRCode(qrInfo.msg);
   
    }

    private void GetInfoeHandle()
    {
        //to do ���뿨�ֳ���
        //payOKEvent_.Reset();
        //to do ����Ͷ��
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient?.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http��ȡ��ά��·��
        //var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[0].id);

        //menuCtr.OnShowQRCode(qrInfo.msg);
    }
    #endregion

    #region 2.Ͷ����ͼ��������Ϣ����
    private void msgPayHandler(NetBase.MsgPay mp)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(msgPayCoroutine(mp));
    }

    private IEnumerator msgPayCoroutine(NetBase.MsgPay mp)
    {
        if (mp.instruct == "100001")//���ѳɹ�����
        {
            if(mp.goodNo == "0" && mp.type == "1")//K�裬֧���ɹ���־
            {
                Debug.LogWarning("K��֧���ɹ�");
                paykaraokeOK_ = true;
                payOKEvent_.Set();
                OnPayKaraokeOKHandle();
            }
            if (mp.goodNo == "1" && mp.type == "1")//Ͷ����֧���ɹ���־
            {
                Debug.LogWarning("Ͷ��֧���ɹ�");
                payscreenOK_ = true;
                payOKEvent_.Set();
                OnPayScreenOKHandle();
            }

            //if (mp.goodNo == "2" && mp.type == "1")//Ӧ����Ԯ
            //{
            //    //payOKEvent_.Set();
            //    OnRescueService();
            //}
        }
        if (mp.instruct == "100002")//����
        {
            Debug.LogWarning("100002ָ���");
            if(mp.goodNo == "1")//Ͷ�����úľ��ر�Ͷ��
            {
                //to do ����Ͷ��
                //OnCloseScreenProj();
                closeExe(jsonParam.deviceinfo.scrnSoftName);//����
                MessageType mes = getMessageType("ScreenOver");//��������Ͷ������
                messageDispatch(mes);//��������Ͷ������
                if (unstoreCtr != null && unstoreCtr.getCurrentUIState() != PanelsCtrlApi.UIState.RESCUE_UI)
                {
                    unstoreCtr?.OnToMainUI();
                }
            }
        }
        if (mp.instruct == "100006")//�������ã��ݲ�����ܴ������ź�
        {
            // go to main UI;
            if (mp.goodNo == "0" && mp.type == "1")//����K��ɹ�
            {
                //if (menuCtr != null)
                //menuCtr?.OnToMainUI();
                Debug.LogWarning("����K��");
                menuCtr?.OnStartBtn();
                unstoreCtr?.OnToMainUI();
                //unstoreCtr?.OnToMainUI();
                MessageType mes = getMessageType("KaraokeOver");//����K����Ϣ
                messageDispatch(mes);//�����¼�ת������������������
            }
            if(mp.goodNo == "2")
            {
                Debug.LogWarning("Ӧ����Ԯ����");
                menuCtr?.OnStartBtn();
                unstoreCtr?.OnToMainUI();
            }
        }
        if(mp.instruct == "100008")
        {
            Debug.LogWarning("Ӧ����Ԯ����");
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
        serviceHandle(currentService);//����ͽ��潻���ķ��������������£�ѡ��

        yield return null;
    }

    private void scrnProStrHandle(string sp)
    {
        Debug.LogWarning("receive  screen soft message:"+sp);
        if(sp.Contains("start"))
        {
            screenConnectEvent_.Reset();
            isConnect_ = false;
            Debug.LogWarning("Ͷ���������");
            Task.Run(() => {
            if(!screenConnectEvent_.WaitOne(jsonParam.deviceinfo.waitConnectTime))
                {
                    if(!isConnect_)
                    {
                        Debug.LogWarning("��ʱδ���ӹر�Ͷ��");
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
            Debug.LogWarning("Ͷ������");
        }
        if (!sp.Contains("close"))
            return;

        Debug.LogWarning("Ͷ���ر��ź�");
        UnityMainThreadDispatcher.Instance().Enqueue(scrnProStrCoroutine(sp));
    }

    IEnumerator scrnProStrCoroutine(string sp)
    {
        Service currentService = getService("close");
        serviceHandle(currentService);//����ͽ��潻���ķ��������������£�ѡ��
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
            if (!(currentService.name == "���󻬶�" || currentService.name == "���һ���"))
            {
                isIgnore = true;
            }
        }
        if (!isIgnore)
        {
            serviceHandle(currentService);//����ͽ��潻���ķ��������������£�ѡ��
            MessageType mes = getMessageType(pi.FACE_DATA.action);
            messageDispatch(mes);//�����¼�ת������������������
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
        //Service customerSay = new Service("�˿�˵", "customer");
        //serviceHandle(customerSay, pv.VOICE_DATA.TEXT.word);//����ʾ˵������
        //
        //Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        //serviceHandle(currentService);
        UnityMainThreadDispatcher.Instance().Enqueue(customerVoiceCoroutine(pv));
    }

    IEnumerator customerVoiceCoroutine(NetBase.PeopleVoice pv)
    {
        //throw new NotImplementedException();
        //Service customerSay = new Service("�˿�˵", "customerSpeak");
        //serviceHandle(customerSay, pv.VOICE_DATA.TEXT.word);//����ʾ˵������

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
        //Service vtuberSay = new Service("С��˵", "xiaoka");
        //serviceHandle(vtuberSay, pv.VOICE_DATA.TEXT.word, pv.VOICE_DATA.VOICE_PATH);//����ʾ˵������
        //
        //Service currentService = getService(pv.VOICE_DATA.TEXT.command);
        //serviceHandle(currentService);
        UnityMainThreadDispatcher.Instance().Enqueue(vtuberVoiceCoroutine(pv));
    }

    private IEnumerator vtuberVoiceCoroutine(NetBase.PeopleVoice pv)
    {
        //throw new NotImplementedException();

        //Service vtuberSay = new Service("С��˵", "aiSpeak");
        //serviceHandle(vtuberSay, pv.VOICE_DATA.TEXT.word, pv.VOICE_DATA.VOICE_PATH);//����ʾ˵������

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
        process.Start();//����
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
        #region 1.�����Ի�������������
        if (service.name == "��ʼ")
        {
            //to do ����������
            //menuCtr.openMainUIPanel();
            menuCtr?.OnStartBtn();
            unstoreCtr?.OnStartBtn();
        }
        if (service.name == "ֹͣ")
        {
            //to do ����������
            //menuCtr.openMainUIPanel();
            //if (menuCtr != null)
            menuCtr?.OnStopBtn();
            unstoreCtr?.OnStopBtn();
        }
        if (service.name == "����������")
        {
            //to do ����������
            //menuCtr.openMainUIPanel();
            menuCtr?.OnToMainUI();
            unstoreCtr?.OnToMainUI();
        }
        #region gesture Service
        if (service.name == "���󻬶�")
        {
            //to do ���󻬶�,��ǰѡ��
            //if (menuCtr != null)
            menuCtr?.OnPreBtn();
            unstoreCtr?.OnLeftGestureHandle();
        }
        if (service.name == "���һ���")
        {
            //to do ���һ��������ѡ��
            menuCtr?.OnNextBtn();
            unstoreCtr?.OnRrightGestureHandle();
        }
        if (service.name == "ȷ��ѡ��")
        {
            //to do ȷ��ѡ��
            menuCtr?.OnSelect();
            unstoreCtr?.OnSureGestureHandle();
        }
        #endregion
        if (service.name == "�˿�˵")
        {
            //to do �˿�˵��
            //if (menuCtr != null)
            menuCtr?.OnCustomerSay(info);
            unstoreCtr?.OnCustomerSay(info);
        }
        if (service.name == "С��˵")
        {
            //to do С��˵��
            menuCtr?.OnXiaokaSay(info, path);
            unstoreCtr?.OnXiaokaSay(info, path);
        }

        if(service.name == "���к�")
        {
            // to  do greet sombody and speak
            unstoreCtr?.OnGreetBtn();
            //unstoreCtr?.SetLastVIPanelName(PanelsCtrlApi.ShoppingPanel);
        }
        #endregion

        #region 2.�շѷ���(������Ŀ��Ͷ���Ϳ��ֳ���)
        if (service.name == "����Ͷ��")
        {
            //OnStartScreenProj();
            //OnStartKaraokeFlow(num, goodsName);
            OnStartScreenProFlow(-1, "Ͷ��");
        }
        if (service.name == "���뿨��ӰԺ")
        {
            //to do ���뿨��ӰԺ
        }
        if (service.name == "���뿨�ֳ���")
        {
            //OnStartKaroke();
            OnStartKaraokeFlow(-1, "K��");
        }
        if(service.name == "Ͷ������")
        {
            //to do ����Ͷ��
            OnCloseScreenProj();
            unstoreCtr?.OnToMainUI();
        }
        #endregion

        #region 3.Ӧ������(������Ŀ)
        if(service.name == "Ӧ����Ԯ")
        {
            //notice ui to display rescue panel;
            OnRescueService();
            OnRescueFlow();
        }
        if (service.name == "Ӧ��֪ʶ����")
        {
            //to do ���뿨��ӰԺ
        }
        if (service.name == "���ص�ͼ����")
        {
            OnRescueService();
            OnRescueFlow();
        }
        if (service.name == "��Ԯ�绰����")
        {
            //to do �˿�˵��
        }
        if (service.name == "�������з���")
        {
            //to do С��˵��
        }
        #endregion

        #region 4.�����̵���Ŀ(�����Ȥζ����)
        if(service.name == "����򻥶�")
        {
            // notice ui display shopping panel,��ת����������
            unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
            unstoreCtr?.ResetStage();
        }
        if (service.name == "����")
        {
            // notice ui display food panel,��ת��ʳƷ���Ͻ���
            //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ShoppingPanel)
            //unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.FOOD_DRINK);
            if(unstoreCtr!=null && unstoreCtr.GetCurChoosePanelName()==PanelsCtrlApi.ShoppingPanel)
            {
                unstoreCtr?.OnServiceBtn();
            }
        }
        //if (service.name == "ʳƷ")
        //{
        //    //notice ui to display boxlist panel for food
        //    //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.FoodPanel)
        //    unstoreCtr?.OnProductBtn();
        //}
        //if (service.name == "����")
        //{
        //    //notice ui to display boxlist panel for drinking
        //    //if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.FoodPanel)//����
        //        unstoreCtr?.OnServiceBtn();
        //}
        if (service.name == "��Ʒ����")
        {
            // to do showing yes or no panel
            //unstoreCtr?.OnShowUIBtnState(UnstorePanelCtr.UIBtnState.YES_NO);
            if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.BoxListPanel)
            {
                //��ת����
                
                unstoreCtr?.OnMoveToTop(info);
                OnSetDealServiceIndex(info);
            }
        }
        if (service.name == "�Ƿ����")
        {
            //notice ui to display YesOrNo panel
            if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.BoxListPanel)
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
        }
        if (service.name == "�Ǽ���")
        {
            //to do select
            // to do get qr code and purchase
            //OnBuyGoodsFlow();
            // notice ui display food panel,��ת��ʳƷ���Ͻ���

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

            //unstoreCtr?.OnSampleShopPlay();//����ѡ����
        }
        if (service.name == "�����")
        {
            //to do cancel
            var LastVIname = unstoreCtr?.GetLastVIPanelName();

            if (LastVIname == PanelsCtrlApi.YesOrNoPanel || LastVIname == PanelsCtrlApi.BoxListPanel)
                unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
                //unstoreCtr?.OnToMainUI();
            if (LastVIname == PanelsCtrlApi.OKroNotPanel)
                unstoreCtr?.OnChatActShort();
        }
        if (service.name == "Ȥζ����")
        {
            if (unstoreCtr?.GetRandChaneStage() == service.name)
                unstoreCtr?.setCurRootPath(jsonParam.charactorVideos.foldPath[1]);
            unstoreCtr?.OnChatAct();
            unstoreCtr?.SetLastVIPanelName("Ȥζ����");
        }
        if (service.name == "����������1" )
        {
            if (unstoreCtr?.GetRandChaneStage() == service.name)
                unstoreCtr?.setCurRootPath(jsonParam.charactorVideos.foldPath[1]);
            //�������죨�����ͱ����ȳ裨����� ��ѡһ
            if (unstoreCtr?.GetLastVIPanelName() == "Ȥζ����")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnRelyChat();
                unstoreCtr?.SetLastVIPanelName("����������1");
            }
        }
        if (service.name == "����������1b")
        {
            //�������죨�����ͱ����ȳ裨����� ��ѡһ
            if (unstoreCtr?.GetLastVIPanelName() == "Ȥζ����")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnNoRelyChat();
                unstoreCtr?.SetLastVIPanelName("����������1b");
            }
        }
        if (service.name == "����������1c")
        {
            //�������죨�����ͱ����ȳ裨����� ��ѡһ
            if (unstoreCtr?.GetLastVIPanelName() == "Ȥζ����")
            {
                //unstoreCtr?.OnChatActLong();
                unstoreCtr?.OnErrRelyChat();
                unstoreCtr?.SetLastVIPanelName("����������1c");
            }
        }
        if (service.name == "����������2")
        {
            if (unstoreCtr?.GetLastVIPanelName() == "����������1")
            {
                unstoreCtr?.OnChatActLong2();
                unstoreCtr?.SetLastVIPanelName("����������2");
            }
        }
        if(service.name == "K���Ͷ��")
        {
            unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.KTVSCREEN);
        }
        if (service.name == "GAME")
        {
            var LastVIname = unstoreCtr?.GetLastVIPanelName();

            //if (LastVIname == "ֱ��" || LastVIname == "����Ƶ")
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
        if (service.name == "����������")
        {
            // ��Ƶ���в���
            var LastVIname = unstoreCtr?.GetLastVIPanelName();
            if (LastVIname == PanelsCtrlApi.GameTitlePanel)
            {
                //unstoreCtr?.OnChatActShort();

                unstoreCtr?.SetLastVIPanelName("����������");
            }
        }
        if (service.name == "ֱ������Ƶ")
        {
            //if (unstoreCtr?.GetLastVIPanelName() == "����������2")
            //{
            //    unstoreCtr?.OnShowUIBtnState(PanelsCtrlApi.UIBtnState.ZHIBOCH);
            //}
            var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(3) - 0.45f);
            if((int)randNum ==0)
            {
                unstoreCtr?.OnZhiboVR();
                unstoreCtr?.SetLastVIPanelName("ֱ��");
            }
            else if ((int)randNum == 1)
            {
                unstoreCtr?.OnShortVideo();
                unstoreCtr?.SetLastVIPanelName("����Ƶ");
            }
            else if ((int)randNum == 2)
            {
                unstoreCtr?.OnVRhost();
                unstoreCtr?.SetLastVIPanelName("����������");
            }
        }
        //if (service.name == "ֱ��")
        //{
        //    //VRֱ���������� ֱ����ѡһ
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnZhiboVR();
        //        unstoreCtr?.SetLastVIPanelName("ֱ��");
        //    }
        //}
        //if (service.name == "����Ƶ")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnShortVideo();
        //        unstoreCtr?.SetLastVIPanelName("����Ƶ");
        //    }
        //}
        //if (service.name == "����������")
        //{
        //    if (unstoreCtr?.GetLastVIPanelName() == PanelsCtrlApi.ZhiBoChoosePanel)
        //    {
        //        unstoreCtr?.OnVRhost();
        //        unstoreCtr?.SetLastVIPanelName("����������");
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
        if(goodsName.Contains("Ӧ��"))
        {
            OnRescueFlow();
            return;
        }
        if(goodsName.Contains("K��"))
        {
            OnStartKaraokeFlow(num, goodsName);
            return;
        }
        if (goodsName.Contains("Ͷ��"))
        {
            OnStartScreenProFlow(num, goodsName);
            return;
        }

        //to do ����Ͷ��
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        ////2.http��ȡ��ά��·��
        //
        ////var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[1].id);
        //var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        //unstoreCtr?.OnShakeGoods(num, 1.0f);

        NetBase.QRCodeInfo qrInfo = new NetBase.QRCodeInfo();
        if (proinfo != null)
            qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        else
        {
            qrInfo.msg = "�����������쳣";
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
                menuCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", errShowTime);
                unstoreCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", errShowTime);
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
                //֧���ɹ�����Ͷ��
                //1.��Ͷ�����
                Debug.LogWarning("Buy goods pay success!");
                // to do check out goods action
                //ת��֧���ɹ���Ϣ
                var mes = getMessageType("PayScs");
                messageDispatch(mes);

                //unstoreCtr?.OnOpenMainUIPanel();
                //unstoreCtr?.OnShowPayShopingResult();
            }
            else
            {
                //ת��֧��ʧ����Ϣ
                var mes = getMessageType("PayFailed");
                messageDispatch(mes);
                //֧����ʱ���ص�������
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
                    case "������Ʒ":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.OutDoor, ServiceName);

                        break;
                    case "������Ʒ":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.GoodsType, ServiceName);

                        break;
                    case "ˮ�����":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.WaterElec, ServiceName);

                        break;
                    case "��ԡ����":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Shower, ServiceName);

                        break;
                    case "Ӧ������":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Rescue, ServiceName);

                        break;
                    case "���ܳ���":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Kitchen, ServiceName);
                        
                        break;
                    case "ʳƷ":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Food, ServiceName);
                        break;
                    case "����":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Drink, ServiceName);
                        break;
                    case "ũ�ز�Ʒ":
                        unstoreCtr?.OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.AgriP, ServiceName);
                        break;
                }
            }

        });
        
        curServiceIndex = "";
    }

    /// <summary>
    /// �ж��ַ����Ƿ�������
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

        //to do ����Ͷ��
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http��ȡ��ά��·��

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
                //3.������Ʒ
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
            menuCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
            unstoreCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
            Invoke("OnPayFailedFlow", 10);
            return;
        }

        payOKEvent_.Reset();
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime+ waitYN))
            {
                //unstoreCtr?.OnCloseQRCode();
                ////֧���ɹ�����Ͷ��
                ////1.��Ͷ�����
                //Debug.LogWarning("ScreeenPro pay success!");
                //// to do check out goods action
                ////ת��֧���ɹ���Ϣ
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
                    ////ת��֧��ʧ����Ϣ
                    //var mes = getMessageType("PayFailed");
                    //messageDispatch(mes);
                    ////֧����ʱ��������������
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

        //to do ����Ͷ��
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http��ȡ��ά��·��
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
                //3.������Ʒ
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
            menuCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
            unstoreCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
            Invoke("OnPayFailedFlow",10);
            return;
        }

        payOKEvent_.Reset();
        Task.Run(() =>
        {
            if (qrInfo.code == 200 && payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime+waitYN))
            {
                //unstoreCtr?.OnCloseQRCode();
                ////֧���ɹ�����Ͷ��
                //Debug.LogWarning("Karoke pay success!");
                //// to do check out goods action
                ////ת��֧���ɹ���Ϣ
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
                    ////ת��֧��ʧ����Ϣ
                    //var mes = getMessageType("PayFailed");
                    //messageDispatch(mes);
                    ////֧����ʱ���ص�������
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
        //ת��֧��ʧ����Ϣ
        var mes = getMessageType("PayFailed");
        messageDispatch(mes);
        //֧����ʱ���ص�������
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
            //֧���ɹ�����Ͷ��
            //1.��Ͷ�����
            Debug.LogWarning("ScreeenPro pay success!");
            // to do check out goods action
            //ת��֧���ɹ���Ϣ
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
            //֧���ɹ�����Ͷ��
            Debug.LogWarning("Karoke pay success!");
            // to do check out goods action
            //ת��֧���ɹ���Ϣ
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
            //1,�������Ͷ��״̬�ر�Ͷ��
            //OnCloseScreenProj();
            closeExe(jsonParam.deviceinfo.scrnSoftName);

            MessageType mes = getMessageType("ScreenOver");//��������Ͷ������
            messageDispatch(mes);//��������Ͷ������

            //sendCloseScreenToTcp();
            Invoke("sendCloseScreenToTcp", 1.0f);     
        }

        //2.��Ӧ����ͼ
        unstoreCtr?.OnRescueService();
        try
        {
            MessageType mes = getMessageType("AskRescue");//��������Ͷ������
            messageDispatch(mes);//��������Ͷ������
        }
        catch (Exception ex)
        {
            Debug.LogError("send AskRescue Err with " + ex.Message);
        }
        yield return null;
    }

    private void OnCloseScreenProj()
    {
        //1.ת��Ͷ��������Ϣ
        NetBase.MsgPay msgConten = new NetBase.MsgPay(
            jsonParam.deviceinfo.equipmentID, "1", "100006");
        NetBase.SocketMessage sm = new NetBase.SocketMessage(
            jsonParam.deviceinfo.equipmentTAG,
            jsonParam.deviceinfo.userID,
            msgConten);
        string closeMsg = JsonHelper.ObjectToJSON<NetBase.SocketMessage>(sm);
        tcpclient.sendMessage(closeMsg);
        //2.�ر�Ͷ�����
        closeExe(jsonParam.deviceinfo.scrnSoftName);

        MessageType mes = getMessageType("ScreenOver");//��������Ͷ������
        messageDispatch(mes);//��������Ͷ������
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
            //to do ����Ͷ��
            //1.http��ȡ��Ŀ��Ϣ
            var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
            //2.http��ȡ��ά��·��
            var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][0]["id"].ToString());
            if(qrInfo.code != 200)
            {
                //if (menuCtr != null)
                menuCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
                return;
            }
            menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime/1000);
            Task.Run(() =>
            {
    
                if (payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime))//�ȴ�֧���ɹ�
                {
                    //1.֧���ɹ����ȴ�K�����
                    Debug.LogWarning("Karoke pay success!");
                    var goodGoll = proinfo["data"]["egoodList"][0]["good"]["goodGoll"].ToObject<JObject>();
                    //if (menuCtr != null)
                    menuCtr?.OnShowPayResultPanel(goodGoll);
                    //ת��֧���ɹ���Ϣ
                    var mes = getMessageType("PayScs");
                    messageDispatch(mes);
                }
                else
                {
                    //2.֧����ʱ���ص�������
                    //ת��֧��ʧ����Ϣ
                    var mes = getMessageType("PayFailed");
                    messageDispatch(mes);
                    menuCtr?.OnToMainUI();
                }
            });
     }
    private void OnStartScreenProj()
    {
        payOKEvent_.Reset();
        //to do ����Ͷ��
        //1.http��ȡ��Ŀ��Ϣ
        var proinfo = httpClient.getEquipGoods(jsonParam.deviceinfo.equipmentID);
        //2.http��ȡ��ά��·��

        //var qrInfo = httpClient.getQRCode(proinfo.data.egoodList[1].id);
        var qrInfo = httpClient.getQRCode(proinfo["data"]["egoodList"][1]["id"].ToString());
        if (qrInfo.code != 200)
        {
            menuCtr?.OnShowErro("��ȡ��ά�����", qrInfo.msg, "����ϵ����Ա", 10);
            return;
        }
        //if (menuCtr != null)
        menuCtr?.OnShowQRCode(qrInfo.msg, jsonParam.deviceinfo.waitPayTime / 1000);
        Task.Run(() =>
        {
            if (payOKEvent_.WaitOne(jsonParam.deviceinfo.waitPayTime))
            {
                //֧���ɹ�����Ͷ��
                //1.��Ͷ�����
                Debug.LogWarning("ScreeenPro pay success!");
                var goodGoll = proinfo["data"]["egoodList"][1]["good"]["goodGoll"].ToObject<JObject>();
                //ת��֧���ɹ���Ϣ
                var mes = getMessageType("PayScs");
                messageDispatch(mes);

                menuCtr?.OnShowPayResultPanel(goodGoll);
                openExe(jsonParam.deviceinfo.scrnSoftPath);
            }
            else
            {
                //֧����ʱ���ص�������
                //ת��֧��ʧ����Ϣ
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
            MessageType mes = getMessageType("AskRescue");//��������Ͷ������
            messageDispatch(mes);//��������Ͷ������
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
