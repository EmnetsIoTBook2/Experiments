import grovepi
import math
import time
sensor = 2                                        # DHT11���ݽӿںͿ�����2����������
blue = 0
while True:
  try:
    [temp,humidity] = grovepi.dht(sensor,blue)    # ��ȡ��ʪ��
    if math.isnan(temp)==False and math.isnan(humidity)==False:
      print("temperature = %.02f C\nhumidity =%.02f%%"%(temp, humidity))
      time.sleep(3)                               # ÿ���3���ӡһ�λ�������
  except IOError:
    print ("Error")
