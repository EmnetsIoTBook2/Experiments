#include <RH_RF95.h>
#define MAX_MESSAGE_LEN 64
#define FLAG_ACK 1
RH_RF95 rf95;

// Fills your params here
const int N = 4;/*需要填写发送次数*/
const uint8_t SERVER_ADDRESS =173 ;/*需要填写接收端节点地址*/
// ========== End ===========

uint8_t last_id = 100;

bool reliable_recv_from(uint8_t* buf, uint8_t* len, uint8_t* from, uint8_t* to, uint8_t* id, uint8_t* flags)
{
   if (rf95.recv(buf, len)) {
    *from = rf95.headerFrom();
    *to = rf95.headerTo();
    *id = rf95.headerId();
    *flags = rf95.headerId();
    if (*to == SERVER_ADDRESS) {
      rf95.setHeaderTo(*from);
      rf95.setHeaderFrom(*to);
      rf95.setHeaderId(*id);
      rf95.setHeaderFlags(FLAG_ACK);
      uint8_t ack_data[] = "!";
      rf95.send(ack_data, 1);
      rf95.waitPacketSent();
    }
    if (*id != last_id && *to == SERVER_ADDRESS){
      return true;
    }
  }
  return false;
}

void setup()
{
  uint8_t from = 0;
  uint8_t to = 0;
  uint8_t id = 0;
  uint8_t flags = 0;
  uint8_t buf[MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.begin(9600);
  if (!rf95.init()) {
    Serial.println("init failed");
  } else{
    // Fills your params here(optional)
    // ========== End ===========
    Serial.println("Server Start");
    rf95.setTxPower(23, false);
    rf95.setThisAddress(SERVER_ADDRESS);
  
    int count = 0;
    while (count < N) {
      if (rf95.available()) {
        // Should be a message for us now
        if (reliable_recv_from(buf, &len, &from, &to, &id, &flags)) {
          last_id = id;
          buf[len] = '\0';
          Serial.print("Receiving from client:");
          Serial.println((char*)buf);
          count++;
        } else {
          Serial.println("recv failed");
        }
      }
    }
    Serial.println("end");
  }
  
}

void loop()
{

}
