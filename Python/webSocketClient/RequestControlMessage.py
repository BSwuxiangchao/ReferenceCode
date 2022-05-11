import json
import secrets
import os
import time
# import Config
import WebSocketClient as WSClient


def local_post(body):
    port = 9092
    apiurl = ('127.0.0.1', port)

    # apiurl = "http://192.168.50.124:6020/octv-unmanned-store/a/store"
    try:
        # r = requests.post(url=apiurl, params=body, headers={'Content-Type':'application/json'})
        r = WSClient.postCustomerVoice(body)
        return True
    except IOError:
        print('总控服务请求失败')
        return False


def post_bot_response(message, wavPath=""):
    # body = {"aiSpeak": message}
    textDic = {"word": message, "command": "aiSpeak"}

    WSClient.postVtuberVoice(textDic, wavPath)
    return


# 发送打招呼请求
def post_greetsombody():
    textDic = {"word": "哈喽!", "command": "greetsombody"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送打开互动请求
def post_askServers():
    textDic = {"word": "服务", "command": "askServers"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送购物请求
def post_askShopping():
    textDic = {"word": "购物", "command": "askShopping"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送购买食品请求
def post_askFood():
    textDic = {"word": "食物", "command": "askFood"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送购买饮料请求
def post_askDrinke():
    textDic = {"word": "饮料", "command": "askDrink"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送展示商品请求
def post_askProduct(num=None):
    textDic = {"word": str(num), "command": "askProduct"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送继续购物还是互动请求
def post_askContinue():
    textDic = {"word": "是否继续", "command": "askContinue"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送继续购物请求
def post_Continue():
    textDic = {"word": "继续", "command": "continue"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送取消，放弃
def post_Break():
    textDic = {"word": "否继续", "command": "break"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送看结束购物请求
def post_toMain():
    textDic = {"word": "不继续", "command": "toMain"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送互动请求
def post_chatOrGame():
    textDic = {"word": "趣味互动", "command": "chatOrGame"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送互动长聊天1请求
def post_chatLong1():
    textDic = {"word": "互动长聊天1", "command": "chatLong1"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送互动长聊天2请求
def post_chatLong2():
    textDic = {"word": "互动长聊天2", "command": "chatLong2"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


# 发送玩游戏请求
def post_GAME():
    textDic = {"word": "GAME", "command": "GamePlay"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

#发送游戏1
def post_GAME1():
    textDic = {"word": "GAME1", "command": "Game1"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

#发送游戏1
def post_GAME2():
    textDic = {"word": "GAME2", "command": "Game2"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送直播或短视频请求
def post_ZhiBoVideo():
    textDic = {"word": "直播或短视频", "command": "ZhiBoVideo"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送直播请求
def post_ZhiBo():
    textDic = {"word": "直播", "command": "ZhiBo"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送直播请求
def post_ShortVideo():
    textDic = {"word": "短视频", "command": "ShortVideo"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return

# 发送互动短聊天请求
def post_chatShort():
    textDic = {"word": "互动短聊天", "command": "chatShort"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


def post_start_record():
    # body = {"startRecord": '1'}
    textDic = {"word": "", "command": "startRecord"}
    WSClient.postVtuberVoice(textDic, voicePath="")
    return


def post_stop_record():
    # body = {"stopRecord": '1'}
    textDic = {"word": "", "command": "stopRecord"}
    return local_post(textDic)


def post_luckyDraw():
    # body = {"luckyDraw": '1',"type": "1"}
    textDic = {"word": "", "command": "luckyDraw"}
    return local_post(textDic)


def post_luckyDrawStop():
    # body = {"luckyDrawStop": '1',"delay": "2.0"}
    textDic = {"word": "", "command": "luckyDraw"}
    return local_post(textDic)

if __name__ == "__main__":
    #post_greetsombody()
    #post_askServers()
    #post_askShopping()
    #post_askProduct("3")
    #post_Continue()
    #post_Break()


    post_chatOrGame()
    #post_chatLong1()
    #post_chatLong2()
    #post_chatShort()

    #post_ZhiBoVideo()
    #post_ZhiBo()
    #post_ShortVideo()
    #post_GAME()
    #post_GAME1()
    #post_GAME2()