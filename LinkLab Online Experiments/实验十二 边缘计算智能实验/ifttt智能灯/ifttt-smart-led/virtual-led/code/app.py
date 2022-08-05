import paho.mqtt.client as mqtt
import time
import json

host = "localhost"
port = 1883
deviceName = "smart-led"
client_id = deviceName
password = ""
dataTopic = deviceName+"/data/post"
serviceTopic = deviceName+"/service/set"
led = 0

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
    if "service" in message:
        service = message["service"]
        if service == "turnOnLed":               
            led = 1
            print("Led turnOn success")
            message ={
                "deviceName":deviceName,
                "data":{
                    "led":led,
                }
            }
            client.publish(dataTopic,payload = json.dumps(message),qos = 0)
        elif service == "turnOffLed":
            led = 0
            print("Led turnOff success")
            message ={
                "deviceName":deviceName,
                "data":{
                    "led":led,
                }
            }
            client.publish(dataTopic,payload = json.dumps(message),qos = 0)
        else:
            print("Undefined service:"+service)

def on_subscribe():
    client.subscribe(serviceTopic, 1)
    client.on_message = on_message_come 

client = mqtt.Client()
client.username_pw_set(deviceName, password) 
connectBroker(host, port)
client.on_connect = on_connect  
client.loop_start()
on_subscribe()
message ={
    "deviceName":deviceName,
    "data":{
        "led":led,
    }
}
client.publish(dataTopic,payload = json.dumps(message),qos = 0)

while 1:
    pass