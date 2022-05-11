using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class BottomPanelCtr : MonoBehaviour
{
    private ServiceManager cfg;
    public PanelInstance bottomPanel;
    public string ServiceCfgAttachName = "Canvas";
    public string btnAttachPanelName = "BottomPanel";

    //GameObject go;
    //private Button[] buttons;

    public Button btnFirst;
    private List<Button> btnList = new List<Button>();
    #region buttom RectTransform
    //public RectTransform buttomRt;
    public float buttomSpacing = 100f;
    public Vector3 buttomPosition = new Vector3(0f,0f,0f);
    public Rect buttomRect;
    public Vector3 buttomRot;
    public Vector3 buttomScale = new Vector3(1.0f,1.0f,1.0f);
    #endregion

    #region buttomMove control
    //private Vector3 startPosition = new Vector3(0f,0f,0f);
    private Vector3 moveSpeed = new Vector3(-0.5f, 0f, 0f);
    #endregion

    #region buttomImg
    public VideoPlayer videoPlayer;
    //public Image buttomImg;
    #endregion

    #region buttom.buttom

    #endregion
    // Start is called before the first frame update
    void Start()
    {
        cfg = GameObject.Find(ServiceCfgAttachName).GetComponent<ServiceManager>();
        if (cfg != null)
            createBottum();

        if (bottomPanel != null)
            btnFirst = bottomPanel.GetComponent<Button>();


    }

    void createBottum()
    {
        //var rectTransform = bottomPanel.transform.GetComponent<RectTransform>();
        //string[] MVtags = cfg.jsonParam.karaokeCfg.TagsHave;
        string[] MVtags = MvTagName.tagDic.Values.ToArray();
        int btnCount = 0;

        if (btnFirst == null)
            return;
        foreach(string tagName in MVtags)
        {
            //1.处理第一个控件
            if(btnCount == 0)
            {
                //1.1第一个控件的显示文字
                btnFirst.transform.Find("Text").GetComponent<Text>().text = tagName;
                
                //1.2添加第一个控件到列表
                btnList.Add(btnFirst);
                buttomPosition = btnFirst.transform.localPosition;//获取第一个控件的位置作为副本的参考位置
                btnCount++;
                continue;
            }

            //2.处理第控件副本
            //2.1 控件的显示的文字
            Button btn = Instantiate(btnFirst, Vector3.zero, Quaternion.identity) as Button;
            btn.transform.parent = bottomPanel.transform;
            btn.transform.localScale = buttomScale;
            btn.transform.localEulerAngles = buttomRot; 
            btn.transform.localPosition = new Vector3(btnCount * buttomSpacing, 0f, 0f) + buttomPosition;
            btn.transform.Find("Text").GetComponent<Text>().text = tagName;


            //2.2添加所有控件副本到列表
            btnList.Add(btn);
            btnCount++;
            //bottomPanel.gameObject.AddComponent<Button>(btn);
        }


    }

    // Update is called once per frame
    void Update()
    {
        moveFloop();
    }

    void moveFloop()//在底下滚动
    {
        if (btnList == null || btnList.Count < 1)
            return;

        var rectTransform = bottomPanel.transform.GetComponent<RectTransform>();

        int ShowCount = (int)(rectTransform.rect.width / buttomSpacing) - 1;

        if (btnList.Count <= ShowCount)
            return;

        var biggest = btnList.OrderByDescending(s => s.transform.localPosition.x).First().transform.localPosition;
        foreach (Button btnTmp in btnList)
        {
            btnTmp.transform.localPosition += moveSpeed;

            if (moveSpeed.x < 0 && btnTmp.transform.localPosition.x < buttomPosition.x - buttomSpacing)
                btnTmp.transform.localPosition = biggest + new Vector3(buttomSpacing, 0f, 0f);

        }

    }

    IEnumerator GetVideoThumb(string urlpath, int index)
    {
        print(index);
        VideoPlayer tmpVp = Instantiate(videoPlayer);
        //VideoPlayer tmpVp = go.gameObject.AddComponent<VideoPlayer>();
        //go.gameObject.AddComponent<IndexNumber>();
        //go.GetComponent<IndexNumber>().index = index;

        tmpVp.source = VideoSource.Url;
        tmpVp.url = urlpath;
        tmpVp.sendFrameReadyEvents = true;
        //绑定的事件会不断调用
        tmpVp.frameReady += OnNewFrame;
        tmpVp.Play();
        yield return null;
        //while (tmpVp.texture == null|| tmpVp.isPlaying==false)
        //{
        //    yield return new   WaitForSeconds(1.5f);
        //}

        //Texture2D videoFrameTexture = new Texture2D(Screen.width, Screen.width,TextureFormat.ARGB32, false);
        //RenderTexture tmp = RenderTexture.GetTemporary(
        //            Screen.width, Screen.width,
        //            0,
        //            RenderTextureFormat.Default,
        //            RenderTextureReadWrite.sRGB);
        ////将texture的像素复制到RenderTexture
        //Graphics.Blit(tmpVp.texture, tmp);

        //// RenderTexture.active = tmp;
        //videoFrameTexture.ReadPixels(new Rect(0, 0, tmp.width, tmp.height), 0, 0);
        //videoFrameTexture.Apply();

        //img.sprite = Sprite.Create(videoFrameTexture, new Rect(0,0, videoFrameTexture.width, videoFrameTexture.height), new Vector2(0.5f, 0.5f));
        //// RenderTexture.active = null;
        //// 释放临时RenderTexture
        //RenderTexture.ReleaseTemporary(tmp);

        //Destroy(go.gameObject);

    }

    void OnNewFrame(VideoPlayer source, long frameIdx)
    {
        RenderTexture renderTexture = source.texture as RenderTexture;

        if (frameIdx >= 3)  //要用 > ,frameIdx可能会跳过这个数字
        {

            Texture2D videoFrameTexture = new Texture2D(Screen.width, Screen.width, TextureFormat.ARGB32, false);
            videoFrameTexture.Resize(renderTexture.width, renderTexture.height);
            RenderTexture.active = renderTexture;
            videoFrameTexture.ReadPixels(new Rect(0, 0, renderTexture.width, renderTexture.height), 0, 0);
            videoFrameTexture.Apply();

            //AllBtnObj[source.GetComponent<IndexNumber>().index].transform.GetComponent<Image>().sprite = Sprite.Create(videoFrameTexture, new Rect(0, 0, videoFrameTexture.width, videoFrameTexture.height), new Vector2(0.5f, 0.5f));

            source.frameReady -= OnNewFrame;
            source.sendFrameReadyEvents = false;
            Destroy(source.gameObject);
        }
    }
}
