using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class UnstoreBoxListCtr : UnstoreBoxListCtrApi
{
    // Start is called before the first frame update
    public PanelInstance boxListPanel;
    //public RawImage rawImg;
    private Button firstBtn;

    public string serviceAttachName = "Instance";

    private List<string> productList = new List<string>();
    private List<Button> productBtnList = new List<Button>();
    private readonly int aniPosCnt = 7;//商品动画位置总数
    private readonly int aniTopIndex = 3;
    private bool uptoTopTri = false;
    void Start()
    {
        if (boxListPanel != null)
            firstBtn = boxListPanel.gameObject.GetComponentInChildren<Button>();

        //destroyBtnList();
        //getProducList(ServiceManager.jsonParam.deviceinfo.productListFolder);
        //creatProductListBtn();
        //updateTop();
    }

    public void CreatFoodList()
    {
        destroyBtnList();
        getProducList(ServiceManager.jsonParam.deviceinfo.productListFolder);
        creatProductListBtn();
        updateTop();
    }

    private void destroyBtnList()
    {
        if (productBtnList == null || productBtnList.Count < 1)
            return;
        for (int i = 0; i < productBtnList.Count; i++)
        {
            if (i == 0)
            {
                continue;
            }
            Destroy(productBtnList[i].gameObject);
        }
        productList.Clear();
        productBtnList.Clear();
    }

    public override void SetBoxType(int type = 1,string serviceName= "服务列表")
    {
        curBoxListType_ = type;
        destroyBtnList();
        //string BoxlistFoder = ServiceManager.jsonParam.deviceinfo.ServiceListFolder;
        if (curBoxListType_ == BoxListType.None || curBoxListType_<0 || curBoxListType_ > BoxListType.AgriP)
        {
            return;
        }
        //if (type != BoxListType.GoodsType)
        //{
        //    BoxlistFoder = ServiceManager.jsonParam.deviceinfo.ServiceListFolder.Replace("服务列表", serviceName);
        //}
        //else
        //    BoxlistFoder = ServiceManager.jsonParam.deviceinfo.productListFolder;
        string BoxlistFoder = ServiceManager.jsonParam.deviceinfo.ServiceListFolder.Replace("服务列表", serviceName);
        getProducList(BoxlistFoder);
        creatProductListBtn();
        updateTop();
    }

    public override int GetBoxType()
    {
        return curBoxListType_;
    }
    private void getProducList(string path,string extender= "*.png")
    {
        if (!Directory.Exists(path))
            return;
        var files = Directory.GetFiles(path, extender);
        productList = files?.ToList<string>();
    }

    private void creatProductListBtn()
    {
        if (firstBtn == null||productList.Count == 0)
            return;


        if(productList.Count >= aniPosCnt)
        {
            layOutGoods();
        }
        else
        {
            layOutGoodsLess();
        }

    }

    private void layOutGoodsLess()
    {
        int cnt = 0;
        int bias = (aniPosCnt - productList.Count) / 2;
        foreach (var file in productList)
        {
            if (cnt == 0)
            {
                setBtnTexture(firstBtn, file, cnt + 1);
                setBtnAnimIndex(firstBtn, cnt+bias);
                productBtnList.Add(firstBtn);
                cnt++;
                continue;
            }
            Button btn = Instantiate(firstBtn, Vector3.zero, Quaternion.identity) as Button;
            btn.transform.parent = firstBtn.transform.parent;
            btn.transform.localScale = firstBtn.transform.localScale;
            btn.transform.localEulerAngles = firstBtn.transform.localEulerAngles;
            btn.transform.localPosition = firstBtn.transform.localPosition;
            setBtnTexture(btn, file, cnt + 1);

            if (cnt < aniPosCnt)
            {

                setBtnAnimIndex(btn, cnt+bias);
            }
            productBtnList.Add(btn);
            cnt++;
        }
    }
    private void layOutGoods()
    {
        int cnt = 0;
        foreach (var file in productList)
        {
            if (cnt == 0)
            {
                setBtnTexture(firstBtn, file, cnt + 1);
                setBtnAnimIndex(firstBtn, cnt);
                productBtnList.Add(firstBtn);
                cnt++;
                continue;
            }
            Button btn = Instantiate(firstBtn, Vector3.zero, Quaternion.identity) as Button;
            btn.transform.parent = firstBtn.transform.parent;
            btn.transform.localScale = firstBtn.transform.localScale;
            btn.transform.localEulerAngles = firstBtn.transform.localEulerAngles;
            btn.transform.localPosition = firstBtn.transform.localPosition;
            setBtnTexture(btn, file, cnt + 1);
            if (cnt < aniPosCnt)
            {
                setBtnAnimIndex(btn, cnt);
            }
            else
            {
                setBtnAnimIndex(btn, -1);//-1为隐藏位置
            }
            productBtnList.Add(btn);
            cnt++;
        }
    }

    public override void initState()
    {
        if (productBtnList == null)
            return;
        int cnt = 0;
        int bias = 0;
        if(productBtnList.Count < aniPosCnt)
            bias = (aniPosCnt - productList.Count) / 2;
        foreach (var btn in productBtnList)
        {
            if (cnt < aniPosCnt)
            {
                setBtnAnimIndex(btn, cnt+ bias);
            }
            else
            {
                setBtnAnimIndex(btn,- 1);//-1为隐藏位置
            }
            cnt++;
        }
    }
    private void setBtnTexture(Button btn,string file,int index = 1)
    {
        string filePath = "file://" + file;
        WWW www = new WWW(filePath);
        if (www.error != null)
        {
            Debug.LogError("no image file found in " + filePath);
        }
        else
        {
            var rimg = btn.gameObject.GetComponentsInChildren<RawImage>();
            rimg[1].texture = www.texture;
            var TextList = btn.gameObject.GetComponentsInChildren<Text>();
            string name = "";
            DirectoryInfo root = new DirectoryInfo(file);
            name = root.Name.TrimEnd(".png".ToCharArray());
            TextList[0].text = index.ToString();
            //TextList[0].color = Color.green;
            TextList[1].text = name;
            //TextList[1].color = Color.green;
        }
    }

    private void setBtnAnimIndex(Button btn,int index)
    {
        var ani = btn.gameObject.GetComponent<Animator>();
        ani.SetInteger("Index", index);
    }

    private int getBtnAniIndex(Button btn)
    {
        var ani = btn.gameObject.GetComponent<Animator>();
        int index = ani.GetInteger("Index");
        return index;
    }

    private bool getBtnSahkeIndex(Button btn)
    {
        var ani = btn.gameObject.GetComponent<Animator>();
        bool index = ani.GetBool("IsShake");
        return index;
    }

    private void updateTop()
    {
        //Invoke("updatetop", 0.25f);
        uptoTopTri = true;
    }

    private void LateUpdate()
    {
        if(uptoTopTri)
        {
            //updatetop();
            Invoke("updatetop",0.1f);
            uptoTopTri = false;
        }
    }
    private void updatetop()
    {
        int layerCnt = aniPosCnt;
        foreach (var btn in productBtnList)
        {
            int index = getBtnAniIndex(btn);
            if (index >= 0)
            {
                //int layindex = layerCnt - Mathf.Abs(index - aniTopIndex);
                int layindex = 0;
                if (index > aniTopIndex)
                {
                    layindex = layerCnt-1 - Mathf.Abs(index - aniTopIndex);
                }
                else
                {
                    layindex = aniTopIndex - Mathf.Abs(index - aniTopIndex);
                }
                btn.transform.SetSiblingIndex(layindex);
            }
            if (index < 0)
            {
                //btn.transform.SetAsFirstSibling();
                btn.transform.SetSiblingIndex(0);
            }
            if (aniTopIndex == index)
            {
                btn.transform.SetAsLastSibling();
            }
        }
    }

    public override void moveFront(int cnt = 1)
    {
        if (productBtnList.Count < 2)
            return;
        int loopCnt = productBtnList.Count;//Mathf.Min(productBtnList.Count,aniPosCnt);
        int moveCnt = cnt % loopCnt;
        int start = 0;
        if (cnt > 0)//向前滑动
        {
            start = getBtnAniIndex(productBtnList[0]);
            for (int i = 0; i < loopCnt - moveCnt; i++)
            {
                var index = getBtnAniIndex(productBtnList[i + moveCnt]);
                setBtnAnimIndex(productBtnList[i], index);
            }
            for (int i = loopCnt - moveCnt; i < loopCnt; i++)
            {
                var index = getBtnAniIndex(productBtnList[i + moveCnt - loopCnt]);
                setBtnAnimIndex(productBtnList[i], index);
            }
            setBtnAnimIndex(productBtnList[loopCnt -1], start);
            updateTop();
            return;
        }

        start = getBtnAniIndex(productBtnList[loopCnt - 1]);
        for (int i = loopCnt -1; i >= -moveCnt; i--)
        {
            var index = getBtnAniIndex(productBtnList[i + moveCnt]);
            setBtnAnimIndex(productBtnList[i], index);
        }
        for (int i = -moveCnt -1; i >= 0; i--)
        {
            var index = getBtnAniIndex(productBtnList[i + moveCnt + loopCnt]);
            setBtnAnimIndex(productBtnList[i], index);
        }
        setBtnAnimIndex(productBtnList[0], start);
        updateTop();
    }


    public override void ShakeAct(int index = 3,bool isShake = true)
    {
        if (productBtnList.Count <= index)
            return;
        var ani = productBtnList[index].gameObject.GetComponent<Animator>();
        ani?.SetBool("IsShake",isShake);
    }

    public override string GetCurrentGoodsID()
    {
        foreach (var btn in productBtnList)
        {
            int index = getBtnAniIndex(btn);
            if (aniTopIndex == index)
            {
                var textList = btn.gameObject.GetComponentsInChildren<Text>();
                if (textList == null || textList.Length < 1)
                    return null;
                return textList[0]?.text;
            }
        }
        return null;
    }

    public override string GetCurrentGoodsName(int num)
    {
        if (num >= productBtnList.Count)
            return null;
        var textList = productBtnList[num].gameObject.GetComponentsInChildren<Text>();
        if (textList == null || textList.Length < 1)
            return null;
        return textList[1]?.text;
    }
}
