import paho.mqtt.client as mqtt
import time
import json

host = "localhost"
port = 1883
deviceName = "ifttt"
client_id = deviceName
password = ""
dataTopic = "mqtt_smart_lightsensor/data/post"
serviceTopic = "smart-led/service/set"
client = mqtt.Client()

def connectBroker(host,port):
    try:
        client.connect(host, port, 60)
    except Exception as e:
        print(e)
        print("Waiting for broker initialization")
        time.sleep(2)
        connectBroker(host,port)

def on_connect(client, userdata, flags, rc): 
    if(rc ==0):
        print ("Connection returned success")
    else:
        print ("Connection returned failed")

def on_message_come(client, userdata, msg):
    print("New message come: "+ str(msg.payload)+ " , timestamp:"+str(time.time()))
    #print(msg.topic + " " + ":" + str(msg.payload))
    message = json.loads(msg.payload)
    if "data" in message:
        data = message["data"]
        if "light" in data:
            light = data["light"]
            if light>400:
                message = {"service":"turnOnLed"}
                client.publish(serviceTopic,payload = json.dumps(message),qos = 0)
            elif light<200:
                message = {"service":"turnOffLed"}
                client.publish(serviceTopic,payload = json.dumps(message),qos = 0)

def on_subscribe():
    client.subscribe(dataTopic, 1)
    client.on_message = on_message_come 

client = mqtt.Client()
client.username_pw_set(deviceName, password) 
connectBroker(host, port)
client.on_connect = on_connect  
client.loop_start()
on_subscribe()

while 1:
    pass