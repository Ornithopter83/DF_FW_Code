// Dedicated PC fallback connection; all state is owned by the main loop.
#include "../DF_Main_Internal.h"
#include "DF_Main_BoardWifi.h"
#include <WiFi.h>
#include <Preferences.h>
#include <esp_system.h>
#include <lwip/sockets.h>
#include <errno.h>

enum { DF_Main_BoardWifi_Port = 45180, DF_Main_BoardWifi_Channel = 1,
       DF_Main_BoardWifi_LineCapacity = 160, DF_Main_BoardWifi_TxCapacity = 384 };
static WiFiServer DF_Main_BoardWifi_Server(DF_Main_BoardWifi_Port);
static WiFiClient DF_Main_BoardWifi_Client;
static String DF_Main_BoardWifi_Id;
static String DF_Main_BoardWifi_Ssid;
static String DF_Main_BoardWifi_Password;
static String DF_Main_BoardWifi_Token;
static String DF_Main_BoardWifi_Boot;
static bool DF_Main_BoardWifi_Started = false;
static bool DF_Main_BoardWifi_Authenticated = false;
static bool DF_Main_BoardWifi_Pending = false;
static bool DF_Main_BoardWifi_SerialRoute = false;
static unsigned long DF_Main_BoardWifi_Request = 0;
static unsigned long DF_Main_BoardWifi_LastRx = 0;
static char DF_Main_BoardWifi_Line[DF_Main_BoardWifi_LineCapacity];
static unsigned int DF_Main_BoardWifi_LineLength = 0;
static char DF_Main_BoardWifi_Tx[DF_Main_BoardWifi_TxCapacity];
static unsigned int DF_Main_BoardWifi_TxLength = 0;
static unsigned int DF_Main_BoardWifi_TxOffset = 0;

static String DF_Main_BoardWifi_RandomHex(unsigned int bytes)
{
    static const char digits[] = "0123456789abcdef";
    String value;
    value.reserve(bytes * 2);
    for (unsigned int i = 0; i < bytes; ++i) {
        uint8_t randomByte;
        esp_fill_random(&randomByte, sizeof(randomByte));
        value += digits[randomByte >> 4];
        value += digits[randomByte & 15];
    }
    return value;
}

static bool DF_Main_BoardWifi_IsHex(const String &value, unsigned int length)
{
    if (value.length() != length) return false;
    for (unsigned int i = 0; i < length; ++i)
        if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'f'))) return false;
    return true;
}

static bool DF_Main_BoardWifi_GameConnected()
{
    return g_SerialEnable && (AP_IS_DF == apType) && CheckAppCmd();
}

static void DF_Main_BoardWifi_CloseClient()
{
    DF_Main_BoardWifi_Client.stop();
    DF_Main_BoardWifi_Authenticated = false;
    DF_Main_BoardWifi_LineLength = 0;
    DF_Main_BoardWifi_TxLength = 0;
    DF_Main_BoardWifi_TxOffset = 0;
}

static void DF_Main_BoardWifi_Queue(const String &message)
{
    if (!DF_Main_BoardWifi_Client.connected()) return;
    const unsigned int length = message.length();
    if (DF_Main_BoardWifi_TxLength + length + 1 > DF_Main_BoardWifi_TxCapacity) {
        DF_Main_BoardWifi_CloseClient();
        return;
    }
    memcpy(DF_Main_BoardWifi_Tx + DF_Main_BoardWifi_TxLength, message.c_str(), length);
    DF_Main_BoardWifi_TxLength += length;
    DF_Main_BoardWifi_Tx[DF_Main_BoardWifi_TxLength++] = '\n';
}

static void DF_Main_BoardWifi_SendPending()
{
    if (DF_Main_BoardWifi_Pending && !DF_Main_BoardWifi_SerialRoute &&
        DF_Main_BoardWifi_Authenticated && ((PS_RUNNING == g_ProcStatus) || (PS_RSPWAIT == g_ProcStatus)))
        DF_Main_BoardWifi_Queue("SHUTDOWN " + DF_Main_BoardWifi_Boot + " " + String(DF_Main_BoardWifi_Request));
}

