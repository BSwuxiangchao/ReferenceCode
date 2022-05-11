using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Fleck;
using System.Text;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using System;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Runtime.Serialization;
using System.Linq;

public class WSServer : NetBase
{

    // Start is called before the first frame update
    private string ip = "127.0.0.1";
    private int port = 9091;
    private static Fleck.WebSocketServer server = null;
    public const string imgAI = "IMG_AI";
    public const string voiceAI = "VOICE_AI";

    private Dictionary<string, IWebSocketConnection> connDic = new Dictionary<string, IWebSocketConnection>();
    void Start()
    {


    }

    public override void run(string ip_,int port_)
    {
        server = new Fleck.WebSocketServer("ws://" + ip_ + ":" + port_);
        if(server != null)
        {
            ip = ip_;
            port = port_;
        }
        else {
            server = new Fleck.WebSocketServer("ws://" + ip + ":" + port);
        }

        //var sockets = new List<IWebSocketConnection>();

        server.Start(socket =>
        {
            socket.OnOpen = () =>
            {
                //sockets.Add(socket);
                connDic.Add(socket.ConnectionInfo.ClientPort.ToString(), socket);
            };

            socket.OnMessage = message =>
            {
                handleMessage(socket, message);
            };

            socket.OnClose = () =>
            {

                foreach (var key in connDic.Keys)
                {
                    if (connDic[key].Equals(socket))
                    {
                        connDic.Remove(key);
                    }
                }
            };
        });
    }
    protected void handleMessage(IWebSocketConnection conn, string bodyStr)
    {
        if (msgDelegate != null)
        {
            msgDelegate(bodyStr);
        }
        string answer = "";
        Result re = new Result();
        re.isOK = true;
        if (bodyStr.Contains("IMG_ATTR"))
        {
            if(!connDic.ContainsKey(imgAI))
            {
                connDic = connDic.ToDictionary(k => k.Key == conn.ConnectionInfo.ClientPort.ToString() ? imgAI : k.Key, k => k.Value);
            }
        }
        else if (bodyStr.Contains("customerAttr"))
        {
            if (!connDic.ContainsKey(voiceAI))
            {
                connDic = connDic.ToDictionary(k => k.Key == conn.ConnectionInfo.ClientPort.ToString() ? voiceAI : k.Key, k => k.Value);
            }
        }
        else if (bodyStr.Contains("vtuberAttr"))
        {
            if (!connDic.ContainsKey(voiceAI))
            {
                connDic = connDic.ToDictionary(k => k.Key == conn.ConnectionInfo.ClientPort.ToString() ? voiceAI : k.Key, k => k.Value);
            }
        }
        else if (bodyStr.Contains("ScreenPro"))
        {

        }
        else
        {
            re.isOK = false;
            Debug.LogWarning("unkown message :" + bodyStr);
            return;
        }
        try
        {
            processRequestBody(bodyStr);
        }
        catch (Exception ex)
        {
            re.isOK = false;
            Debug.LogWarning("handle message exceptiion:" + ex.Message);
        }
        finally
        {
            answer = JsonHelper.ObjectToJSON<Result>(re);
            conn.Send(answer);
        }
    }

    public override void sendMessage(string clientName,string message)
    {

        if(connDic.ContainsKey(clientName))
        {
            connDic[clientName].Send(message);
        }
        else
        {
            Debug.LogError("No " + clientName +" connected,while send Message!");
            return;
        }
    }
}
