using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json.Linq;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

interface IPanelCtrlInterface
{
    #region CommonPanels 
    /// <summary>
    /// 获取当前最新需要手势交互panel 名字
    /// </summary>
    /// <returns></returns>
    public string GetCurChoosePanelName();
    /// <summary>
    /// 设置当前最新需要交互的Panel 名字
    /// </summary>
    /// <returns></returns>
    public void SetCurChoosePanelName(string panel = "");
    /// <summary>
    /// 获取上次语音交互panel 名字
    /// </summary>
    /// <returns></returns>
    public string GetLastVIPanelName();
    /// <summary>
    /// 设置上次交互panel 名字
    /// </summary>
    /// <returns></returns>
    public void SetLastVIPanelName(string panel = "");
    /// <summary>
    /// 获取当前活动Scene 名字
    /// </summary>
    /// <returns></returns>
    public string GetCurActiveSceneName();
    /// <summary>
    /// 提示信息
    /// </summary>
    /// <param name="title"></param>
    /// <param name="details"></param>
    /// <param name="tips"></param>
    /// <param name="time"></param>
    public void OnShowErro(string title = "操作错误", string details = null,
        string tips = null, float time = 3);
    /// <summary>
    /// 展示游戏支付状态UI
    /// </summary>
    /// <param name="info"></param>
    /// <param name="time"></param>
    public void OnShowPayGameResult(JObject info, float time = 3);
    #endregion

    #region ServiceStation
    /// <summary>
    /// 展示紧急救援界面
    /// </summary>
    public void OnShowRescue(bool bShow=true);
    /// <summary>
    /// 展示支付
    /// </summary>
    public void OnShowPayPanel(bool bShow = true);
    /// <summary>
    /// 显示电话呼叫界面
    /// </summary>
    /// <param name="bShow"></param>
    /// <param name="strAdress"></param>
    /// <param name="strSign"></param>
    public void OnShowCalling(bool bShow = true, string strAdress = "云南省昆明市XX区308国道XXX公路旁", string strSign = "东经102 北纬24");
    #endregion

    #region Unstore 
    /// <summary>
    /// 条件展示静态页面
    /// </summary>
    /// <param name="state"></param>
    public void setUIBtnPanelState(int state);
    /// <summary>
    /// Animation方式展示商品列表
    /// </summary>
    /// <param name="istrue"></param>
    public void OnShowProductList(bool bShow, string ServiceName = "");
    /// <summary>
    /// 预制体方式展示商品列表
    /// </summary>
    public void OnShowFoodsList(bool bShow, JObject info = null);
    /// <summary>
    /// 展示/隐藏 购物车
    /// </summary>
    public void OnShowShopingCart(bool bShow, JObject info = null);
    /// <summary>
    /// 展示支付码
    /// </summary>
    /// <param name="qrUrl"></param>
    /// <param name="fCost">消费金额</param>
    /// <param name="time"></param>
    public void OnShowQRCode(string qrUrl, string name,float fCost, float time = 3);
    /// <summary>
    /// 展示支付状态UI
    /// </summary>
    /// <param name="state"></param>
    /// <param name="time"></param>
    public void OnShowPayShopingResult(int state, float time = 2);
    /// <summary>
    /// 展示出货状态对应UI
    /// </summary>
    public void OnShowOutGoodsResult(int state, float time = 2);
    /// <summary>
    /// 展示游戏界面
    /// </summary>
    public void OnShowGameTitle(int index = 1, bool bShow=true);
    /// <summary>
    /// 展示滑动效果
    /// </summary>
    /// <param name="bShow"></param>
    /// <param name="isLeft"></param>
    public void OnShowLRChoosingPanel(bool bShow = true, bool isLeft = true, bool autoplayAmi = true);

    public bool GetLRChoosingLeft();
    #endregion
}

