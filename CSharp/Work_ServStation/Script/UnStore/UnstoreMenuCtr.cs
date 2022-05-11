using Newtonsoft.Json.Linq;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Threading;
using System.Threading.Tasks;
public class UnstoreMenuCtr : BtnManager
{
    private static List<string> interfaceLevel_;
    private string m_changeStage = "";
    // Start is called before the first frame update
    public delegate void MesDispatchDelegate(string recipient, string mes);
    public MesDispatchDelegate mesDispatch_ = null;
    public Action getInfoAction = null;
    public Action getQRCodeAction = null;
    public Action<string> dealServiceAction = null;

    public Action<string> buyGoodsFlowAction = null;

    public Action<int,string> askKTVAction = null;
    public Action<int, string> askScreenProAction = null;
    //public Action localMapAction = null;
    //public Action karaokeAction = null;
    //public Action screenProAction = null;


    //public PanelInstance DebugPanel;
    public Button DebugShowBtn;

    public string panelsCtrAttachName = "Canvas";
    //private UnstorePanelCtr panelCtr;
    private UnstoreBoxListCtrApi unstorProductCtr;

    public string charactorCtrAttachName = "Canvas";
    //private CharactorPanelCtr charactorPanelCtr;
    private CharactorMVCtr charactorMVCtr;
    //public string moviePath = "零度百事可乐.mp4";
    // Start is called before the first frame update
    #region buttons
    //public Button button_Karaoke;
    //public Button button_ScreenPro;
    //public Button button_RescueMap;
    public Button button_Pre;
    public Button button_Next;
    public Button button_Select;
    public Button button_Cancel;
    public Button button_Tomain;
    public Button button_CustomerSay;
    public Button button_XiaokaSay;
    public Button button_Start;
    public Button button_Stop;
    public Button button_AskYes;
    public Button button_PayScs;
    public Button button_Product;
    public Button button_Shopping;
    public Button button_ZhiBoVideo;
    public Button button_GetQRCode;
    public Button button_Greet;
    #endregion

    private int BtnIndex;
    private int GoodsIndex = 0;
    //private Animator ani;
    private PanelsCtrlApi unstorePanelCtrlApi;
    private AutoResetEvent waitEvent_;//handle QR pay event

