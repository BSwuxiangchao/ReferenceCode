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
     *  如：ScenesCtrlApi.Instance.OpenScene(ScenesCtrlApi.SceneOrder.ServiceSce);
     */
    /// <summary>
    /// 打开Scene
    /// </summary>
    /// <param name="SceneOrd"></param>
    public void OpenScene(int SceneOrd);
}

/// <summary>
/// 场景状态
/// </summary>
public abstract class SceneState
{
    /// <summary>
    /// 场景进入时执行的操作
    /// </summary>
    public abstract void OnEnter();

    /// <summary>
    /// 场景退出时执行的操作
    /// </summary>
    public abstract void OnExit();

}

/// <summary>
/// ServiceStation场景
/// </summary>
public class ServiceScene : SceneState
{
    readonly string sceneName = "ServiceStationScene";
    public override void OnEnter()
    {
        Debug.Log($"进入{ sceneName} 场景");

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
    /// 场景加载完成后执行的任务
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} 加载完毕");
    }
}

/// <summary>
/// ServiceStation场景
/// </summary>
public class UnstoreScene : SceneState
{
    readonly string sceneName = "UnstoreScene";
    public override void OnEnter()
    {
        Debug.Log($"进入{ sceneName} 场景");
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
    /// 场景加载完成后执行的任务
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} 加载完毕");
    }
}

/// <summary>
/// VtuberScene场景
/// </summary>
public class VtuberScene : SceneState
{
    readonly string sceneName = "VtuberScene";
    public override void OnEnter()
    {
        Debug.Log($"进入{ sceneName} 场景");
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
    /// 场景加载完成后执行的任务
    /// </summary>
    /// <param name="scene"></param>
    /// <param name="load"></param>
    private void SceneLoaded(Scene scene, LoadSceneMode load)
    {
        Debug.Log($"{ sceneName} 加载完毕");
    }
}

/// <summary>
/// 场景的状态管理系统
/// </summary>
public class SceneSystem
{
    /// <summary>
    /// 场景状态类
    /// </summary>
    SceneState sceneState;

    /// <summary>
    /// 设置当前场景状态并进入场景
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
/// 管理全局的一些东西
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
    /// 单例
    /// </summary>
    public static ScenesCtrlApi Instance { get; private set; }
    /// <summary>
    /// 场景管理器
    /// </summary>
    public SceneSystem SceneSystem { get; private set; }

    private void Awake()
    {
        if (Instance == null)
            Instance = this;
        else
            Destroy(gameObject);
        SceneSystem = new SceneSystem();
        //DontDestroyOnLoad(gameObject);//保证跳转场景后gameObect不被销毁
    }

    private void Start()
    {
        //OpenScene(ScenesCtrlApi.SceneOrder.UnstoreSceH);
    }
    /// <summary>
    /// 打开Scene
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
