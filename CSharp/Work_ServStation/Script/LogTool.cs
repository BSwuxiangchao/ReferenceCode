using UnityEngine;
using System.Collections;
using System.IO;
using System.Text;
using System;

public class LogTool : MonoBehaviour
{
    const float FILEMAXSIZE = 1024.0f;
    string logPath;
    string filePath = "";
    string tempLog = "";
    string oldLog = "";

    void Awake()
    {
        //Application.logMessageReceived += Application_logMessageReceived;//只在主线程
        Application.logMessageReceivedThreaded += Application_logMessageReceivedThreaded;
        Init();
    }

    void Init()
    {

#if UNITY_EDITOR || UNITY_STANDALONE_WIN
        logPath = string.Format("{0}/Log", System.Environment.CurrentDirectory);
#elif UNITY_ANDROID
         logPath = string.Format("{0}/Log", Application.persistentDataPath);
#endif
        filePath = string.Format("{0}/{1}.txt", logPath, System.DateTime.Now.ToString("yyyy-MM-dd"));

        if (!Directory.Exists(logPath))
            Directory.CreateDirectory(logPath);

        if (!File.Exists(filePath))
        {
            FileStream stream = System.IO.File.Create(filePath);
            stream.Close();
            stream.Dispose();
        }
        Application_logMessageReceivedThreaded("-----------------------------------------------", "", LogType.Log);
        Application_logMessageReceivedThreaded("程序启动", "", LogType.Log);
        Invoke("LogClearTask", 100);
    }

    void OnDestroy()
    {
        //Application.logMessageReceived -= Application_logMessageReceived;
        Application.logMessageReceivedThreaded -= Application_logMessageReceivedThreaded;
    }

    void Application_logMessageReceivedThreaded(string condition, string stackTrace, LogType type)
    {
        if (oldLog == condition)//如果上一条与这一条相同，则不记录
        {
            return;
        }

        oldLog = condition;
        if (type == LogType.Assert || type == LogType.Exception || type == LogType.Error)
            tempLog = string.Format("[{0}][{1}]:{2} \r\n    StackTrace:{3}", System.DateTime.Now.ToString("HH:mm:ss"), type.ToString(), condition, stackTrace);
        else
            tempLog = string.Format("[{0}][{1}]:{2}", System.DateTime.Now.ToString("HH:mm:ss"), type.ToString(), condition);

        using (StreamWriter sw = new StreamWriter(filePath, true, Encoding.UTF8))
        {
            sw.WriteLine(tempLog);
        }

    }

    /// <summary>
    /// 执行日志清除
    /// </summary>
    /// <returns></returns>
    private void LogClearTask()
    {
        try
        {
            Debug.Log("开始清除大于60天日志");
            DirectoryInfo di = new DirectoryInfo(logPath);
            FileInfo[] files = di.GetFiles("*.txt");
            for (int i = 0; i < files.Length; i++)
            {
                FileInfo file = files[i];
                DateTime fileDate = Convert.ToDateTime(Path.GetFileNameWithoutExtension(file.FullName));
                DateTime nowDate = DateTime.Now.Date;
                int value = (nowDate - fileDate).Days;
                if (value > 60)
                {
                    file.Delete();
                }
            }
            Debug.Log("日志清除任务完成");
        }
        catch (Exception ex)
        {
            Debug.LogError("日志清除错误： " + ex.Message);
        }
    }
}
