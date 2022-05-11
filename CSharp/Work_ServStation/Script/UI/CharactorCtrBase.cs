using RenderHeads.Media.AVProVideo;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class CharactorCtrBase : MonoBehaviour
{
    // Start is called before the first frame update
    public PanelInstance charactorPanel;
    public Action<string> videoOpenAct = null;
    public Action<string> voiceOpenAct = null;

    public AudioSource audioSource = null;
    protected AudioClip m_sound = null;
    protected bool m_Play = false;
    public string wordAttachName = "words";
    protected GameObject wordObj;
    protected Text wordText;
    private bool isVoiceEn = true;

    private Action sequenceEndAct = null;

    #region 视频播放相关变量
    public MediaPlayer mediaPlayer;
    public DisplayUGUI uGui;
    //private VideoPlayer charactorPlayer = null;
    //private VideoPlayer preLoadPlayer = null;
    public string rawImgAttachName = "CharactorRImage";
    protected RawImage rawImage;
    protected VideosPath charactorVideoPath;
    protected string currentFolder;
    protected string lastVideo = "";//上一次播放的视频
    protected List<string> loopFoders = null;//循环播放的文件夹
    protected List<string> loopVideos = null;//循环播放的视频
    protected List<string> loopVoices = null;//循环播放的音频
    protected int loopIndex = 0;

    protected string currentVoice;
    protected int isRand = 0;
    protected readonly string defaultFolder = "DJ待机类";
    protected List<string> videos = null;
    protected List<string> orderfoders = null;
    protected List<string> ordervoices = null;
    protected int currentIndex = 0;
    protected string curRootPath;
    //private long framepre = -1000;
    #endregion

    //public string serviceManAttachName = "Canvas";
    //void Start()
    //{
    //
    //    //if (charactorPlayer == null)
    //    // charactorPlayer = charactorPanel.gameObject.GetComponent<VideoPlayer>();
    //
    //    if (rawImage == null)
    //        rawImage = GameObject.Find(rawImgAttachName).GetComponent<RawImage>();
    //
    //    if (wordObj == null)
    //        wordObj = GameObject.Find(wordAttachName);
    //
    //    if (wordText == null && wordObj != null)
    //        wordText = wordObj.GetComponentInChildren<Text>();
    //
    //    mediaPlayer?.Events.AddListener(OnVideoEvent);
    //
    //
    //
    //    charactorVideoPath = ServiceManager.jsonParam.charactorVideos;
    //    //mediaOpenFileOnce("DJ待机类","G_DJ002");
    //    //mediaOpenRand("DJ待机类");
    //    //mediaOpenFileOnce("ZH招呼类", "G_ZH003");
    //    //charactorPlayer.prepareCompleted += this.prepared;
    //    //var transParent = charactorPanel.gameObject.GetComponent<Transform>();
    //    //preLoadPlayer = Instantiate(charactorPlayer, transParent);
    //    //preLoadPlayer.prepareCompleted += this.prepared;
    //    //loadVideo(charactorPlayer);
    //    //closeWord();
    //}

    public void setVoiceEn(bool isEn)
    {
        lock (this)
        {
            isVoiceEn = isEn;
        }
    }

    public void setCurRootPath(string rootPath)
    {
        curRootPath = rootPath;
    }

    protected void openVideoWithVoiceFromFile(string videoFullPath, string voiceName = null,string videoName =null)
    {
        //1.paly video
        if (!File.Exists(videoFullPath))
        {
            return;
        }
        //uGui?.setReady(false);
        mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL,
    videoFullPath);


        DirectoryInfo root = new DirectoryInfo(videoFullPath);
        string dic = root.Parent.FullName;
        videoOpenAct(root.Name);
        //2.play local voice
        if (!ServiceManager.jsonParam.deviceinfo.isLocalVoice)
            return;

        string withoutExtName = videoFullPath?.TrimEnd(".mov".ToCharArray());
        if (voiceName != null)
            withoutExtName = dic + "\\" + voiceName;
        string wavFile = withoutExtName + ".wav";
        string mp3File = withoutExtName + ".mp3";
        string fullPath = null;
        if (File.Exists(wavFile))
            fullPath = wavFile;
        if (File.Exists(mp3File))
            fullPath = mp3File;

        currentVoice = fullPath;
        if (fullPath == null)
            return;

        OnAudioPlay(currentVoice);
    }

    public void mediaOpenFiles(string folder, List<string> names, string voice = null, string extender = ".mov")
    {
        isRand = 2;
        videos = names;
        currentIndex = 0;
        UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(folder, videos[0], voice, extender));
        //UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(folder, name, voice, extender));
    }

    public void mediaOpenFileOnce(string folder, string name, string voice = null, string extender = ".mov")
    {
        isRand = 3;
        UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(folder, name, voice, extender));
    }
    public void mediaOpenFileOnceAddLoop(List<string> foders, List<string> names, List<string> voices = null,
        List<string> loopfoders=null, List<string> loopnames=null, List<string> loopvoices=null, string extender = ".mov"
        , Action act = null)
    {
        if (names.Count >= 1)
            UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(foders[0], names[0], voices[0], extender));
        else if (loopnames.Count >= 1)
            UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(loopfoders[0], loopnames[0], loopvoices[0], extender));
        else
            return;

        sequenceEndAct = act;
        isRand = 4;
        currentIndex = 0;
        loopIndex = 0;
        orderfoders = foders;
        videos = names;
        ordervoices = voices;
        loopFoders = loopfoders;
        loopVideos = loopnames;
        loopVoices = loopvoices;
    }
    private void mediaOpenFile(string folder, string name, string voice = null, string extender = ".mov")
    {
        UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenFileCourtine(folder, name, voice, extender));
    }

    protected IEnumerator mediaOpenFileCourtine(string folder, string name, string voice = null, string extender = ".mov")
    {
        mediaOpenFileType(folder, name, voice, extender);
        yield return null;
    }

    public void mediaOpenFileType(string folder, string name, string voice = null, string extender = ".mov")
    {
        currentFolder = folder;
        string fullPath = curRootPath + "\\" + folder + "\\" + name + extender;
        if (!File.Exists(fullPath))
        {
            return;
        }
        //mediaPlayer?.Pause();
        //mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL,
        //   fullPath);
        openVideoWithVoiceFromFile(fullPath, voice,name);
        Enable();

    }

    /// <summary>
    /// 随机播放
    /// </summary>
    /// <param name="folder"></param>
    /// <param name="extender"></param>
    public void mediaOpenRand(string folder, string voice = null, string extender = ".mov")
    {
        UnityMainThreadDispatcher.Instance().Enqueue(mediaOpenRandCourtine(folder, voice, extender));
    }

    protected IEnumerator mediaOpenRandCourtine(string folder, string voice = null, string extender = ".mov")
    {
        mediaOpenRandType(folder, voice, extender);
        yield return null;
    }

    protected void mediaOpenRandType(string folder, string voice = null, string extender = ".mov")
    {
        var mp4 = ".mp4";
        var tmpFolder = curRootPath + "\\" + folder;
        if (!Directory.Exists(tmpFolder))
        {
            return;
        }
        isRand = 1;
        currentFolder = folder;
        var fileList = Directory.GetFiles(tmpFolder, "*" + extender)?.ToList();
        if(extender != mp4)
        {
            var mp4List = Directory.GetFiles(tmpFolder, "*" + mp4)?.ToList();
            fileList = fileList.Concat(mp4List).ToList();
        }
        if (fileList == null || fileList.Count < 1)
            return;

        if(fileList.Count == 1)
        {
            openVideoWithVoiceFromFile(fileList[0], voice);
            Enable();
            return;
        }

        List<string> remainList = new List<string>();
        foreach(var tmpfile in fileList)
        {
            if (tmpfile == lastVideo)
                continue;
            remainList.Add(tmpfile);
        }

        var randNum = (float)UnityEngine.Random.Range(-0.45f, (float)(remainList.Count) - 0.45f);
        int num = (int)randNum;
        //mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL,
        //fileList[num]);
        lastVideo = remainList[num];
        openVideoWithVoiceFromFile(remainList[num], voice);
        Enable();
    }



    public void OnVideoEvent(MediaPlayer mp, MediaPlayerEvent.EventType et, ErrorCode code)
    {
        switch (et)
        {
            case MediaPlayerEvent.EventType.ReadyToPlay:
                mp.Control.Play();
                uGui?.setReady(true);
                //mp.TextureProducer.GetTextureFrameCount();
                break;
            case MediaPlayerEvent.EventType.FirstFrameReady:
                Debug.Log("First frame ready");
                uGui?.setReady(true);
                break;
            case MediaPlayerEvent.EventType.FinishedPlaying:
                replay(mp);
                break;
        }
        Debug.Log("Event: " + et.ToString());
    }

    public virtual void PlayVoice()
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
                if(isVoiceEn)
                    audioSource.Play();
                m_Play = false;
                Debug.LogWarning("play audio... ");
                //StartCoroutine(AudioPlayFinished(audioSource.clip.length));
            }
        }

        if(!isVoiceEn)
        {
            //if (audioSource.isPlaying)
            audioSource?.Pause();
        }
    }

    protected IEnumerator AudioPlayFinished(float time)
    {

        yield return new WaitForSeconds(time);
        //closeWord();
        //setCharactor(defaultFolder);
        mediaOpenRand(defaultFolder);
        Invoke("closeWord", 2.0f);
    }

    public void OnAudioPlay(string voiceFile)//load Audio
    {
        if (voiceFile == null || voiceFile == "" || !File.Exists(voiceFile))
            return;

        lock (this)
        {
            if (!audioSource)
            {
                audioSource = charactorPanel.gameObject.GetComponent<AudioSource>();
            }
            if (audioSource.isPlaying)
                audioSource.Stop();
            try
            {
                WWW www = new WWW(voiceFile);
                m_sound = www.GetAudioClip();
                if (m_sound == null)
                {
                    Debug.LogError("load failure with audio file : " + voiceFile);
                    return;
                }
                DirectoryInfo root = new DirectoryInfo(voiceFile);
                voiceOpenAct(root.Name);
                audioSource.clip = m_sound;
                m_Play = true;
                //audioSource.PlayOneShot(sound);
                Debug.LogWarning("load audio file : " + voiceFile);
                //audioSource.Play();
            }
            catch (Exception ex)
            {
                Debug.LogError("Exception with " + ex.Message);
                Debug.LogError("load failure with audio file : " + voiceFile);
                return;
            }

        }
    }

    public virtual void OnCharactorWord(string word)
    {
        if (word == null)
            return;
        wordText.text = word;
        wordObj?.SetActive(true);
        //Invoke("closeWord", 2.0f);
    }

    protected void replay(MediaPlayer mp)
    {
        if (isRand == 1)//随机不重复
        {
            uGui?.setReady(false);
            mediaOpenRand(currentFolder);
        }
        if(isRand == 0)
        {
            uGui?.setReady(false);
            mp.Control.Rewind();
            uGui?.setReady(true);
            if (!ServiceManager.jsonParam.deviceinfo.isLocalVoice)
                return;
            OnAudioPlay(currentVoice);
        }
        if(isRand == 2)//顺序播放列表
        {
            currentIndex++;
            var index = currentIndex % videos.Count;
            mediaOpenFile(currentFolder, videos[index]);
        }
        if(isRand == 3)//once 
        {
            return;
        }
        if (isRand == 4)//先顺序再循环播放不同列表 
        {
            bool playloop = false;
            if ((++currentIndex) < videos.Count)
            {
                mediaOpenFile(orderfoders[currentIndex], videos[currentIndex],ordervoices[currentIndex]);
            }
            else
                playloop = true;
            if(playloop && loopFoders!=null)
            {
                if(sequenceEndAct != null)
                {
                    sequenceEndAct.Invoke();//响应顺序视频播放完毕事件
                    sequenceEndAct = null;
                }
                loopIndex++;
                var index = loopIndex % loopVideos.Count;
                mediaOpenFile(loopFoders[index], loopVideos[index],loopVoices[index]);
            }
        }
    }
    protected void closeWord()
    {
        if (wordObj == null)
            return;
        wordObj.SetActive(false);
    }

    public virtual void setCharactor(string actionVideoFolder, string word = null, string wavPath = null)
    {
        //lock (this)
        //{
        //    preFrameRemain = preCnt;
        //    isLoad = true;
        //    currentFolder = actionVideoFolder;
        //    isForcePlay = true;
        //}
        OnCharactorWord(word);
        OnAudioPlay(wavPath);
    }

    //void Update()
    //{
    //    PlayVoice();
    //
    //    //updateVideoPlayer();
    //    //framepre = charactorPlayer.frame;
    //}

    public void Enable(bool istrue = true)
    {
        charactorPanel?.gameObject.SetActive(istrue);
    }
}