public class PanelsCtrlApi : PanelManager, IPanelCtrlInterface
{
    public enum UIState
    {
        MAIN_UI = 0,
        RESCUE_UI = 1,
        ADVER_UI = 2,
        KARAOKE_UI = 3,
        SCREENPRO_UI = 4,
        OTHERS_UI = 100
    }
    // Start is called before the first frame update
    public static class UIBtnState
    {
        public static readonly int HIDE = 0;
        public static readonly int YES_NO = 1;
        public static readonly int SHOPPING = 2;
        public static readonly int OK_NOT = 3; 
        public static readonly int APP = 4;
        public static readonly int ZHIBOCH = 5;
        public static readonly int GAMECH = 6;
        public static readonly int SHORTVIDEO = 7;
        public static readonly int KTVSCREEN = 8;
    }
    public static class ShipmentState
    {
        public static readonly int Waiting = 0;//等待出货
        public static readonly int Processing = 1;// 已支付, 出货中
        public static readonly int Success = 2;
        public static readonly int Failed = 3;
    }
    public static class PayState
    {
        public static readonly int Failed = 0;
        public static readonly int Success = 1;
        public static readonly int Catch = 2;//支付异常
    }
    public enum State
    {
        idle,
        playing,
        pause
    }
    public enum State1
    {
        once,
        loop
    }

    #region CommonPanels初始化
    bool isServiceStation = false;
    bool isUnstore = false;
    bool isVtuber = false;
    private string CurActivePanel = "BtnMenuPanel";//当前活动的panel
    private string CurChoosePanel = "";//当前被选择的panel(手势)
    private string LastVIPanel = "";//当前被选择的panel(VI)

    public const string BtnDebugPanel = "BtnDebugPanel";
    public const string ErroPanel = "ErroPanel";
    public const string PayGameResultPanel = "PayGameResultPanel";

    #endregion

    #region ServiceStation初始化
    public const string RescuePanel = "RescuePanel";
    public const string PayPanel = "PayPanel";
    public const string CallingPanel = "CallingPanel";

    //animation play setting
    //[Header("播放方式(循环、单次)")]//默认循环
    private State1 condition = State1.loop;
    //[Header("自动播放")]//默认不自动播放
    private bool Play_Awake = false;
    //播放状态(默认、播放中、暂停)
    private State play_state;
    private Image manimg;
    //[Header("sprite数组")]
    private Sprite[] sprit_arr;
    private int index;
    private float tim;
    private float waittim;
    private bool isplay;

    
    #endregion

    #region Unstore初始化
    public const string SecProductListPanel = "SecProductListPanel";
    public const string UiBtnPanel = "UIButtonPanel";
    public const string LRChoosingPanel = "LRChoosingPanel";
    public const string YesOrNoPanel = "YesNoPanel";
    public const string ShoppingPanel = "ShoppingPanel";
    public const string OKroNotPanel = "OKroNotPanel";
    public const string BoxListPanel = "BoxListPanel";
    public const string KTVScreenPanel = "KTVScreenProChoosePanel";
    //public const string GameChoosePanel = "GameChoosePanel";
    //public const string ZhiBoChoosePanel = "ZhiBoChoosePanel";

    public const string QRCodePanel = "QRCodePanel";
    public const string ShoppingCartPanel = "ShoppingCartPanel";
    public const string PayShopingResultPanel = "PayShopingResultPanel";
    public const string OutGoodsResultPanel = "OutGoodsResultPanel";
    public const string GameTitlePanel = "GameTitlePanel"; 
    public const string APPLoadPanel = "APPLoadPanel";
    public const string ShortVideoPanel = "ShortVideoPanel";

    [Header("是否滚动条展示商品")]
    public bool IsScollShowGoods = false;
    public Text text;
    public InfiniteSV _infiniteSV;

    bool _isLeft = true;

    public Text QRtimeRemainText;
    private float QRtime = -1.0f;

    protected float errPanelShowTime = -1.0f;
    #endregion

    void Awake()
    {
        string CurScence = SceneManager.GetActiveScene().name;
        Debug.Log("当前活跃的场景名称为： " + CurScence);
        if (CurScence == "ServiceStationScene")
        {
            isServiceStation = true;
        }
        else if (CurScence == "UnstoreScene")
        {
            isUnstore = true;
        }
        else if (CurScence == "VtuberScene")
        {
            isVtuber = true;
        }
        if (isServiceStation)
            InitAni();
    }
    void Start()
    {
        if (isUnstore&&IsScollShowGoods)
            _infiniteSV._OnCenterChange += _OnCenterChange_CustomCall;

    }
    void _OnCenterChange_CustomCall(InfiniteSV_ItemData _ItemData)
    {
        text.text = _ItemData.ItemInfo;
    }

