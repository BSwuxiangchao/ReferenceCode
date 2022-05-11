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
            //1.�����һ���ؼ�
            if(btnCount == 0)
            {
                //1.1��һ���ؼ�����ʾ����
                btnFirst.transform.Find("Text").GetComponent<Text>().text = tagName;
                
                //1.2��ӵ�һ���ؼ����б�
                btnList.Add(btnFirst);
                buttomPosition = btnFirst.transform.localPosition;//��ȡ��һ���ؼ���λ����Ϊ�����Ĳο�λ��
                btnCount++;
                continue;
            }

            //2.����ڿؼ�����
            //2.1 �ؼ�����ʾ������
            Button btn = Instantiate(btnFirst, Vector3.zero, Quaternion.identity) as Button;
            btn.transform.parent = bottomPanel.transform;
            btn.transform.localScale = buttomScale;
            btn.transform.localEulerAngles = buttomRot; 
            btn.transform.localPosition = new Vector3(btnCount * buttomSpacing, 0f, 0f) + buttomPosition;
            btn.transform.Find("Text").GetComponent<Text>().text = tagName;


            //2.2������пؼ��������б�
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

    void moveFloop()//�ڵ��¹���
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
        //�󶨵��¼��᲻�ϵ���
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
        ////��texture�����ظ��Ƶ�RenderTexture
        //Graphics.Blit(tmpVp.texture, tmp);

        //// RenderTexture.active = tmp;
        //videoFrameTexture.ReadPixels(new Rect(0, 0, tmp.width, tmp.height), 0, 0);
        //videoFrameTexture.Apply();

        //img.sprite = Sprite.Create(videoFrameTexture, new Rect(0,0, videoFrameTexture.width, videoFrameTexture.height), new Vector2(0.5f, 0.5f));
        //// RenderTexture.active = null;
        //// �ͷ���ʱRenderTexture
        //RenderTexture.ReleaseTemporary(tmp);

        //Destroy(go.gameObject);

    }

    void OnNewFrame(VideoPlayer source, long frameIdx)
    {
        RenderTexture renderTexture = source.texture as RenderTexture;

        if (frameIdx >= 3)  //Ҫ�� > ,frameIdx���ܻ������������
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