void DF_Main_BoardWifi_Begin()
{
    Preferences settings;
    if (!settings.begin("df-boardwifi", false)) {
        LogPrintln(" LG] Board WiFi settings unavailable");
        return;
    }
    DF_Main_BoardWifi_Id = settings.getString("id", "");
    DF_Main_BoardWifi_Password = settings.getString("password", "");
    DF_Main_BoardWifi_Token = settings.getString("token", "");
    if (!DF_Main_BoardWifi_IsHex(DF_Main_BoardWifi_Id, 16) ||
        !DF_Main_BoardWifi_IsHex(DF_Main_BoardWifi_Password, 32) ||
        !DF_Main_BoardWifi_IsHex(DF_Main_BoardWifi_Token, 64)) {
        DF_Main_BoardWifi_Id = DF_Main_BoardWifi_RandomHex(8);
        DF_Main_BoardWifi_Password = DF_Main_BoardWifi_RandomHex(16);
        DF_Main_BoardWifi_Token = DF_Main_BoardWifi_RandomHex(32);
        bool saved = settings.putString("id", DF_Main_BoardWifi_Id) > 0;
        saved = (settings.putString("password", DF_Main_BoardWifi_Password) > 0) && saved;
        saved = (settings.putString("token", DF_Main_BoardWifi_Token) > 0) && saved;
        if (!saved) {
            settings.end();
            LogPrintln(" LG] Board WiFi settings save failed");
            return;
        }
    }
    settings.end();
    DF_Main_BoardWifi_Boot = DF_Main_BoardWifi_RandomHex(8);
    DF_Main_BoardWifi_Ssid = "DF-Board-" + DF_Main_BoardWifi_Id;
    // The STA interface remains available to ESP-NOW; no external AP is joined.
    if (!WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0)) ||
        !WiFi.softAP(DF_Main_BoardWifi_Ssid.c_str(), DF_Main_BoardWifi_Password.c_str(),
                     DF_Main_BoardWifi_Channel, 0, 1)) {
        LogPrintln(" LG] Board WiFi AP start failed");
        return;
    }
    DF_Main_BoardWifi_Server.begin();
    DF_Main_BoardWifi_Server.setNoDelay(true);
    DF_Main_BoardWifi_Started = true;
    LogPrintln(" LG] Board WiFi AP/TCP ready");
}

bool DF_Main_BoardWifi_HandlePairCommand(const String &message)
{
    if (message != "$BWPAIR") return false;
    // This explicit USB-only enrollment command never enters normal command logs/counters.
    if (!DF_Main_BoardWifi_Started || CheckAppCmd() || DF_Main_BoardWifi_Pending ||
        ((PS_RUNNING != g_ProcStatus) && (PS_INITIAL != g_ProcStatus))) {
        Serial.println("$BWPAIR,BUSY%");
        return true;
    }
    Serial.print("$BWPAIR,"); Serial.print(DF_Main_BoardWifi_Ssid);
    Serial.print(","); Serial.print(DF_Main_BoardWifi_Password);
    Serial.print(","); Serial.print(DF_Main_BoardWifi_Token);
    Serial.print(","); Serial.print(DF_Main_BoardWifi_Id);
    Serial.println("%");
    return true;
}

static void DF_Main_BoardWifi_HandleLine(const String &line)
{
    if (!DF_Main_BoardWifi_Authenticated) {
        if (line != "HELLO 1 " + DF_Main_BoardWifi_Token) {
            DF_Main_BoardWifi_CloseClient();
            return;
        }
        DF_Main_BoardWifi_Authenticated = true;
        DF_Main_BoardWifi_Queue("READY 1 " + DF_Main_BoardWifi_Id + " " + DF_Main_BoardWifi_Boot);
        DF_Main_BoardWifi_SendPending();
    } else if (line.startsWith("PING ") && DF_Main_BoardWifi_IsHex(line.substring(5), 16)) {
        DF_Main_BoardWifi_Queue("PONG " + line.substring(5));
    } else if (!DF_Main_BoardWifi_SerialRoute &&
               (line == "ACCEPT " + DF_Main_BoardWifi_Boot + " " + String(DF_Main_BoardWifi_Request))) {
        if (DF_Main_BoardWifi_Pending && (PS_RSPWAIT == g_ProcStatus)) {
            recvPwrOffExec_Resp = 1;
            DF_Main_BoardWifi_Pending = false;
        }
        // Duplicate/late ACKs are harmless and never alter another power phase.
    } else if (!DF_Main_BoardWifi_SerialRoute &&
               (line == "DECLINE " + DF_Main_BoardWifi_Boot + " " + String(DF_Main_BoardWifi_Request))) {
        // No ACK on observation mode or failed Windows shutdown; keep existing wait.
    } else {
        DF_Main_BoardWifi_CloseClient();
        return;
    }
    DF_Main_BoardWifi_LastRx = millis();
}

