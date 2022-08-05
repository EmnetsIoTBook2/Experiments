#include <RH_RF95.h>
const uint16_t DATA_LENGTH = 8;
const uint8_t FLAG_ACK = 1;
const uint8_t FLAG_DATA = 2;
const uint16_t MAX_MESSAGE_LEN = 32;
bool recv_ack = false;
uint8_t headerId = 0;
RH_RF95 rf95;

// Fills your params here
const int N =4 ;/*需要填写发送次数*/
const uint8_t CLIENT_ADDRESS = 172;/*需要填写发送端节点地址*/
const uint8_t SERVER_ADDRESS = 173;/*需要填写接收端节点地址*/
const char data[DATA_LENGTH+1] = "5b503182";/*需要填写发送内容*/
// ========== End ===========

bool reliable_send_to(uint8_t* buf, uint8_t len, uint8_t address, uint8_t max_retry)
{
  // Fills your params here
   uint8_t retry_count = 0;
  uint8_t buf_recv[MAX_MESSAGE_LEN];
  uint8_t recv_len = sizeof(buf_recv);
  recv_ack = false;
  while (retry_count < max_retry) {
    retry_count ++;
    rf95.send(buf, len);
    rf95.waitPacketSent();
    unsigned long start_time = millis();
    while (millis() - start_time < 2000) {
      if (rf95.available()) {
        if (rf95.recv(buf_recv, &recv_len)) {
          if (rf95.headerTo() == CLIENT_ADDRESS &&
            rf95.headerFrom() == SERVER_ADDRESS &&
            rf95.headerFlags() == FLAG_ACK &&
            rf95.headerId() == headerId) {
              recv_ack = true;
              return true;
          }
        }
      }
    }
  }
  return recv_ack;
  // ========== End ===========
}

void setup()
{
  Serial.begin(9600);
  if (!rf95.init()) {
    Serial.println("init failed");
  } else{
    // Fills your params here(optional)
    // ========== End ===========
    Serial.println("Start sending to server");
    rf95.setThisAddress(CLIENT_ADDRESS);
    rf95.setTxPower(23, false);

    rf95.setHeaderTo(SERVER_ADDRESS);
    rf95.setHeaderFrom(CLIENT_ADDRESS);
    rf95.setHeaderId(headerId);
    rf95.setHeaderFlags(FLAG_DATA);

    int max_retry = 10;
    for (int i = 0; i < N; i++) {
      uint8_t send_len = DATA_LENGTH;
    
      rf95.setHeaderId(headerId);
      reliable_send_to((uint8_t*)data, send_len, SERVER_ADDRESS, max_retry);
      Serial.print("Sending to server:");
      Serial.println(data);
      headerId++;
      delay(1000);
    }
    Serial.println("end");
  }
}

void loop()
{
}
