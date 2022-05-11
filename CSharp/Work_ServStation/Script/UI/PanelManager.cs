using UnityEngine;
using System.Collections;
using System;
public class PanelManager : MonoBehaviour
{
    public PanelInstance[] Pages;//���б����Ƶ�Panel
    public PanelInstance currentPanel;//��ǰPanel

    public string[] mainUIpanels;//�����������Panel

    void Start()
    {
        for (int i = 0; i < Pages.Length; i++)
        {
            Pages[i].gameObject.AddComponent<PanelInstance>();
        }
    }

    void Awake()
    {
        if (Pages.Length <= 0)
        {
            return;
        }
        //OpenPanel(Pages[0]);
        //openAll();
        openMainUIPanel();
    }

    /// <summary>
    /// ֻ���������Panel
    /// </summary>
    public void openMainUIPanel()
    {
        //OpenPanelByName("BtnMenuPanel");
        //CloseAllPanels();
        //EnablePanelByName("PayPanel",false);
        closeAllwWithoutMainPanel();
        //OpenPanelByName("BtnMenuPanel");
        //OpenPanelByName("CharactorPanel");
        foreach(var mainPanelStr in mainUIpanels)
        {
            OpenPanelByName(mainPanelStr);
        }
    }

    /// <summary>
    /// ������Panel
    /// </summary>
    public void openAll()
    {
        if (Pages.Length <= 0)
        {
            return;
        }
        foreach(var panel in Pages)
        {
            OpenPanel(panel);
        }
    }

    //ͨ�����ô����
    public void OpenPanel(PanelInstance page)
    {
        if (page == null)
        {
            return;
        }
        page.PanelBefore = currentPanel;
        currentPanel = page;
        //CloseAllPanels();
        Animator anim = page.GetComponent<Animator>();
        if (anim && anim.isActiveAndEnabled)
        {
            anim.SetBool("Open", true);
        }
        page.gameObject.SetActive(true);
        Debug.Log("���" + page.gameObject.name + "�Ѿ���");
    }

    //ͨ�����ִ����
    public void OpenPanelByName(string name)
    {
        PanelInstance page = null;
        for (int i = 0; i < Pages.Length; i++)
        {
            if (Pages[i].name == name)
            {
                page = Pages[i];
                break;
            }
        }
        if (page == null)
        {
            return;
        }
        page.PanelBefore = currentPanel;
        currentPanel = page;
        //CloseAllPanels();
        page.gameObject.SetActive(true);
        Animator anim = page.GetComponent<Animator>();
        if (anim && anim.isActiveAndEnabled)
        {
            anim.SetBool("Open", true);
        }

    }
    public void ClosePanel(PanelInstance page)
    {
        if (page == null)
        {
            return;
        }

        currentPanel = page.PanelBefore;
        Animator anim = page.gameObject.GetComponent<Animator>();
        if (anim && anim.isActiveAndEnabled)
        {
            //Pages[i].gameObject.SetActive(true);
            anim.SetBool("Open", false);
        }
        if (page.isActiveAndEnabled)
        {
            StartCoroutine(DisablePanelDeleyed(page));
        }
    }
    public void ClosePanelByName(string name)
    {
        PanelInstance page = null;
        for (int i = 0; i < Pages.Length; i++)
        {
            if (Pages[i].name == name)
            {
                page = Pages[i];
                break;
            }
        }
        if (page == null)
        {
            return;
        }

        currentPanel = page.PanelBefore;
        Animator anim = page.gameObject.GetComponent<Animator>();
        if (anim && anim.isActiveAndEnabled)
        {
            //Pages[i].gameObject.SetActive(true);
            anim.SetBool("Open", false);
        }
        if (page.isActiveAndEnabled)
        {
            StartCoroutine(DisablePanelDeleyed(page));
        }

    }
    public PanelInstance GetPanelByName(string name)
    {
        PanelInstance page = null;
        for (int i = 0; i < Pages.Length; i++)
        {
            if (Pages[i].name == name)
            {
                page = Pages[i];
                break;
            }
        }
        return page;
    }

    /// <summary>
    /// ����Panel�ļ���״̬
    /// </summary>
    /// <param name="name">Panel����</param>
    /// <param name="isActive">�Ƿ񼤻�</param>
    public void EnablePanelByName(string name,bool isActive = true)
    {
        PanelInstance page = null;
        for (int i = 0; i < Pages.Length; i++)
        {
            if (Pages[i].name == name)
            {
                page = Pages[i];
                break;
            }
        }
        if (page == null)
        {
            return;
        }
        page.gameObject.SetActive(isActive);
    }

    //�ر��������
    public void CloseAllPanels()
    {
        if (Pages.Length <= 0)
        {
            return;
        }

        for (int i = 0; i < Pages.Length; i++)
        {
            if (Pages[i].name == "BtnDebugPanel")
            {
                continue;
            }
            Animator anim = Pages[i].gameObject.GetComponent<Animator>();
            if (anim && anim.isActiveAndEnabled)
            {
                //Pages[i].gameObject.SetActive(true);
                anim.SetBool("Open", false);
            }
            if (Pages[i].isActiveAndEnabled)
            {
                StartCoroutine(DisablePanelDeleyed(Pages[i]));
            }
        }
    }

    /// <summary>
    /// �ر��������������Panel
    /// </summary>
    public void closeAllwWithoutMainPanel()
    {
        if (Pages.Length <= 0)
        {
            return;
        }
        foreach(var paneltmp in Pages)
        {
            if (isPanelMain(paneltmp.name))
                continue;
            if (paneltmp.name == "BtnDebugPanel")
            {
                continue;
            }
            Animator anim = paneltmp.gameObject.GetComponent<Animator>();
            if (anim && anim.isActiveAndEnabled)
            {
                //Pages[i].gameObject.SetActive(true);
                anim.SetBool("Open", false);
            }
            if (paneltmp.isActiveAndEnabled)
            {
                StartCoroutine(DisablePanelDeleyed(paneltmp));
            }
        }
    }

    /// <summary>
    /// ����Panel�����ж��Ƿ�Ϊ�������Panel
    /// </summary>
    /// <param name="panelName">Panel����</param>
    /// <returns>true,��������Panel��false������������Panel</returns>
    bool isPanelMain(string panelName)
    {
        bool isMain = false;
        foreach (var paneltmp in mainUIpanels)
        {
            if (panelName == paneltmp)
                isMain = true;
        }
        return isMain;
    }

    //����Panel
    public IEnumerator DisablePanelDeleyed(PanelInstance page)
    {
        bool closedStateReached = false;
        bool wantToClose = true;
        Animator anim = page.gameObject.GetComponent<Animator>();
        if (anim && anim.enabled)
        {
            while (!closedStateReached && wantToClose)
            {
                if (anim.isActiveAndEnabled && !anim.IsInTransition(0))
                {

                    closedStateReached = anim.GetCurrentAnimatorStateInfo(0).IsName("Closing");
                }
                yield return new WaitForEndOfFrame();
            }
            if (wantToClose)
            {
                anim.gameObject.SetActive(false);
            }
        }
        else
        {
            page.gameObject.SetActive(false);
        }

    }

}