void DF_Main_BoardWifi_Poll()
{
    if (!DF_Main_BoardWifi_Started) return;
    if (DF_Main_BoardWifi_Server.hasClient()) {
        WiFiClient incoming = DF_Main_BoardWifi_Server.available();
        if (DF_Main_BoardWifi_Client.connected()) incoming.stop();
        else {
            DF_Main_BoardWifi_CloseClient();
            DF_Main_BoardWifi_Client = incoming;
            DF_Main_BoardWifi_Client.setNoDelay(true);
            DF_Main_BoardWifi_LastRx = millis();
        }
    }
    if (!DF_Main_BoardWifi_Client.connected()) { DF_Main_BoardWifi_CloseClient(); return; }
    const unsigned long timeout = DF_Main_BoardWifi_Authenticated ? 12000UL : 5000UL;
    if ((unsigned long)(millis() - DF_Main_BoardWifi_LastRx) > timeout) {
        DF_Main_BoardWifi_CloseClient(); return;
    }
    // Bounded input work and nonblocking socket output preserve the scheduler.
    for (unsigned int i = 0; i < 128 && DF_Main_BoardWifi_Client.available(); ++i) {
        const int character = DF_Main_BoardWifi_Client.read();
        if (character == '\r') continue;
        if (character == '\n') {
            DF_Main_BoardWifi_Line[DF_Main_BoardWifi_LineLength] = '\0';
            String line(DF_Main_BoardWifi_Line);
            DF_Main_BoardWifi_LineLength = 0;
            DF_Main_BoardWifi_HandleLine(line);
            if (!DF_Main_BoardWifi_Client.connected()) return;
        } else if (character < 32 || character > 126 ||
                   DF_Main_BoardWifi_LineLength + 1 >= DF_Main_BoardWifi_LineCapacity) {
            DF_Main_BoardWifi_CloseClient(); return;
        } else DF_Main_BoardWifi_Line[DF_Main_BoardWifi_LineLength++] = (char)character;
    }
    if (DF_Main_BoardWifi_TxOffset < DF_Main_BoardWifi_TxLength) {
        const int sent = ::send(DF_Main_BoardWifi_Client.fd(),
            DF_Main_BoardWifi_Tx + DF_Main_BoardWifi_TxOffset,
            DF_Main_BoardWifi_TxLength - DF_Main_BoardWifi_TxOffset, MSG_DONTWAIT);
        if (sent > 0) DF_Main_BoardWifi_TxOffset += (unsigned int)sent;
        else if (sent < 0 && errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
            DF_Main_BoardWifi_CloseClient(); return;
        }
        if (DF_Main_BoardWifi_TxOffset == DF_Main_BoardWifi_TxLength) {
            DF_Main_BoardWifi_TxOffset = 0; DF_Main_BoardWifi_TxLength = 0;
        }
    }
}

void DF_Main_BoardWifi_BeginShutdown()
{
    ++DF_Main_BoardWifi_Request;
    if (0 == DF_Main_BoardWifi_Request) ++DF_Main_BoardWifi_Request;
    DF_Main_BoardWifi_Pending = true;
    DF_Main_BoardWifi_SerialRoute = DF_Main_BoardWifi_GameConnected();
    if (DF_Main_BoardWifi_SerialRoute) StsSendPrintln(STS_APSTS_REQ + "00%");
    else DF_Main_BoardWifi_SendPending();
}

void DF_Main_BoardWifi_RetryShutdown()
{
    if (!DF_Main_BoardWifi_Pending) return;
    if (DF_Main_BoardWifi_SerialRoute && !DF_Main_BoardWifi_GameConnected())
        DF_Main_BoardWifi_SerialRoute = false;
    if (DF_Main_BoardWifi_SerialRoute) StsSendPrintln(STS_APSTS_REQ + "00%");
    else DF_Main_BoardWifi_SendPending();
}

void DF_Main_BoardWifi_CancelShutdown()
{
    DF_Main_BoardWifi_Pending = false;
}
