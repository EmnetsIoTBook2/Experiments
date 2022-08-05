import paho.mqtt.client as mqtt
import time
import random
import json


host = "localhost"
port = 1883
collectCycle = 10
deviceName = "mqtt_smart_lightsensor"
client_id = deviceName
topic = deviceName+"/data/post"
password = ""

mqttClient = mqtt.Client()

def connectBroker(host,port):
    try:
        mqttClient.connect(host, port, 60)
    except Exception as e:
        print(e)
        print("Waiting for broker initialization")
        time.sleep(2)
        connectBroker(host,port)


if __name__ == "__main__":

    def on_connect(client, userdata, flags, rc): 
        if(rc ==0):
            print ("Connection returned success")
        else:
            print ("Connection returned failed")

    mqttClient.username_pw_set(deviceName, password) 
    connectBroker(host,port)
    mqttClient.on_connect = on_connect
    mqttClient.loop_start()
   
    while 1:
        nowTime = time.time()
        message ={
            "deviceName":deviceName,
            "time":str(nowTime),
            "data":{
                "light":random.uniform(100,500)
            }
        }
        print("Publish new message, topic: "+topic+" , msg: "+str(message)+" , timestamp:"+str(nowTime))
        mqttClient.publish(topic,payload = json.dumps(message),qos = 0)
        time.sleep(collectCycle)