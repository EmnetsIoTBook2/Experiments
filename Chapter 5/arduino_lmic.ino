/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *H
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the (early prototype version of) The Things Network.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1,
 *  0.1% in g2).
 *
 * Change DEVADDR to a unique address!
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h.
 *
 *
 * Required Library: 
 *    * https://github.com/matthijskooijman/arduino-lmic 
 * 
 * Require Hardware:
 *    * LoRa Shield + Arduino
 *    * LoRa GPS Shield + Arduino 
 *    * LoRa Mini etc. 
 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
//ttn
//static const PROGMEM u1_t NWKSKEY[16] = { 0xF8, 0xE8, 0x57, 0x5F, 0x63, 0x7D, 0xFF, 0x2D, 0x1E, 0xB6, 0x40, 0x77, 0x8F, 0xD7, 0xAF, 0x8E };
//static const PROGMEM u1_t NWKSKEY[16] = { 0x70, 0x11, 0x23, 0x59, 0xF7, 0xE2, 0x91, 0x47, 0xC9, 0x07, 0xF2, 0x95, 0x3B, 0x8F, 0x49, 0xBB };
//55 28 6D 86 CB E7 9C 5D B6 F0 DE A9 CF 7B BB 6E
static const PROGMEM u1_t NWKSKEY[16] = { 0x1E, 0x58, 0x0B, 0x97, 0x3B, 0x2E, 0xF2, 0x14, 0x89, 0xBE, 0xA4, 0x2D, 0xA9, 0x61, 0x8D, 0xD3 };
// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
//ttn
static const u1_t PROGMEM APPSKEY[16] = { 0x4B, 0x9B, 0x0E, 0x45, 0xD2, 0x64, 0xDB, 0xCC, 0x0A, 0xD8, 0xEE, 0xD9, 0xB3, 0x4A, 0x48, 0x59 };
//static const u1_t PROGMEM APPSKEY[16] = { 0x00, 0x37, 0x1D, 0xDB, 0x47, 0x6B, 0x69, 0x03, 0x09, 0xAC, 0x86, 0xD5, 0x8B, 0x26, 0x45, 0x37 };
//static const u1_t PROGMEM APPSKEY[16] = { 0x17, 0x48, 0x32, 0x98, 0xF9, 0xFC, 0x25, 0x3E, 0x22, 0xCE, 0xD0, 0x2A, 0x00, 0x65, 0x61, 0xA1 };
//
// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// ttn
static const u4_t DEVADDR = 0x260DD921;
//static const u4_t DEVADDR = 0x26011D97;
//static const u4_t DEVADDR = 0x26011467;


// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

// static uint8_t mydata[] = "Hello, world!";
static uint8_t mydata[] = "abcdefg!";
static osjob_t initjob,sendjob,blinkjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20; //duty cycle 设置,即发送间隔

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 6, 7},
};
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("OP_TXRXPEND, not sending");
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial.println("Packet queued");
        Serial.println(LMIC.freq);
//        Serial.println("Hello xwf");
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    Serial.println(ev);
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println("EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
            Serial.println("EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
            Serial.println("EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
            Serial.println("EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
            Serial.println("EV_JOINING");
            break;
        case EV_JOINED:
            Serial.println("EV_JOINED");
            break;
        case EV_RFU1:
            Serial.println("EV_RFU1");
            break;
        case EV_JOIN_FAILED:
            Serial.println("EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            Serial.println("EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
            Serial.println("EV_TXCOMPLETE (includes waiting for RX windows)");
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print("Data Received: ");
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send); //考虑了发送间隔时间
            break;
        case EV_LOST_TSYNC:
            Serial.println("EV_LOST_TSYNC");
            break;
        case EV_RESET:
            Serial.println("EV_RESET");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println("EV_RXCOMPLETE");
            break;
        case EV_LINK_DEAD:
            Serial.println("EV_LINK_DEAD");
            break;
        case EV_LINK_ALIVE:
            Serial.println("EV_LINK_ALIVE");
            break;
         default:
            Serial.println("Unknown event");
            break;
    }
}

void setup() {
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Starting");
    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    //LMIC_setClockError(MAX_CLOCK_ERROR * 1/100);
    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly 
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif
    
    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;
    
    // Set data rate and transmit power (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);
    LMIC.freq = 486300000;  // Tx frequency setting
    // Start job
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
