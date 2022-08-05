# -*- coding: utf-8 -*-
import time
import RPi.GPIO
from azure.iot.device import IoTHubDeviceClient #引入Azure IoT SDK
GPIO.setmode(GPIO.BOARD)
GPIO.setup(11,GPIO.OUT)  #设置灯泡所接管脚

CONNECTION_STRING = "{deviceConnectionString}"

def message_handler(message):  #处理指令
    size = len(message.data)
    msg = message.data[:size].decode(‘utf-8’)
    if msg == “open”:
         GPIO.output(11,GPIO.HIGH)  #开灯
    elif msg == “close”:
        GPIO.output(11,GPIO.LOW)  #关灯
    else:
        print("Do nothing!")

def main():  #接收云到设备消息
    client = IoTHubDeviceClient.create_from_connection_string (CONNECTION_STRING)
    try:
         client.on_message_received = message_handler
            while True:
               time.sleep(1000)
    except KeyboardInterrupt:
        print(“stopped”)
    finally:
       client.shutdown()
	    
if __name__ == '__main__':
    main()
