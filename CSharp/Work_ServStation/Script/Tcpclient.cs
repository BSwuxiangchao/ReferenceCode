using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System;
using Newtonsoft.Json;
using System.Linq;
using Newtonsoft.Json.Linq;

public class Tcpclient : NetBase
{
    private string ip = "127.0.0.1";
    private int port = 8081;
    const int bufferLength = 1024*1;
    //IPAddress ipa;
    //IPEndPoint ipEnd;
    Socket serverSocket;//服务端套接字

    IPEndPoint ipPoint;

    AutoResetEvent heartBeatEvent;
    int waitTime = 2000;//心跳回复等待时间ms
    int interval = 4;//心跳发送间隔时间s
    // Start is called before the first frame update
    private string userId = null;
    bool isClosed = false;
    bool isColsedRcv = false;
    void Start()
    {

    }

    public override void run(string ip_, int port_)
    {

        ipPoint = new IPEndPoint(IPAddress.Parse(ip_), port_);
        serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        heartBeatEvent = new AutoResetEvent(true);
        Task.Run(() =>
        {
            ip = ip_;
            port = port_;
            connectLoop();
            if (serverSocket.Connected)
            {
                ip = ip_;
                port = port_;
                Debug.LogWarning("scs of tcpclient connected to :"+ ip + ":" + port);
                if (userId != null)
                    login(userId);
                Task.Run(() => {
                    SocketReceive();
                });
                Task.Run(() =>
                {
                    heartBeatLoop();
                });
            }
        });
    }

    public void login(string userID)
    {
        userId = userID;
        LoginCmd cmd = new LoginCmd(userID);
        var str = JsonHelper.ObjectToJSON(cmd);
        sendMessage(str);
    }

    private void connectLoop()
    {
        if (ipPoint != null && serverSocket != null)
        {
            while(!serverSocket.Connected)
            {
                if (isClosed)
                    return;
                try
                {
                    Thread.Sleep(250);
                    serverSocket.Connect(ipPoint);
                }
                catch (Exception ex)
                {
                    Debug.LogWarning("failure of tcpclient connected to :"
                        + ip + ":" + port + " with exception:" + ex.Message);
                }
            }

        }
        else
        {
            Debug.LogError("failure of tcpclient connected to :" + ip + ":" + port);
        }
    }

    void heartBeatLoop()
    {
        //bool isReceive = true;
        while (true)
        {
            if (isClosed)
                return;
            //heartBeatEvent.Reset();
            Thread.Sleep(interval * 1000);
            //Invoke("sendHeartBeatCmd", interval);
            try
            {
                sendHeartBeatCmd();
            }
            catch(Exception ex)
            {
                Debug.LogWarning("Heartbeat send failed with excption :"+ex.Message);
                continue;
            }
        }
        //Task.Run(() =>
        //{
        //    connectLoop();
        //});
    }

    void sendHeartBeatCmd()
    {
        HeartBeatMsg heartBeatCmd = new HeartBeatMsg();
        string cmd = JsonHelper.ObjectToJSON<HeartBeatMsg>(heartBeatCmd);
        sendMessage(cmd);   
    }

    void SocketReceive()
    {
        isColsedRcv = false;
        while (true)
        {
            if (isClosed)
                return;
            if (isColsedRcv)
                return;
            var recvData = new byte[bufferLength];
            if (serverSocket == null)
                continue;
            var recvLen = serverSocket.Receive(recvData);
            if(recvLen == 0||!serverSocket.Connected)
            {
                // reconnect
                serverSocket.Connect(ipPoint);
                Debug.LogWarning("loss of tcp rcv");
                continue;
            }
            var recvStr = Encoding.ASCII.GetString(recvData, 0, recvLen);
            processRequestBody(recvStr);
        }
    }

    protected override void processRequestBody(string bodyStr)
    {
        Debug.LogWarning("recv tcp :" + bodyStr);
        System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();
        stopwatch.Start(); //  开始监视代码运行时间

        bodyStr = bodyStr.TrimEnd("$_".ToCharArray());

        //MsgType msgType = JsonHelper.JsonToObject<MsgType>(bodyStr);
        var msgType = (JObject)JsonConvert.DeserializeObject(bodyStr);

        if(msgType["msgType"].ToString() == "0")//登录请求
        {
            LogInRcv logInRcv = JsonHelper.JsonToObject<LogInRcv>(bodyStr);
            if(logInRcv.msgContent.type == "0")
            {
                // login OK
            }
            else
            {
                // login NG
            }

        }
        if(msgType["msgType"].ToString() == "1")//心跳回复
        {
            heartBeatEvent.Set();
            HeartBeatMsg heartBeatRcv = JsonHelper.JsonToObject<HeartBeatMsg>(bodyStr);
        }
        if(msgType["msgType"].ToString() == "4")//收到业务指令
        {
            SocketMessage mes = JsonHelper.JsonToObject<SocketMessage>(bodyStr);
           
            if (msgPayDelegate != null)
                msgPayDelegate(mes.msgContent);
        }
        if(msgType["msgType"].ToString() == "5")//登出
        {
            // useless at present
        }
        if(msgType["msgType"].ToString() == "9")//通用数据
        {
            // useless at present
        }

        stopwatch.Stop(); //  停止监视
        TimeSpan timespan = stopwatch.Elapsed; //  获取当前实例测量得出的总时间
        double milliseconds = timespan.TotalMilliseconds;  //  总毫秒数
        Debug.Log("Parese body total time : " + milliseconds);
    }

    public override void sendMessage(string message)
    {
        if (serverSocket == null)
            return;
        try
        {
            string cmd = message + TCPEndNote;//添加终止符
            Debug.LogWarning("Senddata to tcpServer:" + message);
            var sendData = new byte[bufferLength];
            sendData = Encoding.ASCII.GetBytes(cmd);
            lock (this)
            {
                try
                {
                    serverSocket.Send(sendData, sendData.Length, SocketFlags.None);
                }
                catch (SocketException e)
                {
                    if (!e.SocketErrorCode.Equals(10035))
                        Reconnect();
                    //serverSocket.Connect(ipPoint);

                }

            }
        }
        catch(Exception ex)
        {
            Debug.LogError("Senddata to tcpServer:"+ex.Message+" ,with" + message);
        }

    }

    private void Reconnect()
    {
        Debug.LogWarning("failure send occured with :"+ ip + ":" + port);
        Debug.LogWarning("reconnect start");
        isColsedRcv = true;
        serverSocket.Close();
        serverSocket = null;

        ipPoint = new IPEndPoint(IPAddress.Parse(ip), port);
        serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        try
        {
            serverSocket.Connect(ipPoint);

            if (userId != null)
                login(userId);

            Task.Run(() => {
                Thread.Sleep(500);
                SocketReceive();
            });
        }
        catch (Exception ex)
        {
            Debug.LogWarning("failure of tcpclient reconnected to :"
                + ip + ":" + port + " with exception:" + ex.Message);
        }
    }

    private void OnApplicationQuit()
    {
        isClosed = true;
    }
}
