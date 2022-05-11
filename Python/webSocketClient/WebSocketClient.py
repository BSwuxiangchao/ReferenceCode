import json
import time
import threading
import pubsub
import websocket
from gevent import socket
#from pubsub import pubnishi
from websocket import create_connection
from typing import Callable, Any

UserListener = Callable[..., Any]

address = "ws://127.0.0.1:9092/"


class WsClient:
    def __init__(self, address):
        self.ws = websocket.create_connection(address)
        self.receiveChannel = "Recive"
        #self.lock = threading.Lock()

    def send(self, params):
        print("Sending ...")
        self.ws.send(json.dumps(params))
        print("Sending Data:{}".format(params))
        #result = self.ws.recv()
        #print("Received '{}'".format(result))
        #print(result)
        return

    def subscribeReceiveHandle(self, listener: UserListener):
        topicName = self.receiveChannel
        print("sub channel", topicName)
        pubsub.pub.subscribe(listener, topicName)

    def startListen(self):
        t1 = threading.Thread(target=self.listen)
        t1.start()

    def listen(self):
        print("listening start ...")
        while True:
            try:
                result = self.ws.recv()
                print("Received '{}'".format(result))
                # if result:
                pubsub.pub.sendMessage(self.receiveChannel, cmd=result)
            except:
                #print("Excetion throw while websocket listening ")
                continue

    def pubMes(self, cmd):
        pubsub.pub.sendMessage(self.receiveChannel, cmd=cmd)

    def quit(self):
        self.ws.close()


web_client = WsClient(address)
web_client.startListen()

def postTestMsg(arg1=None):

    try:
        # rep = conn.post(address, data=postData, headers=headers, timeout=3.0)
        #web_client.lock.acquire()
        web_client.send(arg1)
        #web_client.lock.release()
        # print(rep.text)
    except:
        print('WebSocketClient connect erro')


def postFaceData(arg1=None):
    # print("post faceData = ", arg1.toDic())
    # face = arg1.toDic()
    face = arg1
    timeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    postDataDic = {
        "IMG_ATTR": "faceAttr",
        "FACE_DATA": face,
        "TIME": timeStr,
    }
    print(postDataDic)
    # postData = json.dumps(postDataDic)
    try:
        # rep = conn.post(address, data=postData, headers=headers, timeout=3.0)
        #web_client.lock.acquire()
        web_client.send(postDataDic)
        #web_client.lock.release()
        # print(rep.text)
    except:
        print('WebSocketClient connect erro')


def postVtuberVoice(textDic, voicePath, voiceStream=None):
    streamList = None
    if voiceStream != None:
        streamList = voiceStream.tolist()
    timeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    voicedata = {
        "TEXT": textDic,  # leave or comein
        "VOICE_PATH": voicePath,
        "VOICE_STRAM": streamList,
    }
    postDataDic = {
        "VOICE_ATTR": "vtuberAttr",
        "VOICE_DATA": voicedata,
        "TIME": timeStr,
    }
    print(postDataDic)
    # postData = json.dumps(postDataDic)
    try:
        # rep = conn.post(url, data=postData, headers=headers, timeout=3.0)
        #web_client.lock.acquire()
        web_client.send(postDataDic)
        #web_client.lock.release()
        # print(rep.text)
    except:
        print('websocketClient connect erro')
    return


def postCustomerVoice(textDic, voicePath=None, voiceStream=None):
    # print("post faceData = ", arg1.toDic())
    # face = arg1.toDic()
    # face = arg1
    # print("shape data = ", voiceStream.shape)
    streamList = None
    if voiceStream != None:
        streamList = voiceStream.tolist()
    timeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    voicedata = {
        "TEXT": textDic,  # leave or comein
        "VOICE_PATH": voicePath,
        "VOICE_STRAM": streamList,
    }
    postDataDic = {
        "VOICE_ATTR": "customerAttr",
        "VOICE_DATA": voicedata,
        "TIME": timeStr,
    }
    print(postDataDic)
    # postData = json.dumps(postDataDic)

    try:
        # rep = conn.post(url, data=postData, headers=headers, timeout=3.0)
        #web_client.lock.acquire()
        result = web_client.send(postDataDic)

        # print(rep.text)
    except:
        result = 0
        print('websocketClient connect erro')
    return result


def registerReceiveHandle(listener: UserListener):
    web_client.subscribeReceiveHandle(listener)


def listenCallback(cmd):
    mes = str(cmd)
    print('------{}------'.format(mes))


def init():
    textDic = {"word": "注册", "command": "login"}
    postCustomerVoice(textDic)

"""
if __name__ == "__main__":
    # post_info()
    # psot_info_3()
    # test_post()
    imagedata = {"Name": "ZhangSan", "age": 30, "gender": "male"
        , "emotion": "smile", "action": "toMain"}
    # postFaceData(imagedata)

    #textDic = {"word": "哈喽!", "command": "GamePlay"}
    textDic = {"word": "直播或视频", "command": "ZhiBoVideo"}
    wave_path = "E:/studySpace/VTuber-Python-Unity-main/Audio/2021_01_12_20_36_59.wav"
    # voice_data, sample_rate = sf.read(wave_path)
    # print("sample_rate = ", sample_rate)
    # postVtuberVoice(textDic, wave_path)

    postCustomerVoice(textDic)
"""
if __name__=="__main__":
    while 1:
        msg = input("Enter Msg:")
        postTestMsg(msg)

