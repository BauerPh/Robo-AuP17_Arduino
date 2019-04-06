#include "global.h"

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------
void _sendNewLine() {
    Serial.print(F("\n"));
}

void _rcvCON() {
    sendACK();
    sendESS();
    sendLSS();
    sendPOS();
}

bool _parseMsg(String &msg, struct structMsgData &msgData) {

    msg.trim();
    msg.toLowerCase();

    _clearMsgData();

    //Länge prüfen
    if (msg.length() < 3) return false;
    //auszuführende Funktion / Aktion
    msgData.func = msg.substring(0, 3);
    //'#' - Zeichen prüfen
    if (msg[3] != '#') return true; //keine Parameter
    String tmMsg = msg.substring(4);

    uint16_t iParSet = 0;
    uint16_t iPar = 0;
    String tmpValue = "";
    //Alle Zeichen durchlaufen
    for (uint16_t i = 0; i < tmMsg.length(); i++) {
        char c = tmMsg[i];
        if (c == '#' || c == ',') {
            //Wert speichern
            msgData.parSet[iParSet][iPar] = tmpValue.toInt();
            tmpValue = "";
            //Zähler anpassen
            if (c == '#') {
                iParSet++;
                iPar = 0;
            } else iPar++;
        } else {
            //Prüfen ob Zahl
            if (c < '0' || c > '9') return false;
            tmpValue += c;
        }
    }
    //Letzten Wert speichern
    msgData.parSet[iParSet][iPar] = tmpValue.toInt();

    msgData.cnt = iParSet + 1;
    tmMsg = "";

    return true;
}

void _clearMsgData() {
    MsgData.cnt = 0;
    MsgData.func = "";
    memset(MsgData.parSet, 0, sizeof(MsgData.parSet));
}

void _rcvMsg() {
    // wait for serial message
    static String Msg = "";
    static bool MsgIn = false;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '<') {
            MsgIn = true;
            Msg = "";
        } else if (c == '>') {
            MsgIn = false;

            //parse Msg
            if (!_parseMsg(Msg, MsgData)) sendERR(2);
            Msg = "";

            //CON
            //***************
            if (MsgData.func == "con") _rcvCON();
            //MOV
            //***************
            else if (MsgData.func == "mov") roboMOV();
            //REF
            //***************
            else if (MsgData.func == "ref") roboREF();
            //SRV
            //***************
            else if (MsgData.func == "srv") roboSRV();
            //WAI
            //***************
            else if (MsgData.func == "wai") roboWAI();
            //NO FUNCTION
            //***************
            else sendERR(1);
        } else if (MsgIn) Msg += c;
    }
}

// -----------------------------------------------------------------------------
// Public
// -----------------------------------------------------------------------------
structMsgData MsgData;

void sendACK() {
    Serial.print(F("ack"));
    _sendNewLine();
}

void sendFIN() {
    Serial.print(F("fin"));
    _sendNewLine();
}

void sendERR(uint8_t errnum) {
    Serial.print(String(F("err#")) + String(errnum));
    _sendNewLine();
}

void sendPOS() {
    Serial.print(F("pos"));
    for (uint8_t i = 0; i < 6; i++) {
        Serial.printf(PSTR("#%u,%u,%d"), i+1, roboGetRefOkay(i) ? 1 : 0, roboGetStepperPos(i));
    }
    _sendNewLine();
}

void sendESS() {
    Serial.printf(PSTR("ess#%u"), roboGetEstop() ? 0 : 1);
    _sendNewLine();
}

void sendLSS() {
    Serial.print(F("lss"));
    for (uint8_t i = 0; i < 6; i++) {
        Serial.printf(PSTR("#%u,%u"), i+1, roboGetLimitSwitch(i) ? 1 : 0);
    }
    _sendNewLine();
}

void sendRES() {
    Serial.print(F("res"));
    _sendNewLine();
}

// -----------------------------------------------------------------------------
// Loop
// -----------------------------------------------------------------------------
void comLoop() {
    // Check Serial Data
    _rcvMsg();
}
// -----------------------------------------------------------------------------
// Konfig
// -----------------------------------------------------------------------------
void comSetup() {
    Serial.begin(BAUDRATE);
}