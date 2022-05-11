using RenderHeads.Media.AVProVideo;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class UnstorePanelCtr : MonoBehaviour
{
    //public string moviePath = "零度百事可乐.mp4";
    // Start is called before the first frame update
    public string videoPath;
    public string videoAttachTarget = "MoviePlayInterface";
    public string PanelAttachTarget = "Canvas";

    public MediaPlayer mediaPlayer;
    private DisplayUGUI uGui;

    public string rawImgPath;
    public string rawImgAttachTarget = "RawImage";

    //private VideoPlayer videoPlayer;
    private RawImage rawImage;
    public Text text_PlayOrPause;
    public Button button_PlayOrPause;
    //public Button button_Pre;
    //public Button button_Next;
    //private Animator ani;
    private PanelsCtrlApi unstorePanelCtrlApi;
    //public string menuCtrAttatcName = "Canvas";
    //private MenuControl menuCtr;
    void Start()
    {
        //Handheld.PlayFullScreenMovie(moviePath, Color.black, FullScreenMovieControlMode.Full);
        //Handheld.PlayFullScreenMovie("test.mp4", Color.black, FullScreenMovieControlMode.CancelOnInput);
        //videoPlayer = GameObject.Find(videoAttachTarget).GetComponent<VideoPlayer>();
        //mediaPlayer = GameObject.Find(videoAttachTarget).GetComponentInChildren<MediaPlayer>();
        rawImage = GameObject.Find(rawImgAttachTarget).GetComponentInChildren<RawImage>();
        //menuCtr = GameObject.Find(menuCtrAttatcName).GetComponent<MenuControl>();
        if(uGui !=null)
        {
            uGui = GameObject.Find(rawImgAttachTarget).GetComponentInChildren<DisplayUGUI>();
            uGui._mediaPlayer = mediaPlayer;
        }
        unstorePanelCtrlApi = GameObject.Find(PanelAttachTarget).GetComponentInChildren<PanelsCtrlApi>();
        unstorePanelCtrlApi?.gameObject.SetActive(true);
        //ani = Pages[0].GetComponent<Animator>();

        button_PlayOrPause?.onClick.AddListener(OnPlayOrPauseVideo);
        //button_Next.onClick.AddListener(OnNextVideo);
        //button_Pre.onClick.AddListener(OnPreVideo);
        mediaPlayer?.Events.AddListener(OnVideoEvent);
        if (!File.Exists(rawImgPath))
        {
            Debug.LogWarning("not found background at :" + rawImgPath);
            rawImgPath = ServiceManager.jsonParam.deviceinfo.backGroudPic;
            Debug.LogWarning("loading background at :" + rawImgPath);
        }
        loadRawImg(rawImgPath);
        if (!File.Exists(videoPath))
        {
            Debug.LogWarning("not found adv video at :" + videoPath);
            videoPath = getAdvVideo();
            Debug.LogWarning("loading adv video at :" + videoPath);
        }
        //if(videoPlayer != null)
        //{
        //    videoPlayer.url = videoPath;
        //    OnPlayAdvertisement();
        //}

        mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL,videoPath);

        unstorePanelCtrlApi.setUIBtnPanelState(PanelsCtrlApi.UIBtnState.HIDE);
        //mediaPlayer?.OpenVideoFromFile(MediaPlayer.FileLocation.AbsolutePathOrURL, videoPath);

    }

    public void OnVideoEvent(MediaPlayer mp, MediaPlayerEvent.EventType et,ErrorCode code)
    {
        switch (et)
        {
            case MediaPlayerEvent.EventType.ReadyToPlay:
                mp.Control.Play();
                break;
            case MediaPlayerEvent.EventType.FirstFrameReady:
                Debug.Log("First frame ready");
                break;
            case MediaPlayerEvent.EventType.FinishedPlaying:
                mp.Control.Rewind();
                break;
        }
        Debug.Log("Event: " + et.ToString());
    }

    string getAdvVideo()
    {
        string path = null;
        foreach (var pair in ServiceManager.jsonParam.adverVideos.foldDic)
        {
            foreach (var tmppath in pair.Value)
            {
                if (File.Exists(tmppath))
                {
                    path = tmppath;
                    Debug.LogWarning("found advertisement video at " + tmppath);
                    return path;
                }
                else
                {
                    Debug.LogWarning("not found advertisement video at " + tmppath);
                }
            }
        }
        return path;
    }

    // Update is called once per frame
    void Update()
    {
        //没有视频则返回，不播放
        //OnVideoPlayerUpdate();
        OnMediaPlayerUpdate();
    }

    private void OnMediaPlayerUpdate()
    {
        if (mediaPlayer == null)
            return;
       if(mediaPlayer.Control.IsPlaying()&&rawImage!=null)
            rawImage.texture = mediaPlayer?.TextureProducer.GetTexture();
        //if (mediaPlayer.Control.IsPlaying() && uGui != null)
            //uGui._defaultTexture = mediaPlayer?.TextureProducer.GetTexture();
    }


    /// <summary>
    /// 播放和暂停当前视频
    /// </summary>
    private void OnPlayOrPauseVideo()
    {
        //判断视频播放情况，播放则暂停，暂停就播放，并更新相关文本
        if (mediaPlayer.Control.IsPlaying()== true)
        {
            //videoPlayer.Pause();
            mediaPlayer?.Play();
            text_PlayOrPause.text = "播放";
            //ani.SetBool("Open", true);
            //OpenPanel(Pages[0]);
            //openAll();
            unstorePanelCtrlApi.openMainUIPanel();
            loadRawImg(rawImgPath);
        }
        else
        {
            //videoPlayer?.Play();
            mediaPlayer?.Play();
            text_PlayOrPause.text = "暂停";
            //ani.SetBool("Open", false);
            unstorePanelCtrlApi.CloseAllPanels();
        }
    }

    public void OnPlayAdvertisement(bool isplay = true)
    {
        //判断视频播放情况，播放则暂停，暂停就播放，并更新相关文本
        if (isplay)
        {
            //videoPlayer?.Play();
            mediaPlayer?.Play();
            text_PlayOrPause.text = "暂停";
            //ani.SetBool("Open", false);
            unstorePanelCtrlApi.CloseAllPanels();
        }
        else
        {
            //videoPlayer?.Pause();
            mediaPlayer?.Pause();
            text_PlayOrPause.text = "播放";
            //ani.SetBool("Open", true);
            unstorePanelCtrlApi.openMainUIPanel();
            loadRawImg(rawImgPath);
        }
    }

    ///// <summary>
    ///// 切换上一个视频(待实现)
    ///// </summary>
    //private void OnPreVideo()
    //{
    //    //视频列表减一播放上一个视频，并进行避免越界操作
    //    //currentClipIndex -= 1;
    //    //if (currentClipIndex < 0)
    //    //{
    //    //    currentClipIndex = videoClips.Length - 1;
    //    //}
    //    //videoPlayer.clip = videoClips[currentClipIndex];
    //
    //    text_PlayOrPause.text = "暂停";
    //}
    //
    ///// <summary>
    ///// 切换下一个视频(待实现)
    ///// </summary>
    //private void OnNextVideo()
    //{
    //    //视频列表加一播放下一个视频，并进行避免越界操作
    //    //currentClipIndex += 1;
    //    //currentClipIndex = currentClipIndex % videoClips.Length;
    //    //videoPlayer.clip = videoClips[currentClipIndex];
    //    text_PlayOrPause.text = "暂停";
    //}

    private void loadRawImg(string file)
    {
        string filePath = "file://" + file;
        WWW www = new WWW(filePath);
        if (www.error != null)
        {
            Debug.LogError("no image file found in " + filePath);
        }
        else
        {
            if(rawImage !=null)
                rawImage.texture = www.texture;
        }
    }
}
