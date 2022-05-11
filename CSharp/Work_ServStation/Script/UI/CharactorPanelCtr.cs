using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class CharactorPanelCtr : CharactorCtrBase
{

    #region 视频播放相关变量
    //private VideoPlayer charactorPlayer = null;
    //private VideoPlayer preLoadPlayer = null;

    #endregion

    //public string serviceManAttachName = "Canvas";
    private bool isHold_ = false;

    #region 预加载控制用变量
    //private int preCnt = 15;//预加载frame
    //bool isSwap = false;
    //bool isLoad = false;
    //bool isForcePlay = false;
    //private int preFrameRemain = 0;
    #endregion
    void Start()
    {


        if (rawImage == null)
            rawImage = GameObject.Find(rawImgAttachName).GetComponent<RawImage>();

        if (wordObj == null)
            wordObj = GameObject.Find(wordAttachName);

        if (wordText == null && wordObj != null)
            wordText = wordObj.GetComponentInChildren<Text>();

        mediaPlayer?.Events.AddListener(OnVideoEvent);
        charactorVideoPath = ServiceManager.jsonParam.charactorVideos;

        mediaOpenRand("DJ待机类");
        closeWord();
    }


    public override void OnCharactorWord(string word)
    {
        if (word == null)
            return;
        wordText.text = word;
        wordObj?.SetActive(true);
        //Invoke("closeWord", 2.0f);
    }


    public override void setCharactor(string actionVideoFolder, string word = null, string wavPath = null)
    {
        //Invoke("closeWord", 0f);
        //play action
        UnityMainThreadDispatcher.Instance().Enqueue(() => { OnCharactorWord(word); });
        UnityMainThreadDispatcher.Instance().Enqueue(() => { OnAudioPlay(wavPath); });
        mediaOpenRand(actionVideoFolder);

    }

    public override void PlayVoice()
    {
        //Check to see if you just set the toggle to positive
        if (m_Play)
        {
            //Play the audio you attach to the AudioSource component
            audioSource.enabled = true;
            //if(m_sound.isReadyToPlay)
            if (m_sound.loadState == AudioDataLoadState.Loaded)
            {
                //audioSource.PlayOneShot(m_sound);
                audioSource.Play();
                m_Play = false;
                Debug.LogWarning("play audio... ");
                StartCoroutine(AudioPlayFinished(audioSource.clip.length));
            }
        }
    }

    public void SetHold(bool isHold = false)
    {
        lock (this)
        {
            isHold_ = isHold;
        }
    }
    private void Play(bool istrue = true)
    {
        //charactorPanel?.gameObject.SetActive(true);
        if (!istrue)
        {
            if (audioSource.isPlaying)
                audioSource?.Pause();
            if (mediaPlayer.Control.IsPlaying())
                mediaPlayer?.Control.Pause();
            return;
        }

        if (!audioSource.isPlaying)
            m_Play = true;
        if (!mediaPlayer.Control.IsPlaying())
            mediaPlayer?.Control.Play();
    }
    void Update()
    {
        PlayVoice();

        Play(!isHold_);
    }
}
