//using Common.Logging;
//using log4net;
//using log4net.Config;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;


public class Logger : MonoBehaviour
{
    public int NumOfSingleFiles = 100;

    [Header("保存日志类型")]
    public bool _Log = true;
    public bool _Warning = true;
    public bool _Assert = true;
    public bool _Exception = true;
    public bool _Error = true;

    Queue<string> list;

    long count;

    string fileName
    {
        get
        {
            return DateTime.Now.ToString("yyyy-M-dd_HH-mm-ss");
        }
    }


    private void Awake()
    {
        list = new Queue<string>();
        count = 0;
        Application.logMessageReceivedThreaded += CaptureLogThread;
        Debug.Log("Log begin.");
    }

    private void CaptureLogThread(string condition, string stackTrace, LogType type)
    {
        count++;
        string error = "";
        switch (type)
        {
            case LogType.Error:
                error = StackTraceUtility.ExtractStackTrace();
                if (!_Error)
                {
                    return;
                }
                break;
            case LogType.Assert:
                if (!_Assert)
                {
                    return;
                }
                break;
            case LogType.Warning:
                if (!_Warning)
                {
                    return;
                }
                break;
            case LogType.Log:
                if (!_Log)
                {
                    return;
                }
                break;
            case LogType.Exception:
                error = StackTraceUtility.ExtractStackTrace();
                if (!_Exception)
                {
                    return;
                }
                break;
            default:
                break;
        }
        string _log = type + $" >> {DateTime.Now.ToString()} : {condition} \n {error} \n";

        list.Enqueue(_log);

        if (list.Count >= NumOfSingleFiles)
        {
            string log = "";
            for (int i = 0; i < NumOfSingleFiles; i++)
            {
                log += list.Dequeue();
            }
            SaveLog(log);
        }
    }

    private void OnApplicationQuit()
    {
        string log = "";
        while (true)
        {
            if (list!=null && list.Count > 0)
            {
                log += list.Dequeue();
            }
            else
            {
                log += "\nnumOfLog: " + count;
                SaveLog(log);
                break;
            }
        }

    }


    void SaveLog(string log)
    {
        string path = Path.Combine(Directory.GetCurrentDirectory(), "Log");
        if (!File.Exists(path))
        {
            Directory.CreateDirectory(path);
        }

        string filePath = Path.Combine(path, fileName + ".log");
        File.WriteAllText(filePath, log);
    }

}
