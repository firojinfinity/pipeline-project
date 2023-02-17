#include <Arduino.h>
#include<SoftwareSerial.h>
SoftwareSerial e5(0, 1);
static char recv_buf[512];
static bool is_exist = false;
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
int ch = 0;
int index = 0;
int startMillis = 0;
va_list args;
memset(recv_buf, 0, sizeof(recv_buf));
va_start(args, p_cmd);
e5.printf(p_cmd, args);
Serial.printf(p_cmd, args);
va_end(args);
delay(200);
startMillis = millis();
if (p_ack == NULL)
{
return 0;
}
do
{
while (e5.available() > 0)
{
ch = e5.read();
recv_buf[index++] = ch;
Serial.print((char)ch);
delay(2);
}
if (strstr(recv_buf, p_ack) != NULL)
{
return 1;
}
} while (millis() - startMillis < timeout_ms);
return 0;
}
static int recv_prase(void)
{
char ch;
int index = 0;
memset(recv_buf, 0, sizeof(recv_buf));
while (e5.available() > 0)
{
ch = e5.read();
recv_buf[index++] = ch;
Serial.print((char)ch);
delay(2);
}
if (index)
{
char *p_start = NULL;
char data[32] = {
0,
};
int rssi = 0;
int snr = 0;
p_start = strstr(recv_buf, "RSSI:");
if (p_start && (1 == sscanf(p_start, "RSSI:%d,", &rssi)))
p_start = strstr(recv_buf, "SNR:");
return 1;
}
return 0;
}
static int node_recv(uint32_t timeout_ms)
{
at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");
int startMillis = millis();
do
{
if (recv_prase())
{
return 1;
}
} while (millis() - startMillis < timeout_ms);
return 0;
}
void setup(void)
{
Serial.begin(115200);
while (!Serial);
e5.begin(9600);
Serial.print("Receiver\r\n");
if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
{
is_exist = true;
at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
delay(500);
}
else
{
is_exist = false;
Serial.print("No E5 module found.\r\n");
}
}
void loop(void)
{
if (is_exist)
{
node_recv(2000);
}
}
