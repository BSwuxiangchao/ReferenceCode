using RenderHeads.Media.AVProVideo;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class CharactorMVCtr : CharactorCtrBase
{
    // Start is called before the first frame update
    //public PanelInstance charactorPanel;
    public Text videoNameText;
    public Text voiceNameText;
    private bool isShowText = false;
    void Start()
    {

        //if (charactorPlayer == null)
           // charactorPlayer = charactorPanel.gameObject.GetComponent<VideoPlayer>();

        if (rawImage == null)
            rawImage = GameObject.Find(rawImgAttachName).GetComponent<RawImage>();

        if (wordObj == null)
            wordObj = GameObject.Find(wordAttachName);

        if (wordText == null && wordObj != null)
            wordText = wordObj.GetComponentInChildren<Text>();

        mediaPlayer?.Events.AddListener(OnVideoEvent);

        videoOpenAct += OnVideoOpen;
        voiceOpenAct += OnVoiceOpen;
        charactorVideoPath = ServiceManager.jsonParam.charactorVideos;
        setCurRootPath(charactorVideoPath.foldPath[0]);
        //mediaOpenFileOnce("DJ待机类","G_DJ002");
        mediaOpenRand("DJ待机类");
        //mediaOpenFileOnce("ZH招呼类", "G_ZH003");
        //charactorPlayer.prepareCompleted += this.prepared;
        //var transParent = charactorPanel.gameObject.GetComponent<Transform>();
        //preLoadPlayer = Instantiate(charactorPlayer, transParent);
        //preLoadPlayer.prepareCompleted += this.prepared;
        //loadVideo(charactorPlayer);
        //closeWord();

    }

    private void OnVideoOpen(string videoName)
    {
        videoNameText.text = videoName;
    }

    private void OnVoiceOpen(string voiceName)
    {
        voiceNameText.text = voiceName;
    }

    void Update()
    {
        PlayVoice();
        //ShowVideoName();
        //updateVideoPlayer();
        //framepre = charactorPlayer.frame;
        if (Input.GetKeyDown(KeyCode.Q))
        {
            isShowText = true;
            videoNameText?.gameObject?.SetActive(isShowText);
            voiceNameText?.gameObject?.SetActive(isShowText);
        }
        if (Input.GetKeyDown(KeyCode.W))
        {
            isShowText = false;
            videoNameText?.gameObject?.SetActive(isShowText);
            voiceNameText?.gameObject?.SetActive(isShowText);
        }
    }

}
