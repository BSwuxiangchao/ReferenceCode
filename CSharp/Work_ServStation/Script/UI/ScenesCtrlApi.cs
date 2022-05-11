using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

interface ScenesCtrlInterface
{
    /*
     *      ServiceSce = 1;
     *      UnstoreSce = 2;
     *      VtuberSce = 3;
     *  �磺ScenesCtrlApi.Instance.OpenScene(ScenesCtrlApi.SceneOrder.ServiceSce);
     */
    /// <summary>
    /// ��Scene
    /// </summary>
    /// <param name="SceneOrd"></param>
    public void OpenScene(int SceneOrd);
}

/// <summary>
/// ����״̬
/// </summary>
public abstract class SceneState
{
    /// <summary>
    /// ��������ʱִ�еĲ���
    /// </summary>
    public abstract void OnEnter();

    /// <summary>
    /// �����˳�ʱִ�еĲ���
    /// </summary>
    public abstract void OnExit();

}

/// <summary>
/// ServiceStation����
/// </summary>
public class ServiceScene : SceneState
{
    readonly string sceneName = "ServiceStationScene";
    public override void OnEnter()
    {
        Debug.Log($"����{ sceneName} ����");

        if (SceneManager.GetActiveScene().name != sceneName)
        {
            SceneManager.LoadScene(sceneName);
            SceneManager.sceneLoaded += SceneLoaded;
        }
        else
        {
        }
    }


    public override void OnExit()
    {
        SceneManager.sceneLoaded -= SceneLoaded;
    }

    /// <summary>
    /// ����������ɺ�ִ�е�����
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} �������");
    }
}

/// <summary>
/// ServiceStation����
/// </summary>
public class UnstoreScene : SceneState
{
    readonly string sceneName = "UnstoreScene";
    public override void OnEnter()
    {
        Debug.Log($"����{ sceneName} ����");
        if (SceneManager.GetActiveScene().name != sceneName)
        {
            SceneManager.LoadScene(sceneName);
            SceneManager.sceneLoaded += SceneLoaded;
        }
        else
        {
        }
    }


    public override void OnExit()
    {
        SceneManager.sceneLoaded -= SceneLoaded;
    }

    /// <summary>
    /// ����������ɺ�ִ�е�����
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} �������");
    }
}

/// <summary>
/// VtuberScene����
/// </summary>
public class VtuberScene : SceneState
{
    readonly string sceneName = "VtuberScene";
    public override void OnEnter()
    {
        Debug.Log($"����{ sceneName} ����");
        if (SceneManager.GetActiveScene().name != sceneName)
        {
            SceneManager.LoadScene(sceneName);
            SceneManager.sceneLoaded += SceneLoaded;
        }
        else
        {
        }
    }


    public override void OnExit()
    {
        SceneManager.sceneLoaded -= SceneLoaded;
    }

    /// <summary>
    /// ����������ɺ�ִ�е�����
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} �������");
    }
}

/// <summary>
/// ������״̬����ϵͳ
/// </summary>
public class SceneSystem
{
    /// <summary>
    /// ����״̬��
    /// </summary>
    SceneState sceneState;

    /// <summary>
    /// ���õ�ǰ����״̬�����볡��
    /// </summary>
    /// <param name="state"></param>
    public void SetScene(SceneState state)
    {
        //if (sceneState != null)
        //{
        //    sceneState.OnExit();
        //}
        //sceneState = state;
        //if (sceneState != null)
        //    sceneState.OnEnter();

        sceneState ?.OnExit();
        sceneState = state;
        sceneState?.OnEnter();
    }
}


/// <summary>
/// ����ȫ�ֵ�һЩ����
/// </summary>
public class ScenesCtrlApi : MonoBehaviour, ScenesCtrlInterface
{
    public static class SceneOrder
    {
        public static readonly int UnstoreSce = 1;
        public static readonly int VtuberSce = 2;
        public static readonly int ServiceSce = 3;
    }
    /// <summary>
    /// ����
    /// </summary>
    public static ScenesCtrlApi Instance { get; private set; }
    /// <summary>
    /// ����������
    /// </summary>
    public SceneSystem SceneSystem { get; private set; }

    private void Awake()
    {
        if (Instance == null)
            Instance = this;
        else
            Destroy(gameObject);
        SceneSystem = new SceneSystem();
        //DontDestroyOnLoad(gameObject);//��֤��ת������gameObect��������
    }

    private void Start()
    {
        //OpenScene(ScenesCtrlApi.SceneOrder.UnstoreSceH);
    }
    /// <summary>
    /// ��Scene
    /// </summary>
    /// <param name="SceneOrd"></param>
    public void OpenScene(int SceneOrd)
    {
        switch(SceneOrd)
        {
            case 1:
                SceneSystem.SetScene(new UnstoreScene());
                break;
            case 2:
                SceneSystem.SetScene(new VtuberScene());
                break;
            case 3:
                SceneSystem.SetScene(new ServiceScene());
                break;
        }
    }
}