    public PanelInstance[] RescuePages;//所有应急救援Panel
    private int currentRescueIndex = 0;
    private PanelsCtrlApi.UIState uiState = PanelsCtrlApi.UIState.MAIN_UI;
    void Start()
    {
        //初始化互动聊天流程

        interfaceLevel_ = new List<string>();
        interfaceLevel_.Add("趣味互动");
        interfaceLevel_.Add("互动长聊天1");
        interfaceLevel_.Add("ChoosePanel");
        interfaceLevel_.Add("ChatAfterGame");

        waitEvent_ = new AutoResetEvent(true);
        //Handheld.PlayFullScreenMovie(moviePath, Color.black, FullScreenMovieControlMode.Full);
        //Handheld.PlayFullScreenMovie("test.mp4", Color.black, FullScreenMovieControlMode.CancelOnInput);
        button_Next?.onClick.AddListener(OnRrightGestureHandle);
        button_Pre?.onClick.AddListener(OnLeftGestureHandle);
        button_Cancel?.onClick.AddListener(OnCancel);
        button_Select?.onClick.AddListener(OnSureGestureHandle);
        button_CustomerSay?.onClick.AddListener(OnCustomerSayBtn);
        button_XiaokaSay?.onClick.AddListener(OnXiaokaSayBtn);
        button_Tomain?.onClick.AddListener(OnToMainUI);
        button_Start?.onClick.AddListener(OnStartBtn);
        button_Stop?.onClick.AddListener(OnStopBtn);
        button_ZhiBoVideo.onClick.AddListener(OnGetInfoBtn);
        button_GetQRCode?.onClick.AddListener(OnGetQRCodeBtn);
        button_Greet?.onClick.AddListener(OnGreetBtn);
        button_Product?.onClick.AddListener(OnServiceBtn);
        button_Shopping?.onClick.AddListener(OnShoppingBtn);
        button_AskYes?.onClick.AddListener(OnAskYesBtn);
        //button_Karaoke.onClick.AddListener(OnKaraokeBtn);
        //button_ScreenPro.onClick.AddListener(OnScreenProBtn);
        //button_RescueMap.onClick.AddListener(OnRescueMapBtn);
        Init();
        BtnIndex = GetCurrentBtnIndex();

        unstorePanelCtrlApi = GameObject.Find(panelsCtrAttachName).GetComponentInChildren<PanelsCtrlApi>();
        unstorePanelCtrlApi?.gameObject.SetActive(true);
        //charactorPanelCtr = GameObject.Find(charactorCtrAttachName).GetComponent<CharactorPanelCtr>();
        charactorMVCtr = GameObject.Find(charactorCtrAttachName).GetComponent<CharactorMVCtr>();

        unstorProductCtr = GameObject.Find(panelsCtrAttachName).GetComponent<UnstoreBoxListCtr>();
        //ShowMsgDebug("测试");
    }
    public void ResetStage()
    {
        m_changeStage = "";
        setCurRootPath(ServiceManager.jsonParam.charactorVideos.foldPath[0]);
        Debug.Log("重置随机切换角色" + m_changeStage);

    }
    public string GetRandChaneStage()
    {
        if (m_changeStage == "") 
        {
            var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(interfaceLevel_.Count) - 0.45f);
            int num = (int)randNum;
            m_changeStage = interfaceLevel_[num];
            Debug.Log("随机切换角色的阶段：" + m_changeStage);
        }
        return m_changeStage;
    }
    private void OnAskYesBtn()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.YES_NO);
    }

    private void OnShoppingBtn()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
        ResetStage();
    }

    public void OnServiceBtn()
    {
        //waitEvent_.Reset();
        //Task.Run(() =>
        //{
        //    if (!waitEvent_.WaitOne(2000))
        //    {
        //        OnCloseAllPanel();
        //        OnShowServicesList();
        //    }
        //});
        mediaOpenFileOnceAddLoop("YS演示类", "G_YS001", getRand("G_YS001_1", "G_YS001_2"));
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        charactorMVCtr?.Enable(false);
        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(12000))
            {
                OnShowServicesList();
            }
        });
        
    }
    public void OnShowAmusement()
    {
        mediaOpenFileOnceAddLoop("YS演示类", "G_YS001", "G_YS001");
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        charactorMVCtr?.Enable(false);
        //OnSampleShopPlay();
        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(4000))
            {
                OnCloseAllPanel();
                OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Amusement, "影音娱乐");
            }
        });
    }
    // Update is called once per frame
    void Update()
    {

    }

    #region 1.button reaction
    /// <summary>
    /// 切换上一个选择(待实现)
    /// </summary>
    public void OnPreBtn()
    {
        if (Btns != null && Btns.Length > 0)
        {
            BtnIndex--;
            if (BtnIndex < 0)
            {
                BtnIndex = Btns.Length - 1;
            }
            SetCurrentBtn(Btns[BtnIndex]);
        }


        unstorProductCtr?.moveFront(1);
    }

    /// <summary>
    /// 切换下一个选择(待实现)
    /// </summary>
    public void OnNextBtn()
    {
        //视频列表加一播放下一个视频，并进行避免越界操作
        //currentClipIndex += 1;
        //currentClipIndex = currentClipIndex % videoClips.Length;
        //videoPlayer.clip = videoClips[currentClipIndex];
        //text_PlayOrPause.text = "暂停";
        if(Btns !=null && Btns.Length >0)
        {
            BtnIndex++;
            if (BtnIndex >= Btns.Length)
            {
                BtnIndex = 0;
            }
            SetCurrentBtn(Btns[BtnIndex]);
        }


        unstorProductCtr?.moveFront(-1);
    }

    /// <summary>
    /// 确认选择
    /// </summary>
    public void OnSelect()
    {
        SetState(ButtonState.SELECTED);

        var currentBtnText = currentBtn?.gameObject.name;

        OnServiceByName(currentBtnText);

        //OnShowUIBtnState(UnstorePanelCtr.UIBtnState.HIDE);

        //charactorMVCtr?.Enable(false);
        var id = unstorProductCtr?.GetCurrentGoodsID();//1~

        //判断BoxList 类型然后处理对应事件
        var boxType = GetBoxType();
        if (boxType == UnstoreBoxListCtrApi.BoxListType.Service || boxType == UnstoreBoxListCtrApi.BoxListType.GoodsType )
        {
            dealServiceAction(id);
        }
        else
            buyGoodsFlowAction(id);
        
    }

    public void OnCharactorEn(bool isEn = true)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnCharactorCourtine(isEn));
    }

    private IEnumerator  OnCharactorCourtine(bool isEn = true)
    {
        charactorMVCtr?.Enable(isEn);
        yield return null;
    }

    public void OnMoveToTop(string num ="4")
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnMoveToTopCourtine(num));
    }
    private IEnumerator OnMoveToTopCourtine(string num = "4")
    {
        //GoodsIndex = num;
        //int centerIndex = 3;

        var centerIndex = Convert.ToInt32(unstorProductCtr?.GetCurrentGoodsID());//1~
        int iNum = Convert.ToInt32(num);
        if(ServiceManager.IsNumber(num))
        {
            if (iNum > centerIndex)
            {
                for(int i = 0;i<iNum-centerIndex;i++)
                {
                    unstorProductCtr?.moveFront(-1);
                }

            }
            else
            {
                for (int i = 0; i < centerIndex-iNum; i++)
                {
                    unstorProductCtr?.moveFront(1);
                }
            }
        }
        
        yield return null;
    }
    public void OnShakeGoods(int num = 0,float time = 1.0f)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnShakeGoodsCourtine(num,time));
    }
    private IEnumerator OnShakeGoodsCourtine(int num = 0, float time = 1.0f)
    {
        GoodsIndex = num;
        unstorProductCtr?.ShakeAct(num, true);
        Invoke("ShakeStop", time);
        yield return null;
    }
    public string OnGetGoodsName(int num=0)
    {
        return unstorProductCtr?.GetCurrentGoodsName(num);
    }
    

    private void ShakeStop()
    {
        unstorProductCtr?.ShakeAct(GoodsIndex, false);
    }
    /// <summary>
    /// 通过按钮名称判断选择的服务
    /// </summary>
    /// <param name="btnName">按钮对象的名称</param>
    public void OnSelectByWord(string btnName)
    {
        foreach (var btn in Btns)
        {
            if (btnName == btn.gameObject.name)
            {
                SetCurrentBtn(btn);
            }
        }
        SetState(ButtonState.SELECTED);

        var currentBtnText = currentBtn?.gameObject.name;

        OnServiceByName(currentBtnText);
    }

    private void OnServiceByName(string btnName)
    {
        switch (btnName)
        {
            case "Movie":
                //OnMovieService();
                break;
            case "Emgcyknowledge":
                //OnEmgcyKnowledgeService();
                break;
            case "EmgcyPhone":
                //OnEmgcyPhoneService();
                break;
            case "EmgcyCall":
                //OnEmgcyCallService();
                break;
        }
    }

    /// <summary>
    /// 关闭全部UI
    /// </summary>
    public void OnCloseAllPanel()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnCloseAllPanelCourtine());
    }
    private IEnumerator OnCloseAllPanelCourtine()
    {
        unstorePanelCtrlApi?.CloseAllPanels();
        yield return null;
    }
    /// <summary>
    /// 打开主界面
    /// </summary>
    public void OnOpenMainUIPanel()
    {
        uiState = PanelsCtrlApi.UIState.MAIN_UI;

        UnityMainThreadDispatcher.Instance().Enqueue(OnOpenMainUIPanelCourtine());
    }
    private IEnumerator OnOpenMainUIPanelCourtine()
    {
        unstorePanelCtrlApi?.openMainUIPanel();
        yield return null;
    }
    /// <summary>
    /// 返回主界面(音频)
    /// </summary>
    public void OnToMainUI()
    {
        //OnShowUIBtnState(UnstorePanelCtr.UIBtnState.HIDE);
        //panelCtr?.openMainUIPanel();
        //charactorMVCtr?.mediaOpenRand("DJ待机类");
        UnityMainThreadDispatcher.Instance().Enqueue(OnToMainCourtine());
    }

    private IEnumerator OnToMainCourtine()
    {
        ResetStage();
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
        unstorePanelCtrlApi?.openMainUIPanel();
        charactorMVCtr?.setVoiceEn(true);
        charactorMVCtr?.mediaOpenRand("DJ待机类");
        uiState = PanelsCtrlApi.UIState.MAIN_UI;
        SetLastVIPanelName();
        SetCurChoosePanelName();
        yield return null;
    }

    public void OnShowGameTitle(int i)
    {
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        //mediaOpenFileOnceAddLoop("CG成功类", "G_CG003", "G_CG003");
        
        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(5000))
            {
                UnityMainThreadDispatcher.Instance().Enqueue(OnShowGameTitleCourtine(1));

            }
        });
    }

    private IEnumerator OnShowGameTitleCourtine(int i)
    {
        unstorePanelCtrlApi?.OnShowGameTitle(i);

        yield return null;
    }

    public void OnShortVideo()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);

        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("YS演示类");
        videos.Add("短视频");
        voices.Add("nullQuit");
        foders.Add("ZH招呼类");
        videos.Add("G_ZH003");
        voices.Add("G_ZH003");
        foders.Add("CG成功类");
        videos.Add("G_CG002");
        voices.Add("G_CG002");
        foders.Add("TS特殊类");
        videos.Add("G_TS001");
        voices.Add("G_TS001");
        foders.Add("TS特殊类");
        videos.Add("G_TS002");
        voices.Add("G_TS002");
        foders.Add("SH说话类");
        videos.Add("G_SH006");
        voices.Add("G_SH006");
        mediaOpenFileOrderAddLoop(foders, videos, voices);
    }
    private IEnumerator OnShortVideoCourtine()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHORTVIDEO);
        yield return null;
    }

    public void OnBuyGoodsChatAct()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("TS特殊类");
        videos.Add("G_TS004");
        voices.Add("G_TS004_1");//太好了终于等到你了
        mediaOpenFileOrderAddLoop(foders, videos, voices);

    }
    public void OnChatAct()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add(getRand("G_SH001_XNFW_1", "G_SH001_XNFW_2"));//太好了终于等到你了
        foders.Add("SH说话类");
        videos.Add("G_SH002");
        voices.Add("G_SH002_XNFW_1");//等到菠菜都变成菜干了
        foders.Add("SH说话类");
        videos.Add("G_SH003");
        voices.Add("G_SH003_XNFW_1");//一看就知道你一定很喜欢旅游，是吧
        mediaOpenFileOrderAddLoop(foders, videos, voices);
        
    }
    private IEnumerator OnChatActCourtine()
    {
        
        yield return null;
    }

    public void OnRelyChat()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnReplyChatCourtine());
    }

    private IEnumerator OnReplyChatCourtine()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        var voice = getRand("G_SH003_XNFWHF_1", "G_SH001_XNFWHF_1");
        if (voice.Contains("G_SH003"))
        {
            videos.Add("G_SH003");
        }
        if (voice.Contains("G_SH001"))
        {
            videos.Add("G_SH001");
        }
        voices.Add(voice);//真的吗？看您的样子就知道是个旅游达人；
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add("G_SH001_XNFWHF_2");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        foders.Add("SH说话类");
        videos.Add("G_SH003");
        voices.Add("G_SH003_XNFWHF_2");//您是喜欢自驾游这种旅游方式吧？我觉得自驾游超酷的；
        mediaOpenFileOrderAddLoop(foders, videos, voices, OnShowKTVScreen);
        yield return null;
    }

    public void OnNoRelyChat()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnNoReplyChatCourtine());
    }

    private IEnumerator OnNoReplyChatCourtine()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        videos.Add("G_SH002");
        voices.Add("G_SH002_XNFWHF_1");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add("G_SH001_XNFWHF_2");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        foders.Add("SH说话类");
        videos.Add("G_SH003");
        voices.Add("G_SH003_XNFWHF_2");//您是喜欢自驾游这种旅游方式吧？我觉得自驾游超酷的；
        mediaOpenFileOrderAddLoop(foders, videos, voices, OnShowKTVScreen);
        yield return null;
    }

    public void OnErrRelyChat()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnErrReplyChatCourtine());
    }

    private IEnumerator OnErrReplyChatCourtine()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SW失望类");
        videos.Add("G_SW001");
        voices.Add("G_SW001_XNFWHF_1");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add("G_SH001_XNFWHF_2");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        foders.Add("SH说话类");
        videos.Add("G_SH003");
        voices.Add("G_SH003_XNFWHF_2");//您是喜欢自驾游这种旅游方式吧？我觉得自驾游超酷的；
        mediaOpenFileOrderAddLoop(foders, videos, voices, OnShowKTVScreen);
        yield return null;
    }

    private void OnShowKTVScreen()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.KTVSCREEN);
    }
    public void OnChatActLong()
    {

        UnityMainThreadDispatcher.Instance().Enqueue(OnChatActLongCourtine());
    }
    private IEnumerator OnChatActLongCourtine()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        videos.Add("G_SH008");
        voices.Add("G_SH008");//真的吗？看您的样子就知道是个旅游达人；
        foders.Add("SH说话类");
        videos.Add("G_SH004");
        voices.Add("G_SH004");//是吧，是吧，我猜对了菠菜也喜欢旅游；
        mediaOpenFileOrderAddLoop(foders, videos, voices);

        yield return null;
    }

    public void OnChatAfterGame()
    {
        if (GetRandChaneStage() == "ChatAfterGame")
            setCurRootPath(ServiceManager.jsonParam.charactorVideos.foldPath[1]);
        unstorePanelCtrlApi?.OnShowGameTitle(1,false);
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add("G_SH001_LT_1");
        foders.Add("CG成功类");
        videos.Add("G_CG001");
        voices.Add("G_CG001_LT_1");
        foders.Add("TS特殊类");
        videos.Add("G_TS001");
        voices.Add("nullQuit");
        foders.Add("TS特殊类");
        videos.Add("G_TS003");
        voices.Add(getRand("G_TS003_LT_1", "G_TS003_LT_2"));
        foders.Add("SH说话类");
        videos.Add("G_SH003");
        voices.Add("G_SH003_LT_1");

        mediaOpenFileOrderAddLoop(foders, videos, voices, openRandPanel);
    }

    private void openRandPanel()
    {
        //unstorePanelCtrlApi?.OpenPanelByName(getRand("VirtualHostPanel"
        //    , "LiveBroadcastPanel", "ShortVideoPanel"));
        
        //unstorePanelCtrlApi?.OpenPanelByName("ZhiBoChoosePanel");

        Invoke("RandLivBroadCast", 5.0f);
    }

    private void RandLivBroadCast()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("YS演示类");
        videos.Add(getRand("直播", "短视频", "虚拟主持人"));
        voices.Add("nullQuit");
        mediaOpenFileOrderAddLoop(foders, videos, voices,OnToMainUI);
    }

    public void OnChatActLong2()
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("SH说话类");
        videos.Add("G_SH005");
        voices.Add("G_SH005");
        foders.Add("YS演示类");
        videos.Add("G_YS001");
        voices.Add("G_YS001");
        mediaOpenFileOrderAddLoop(foders, videos, voices);
        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(4000))
            {
                UnityMainThreadDispatcher.Instance().Enqueue(OnChatActLong2Courtine());
            }
        });
    }
    private IEnumerator OnChatActLong2Courtine()
    {
        OnCloseAllPanel();
        charactorMVCtr?.Enable(false);
        OnShowConcreteServerList(UnstoreBoxListCtrApi.BoxListType.Amusement, "影音娱乐");

        yield return null;
    }
    public void OnChatActShort()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnChatActShortCourtine());
    }
    private IEnumerator OnChatActShortCourtine()
    {
        //var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(2) - 0.45f);
        //int num = (int)randNum;
        //if (num == 0)
        //{
        //    mediaOpenFileOnceAddLoop("CG成功类", "G_CG003", "G_CG003");
        //}
        //if (num == 1)
        //{
        //    mediaOpenFileOnceAddLoop("SW失望类", "G_SW001", "G_SW001");
        //}
        mediaOpenFileOnceAddLoop("SW失望类", "G_SW001", "G_SW001");
        //waitEvent_.Reset();
        //Task.Run(() =>
        //{
        //    if (!waitEvent_.WaitOne(6000))
        //    {
        //        mediaOpenFileOnceAddLoop("SW失望类", "G_SW002", "G_SW002");
        //    }
        //});
        yield return null;
    }

    public void OnDownLoadAPP()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnDownLoadAPPCourtine());
        mediaOpenFileOnceAddLoop("CG成功类", "G_CG003", "G_CG003");
    }
    private IEnumerator OnDownLoadAPPCourtine()
    {
        unstorePanelCtrlApi.setUIBtnPanelState(PanelsCtrlApi.UIBtnState.APP);

        yield return null;
    }
    public void OnZhiboVR()
    {
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add("YS演示类");
        videos.Add("直播");
        voices.Add("nullQuit");
        foders.Add("ZH招呼类");
        videos.Add("G_ZH003");
        voices.Add("G_ZH003");
        foders.Add("CG成功类");
        videos.Add("G_CG002");
        voices.Add("G_CG002");
        foders.Add("TS特殊类");
        videos.Add("G_TS001");
        voices.Add("G_TS001");
        foders.Add("TS特殊类");
        videos.Add("G_TS002");
        voices.Add("G_TS002");
        foders.Add("SH说话类");
        videos.Add("G_SH006");
        voices.Add("G_SH006");
        mediaOpenFileOrderAddLoop(foders, videos, voices);
    }
    private IEnumerator OnZhiboVRCourtine()
    {
        //unstorePanelCtrlApi.setUIBtnPanelState(PanelsCtrlApi.UIBtnState.ZHIBO);
        yield return null;
    }
    public void OnVRhost()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnVRhostCourtine());
        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(6000))
            {
                OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);

                List<string> foders = new List<string>();
                List<string> videos = new List<string>();
                List<string> voices = new List<string>();
                foders.Add("ZH招呼类");
                videos.Add("G_ZH003");
                voices.Add("G_ZH003");
                foders.Add("CG成功类");
                videos.Add("G_CG002");
                voices.Add("G_CG002");
                foders.Add("TS特殊类");
                videos.Add("G_TS001");
                voices.Add("G_TS001");
                foders.Add("TS特殊类");
                videos.Add("G_TS002");
                voices.Add("G_TS002");
                foders.Add("SH说话类");
                videos.Add("G_SH006");
                voices.Add("G_SH006");
                mediaOpenFileOrderAddLoop(foders, videos, voices);
            }
        });
    }
    private IEnumerator OnVRhostCourtine()
    {
        //unstorePanelCtrlApi.setUIBtnPanelState(PanelsCtrlApi.UIBtnState.ZHIBO);
        
        yield return null;
    }

    public void OnShowPayShopingResult(int state = 1, float time = 2)
    {

        //StartCoroutine(OnShowPayShopingResultCourtine(state, time));
        UnityMainThreadDispatcher.Instance().Enqueue(OnShowPayShopingResultCourtine());

    }
    private IEnumerator OnShowPayShopingResultCourtine(/*int state, float time*/ )
    {
        //unstorePanelCtrlApi?.OnShowPayShopingResult(state,time);
        unstorePanelCtrlApi?.OnShowPayShopingResult(0);
        yield return null;
    }
   
    public void OnShowOutgoodResult(int state = 2, float time = 2)
    {
        StartCoroutine(this.OnShowOutgoodResultCourtine(state, time));
    }
    private IEnumerator OnShowOutgoodResultCourtine(int state, float time)
    {
        unstorePanelCtrlApi?.OnShowOutGoodsResult(state, time);
        yield return null;
    }
    private void OnCustomerSayBtn()
    {
        string content = "你好";
        OnCustomerSay(content);
    }

    private void OnXiaokaSayBtn()
    {
        string content = "你好";
        string wavPath = @"F:\source1.mp3";
        OnXiaokaSay(content, wavPath);
    }

    public void OnStartBtn()
    {
        if (mesDispatch_ == null)
            return;
        mesDispatch_("VOICE_AI", "startgame");
        Debug.LogWarning("发送了一次startgame");
        //panelCtr.OnPlayAdvertisement(false);
    }

    public void OnStopBtn()
    {
        //panelCtr.OnPlayAdvertisement(true);
        if (mesDispatch_ == null)
            return;
        mesDispatch_("VOICE_AI", "endgame");
    }

    public void OnGreetBtn()
    {
        ResetStage();
        //charactorMVCtr?.mediaOpenRand("ZH招呼类");
        //mediaOpenFileOnceAddLoop("ZH招呼类", "G_ZH001", "G_ZH001");
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        foders.Add("ZC转场类");
        videos.Add("G_ZC003");
        voices.Add("nullQuit");
        foders.Add("SH说话类");
        videos.Add("G_SH001");
        voices.Add(getRand("G_SH001_1", "G_SH001_2"));
        mediaOpenFileOrderAddLoop(foders, videos, voices);
    }

    private string getRand(string str1,string str2=null,
        string str3=null,string str4 = null)
    {
        string str = str1;
        List<string> list = new List<string>();
        if(str1 != null)
            list.Add(str1);
        if (str2 != null)
            list.Add(str2);
        if (str3 != null)
            list.Add(str3);
        if (str4 != null)
            list.Add(str4);

        var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(list.Count) - 0.45f);
        int num = (int)randNum;
        //mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL,
        //fileList[num]);
        str = list[num];
        return str;
    }

    public void OnLeave()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnLeaveCourtine());
    }

    private IEnumerator OnLeaveCourtine()
    {
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        mediaOpenFileOnceAddLoop("SW失望类", "G_SW003", "G_SW002");

        waitEvent_.Reset();
        Task.Run(() =>
        {
            if (!waitEvent_.WaitOne(5500))
            {
                OnToMainUI();
            }
        });

        yield return null;
    }

    public void OnSampleShopPlay()
    {
        charactorMVCtr?.mediaOpenRand("YS演示类");
    }

    public void OnSelectPlay()
    {
        charactorMVCtr?.mediaOpenRand("XZ选择类");
    }

    public void ShowMsgDebug(string msg)
    {
        if (DebugShowBtn == null)
            return;
        Text word = DebugShowBtn.gameObject.GetComponentInChildren<Text>();
        word.text = msg;
    }

    public void OnGetInfoBtn()
    {
        //if (getInfoAction == null)
        //    return;
        //getInfoAction();
        OnShowUIBtnState(PanelsCtrlApi.UIBtnState.ZHIBOCH);
        //OnSampleShopPlay();//播放选择动作
    }

    public void OnGetQRCodeBtn()
    {
        if (getQRCodeAction == null)
            return;
        getQRCodeAction();
    }

    public void OnSwitchToGameChoosePanel()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnSwitchToGameChoosePanelCourtine());
    }

    private IEnumerator OnSwitchToGameChoosePanelCourtine()
    {
        if (GetRandChaneStage() == "ChoosePanel")
            setCurRootPath(ServiceManager.jsonParam.charactorVideos.foldPath[1]);
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        OnCloseAllPanel();
        OnOpenMainUIPanel();
        foders.Add("ZH招呼类");
        videos.Add("G_ZH003");
        voices.Add(getRand("G_SH003_LT_1", "G_SH003_LT_2"));
        mediaOpenFileOrderAddLoop(foders, videos, voices, OnGameShow);
        //Invoke("openGameChoosePanel", 0.25f);

        //Invoke("openGameChoosePanel", 0.25f);
        //OnChatAfterGame
        yield return null;
    }
   
    private void OnGameShow()
    {
        OnShowGameTitle(1);
        Invoke("OnChatAfterGame", 7.0f);
    }

    private void openGameChoosePanel()
    {
        unstorePanelCtrlApi?.OpenPanelByName("GameChoosePanel");
    }
    
    #endregion


    #region service handle

    #region 1.语音动作基本交互服务
    public int GetBoxType()
    {
        return (int)unstorProductCtr?.GetBoxType();
    }

    public void OnCloseBoxList(float time = 0.0f)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnCloseBoxListCourtine(time));
    }
    private IEnumerator OnCloseBoxListCourtine(float time)
    {
        Invoke("closeBoxList", time);

        yield return null;
    }
    private void closeBoxList()
    {
        unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.None);
        unstorePanelCtrlApi?.OnShowProductList(false);
    }

    public void OnShowServicesList()
    {
        UnityMainThreadDispatcher.Instance().Enqueue(onShowServiceListCourtine());
    }
    private IEnumerator onShowServiceListCourtine()
    {
        mediaOpenFileOnceAddLoop("YS演示类", "G_YS001", "G_YS001_3");
        unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Service);
        unstorePanelCtrlApi?.OnShowProductList(true);
        unstorProductCtr?.initState();
        yield return null;
    }
    
    public void OnShowConcreteServerList(int serverType, string ServiceName)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnShowConcreteServerListCourtine(serverType,ServiceName));
    }
    private IEnumerator OnShowConcreteServerListCourtine(int serverType, string ServiceName)
    {
        //OnCloseAllPanel();

        switch (serverType)
        {
            case 2:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.GoodsType, ServiceName);
                break;
            case 3:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.OutDoor, ServiceName);
                break;
            case 4:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.WaterElec, ServiceName);
                break;
            case 5:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Shower, ServiceName);
                break;
            case 6:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Rescue, ServiceName);
                break;
            case 7:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Kitchen, ServiceName);
                break;
            case 8:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Amusement, ServiceName);
                break;
            case 9:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Food, ServiceName);
                break;
            case 10:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.Drink, ServiceName);
                break;
            case 11:
                unstorProductCtr?.SetBoxType(UnstoreBoxListCtrApi.BoxListType.AgriP, ServiceName);
                break;
        }
        unstorePanelCtrlApi?.OnShowProductList(true, ServiceName);
        unstorProductCtr?.initState();
        yield return null;
    }

    public void OnCloseQRCode()
    {
        unstorePanelCtrlApi?.SetQRCodeClose();
    }
    public void OnShowQRCode(string qrUrl, string name ="",float fCost = 10, float time = 20)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnShowQRCodeCourtine(qrUrl, name, fCost, time));
    }
    private IEnumerator OnShowQRCodeCourtine(string qrUrl, string name = "", float fCost = 10, float time = 20)
    {
        unstorePanelCtrlApi?.OnShowQRCode(qrUrl, name, fCost, time);
        yield return null;
    }
    public void OnShowErro(string title = "操作错误", string details = null,
        string tips = null, float time = 3)
    {
        unstorePanelCtrlApi?.OnShowErro(title, details, tips, time);
    }
    public void OnShowPayGameResult(JObject info, float time = 3)
    {
        unstorePanelCtrlApi?.OnShowPayGameResult(info,time);
    }
    public string GetCurChoosePanelName()
    {
        return unstorePanelCtrlApi?.GetCurChoosePanelName();
    }
   
    public void SetCurChoosePanelName(string panel ="")
    {
        unstorePanelCtrlApi?.SetCurChoosePanelName(panel);
    }
    public string GetLastVIPanelName()
    {
        return unstorePanelCtrlApi?.GetLastVIPanelName();
    }
    public void SetLastVIPanelName(string panel = "")
    {
        unstorePanelCtrlApi?.SetLastVIPanelName(panel);
    }
    public void OnLeftGestureHandle()
    {
        //unstorePanelCtrlApi?.OnShowLRChoosingPanel(false);
        OnMoveRecueMap(-1);
        switch (GetCurChoosePanelName())
        {
            case PanelsCtrlApi.BoxListPanel:
                //menuCtr?.OnPreBtn();
                OnNextBtn();//镜像
                break;
            case PanelsCtrlApi.YesOrNoPanel:
            case PanelsCtrlApi.ShoppingPanel:
            case PanelsCtrlApi.OKroNotPanel:
            case PanelsCtrlApi.KTVScreenPanel:
                //case PanelsCtrlApi.ZhiBoChoosePanel:
                //case PanelsCtrlApi.GameChoosePanel:
                unstorePanelCtrlApi?.OnShowLRChoosingPanel(true,true);
                break;
        }

    }
    public void OnRrightGestureHandle()
    {
        //unstorePanelCtrlApi?.OnShowLRChoosingPanel(false);
        OnMoveRecueMap(1);
        switch (GetCurChoosePanelName())
        {
            case PanelsCtrlApi.BoxListPanel:
                //menuCtr?.OnNextBtn();
                OnPreBtn();//镜像
                break;
            case PanelsCtrlApi.YesOrNoPanel:
            case PanelsCtrlApi.ShoppingPanel:
            case PanelsCtrlApi.OKroNotPanel:
            case PanelsCtrlApi.KTVScreenPanel:
                //case PanelsCtrlApi.ZhiBoChoosePanel:
                //case PanelsCtrlApi.GameChoosePanel:
                unstorePanelCtrlApi?.OnShowLRChoosingPanel(true,false);
                break;
        }
    }
    public void OnSureGestureHandle()
    {
        bool isLeft = unstorePanelCtrlApi.GetLRChoosingLeft();
        var state = PanelsCtrlApi.UIBtnState.HIDE;
        var CurChoosePanel = GetCurChoosePanelName();
        switch (CurChoosePanel)
        {
            case PanelsCtrlApi.BoxListPanel:
                OnSelect();
                break;
            case PanelsCtrlApi.YesOrNoPanel:
                if (isLeft)
                    OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);//继续购物
                else
                    OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
                break;
            case PanelsCtrlApi.ShoppingPanel:
                //if(isLeft)
                OnShowUIBtnState(PanelsCtrlApi.UIBtnState.HIDE);
                if (!isLeft)
                {
                    OnServiceBtn();
                }
                else
                {
                    OnChatAct();
                    SetLastVIPanelName("趣味互动");
                }
                break;
            case PanelsCtrlApi.OKroNotPanel:
                if (isLeft)
                {
                    OnDownLoadAPP();
                }
                else
                    OnChatActShort();
                break;
            case PanelsCtrlApi.KTVScreenPanel:
                if(!isLeft)
                {
                    askKTVAction(-1,"K歌");
                }
                else
                {
                    askScreenProAction(-1,"投屏");
                }
                break;

                #region old Code
                //case PanelsCtrlApi.ZhiBoChoosePanel:
                //    if (isLeft)
                //    {
                //        //var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(2) - 0.45f);
                //        //int num = (int)randNum;
                //        //if (num == 0)
                //        //{
                //        //    // VR直播
                //        //    OnZhiboVR();
                //        //}
                //        //if (num == 1)
                //        //{
                //        //    //主持人 直播
                //        //    OnZhiboRealMan();
                //        //}
                //        OnZhiboVR();
                //        SetLastVIPanelName("直播");
                //    }
                //    else
                //    {
                //        OnShortVideo();
                //        SetLastVIPanelName("短视频");
                //    }
                //    break;
                //case PanelsCtrlApi.GameChoosePanel:
                //    if (isLeft)
                //    {
                //        OnShowGameTitle(1);
                //    }
                //    else
                //    {
                //        OnShowGameTitle(2);
                //    }
                //    break;
                #endregion
        }
        unstorePanelCtrlApi?.OnShowLRChoosingPanel(false);
    }

    /// <summary>
    /// 取消选择
    /// </summary>
    public void OnCancel()
    {
        SetState(ButtonState.DEFAULT);

        var CurChoosePanel = GetCurChoosePanelName();
        switch (CurChoosePanel)
        {
            case PanelsCtrlApi.BoxListPanel:
                OnShowUIBtnState(PanelsCtrlApi.UIBtnState.SHOPPING);
                break;

            case PanelsCtrlApi.OKroNotPanel:
                OnChatActShort();
                break;

        }
    }
    public void setCurRootPath(string rootPath)
    {
        charactorMVCtr?.setCurRootPath(rootPath);
    }
    private void mediaOpenFileOrderAddLoop(List<string> foders, List<string> videos, List<string> voices, Action act = null)
    {
        List<string> loopfoders = new List<string>();
        List<string> loopvideos = new List<string>();
        List<string> loopvoices = new List<string>();
        //循环动作
        loopfoders.Add("T基本类");
        loopvideos.Add("G_T001");
        loopvoices.Add("nullQuit");
        charactorMVCtr?.mediaOpenFileOnceAddLoop(foders, videos, voices, loopfoders,loopvideos, loopvoices,".mov", act);
    }
    private void mediaOpenFileOnceAddLoop(string folder,string video,string voice)
    {
        List<string> foders = new List<string>();
        List<string> videos = new List<string>();
        List<string> voices = new List<string>();
        foders.Add(folder);
        videos.Add(video);
        voices.Add(voice);
        List<string> loopfoders = new List<string>();
        List<string> loopvideos = new List<string>();
        List<string> loopvoices = new List<string>();
        //循环动作
        loopfoders.Add("T基本类");
        loopvideos.Add("G_T001");
        loopvoices.Add("nullQuit");
        charactorMVCtr?.mediaOpenFileOnceAddLoop(foders, videos, voices, loopfoders,loopvideos, loopvoices);
    }

    public void OnShowUIBtnState(int state)
    {
        //panelCtr.setUIBtnPanelState(state);
        OnOpenMainUIPanel();
        UnityMainThreadDispatcher.Instance().Enqueue(showUIBtnCourtine(state));
    }

    private IEnumerator showUIBtnCourtine(int state)
    {
        //unstorePanelCtrlApi?.openMainUIPanel();
        //uiState = PanelsCtrlApi.UIState.MAIN_UI;
        switch (state)
        {
            case 0:
                break;
            case 1://YES_NO
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", getRand("G_XZ001_JX_1", "G_XZ001_JX_2"));
                break;
            case 2://购物或互动
                //mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", "G_XZ001");
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", getRand("G_XZ001_1", "G_XZ001_2"));
                break;
            case 3://APP下载选择
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", "G_SH007");
                break;
            case 4://APP下载
                //charactorMVCtr?.mediaOpenFileOnce("XZ选择类", "G_XZ001", "G_XZ002");
                break;
            case 5://直播选择
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", "G_XZ004");
                break;
            case 6://游戏选择
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", "G_XZ005");
                break;
            case 7://短视频
                break;
            case 8://投屏或K歌
                mediaOpenFileOnceAddLoop("XZ选择类", "G_XZ001", "G_XZ001_XNFWXZ");
                break;
        }
        unstorePanelCtrlApi.setUIBtnPanelState(state);
        yield return null;
    }

    /// <summary>
    /// 响应顾客说话
    /// </summary>
    /// <param name="info">客户说话的内容文本</param>
    public void OnCustomerSay(string info)
    {

    }

    /// <summary>
    /// 响应小卡说话
    /// </summary>
    /// <param name="info">小卡说话的文本</param>
    /// <param name="wavPath">小卡说话的语音地址</param>
    public void OnXiaokaSay(string info, string wavPath)
    {
        //charactorPanelCtr?.setCharactor("WRSD_B说话类", info, wavPath);
        //charactorMVCtr?.mediaOpenFile("SH说话类", "G_SH0001");
        charactorMVCtr?.OnAudioPlay(wavPath);     
    }
    public void OnMediaPlay(string folder,string actionName)
    {
        charactorMVCtr?.mediaOpenFileOnce(folder, actionName);
    }

    #endregion

    #region 2.收费服务
    /// <summary>
    /// 判断是否屏蔽AI消息，（唱歌，投屏，应急使用中屏蔽消息）
    /// </summary>
    /// <returns></returns>
    public bool IsIgnoredAI()
    {
        bool isIgnored = false;
        Debug.Log("IsIgnoredAI 当前UI状态 = " + uiState);
        if (uiState == PanelsCtrlApi.UIState.KARAOKE_UI ||
            uiState == PanelsCtrlApi.UIState.SCREENPRO_UI ||
            uiState == PanelsCtrlApi.UIState.RESCUE_UI)
            isIgnored = true;
        return isIgnored;
    }
    public void OnShowKaraokePanel()
    {
        //uiState = UIState.KARAOKE_UI;
        //panelCtr?.OpenPanelByName("KaraokePanel");
        //Invoke("OnCloseKaraokePanel", time);
        UnityMainThreadDispatcher.Instance().Enqueue(OnShowKaraokePanelCourtine());
    }

    private IEnumerator OnShowKaraokePanelCourtine()
    {
        uiState = PanelsCtrlApi.UIState.KARAOKE_UI;
        charactorMVCtr?.setVoiceEn(false);
        unstorePanelCtrlApi?.OpenPanelByName("KaraokePanel");
        yield return null;
    }

    public void OnShowScreenProPanel()
    {
        //uiState = UIState.SCREENPRO_UI;
        //panelCtr?.OpenPanelByName("ScreenProPanel");
        //Invoke("OnCloseScreenProPanel", time);
        UnityMainThreadDispatcher.Instance().Enqueue(OnShowScreenProPanelCourtine());
    }

    private IEnumerator OnShowScreenProPanelCourtine()
    {
        uiState = PanelsCtrlApi.UIState.SCREENPRO_UI;
        charactorMVCtr?.setVoiceEn(false);
        unstorePanelCtrlApi?.OpenPanelByName("ScreenProPanel");
        yield return null;
    }

    #endregion

    #region 3.免费服务
    public PanelsCtrlApi.UIState getCurrentUIState()
    {
        Debug.Log("getCurrentUIState 当前UI状态 = " + uiState);
        return uiState;
    }
    public void OnRescueService()
    {
        if (RescuePages == null || RescuePages.Length < 1)
            return;
        //panelCtr.OpenPanelByName("RescuePanel");
        UnityMainThreadDispatcher.Instance().Enqueue(OnRescueServiceCourtine());
    }

    private IEnumerator OnRescueServiceCourtine()
    {
        foreach (var ptmp in RescuePages)
        {
            unstorePanelCtrlApi?.ClosePanel(ptmp);
        }
        //unstorePanelCtrlApi.OnPlayAdvertisement(false);
        uiState = PanelsCtrlApi.UIState.RESCUE_UI;
        //panelCtr.OpenPanelByName("RescuePanel");
        charactorMVCtr.setVoiceEn(false);
        Invoke("openrescueMap", 0.5f);
        yield return null;
    }

    private void openrescueMap()
    {
        currentRescueIndex = 0;
        unstorePanelCtrlApi?.OpenPanel(RescuePages[currentRescueIndex]);
    }

    private void OnMoveRecueMap(int index)
    {
        if (uiState != PanelsCtrlApi.UIState.RESCUE_UI)
            return;
        if (RescuePages == null || RescuePages.Length < 1)
            return;
        UnityMainThreadDispatcher.Instance().Enqueue(OnMoveRecueMapCourtine(index));
    }
    private IEnumerator OnMoveRecueMapCourtine(int index)
    {
        currentRescueIndex = (currentRescueIndex + index + RescuePages.Length)
            % RescuePages.Length;

        foreach (var ptmp in RescuePages)
        {
            unstorePanelCtrlApi?.ClosePanel(ptmp);
        }
        unstorePanelCtrlApi?.OpenPanel(RescuePages[currentRescueIndex]);
        yield return null;
    }
    #endregion

    #endregion
}
