using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BtnManager : MonoBehaviour
{
    public static class ButtonState
    {
        public static int DEFAULT = 0;
        public static int CURRENT = 1;
        public static int SELECTED = 2;
    }

    public Button[] Btns;
    public Button currentBtn;
    private Button preBtn;
    const string StateIndexID = "StateIndex";
    // Start is called before the first frame update
    public void Init()
    {
        for (int i = 0; i < Btns.Length; i++)
        {
            Btns[i].gameObject.AddComponent<Button>();
        }
        if(Btns.Length>0)
        {
            SetCurrentBtn(Btns[0],1);
        }
    }

    public void Awake()
    {
        if (Btns.Length <= 0)
        {
            return;
        }
        //OpenPanel(Pages[0]);
    }

    public void SetCurrentBtn(Button page,int currentStateIndex = 1)
    {
        if (page == null)
        {
            return;
        }
        if(currentBtn != null)//将之前的button恢复到默认状态
        {
            SetState(0);
        }
        preBtn = currentBtn;
        currentBtn = page;
        SetState(currentStateIndex);//将现在选中的button设为选中状态
    }

    public void SetState(int currentStateIndex)
    {
        Animator anim = currentBtn?.GetComponent<Animator>();
        if (anim && anim.isActiveAndEnabled)
        {
            anim?.SetInteger(StateIndexID, currentStateIndex);
        }
        currentBtn?.gameObject.SetActive(true);
        Debug.Log("按钮" + currentBtn?.gameObject.name + "状态更新");
    }

    public int GetCurrentBtnIndex()
    {
        int index = 0;
        if(currentBtn == null)
        {
            return index;
        }   
        for(int i = 0;i < Btns.Length;i++)
        {
            if(Btns[i].name == currentBtn.name)
            {
                index = i;
            }
        }
        return index;
    }

    public int GetCurrentBtnState()
    {
        int index = 0;
        if (currentBtn == null)
        {
            return index;
        }
        Animator ani = currentBtn.gameObject.GetComponent<Animator>();
        index = ani.GetInteger(StateIndexID);
        return index;
    }

}
