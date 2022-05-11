using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class PanelsCtr : PanelManager
{
    //public string moviePath = "零度百事可乐.mp4";
    // Start is called before the first frame update
    public string videoPath;
    public string videoAttachTarget = "MoviePlayInterface";

    public string rawImgPath;
    public string rawImgAttachTarget = "RawImage";

    public string CharactorCtrAttach = "Canvas";
    private CharactorPanelCtr charCtr = null;

    private VideoPlayer videoPlayer;
    private RawImage rawImage;
    public Text text_PlayOrPause;
    public Button button_PlayOrPause;
    //public Button button_Pre;
    //public Button button_Next;
    private Animator ani;

    //public string menuCtrAttatcName = "Canvas";
    //private MenuControl menuCtr;
    void Start()
    {
        //Handheld.PlayFullScreenMovie(moviePath, Color.black, FullScreenMovieControlMode.Full);
        //Handheld.PlayFullScreenMovie("test.mp4", Color.black, FullScreenMovieControlMode.CancelOnInput);
        videoPlayer = GameObject.Find(videoAttachTarget).GetComponent<VideoPlayer>();
        rawImage = GameObject.Find(rawImgAttachTarget).GetComponent<RawImage>();
        charCtr = GameObject.Find(CharactorCtrAttach).GetComponent<CharactorPanelCtr>();
        //menuCtr = GameObject.Find(menuCtrAttatcName).GetComponent<MenuControl>();
        ani = Pages[0].GetComponent<Animator>();

        button_PlayOrPause.onClick.AddListener(OnPlayOrPauseVideo);
        //button_Next.onClick.AddListener(OnNextVideo);
        //button_Pre.onClick.AddListener(OnPreVideo);
        if (!File.Exists(videoPath))
        {
            Debug.LogWarning("not found adv video at :" + videoPath);
            videoPath = getAdvVideo();
            Debug.LogWarning("loading adv video at :" + videoPath);
        }
        videoPlayer.url = videoPath;
        if (!File.Exists(rawImgPath))
        {
            Debug.LogWarning("not found background at :" + rawImgPath);
            rawImgPath = ServiceManager.jsonParam.deviceinfo.backGroudPic;
            Debug.LogWarning("loading background at :" + rawImgPath);
        }
        loadRawImg(rawImgPath);
        //OnPlayAdvertisement();
        Invoke("PlayAdv", 0.1f);
    }

    private void PlayAdv()
    {
        OnPlayAdvertisement();
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
        if (videoPlayer.texture == null)
        {
            return;
        }

        if (videoPlayer.frame >= (long)(videoPlayer.frameCount - 1) && !videoPlayer.isLooping)//判断视频是否播放完毕
        {
            videoPlayer.Pause();
            text_PlayOrPause.text = "播放";
            loadRawImg(rawImgPath);
            //text_PlayOrPause.text = "播放";
            videoPlayer.frame = 0;
            return;
        }

        //渲染视频到UGUI上
        if (videoPlayer.isPlaying == true)
        {
            rawImage.texture = videoPlayer.texture;
        }

        if (Input.GetKeyDown(KeyCode.D))
        {
            OpenPanelByName("BtnDebugPanel");
        }
        if (Input.GetKeyDown(KeyCode.C))
        {
            ClosePanelByName("BtnDebugPanel");
        }
    }

    /// <summary>
    /// 播放和暂停当前视频
    /// </summary>
    private void OnPlayOrPauseVideo()
    {
        //判断视频播放情况，播放则暂停，暂停就播放，并更新相关文本
        if (videoPlayer.isPlaying == true)
        {

            //videoPlayer.Pause();
            //text_PlayOrPause.text = "播放";
            ////ani.SetBool("Open", true);
            ////OpenPanel(Pages[0]);
            ////openAll();
            //openMainUIPanel();
            //charCtr?.Play(true);
            //OnPlayAdvertisement(false);
            //PlayAdv();
            Invoke("PlayAdv", 0.1f);
        }
        else
        {

            //videoPlayer.Play();
            //text_PlayOrPause.text = "暂停";
            ////ani.SetBool("Open", false);
            //CloseAllPanels();
            //charCtr?.Play(false);
            OnPlayAdvertisement(true);
        }
    }

    public void OnPlayAdvertisement(bool isplay = true)
    {
        UnityMainThreadDispatcher.Instance().Enqueue(OnPlayAdvertisementCourtine(isplay));
    }
    public IEnumerator OnPlayAdvertisementCourtine(bool isplay = true)
    {
        //判断视频播放情况，播放则暂停，暂停就播放，并更新相关文本
        if (isplay)
        {
            charCtr?.SetHold(true);
            videoPlayer.Play();
            text_PlayOrPause.text = "暂停";
            //ani.SetBool("Open", false);
            CloseAllPanels();

        }
        else
        {
            charCtr?.SetHold(false);
            //charCtr?.Play(true);
            videoPlayer.Pause();
            text_PlayOrPause.text = "播放";
            //ani.SetBool("Open", true);
            openMainUIPanel();

        }
        yield return null;
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
            rawImage.texture = www.texture;
        }
    }

}
