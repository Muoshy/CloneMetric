#ifndef _NTPClient_h
#define _NTPClient_h
#include <Arduino.h>

extern const int NTP_PACKET_SIZE;		// NTP time is in the first 48 bytes of message
extern byte packetBuffer[]; //buffer to hold incoming & outgoing packets
extern const char *ntpServerName;
extern const int UTC_OFFSET; //set utc offset

class IPAddress;
class WiFiUDP;

String printDigits(int digits);
void digitalClockDisplay();
void sendNTPpacket(WiFiUDP Udp, IPAddress &address);
time_t getNtpTime(WiFiUDP Udp);

String getMonth();
String getDay();

#endif