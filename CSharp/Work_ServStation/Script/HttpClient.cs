using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Net;
using System.Text;
using System;
using Newtonsoft.Json;
using System.Linq;
using Newtonsoft.Json.Linq;

public class HttpClient : NetBase
{
    // Start is called before the first frame update
    string urlEquipGoods = "https://workstation.octvisionland.com/octv-work/api/client/getEquipGoods?equipNo=";
    string urlQRCode = "https://workstation.octvisionland.com/octv-work/api/client/getGoodQrcode?egrId=";
    void Start()
    {
        
    }

    public JObject getEquipGoods(string equipNo)
    {
        try
        {
            string url = urlEquipGoods + equipNo;
            HttpWebRequest quest = (HttpWebRequest)WebRequest.Create(url);
            quest.Method = "GET";
            quest.ContentType = "application/json";
            HttpWebResponse response = (HttpWebResponse)quest.GetResponse();
            Stream stream = response.GetResponseStream();
            StreamReader streamReader = new StreamReader(stream);
            string bodyStr = streamReader.ReadToEnd();
            //EquipInfo info = JsonHelper.JsonToObject<EquipInfo>(bodyStr);
            //EquipInfo info = JsonConvert.DeserializeObject<EquipInfo>(bodyStr);
            var jsObj = (JObject)JsonConvert.DeserializeObject(bodyStr);
            streamReader.Close();
            stream.Close();
            response.Close();
            return jsObj;
        }
        catch(Exception ex)
        {
            Debug.LogError("Httpclient Erro:" + ex.Message);
            return null;
        }

        
    }

    public QRCodeInfo getQRCode(string egrId)
    {
        try
        {
            HttpWebRequest quest = (HttpWebRequest)WebRequest.Create(urlQRCode+ egrId);
            quest.Method = "GET";
            quest.ContentType = "application/json";
            HttpWebResponse response = (HttpWebResponse)quest.GetResponse();
            Stream stream = response.GetResponseStream();
            StreamReader streamReader = new StreamReader(stream);
            string bodyStr = streamReader.ReadToEnd();
            QRCodeInfo info = JsonHelper.JsonToObject<QRCodeInfo>(bodyStr);
            streamReader.Close();
            stream.Close();
            response.Close();
            return info;
        }
        catch (Exception ex)
        {
            Debug.LogError("Httpclient Erro:" + ex.Message);
            return null;
        }
    }

}
