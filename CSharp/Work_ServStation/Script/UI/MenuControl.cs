using Newtonsoft.Json.Linq;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class MenuControl : BtnManager
{
    public delegate void MesDispatchDelegate(string recipient,string mes);
    public MesDispatchDelegate mesDispatch_ = null;
    public Action getInfoAction = null;
    public Action getQRCodeAction = null;
    public Action localMapAction = null;
    public Action karaokeAction = null;
    public Action screenProAction = null;


    //public PanelInstance DebugPanel;
    public Button DebugShowBtn;

    public string panelsCtrAttachName = "Canvas";
    private PanelsCtr panelCtr;

    public string charactorCtrAttachName = "Canvas";
    private CharactorPanelCtr charactorPanelCtr;
    //public string moviePath = "��Ȱ��¿���.mp4";
    // Start is called before the first frame update
    #region buttons
    public Button button_Karaoke;
    public Button button_ScreenPro;
    public Button button_RescueMap;
    public Button button_Pre;
    public Button button_Next;
    public Button button_Select;
    public Button button_Cancel;
    public Button button_Tomain;
    public Button button_CustomerSay;
    public Button button_XiaokaSay;
    public Button button_Start;
    public Button button_Stop;
    public Button button_Regular;
    public Button button_PayScs;
    public Button button_PayFail;
    public Button button_PayCancel;
    public Button button_GetInfo;
    public Button button_GetQRCode;
    #endregion

    private int BtnIndex;
    //private Animator ani;


    void Start()
    {
        //Handheld.PlayFullScreenMovie(moviePath, Color.black, FullScreenMovieControlMode.Full);
        //Handheld.PlayFullScreenMovie("test.mp4", Color.black, FullScreenMovieControlMode.CancelOnInput);
        button_Next.onClick.AddListener(OnNextBtn);
        button_Pre.onClick.AddListener(OnPreBtn);
        button_Cancel.onClick.AddListener(OnCancel);
        button_Select.onClick.AddListener(OnSelect);
        button_CustomerSay.onClick.AddListener(OnCustomerSayBtn);
        button_XiaokaSay.onClick.AddListener(OnXiaokaSayBtn);
        button_Tomain.onClick.AddListener(OnToMainUI);
        button_Start.onClick.AddListener(OnStartBtn);
        button_Stop.onClick.AddListener(OnStopBtn);
        button_GetInfo.onClick.AddListener(OnGetInfoBtn);
        button_GetQRCode.onClick.AddListener(OnGetQRCodeBtn);

        button_Karaoke.onClick.AddListener(OnKaraokeBtn);
        button_ScreenPro.onClick.AddListener(OnScreenProBtn);
        button_RescueMap.onClick.AddListener(OnRescueMapBtn);
        Init();
        BtnIndex = GetCurrentBtnIndex();

        panelCtr = GameObject.Find(panelsCtrAttachName).GetComponent<PanelsCtr>();
        charactorPanelCtr = GameObject.Find(charactorCtrAttachName).GetComponent<CharactorPanelCtr>();

        ShowMsgDebug("����");
    }



    // Update is called once per frame
    void Update()
    {

    }

    #region 1.button reaction
    /// <summary>
    /// �л���һ��ѡ��(��ʵ��)
    /// </summary>
    public void OnPreBtn()
    {
        //��Ƶ�б��һ������һ����Ƶ�������б���Խ�����
        //currentClipIndex -= 1;
        //if (currentClipIndex < 0)
        //{
        //    currentClipIndex = videoClips.Length - 1;
        //}
        //videoPlayer.clip = videoClips[currentClipIndex];

        //text_PlayOrPause.text = "��ͣ";
        BtnIndex--;
        if (BtnIndex < 0)
        {
            BtnIndex = Btns.Length - 1;
        }
        SetCurrentBtn(Btns[BtnIndex]);
    }

    /// <summary>
    /// �л���һ��ѡ��(��ʵ��)
    /// </summary>
    public void OnNextBtn()
    {
        //��Ƶ�б��һ������һ����Ƶ�������б���Խ�����
        //currentClipIndex += 1;
        //currentClipIndex = currentClipIndex % videoClips.Length;
        //videoPlayer.clip = videoClips[currentClipIndex];
        //text_PlayOrPause.text = "��ͣ";
        BtnIndex++;
        if (BtnIndex >= Btns.Length)
        {
            BtnIndex = 0;
        }
        SetCurrentBtn(Btns[BtnIndex]);
    }

    /// <summary>
    /// ȡ��ѡ��
    /// </summary>
    public void OnCancel()
    {
        SetState(ButtonState.DEFAULT);
    }

    /// <summary>
    /// ȷ��ѡ��
    /// </summary>
    public void OnSelect()
    {
        SetState(ButtonState.SELECTED);

        var currentBtnText = currentBtn.gameObject.name;

        OnServiceByName(currentBtnText);
        //if(currentBtnText == "Karaoke")
        //    OnKaraokeService();
        //if (currentBtnText == "Movie")
        //    OnMovieService();
        //if (currentBtnText == "ScreenPro")
        //    OnScreenProService();
        //if (currentBtnText == "Emgcyknowledge")
        //    OnEmgcyKnowledgeService();
        //if (currentBtnText == "LocalMap")
        //    OnLocalMapService();
        //if (currentBtnText == "EmgcyPhone")
        //    OnEmgcyPhoneService();
        //if (currentBtnText == "EmgcyCall")
        //    OnEmgcyCallService();
    }

    /// <summary>
    /// ͨ����ť�����ж�ѡ��ķ���
    /// </summary>
    /// <param name="btnName">��ť���������</param>
    public void OnSelectByWord(string btnName)
    {
        foreach(var btn in Btns)
        {
            if(btnName == btn.gameObject.name)
            {
                SetCurrentBtn(btn);
            }
        }
        SetState(ButtonState.SELECTED);

        var currentBtnText = currentBtn.gameObject.name;

        OnServiceByName(currentBtnText);
    }

    private void OnServiceByName(string btnName)
    {
        switch (btnName)
        {
            case "Karaoke":
                OnKaraokeService();
                break;
            case "Movie":
                OnMovieService();
                break;
            case "ScreenPro":
                OnScreenProService();
                break;
            case "Emgcyknowledge":
                OnEmgcyKnowledgeService();
                break;
            case "LocalMap":
                OnLocalMapService();
                break;
            case "EmgcyPhone":
                OnEmgcyPhoneService();
                break;
            case "EmgcyCall":
                OnEmgcyCallService();
                break;
            case "RescueMap":
                OnLocalMapService();
                break;
        }
    }

    /// <summary>
    /// ����������
    /// </summary>
    public void OnToMainUI()
    {
        panelCtr.openMainUIPanel();
    }

    private void OnCustomerSayBtn()
    {
        string content = "���";
        OnCustomerSay(content);
    }

    private void OnXiaokaSayBtn()
    {
        string content = "���";
        string wavPath = @"F:\source1.mp3";
        OnXiaokaSay(content, wavPath);
    }

    public void OnStartBtn()
    {
        if (mesDispatch_ == null)
            return;
        mesDispatch_("VOICE_AI", "startgame");

        panelCtr.OnPlayAdvertisement(false);
    }

    public void OnStopBtn()
    {
        panelCtr.OnPlayAdvertisement(true);
        if (mesDispatch_ == null)
            return;
        mesDispatch_("VOICE_AI", "endgame");
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
        if (getInfoAction == null)
            return;
        getInfoAction();
    }

    public void OnGetQRCodeBtn()
    {
        if (getQRCodeAction == null)
            return;
        getQRCodeAction();
    }

    private void OnRescueMapBtn()
    {
        //throw new NotImplementedException();
        OnLocalMapService();
    }

    private void OnScreenProBtn()
    {
        //throw new NotImplementedException();
        OnScreenProService();
    }

    private void OnKaraokeBtn()
    {
        OnKaraokeService();
    }
    #endregion


    #region service handle

    #region 1.��������������������
    /// <summary>
    /// ��Ӧ�˿�˵��
    /// </summary>
    /// <param name="info">�ͻ�˵���������ı�</param>
    public void OnCustomerSay(string info)
    {

    }

    /// <summary>
    /// ��ӦС��˵��
    /// </summary>
    /// <param name="info">С��˵�����ı�</param>
    /// <param name="wavPath">С��˵����������ַ</param>
    public void OnXiaokaSay(string info ,string wavPath)
    {
        charactorPanelCtr?.setCharactor("SH˵����", info, wavPath);
    }

    public void OnShowQRCode(string qrUrl, float time = 3)
    {
        StartCoroutine(this.loadQRImg(qrUrl,time));
    }

    private IEnumerator loadQRImg(string qrUrl,float time = 3)
    {
        WWW www = new WWW(qrUrl);
        yield return www;
        if(www.isDone && www.error == null)
        {
            var tmpPanel = panelCtr.GetPanelByName("PayPanel");
            var imgTexture = tmpPanel.gameObject.GetComponentInChildren<RawImage>();
            imgTexture.texture = www.texture;
            panelCtr.OpenPanelByName("PayPanel");
        }
        Invoke("closeQRPanel", time);
    }

    public void closeQRPanel()
    {
        panelCtr.ClosePanelByName("PayPanel");
    }

    public void OnShowErro(string title = "��������", string details = null,
        string tips = null,float time = 3)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(loadErrInfo(title, details, tips, time));
    }

    IEnumerator loadErrInfo(string title, string details,
        string tips, float time)
    {
        PanelInstance erroPanel = panelCtr.GetPanelByName("ErroPanel");
        //string title = "֧���ɹ���";
        //string details = "���θ���" + cost + "Ԫ,����ʹ��ʱ��" + tollMaxTime + "����.";
        //string tips = closeRemark + ":" + tollStandard + "," + remarks;
        string timeRemain = "��" + time + "s��ر�";
        Text[] textList = erroPanel.gameObject.GetComponentsInChildren<Text>();
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
            Debug.LogWarning("not found any Text in  ErroPanel");
        }
        panelCtr.OpenPanelByName("ErroPanel");
        Invoke("closeErroPanel", time);
        yield return null;
    }
    public void OnShowPayResultPanel(JObject info,float time = 3)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(loadPayInfo(info,time));
    }

    private IEnumerator loadPayInfo(JObject goodGoll, float time = 3)
    {
        PanelInstance payResultPanel = panelCtr.GetPanelByName("PayResultPanel");
        float tollTimeVal = ((float)goodGoll["tollTimeVal"]);
        int tollTimeType = ((int)goodGoll["tollTimeType"]);
        float tollMaxTime = ((float)goodGoll["tollMaxTime"]);
        Debug.LogWarning("tollTimeVal = " + tollTimeVal+
            ", tollTimeType = "+ tollTimeType +
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
        string title = "֧���ɹ���";
        string details = "���θ���" + cost + "Ԫ,����ʹ��ʱ��" + tollMaxTime + "����.";
        string tips = closeRemark + ":" + tollStandard + "," + remarks;
        string timeRemain = "��" + time+"s��رգ�����ʼ��ʱ";
        Text[] textList = payResultPanel.gameObject.GetComponentsInChildren<Text>();
        if(textList != null)
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
        panelCtr.OpenPanelByName("PayResultPanel");
        Invoke("closePayResultPanel", time);
        yield return null;
    }
    public void closePayResultPanel()
    {
        panelCtr.ClosePanelByName("PayResultPanel");
    }

    public void closeErroPanel()
    {
        panelCtr.ClosePanelByName("ErroPanel");
    }
    #endregion

    #region 2.�շѷ���
    /// <summary>
    /// ���ֳ��ɷ���
    /// </summary>
    public void OnKaraokeService()
    {
        //panelCtr.OpenPanelByName("PayPanel");
        if (karaokeAction != null)
            karaokeAction();
    }

    /// <summary>
    /// ����ӰԺ����
    /// </summary>
    public void OnMovieService()
    {
        panelCtr.OpenPanelByName("PayPanel");
    }

    /// <summary>
    /// Ͷ������
    /// </summary>
    public void OnScreenProService()
    {
        //panelCtr.OpenPanelByName("PayPanel");
        if (screenProAction != null)
            screenProAction();
    }

    #endregion

    #region 3.��ѷ���
    public void OnRescueService()
    {
        //panelCtr.OpenPanelByName("RescuePanel");
        UnityMainThreadDispatcher.Instance().Enqueue(OnRescueServiceCourtine());
    }

    private IEnumerator OnRescueServiceCourtine()
    {
        panelCtr.OpenPanelByName("RescuePanel");
        yield return null;
    }

    /// <summary>
    /// Ӧ��֪ʶ����
    /// </summary>
    public void OnEmgcyKnowledgeService()
    {

    }

    /// <summary>
    /// ���ص�ͼ����
    /// </summary>
    public void OnLocalMapService()
    {
        if (localMapAction != null)
            localMapAction();
        //panelCtr.OpenPanelByName("RescuePanel");
    }

    /// <summary>
    /// ��Ԯ�绰����
    /// </summary>
    public void OnEmgcyPhoneService()
    {

    }

    /// <summary>
    /// �������з���
    /// </summary>
    public void OnEmgcyCallService()
    {

    }
    #endregion
    
    #endregion
}
