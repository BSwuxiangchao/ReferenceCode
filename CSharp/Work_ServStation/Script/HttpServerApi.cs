using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using System;
using System.Runtime.Serialization.Json;
using System.IO;
using System.Runtime.Serialization;

public class HttpServerApi : NetBase
{

    #region HttpServer Start and process

    static HttpListener httpListener = null;
    public string url = "http://127.0.0.1:8086/";
    // Start is called before the first frame update
    void Start()
    {
        if (httpListener == null)
        {
            httpListener = new HttpListener();
        }

        httpListener.AuthenticationSchemes = AuthenticationSchemes.Anonymous;
        httpListener.Prefixes.Add(url);

        if (!httpListener.IsListening)
        {
            Task.Factory.StartNew(() => {
                try
                {
                    httpListener.Start();
                    while (true)
                    {
                        loop(httpListener);
                    }
                }
                catch (Exception ex)
                {
                    Debug.LogError("Exception occured at HttpServer create " + url + ex.Message);
                    httpListener.Stop();
                }
            });
        }
    }

    // // Update is called once per frame
    // void Update()
    // {
    //     
    // }


    private void loop(HttpListener httpListener)
    {
        HttpListenerContext context = httpListener.GetContext();
        HttpListenerRequest hRequest = context.Request;
        HttpListenerResponse hResponse = context.Response;
        string answer = "";
        Result re = new Result();
        if (hRequest.HttpMethod == "POST")
        {
            Debug.Log("POST:" + hRequest.Url + " ->message:" + hRequest.ToString());
            pareseRequestBody(hRequest);
            re.isOK = true;

        }
        else if (hRequest.HttpMethod == "GET")
        {
            Debug.Log("GET:" + hRequest.Url + " ->message:" + hRequest.ToString());
            pareseRequestBody(hRequest);
            re.isOK = true;
        }
        answer = JsonHelper.ObjectToJSON<Result>(re);
        byte[] res = Encoding.UTF8.GetBytes(answer);

        using (StreamWriter writer = new StreamWriter(hResponse.OutputStream, Encoding.UTF8))
        {
            writer.Write(answer);
            writer.Close();
            hResponse.Close();
        }

    }

    private void pareseRequestBody(HttpListenerRequest hRequest)
    {
        if (!hRequest.HasEntityBody)
            return;

        using (System.IO.Stream body = hRequest.InputStream)
        {
            using (System.IO.StreamReader reader = new System.IO.StreamReader(body, hRequest.ContentEncoding))
            {
                string bodyStr = reader.ReadToEnd();
                processRequestBody(bodyStr);
            }
        }
    }

    #endregion

}
