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

void _parseMsg(String &_msg, struct structMsgData &_msgData) {

    _msgData.cnt = 0;

    _msg.trim();
    _msg.toLowerCase();
    
    //Länge prüfen
    if (_msg.length() < 3) return;
    //auszuführende Funktion / Aktion
    _msgData.func = _msg.substring(0, 3);
    //Länge prüfen
    if (_msg.length() < 4) return;
    String tmpCommand = _msg.substring(4);
    //Parametersatz splitten
    uint16_t iParSet = 0;
    uint16_t lastParSetIndex = 0;
    while (tmpCommand.indexOf('#', lastParSetIndex) > 0) { //es gibt noch ein '#' Zeichen!
        uint16_t ParSetIndex = tmpCommand.indexOf('#', lastParSetIndex);

        String tmpParSet = tmpCommand.substring(lastParSetIndex, ParSetIndex);
        //Parameter splitten
        uint16_t iPar = 0;
        uint16_t lastParIndex = 0;
        while (tmpParSet.indexOf(',', lastParIndex) > 0) { //es gibt noch ein ',' Zeichen
            uint16_t ParIndex = tmpParSet.indexOf(',', lastParIndex);
            _msgData.parSet[iParSet][iPar] = tmpParSet.substring(lastParIndex, ParIndex).toInt();
            iPar++;
            lastParIndex = ParIndex + 1;
        }
        //letzten Parameter berücksichtigen
        _msgData.parSet[iParSet][iPar] = tmpParSet.substring(lastParIndex).toInt();
        //restliche Parameter mit 0 befüllen
        for (uint16_t i = iPar + 1; i < 8; i++) _msgData.parSet[iParSet][i] = 0;

        iParSet++;
        lastParSetIndex = ParSetIndex + 1;
        tmpParSet = "";
    }

    //letzten Parametersatz berücksichtigen, wenn es einen gibt!
    if (tmpCommand.length() > 0) {
        String tmpParSet = tmpCommand.substring(lastParSetIndex);
        //Split parameter
        uint16_t iPar = 0;
        uint16_t lastParIndex = 0;
        while (tmpParSet.indexOf(',', lastParIndex) > 0) { //es gibt noch ein ',' Zeichen
            uint16_t ParIndex = tmpParSet.indexOf(',', lastParIndex);
            _msgData.parSet[iParSet][iPar] = tmpParSet.substring(lastParIndex, ParIndex).toInt();
            iPar++;
            lastParIndex = ParIndex + 1;
        }
        //letzten Parameter berücksichtigen
        _msgData.parSet[iParSet][iPar] = tmpParSet.substring(lastParIndex).toInt();
        //restliche Parameter mit 0 befüllen
        for (uint16_t i = iPar + 1; i < 8; i++) _msgData.parSet[iParSet][i] = 0;

        iParSet++;
    }
    _msgData.cnt = iParSet;
    tmpCommand = "";
}

void _rcvMsg() {
    // wait for serial message
    static String Msg = "";
    static bool MsgIn = false;

    while (Serial.available() > 0)
    {
        char c = Serial.read();
        if (c == '<') {
            MsgIn = true;
            Msg = "";
        }
        else if (c == '>')
        {
            MsgIn = false;

            //parse Msg
            _parseMsg(Msg, MsgData);
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
        }
        else if (MsgIn) Msg += c;
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
        //Serial.print(String (F("#")) + String(i+1) + String(roboGetRefOkay(i) ? F(",1,") : F(",0,")) + String(roboGetStepperPos(i)));
    }
    _sendNewLine();
}

void sendESS() {
    Serial.printf(PSTR("ess#%u"), roboGetEstop() ? 0 : 1);
    //Serial.print(String(F("ess#")) + String(roboGetEstop() ? F("0") : F("1")));
    _sendNewLine();
}

void sendLSS() {
    Serial.print(F("lss"));
    for (uint8_t i = 0; i < 6; i++) {
        Serial.printf(PSTR("#%u,%u"), i+1, roboGetLimitSwitch(i) ? 1 : 0);
        //Serial.print(String(F("#")) + String(i+1) + String(F(",")) + String(roboGetLimitSwitch(i) ? F("1") : F("0")));
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