    void updateQRtime()
    {
        if(QRtime > 0.0f)
        {
            QRtime -= Time.deltaTime;
            string info = (int)QRtime + "s";
            QRtimeRemainText.text = info;
        }
        if(QRtime > -0.5f && QRtime < 00f)
        {
            AutoCloseQRPanel();
        }
        //if(QRtime < -0.5f)
        //{
        //    
        //}
        if(errPanelShowTime > 0.0f)
        {
            errPanelShowTime -= Time.deltaTime;
            string timeRemain = "在" + (int)errPanelShowTime + "s后关闭";
            var objErroPanel = GetPanelByName(ErroPanel);
            Text[] textList = objErroPanel.gameObject.GetComponentsInChildren<Text>();
            if (textList != null)
            {
                if (textList.Length > 3)
                    textList[3].text = timeRemain;
            }
            else
            {
                Debug.LogWarning("not found any Text in  ErroPanel");
            }
        }
    }
    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.D))
        {
            OnShowPanel(BtnDebugPanel);
        }
        if (Input.GetKeyDown(KeyCode.C))
        {
            OnShowPanel(BtnDebugPanel,false);
        }
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            Quit();
        }
        if (Input.GetKeyDown(KeyCode.S))
        {
            if (isServiceStation)
            {
                OnShowCalling();
            }
            //OnShowFoodsList();
        }
        if (Input.GetKeyDown(KeyCode.H))
        {
            //OnShowFoodsList();
            if (isServiceStation)
            {
                OnShowCalling(false);
            }
        }
        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            ScenesCtrlApi.Instance.OpenScene(ScenesCtrlApi.SceneOrder.UnstoreSce);
        }
        if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            ScenesCtrlApi.Instance.OpenScene(ScenesCtrlApi.SceneOrder.VtuberSce);
        }
        if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            ScenesCtrlApi.Instance.OpenScene(ScenesCtrlApi.SceneOrder.ServiceSce);
        }
        updateQRtime();
        AniUpMove();
    }


    #region 界面TOOLS
    public GameObject FindObjectInObject(GameObject tempObj, string name)
    {
        var transforms = tempObj.GetComponentsInChildren<Transform>();
        foreach (Transform item in transforms)
        {
            if (item.name == name)
                return item.gameObject;
        }
        Debug.LogError(tempObj.name + "里找不到名为" + name + "的对象");
        return null;
    }
    /// <summary>
    /// 在panel里根据名称找子物体
    /// </summary>
    /// <param name="panel"></param>
    /// <param name="name"></param>
    /// <returns></returns>
    public GameObject FindGameObjectInPanel(PanelInstance panel, string name)
    {
        return FindObjectInObject(panel.gameObject, name);
    }
    public void OnShowPanel(PanelInstance panel, bool bShow = true)
    {
        if (bShow)
        {
            OpenPanel(panel);
        }
        else
            ClosePanel(panel);
    }
    public void OnShowPanel(string panelName, bool bShow = true)
    {
        if (bShow)
        {
            CurActivePanel = panelName;

            OpenPanelByName(panelName);
        }
        else
            ClosePanelByName(panelName);
    }

    public void Quit()
    {
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#else
Application.Quit();
#endif
    }

    #region animationCtrl
    private void AniUpMove()
    {
        //单播
        if (condition == State1.once)
        {
            if (play_state == State.idle && isplay)
            {
                play_state = State.playing;
                index = 0;
                tim = 0;
            }
            if (play_state == State.pause && isplay)
            {
                play_state = State.playing;
                tim = 0;
            }
            if (play_state == State.playing && isplay)
            {
                tim += Time.deltaTime;
                if (tim >= waittim)
                {
                    tim = 0;
                    index++;
                    if (index >= sprit_arr.Length)
                    {
                        index = 0;
                        manimg.sprite = sprit_arr[index];
                        isplay = false;
                        play_state = State.idle;
                        //此处可添加结束回调函数
                        
                    }
                    manimg.sprite = sprit_arr[index];
                }
            }
        }
        //循环播放
        if (condition == State1.loop)
        {
            if (play_state == State.idle && isplay)
            {
                play_state = State.playing;
                index = 0;
                tim = 0;
            }
            if (play_state == State.pause && isplay)
            {
                play_state = State.playing;
                tim = 0;
            }
            if (play_state == State.playing && isplay)
            {
                tim += Time.deltaTime;
                if (tim >= waittim)
                {
                    tim = 0;
                    index++;
                    if (index >= sprit_arr.Length)
                    {
                        index = 0;
                        //此处可添加结束回调函数
                    }
                    manimg.sprite = sprit_arr[index];
                }
            }
        }
    }

    private void InitAniPlayerCfg(string PanelName,string BtnName,string ImgPath,bool autoPlay = false, State1 Cstate = State1.once,float framCount =30f)
    {
        tim = 0;
        index = 0;
        waittim = 1 / framCount;//呼叫动画fps(帧 / 秒)
        play_state = State.idle;
        isplay = false;
        condition = Cstate;

        sprit_arr = Resources.LoadAll<Sprite>(ImgPath);
        var obCallingPanel = GetPanelByName(PanelName);
        GameObject CallImgObject = FindGameObjectInPanel(obCallingPanel, BtnName);
        manimg = CallImgObject.GetComponent<Image>();
        if (manimg == null)
        {
            Debug.LogWarning("Image为空，请添加Image组件！！！");
            return;
        }
        if (sprit_arr.Length < 1)
        {
            Debug.LogWarning("sprite数组为0，请给sprite数组添加元素！！！");
        }
        manimg.sprite = sprit_arr[0];
        if(autoPlay)
            AniPlay();
    }
 
    private void InitAni()
    {
        tim = 0;
        index = 0;
        waittim = 1 / 30;//呼叫动画fps(帧 / 秒)
        play_state = State.idle;
        isplay = false;
        sprit_arr = Resources.LoadAll<Sprite>(@"Images\ServiceStationScene\CallingImg\");
        var obCallingPanel = GetPanelByName(CallingPanel);
        GameObject CallImgObject = FindGameObjectInPanel(obCallingPanel, "CallingImage");
        manimg = CallImgObject.GetComponent<Image>();
        if (manimg == null)
        {
            Debug.LogWarning("Image为空，请添加Image组件！！！");
            return;
        }
        if (sprit_arr.Length < 1)
        {
            Debug.LogWarning("sprite数组为0，请给sprite数组添加元素！！！");
        }
        manimg.sprite = sprit_arr[0];
        if (Play_Awake)
        {
            AniPlay();
        }
    }

    /// <summary>
    /// 播放
    /// </summary>
    public void AniPlay()
    {
        isplay = true;
    }
    /// <summary>
    /// 暂停
    /// </summary>
    public void AniPause()
    {
        isplay = false;
        play_state = State.pause;
    }
    /// <summary>
    /// 停止
    /// </summary>
    public void AniStop()
    {
        isplay = false;
        play_state = State.idle;
        index = 0;
        tim = 0;
        if (manimg == null)
        {
            Debug.LogWarning("Image为空，请赋值");
            return;
        }
        manimg.sprite = sprit_arr[index];
    }
    /// <summary>
    /// 重播
    /// </summary>
    public void AniReplay()
    {
        isplay = true;
        play_state = State.playing;
        index = 0;
        tim = 0;
    }
    #endregion
    #endregion

    #region CommonPanels
    
    public string GetCurActiveSceneName()
    {
        return SceneManager.GetActiveScene().name;
    }
   
    public string GetCurChoosePanelName()
    {
        return CurChoosePanel;
    }
   
    public void SetCurChoosePanelName(string panel = "")
    {
        CurChoosePanel = panel;
    }
   
    public string GetLastVIPanelName()
    {
        return LastVIPanel;
    }
    
    public void SetLastVIPanelName(string panel = "")
    {
        LastVIPanel = panel;
    }
    
   
    public void OnShowErro(string title = "操作错误", string details = null,
        string tips = null, float time = 3)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(loadErrInfo(title, details, tips, time));
        //StartCoroutine(this.loadErrInfo(title, details, tips, time));
    }

    IEnumerator loadErrInfo(string title, string details,
        string tips, float time)
    {
        //string title = "支付成功！";
        //string details = "本次付费" + cost + "元,可以使用时长" + tollMaxTime + "分钟.";
        //string tips = closeRemark + ":" + tollStandard + "," + remarks;
        errPanelShowTime = time;
        string timeRemain = "在" + time + "s后关闭";
        var objErroPanel = GetPanelByName(ErroPanel);
        Text[] textList = objErroPanel.gameObject.GetComponentsInChildren<Text>();
        if (textList != null)
        {
            textList[0].text = title;
            if (textList.Length > 1)
            {
                if (details != null)
                    details = "细节：" + details;
                textList[1].text = details;
            }
            if (textList.Length > 2)
            {
                if (tips != null)
                    tips = "帮助：" + tips;
                textList[2].text = tips;
            }
            if (textList.Length > 3)
                textList[3].text = timeRemain;
        }
        else
        {
            Debug.LogWarning("not found any Text in  ErroPanel");
        }
        OnShowPanel(ErroPanel);
        Invoke("AutoCloseErroPanel", time);
        yield return null;
    }
    public void AutoCloseErroPanel()
    {
        errPanelShowTime = -1.0f;
        OnShowPanel(ErroPanel, false);
    }

    
    public void OnShowPayGameResult(JObject info, float time = 3)
    {
        //UnityMainThreadDispatcher.Instance().Enqueue(loadPayInfo(info, time));
        StartCoroutine(this.loadPayInfo(info, time));
    }

    private IEnumerator loadPayInfo(JObject goodGoll, float time = 3)
    {
        float tollTimeVal = ((float)goodGoll["tollTimeVal"]);
        int tollTimeType = ((int)goodGoll["tollTimeType"]);
        float tollMaxTime = ((float)goodGoll["tollMaxTime"]);
        Debug.LogWarning("tollTimeVal = " + tollTimeVal +
            ", tollTimeType = " + tollTimeType +
            ", tollMaxTime = " + tollMaxTime);
        float cost = ((float)goodGoll["cost"]);
        float tollAmount = ((float)goodGoll["tollAmount"]);
        float deposit = ((float)goodGoll["deposit"]);
        Debug.LogWarning("cost = " + cost +
            ", tollAmount = " + tollAmount +
            ", deposit = " + deposit);
        string tollStandard = goodGoll["tollStandard"].ToString();
        string closeRemark = goodGoll["closeRemark"].ToString();
        string remarks = goodGoll["remarks"].ToString();
        Debug.LogWarning("tollStandard = " + tollStandard +
            ", closeRemark = " + closeRemark +
            ", remarks = " + remarks);
        string title = "支付成功！";
        string details = "本次付费" + cost + "元,可以使用时长" + tollMaxTime + "分钟.";
        string tips = closeRemark + ":" + tollStandard + "," + remarks;
        string timeRemain = "在" + time + "s后关闭，并开始计时";
        var objPayGameResultPanel = GetPanelByName(PayGameResultPanel);
        Text[] textList = objPayGameResultPanel.gameObject.GetComponentsInChildren<Text>();
        if (textList != null)
        {
            textList[0].text = title;
            if (textList.Length > 1)
                textList[1].text = details;
            if (textList.Length > 2)
                textList[2].text = tips;
            if (textList.Length > 3)
                textList[3].text = timeRemain;
        }
        else
        {
            Debug.LogWarning("not found any Text in  pay result panel");
        }
        OnShowPanel(PayGameResultPanel);
        Invoke("closePayResultPanel", time);
        yield return null;
    }
    public void closePayGameResult()
    {
        OnShowPanel(PayGameResultPanel, false);
    }

    #endregion

    #region ServiceStation
    
    public void OnShowRescue(bool bShow)
    {
        if(bShow)
        {
            LastVIPanel = RescuePanel;
        }
        OnShowPanel(RescuePanel, bShow);
    }

    public void OnShowPayPanel(bool bShow)
    {
        if (bShow)
        {
            LastVIPanel = PayPanel;
        }
        OnShowPanel(PayPanel, bShow);
    }
    
    public void OnShowCalling(bool bShow=true,string strAdress = "云南省昆明市XX区308国道XXX公路旁",string strSign= "东经102 北纬24")
    {
        if(bShow)
        {
            var obCallingPanel = GetPanelByName(CallingPanel);
            GameObject AdressObject = FindGameObjectInPanel(obCallingPanel, "TextAdress");
            Text TextAdress = AdressObject.GetComponent<Text>();
            TextAdress.text = strAdress;
            GameObject SignObject = FindGameObjectInPanel(obCallingPanel, "TextSign");
            Text TextSign = SignObject.GetComponent<Text>();
            TextSign.text = strSign;
            OnShowPanel(CallingPanel);
            AniReplay();
        }    
        else
        {
            OnShowPanel(CallingPanel, false);
            AniStop();
        }
    }
    #endregion

    #region Unstore界面
    public void setUIBtnPanelState(int state)
    {
        bool isUIBtnEn = false;
        bool isYesNoEn = false;
        bool isShoppingEn = false;
        bool isOKEn = false;
        bool isGameChoose = false;
        bool isZhiBoChoose = false;
        bool isAPPLoad = false;
        bool isShortVideo = false;
        bool isLRChoose = false;
        bool isKTV = false;
        switch (state)
        {
            case 0:
                break;
            case 1:
                isUIBtnEn = true;
                isYesNoEn = true;
                isLRChoose = true;
                CurChoosePanel = YesOrNoPanel;
                LastVIPanel = YesOrNoPanel;
                break;
            case 2:
                isUIBtnEn = true;
                isShoppingEn = true;
                isLRChoose = true;
                CurChoosePanel = ShoppingPanel;
                LastVIPanel = ShoppingPanel;
                break;
            case 3:
                isUIBtnEn = true;
                isOKEn = true;
                isLRChoose = true;
                CurChoosePanel = OKroNotPanel;
                LastVIPanel = OKroNotPanel;
                break;
            case 4:
                isAPPLoad = true;
                LastVIPanel = APPLoadPanel;
                break;
            //case 5:
            //    isUIBtnEn = true;
            //    isZhiBoChoose = true;
            //    isLRChoose = true;
            //    CurChoosePanel = ZhiBoChoosePanel;
            //    LastVIPanel = ZhiBoChoosePanel;
            //    break;
            //case 6:
            //    isUIBtnEn = true;
            //    isGameChoose = true;
            //    isLRChoose = true;
            //    CurChoosePanel = GameChoosePanel;
            //    LastVIPanel = GameChoosePanel;
            //    break;
            case 7:
                isShortVideo = true;
                LastVIPanel = ShortVideoPanel;
                break;
            case 8:
                isUIBtnEn = true;
                isKTV = true;
                isLRChoose = true;
                CurChoosePanel = KTVScreenPanel;
                LastVIPanel = KTVScreenPanel;
                break;
        }
        OnShowPanel(UiBtnPanel,isUIBtnEn);
        OnShowPanel(YesOrNoPanel,isYesNoEn);
        OnShowPanel(ShoppingPanel,isShoppingEn); 
        OnShowPanel(OKroNotPanel, isOKEn);
        OnShowPanel(APPLoadPanel, isAPPLoad);
        //OnShowPanel(ZhiBoChoosePanel, isZhiBoChoose);
        //OnShowPanel(GameChoosePanel, isGameChoose);
        OnShowPanel(ShortVideoPanel, isShortVideo);
        OnShowPanel(KTVScreenPanel, isKTV);
        OnShowLRChoosingPanel(isLRChoose,true,false);
    }

    public void OnShowProductList(bool bShow,string ServiceName = "服务列表")
    {
        if (bShow)
        {
            CurChoosePanel = BoxListPanel;
            LastVIPanel = BoxListPanel;
            InitAniPlayerCfg(BoxListPanel, "SelectImage", @"Images\Unstore\SeqFrame\商品框选中光线\", true,State1.loop,4f);
            var obCallingPanel = GetPanelByName(BoxListPanel);
            GameObject CallImgObject = FindGameObjectInPanel(obCallingPanel, "ServiceName");
            Text TextSign = CallImgObject.GetComponent<Text>();
            TextSign.text = ServiceName;
        }
        OnShowPanel(BoxListPanel, bShow);
    }

    public void OnShowFoodsList(bool bShow=true, JObject info = null)
    {
        if (bShow)
        {
            CurChoosePanel = SecProductListPanel;
            LastVIPanel = SecProductListPanel;
            //加载商品数据
            LoadGoodsInfo(info);
        }
        OnShowPanel(SecProductListPanel, bShow);
    }
    public void LoadGoodsInfo(JObject info)
    {
        _infiniteSV.ClearAllGoodInfo();

        _infiniteSV.AddGoodsInfo("百草味雪花酥200g", "1");
        _infiniteSV.AddGoodsInfo("辣方了210g", "2");
        _infiniteSV.AddGoodsInfo("三只松鼠每日芒果干蜜饯果干88g", "3");
        _infiniteSV.AddGoodsInfo("迷你烤肠60g", "4");
        _infiniteSV.AddGoodsInfo("牛肉条五香味50g", "5");
        _infiniteSV.AddGoodsInfo("甜辣鸭脖170g", "6");
        _infiniteSV.AddGoodsInfo("甘栗仁100g", "7");
        _infiniteSV.AddGoodsInfo("蔓越莓曲奇100g", "8");
        _infiniteSV.AddGoodsInfo("七千里臭豆腐120g", "9");
        _infiniteSV.AddGoodsInfo("香酥小黄鱼96g", "10");

        _infiniteSV.InitStyleAndData();
    }
    
    public void OnShowShopingCart(bool bShow, JObject info = null)
    {
        if (bShow)
        {
            //加载购物车数据，待实现
        }
        OnShowPanel(ShoppingCartPanel, bShow);
    }

    
    public void OnShowQRCode(string qrUrl, string name = "", float fCost=10, float time = 20)
    {
        StartCoroutine(this.loadQRImg(qrUrl,name, fCost, time));
    }

    private IEnumerator loadQRImg(string qrUrl, string name = "", float fCost=10, float time = 20)
    {
        QRtime = time;
        WWW www = new WWW(qrUrl);
        yield return www;
        if (www.isDone && www.error == null)
        {
            var objQRCodePanel = GetPanelByName(QRCodePanel);
            GameObject CostImgObject = FindGameObjectInPanel(objQRCodePanel, "QRCodeRImg");
            RawImage imgTexture = CostImgObject.GetComponent<RawImage>(); 
            imgTexture.texture = www.texture;
            GameObject CostTextObject = FindGameObjectInPanel(objQRCodePanel, "CostText");
            Text CostText = CostTextObject.GetComponent<Text>();
            CostText.text = fCost + "元";
            GameObject NameTextObject = FindGameObjectInPanel(objQRCodePanel, "NameText");
            Text NameText = NameTextObject.GetComponent<Text>();
            NameText.text = name;
            OnShowPanel(QRCodePanel);
        }
        //Invoke("AutoCloseQRPanel", time);
    }

    public void SetQRCodeClose()
    {
        QRtime = -0.25f;
    }
   
    public void AutoCloseQRPanel()
    {
        QRtime = -1.0f;
        OnShowPanel(QRCodePanel,false);
    }

    
    public void OnShowPayShopingResult(int state, float time = 2)
    {
        StartCoroutine(this.LoadPayShopingPics(state, time));
    }
    private IEnumerator LoadPayShopingPics(int state, float time = 2)
    {
        var objPayShopingResultPanel = GetPanelByName(PayShopingResultPanel);
        GameObject TextImageObject = FindGameObjectInPanel(objPayShopingResultPanel, "TextImage");
        RawImage TextImage = TextImageObject.GetComponent<RawImage>();
        switch (state)
        {
            case 0://失败
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\付款失败") as Texture;
                break;
            case 1://成功
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\付款成功") as Texture;
                break;
            case 2://异常
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\付款异常") as Texture;
                break;
        }
        OnShowPanel(PayShopingResultPanel);
        Invoke("AutoClosePayShopingResult", time);
        yield return null;
    }
    public void AutoClosePayShopingResult()
    {
        OnShowPanel(PayShopingResultPanel,false);
    }
    
   
    public void OnShowOutGoodsResult(int state, float time = 2)
    {
        StartCoroutine(this.LoadOutGoodsPics(state, time));
    }
    private IEnumerator LoadOutGoodsPics(int state, float time = 2)
    {
        var objOutGoodsResultPanel = GetPanelByName(OutGoodsResultPanel);
        GameObject TextImageObject = FindGameObjectInPanel(objOutGoodsResultPanel, "TextImage");
        RawImage TextImage = TextImageObject.GetComponent<RawImage>();
        switch (state)
        {
            case 0://等待出货
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\备货中") as Texture;
                break;
            case 1://出货中
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\备货中") as Texture;
                break;
            case 2://出货成功
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\出货成功") as Texture;
                break;
            case 3://出货失败
                TextImage.texture = Resources.Load(@"Images\Unstore\Icons\出货失败") as Texture;
                break;
        }
        OnShowPanel(OutGoodsResultPanel);
        Invoke("AutoCloseOutResult", time);
        yield return null;
    }
    public void AutoCloseOutResult()
    {
        OnShowPanel(OutGoodsResultPanel,false);
    }

    public void OnShowGameTitle(int index = 1, bool bShow = true)
    {
        if(bShow)
        {
            var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(4) - 0.45f);
            index = (int)randNum;
            var objOutGoodsResultPanel = GetPanelByName(GameTitlePanel);
            GameObject TextImageObject = FindGameObjectInPanel(objOutGoodsResultPanel, "TitleImage");
            RawImage TextImage = TextImageObject.GetComponent<RawImage>();
            //string path = @"Images\Unstore\gameTitle\game"+index;
            if(index == 0)
            {
                TextImage.texture = Resources.Load(@"Images\Unstore\gameTitle\猜拳") as Texture;
            }
            else if(index == 1)
            {
                TextImage.texture = Resources.Load(@"Images\Unstore\gameTitle\抓娃娃") as Texture;
            }
            else if (index == 2)
            {
                TextImage.texture = Resources.Load(@"Images\Unstore\gameTitle\体感火箭") as Texture;
            }
            else if (index == 3)
            {
                TextImage.texture = Resources.Load(@"Images\Unstore\gameTitle\合照") as Texture;
            }
            LastVIPanel = GameTitlePanel;
        }
        OnShowPanel(GameTitlePanel, bShow);
    }

    public void OnShowLRChoosingPanel(bool bShow = true,bool isLeft = true,bool autoplayAmi =true)
    {
        if (bShow)
        {
            var objOutGoodsResultPanel = GetPanelByName(LRChoosingPanel);
            var btnList = objOutGoodsResultPanel.gameObject.GetComponentsInChildren<Button>();
            //foreach(var btn in btnList)
            //{
            //    btn.gameObject.GetComponentInChildren<Image>().gameObject.SetActive(true);
            //}
            _isLeft = isLeft;
            if (isLeft)
            {
                var currentpos = btnList[0].gameObject.transform.localPosition;
                btnList[0].gameObject.transform.localPosition = new Vector3(-Mathf.Abs(currentpos.x), currentpos.y, currentpos.z);
                switch (GetCurChoosePanelName())
                {
                    case YesOrNoPanel:
                        //InitAniPlayerCfg(LRChoosingPanel, "Buttonleft", @"Images\Unstore\SeqFrame\继续\", autoplayAmi);
                        break;
                    case ShoppingPanel:
                        break;
                    case OKroNotPanel:
                        break;
                }
            }
            else
            {
                var currentpos = btnList[0].gameObject.transform.localPosition;
                btnList[0].gameObject.transform.localPosition = new Vector3(Mathf.Abs(currentpos.x), currentpos.y, currentpos.z);
                switch (GetCurChoosePanelName())
                {
                    case YesOrNoPanel:
                        //InitAniPlayerCfg(LRChoosingPanel, "Buttonleft", @"Images\Unstore\SeqFrame\取消\", autoplayAmi);
                        break;
                    case ShoppingPanel:
                        break;
                    case OKroNotPanel:
                        break;
                }
            }
        }
        else
        {
            CurChoosePanel = "";
        }
        OnShowPanel(LRChoosingPanel, bShow);
    }

    public bool GetLRChoosingLeft()
    {
        return _isLeft;
    }
    #endregion
}
