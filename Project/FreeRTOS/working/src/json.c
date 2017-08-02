
#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <main.h>
#include "UART_Perlib.h"



static const char *JSON_1_macAddr = "iot_mac_addr";
static const char *JSON_2_reqSerial = "request_serial_no";
static const char *JSON_3_chkTime = "check_time";
static const char *JSON_4_chkData = "check_data";

char post_packet[450];
int serialNum = 1;
float iot_item_1 = 11.1;
float iot_item_2 = 12.1;
int RTC_year = 2017;
int RTC_month = 7;
int RTC_day = 10;
int RTC_hour = 10;
int RTC_minute = 15;
int RTC_second = 7;


extern int recv_serial;


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}



uint8_t json_Parser(int r, jsmntok_t *t, jsmn_parser p, char *str)
{
	int i;
	char *string; 
	
	
	r = jsmn_parse(&p, str, strlen(str), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(str, &t[i], JSON_1_macAddr) == 0) {
			/* We may use strndup() to fetch string value */
			//printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,str + t[i + 1].start);
			 sprintf(string, "%s %s", "asdasd", "asdasd" );
			i++;
		}
		else if (jsoneq(str, &t[i], JSON_2_reqSerial) == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			//printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,str + t[i + 1].start);
			i++;
		}
		else if (jsoneq(str, &t[i], JSON_3_chkTime) == 0) {
			/* We may want to do strtol() here to get numeric value */
			//printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,str + t[i + 1].start);
			i++;
		}
		else if (jsoneq(str, &t[i], JSON_4_chkData) == 0) {
			int j;
			//printf("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i + 1].size; j++) {
				jsmntok_t *g = &t[i + j + 2];
			//	printf("  * %.*s\n", g->end - g->start, str + g->start);
			}
			i += t[i + 1].size + 1;
		}
		else {
			//printf("Unexpected key: %.*s\n", t[i].end - t[i].start,str + t[i].start);
		}
	}
	return 0;
}




uint8_t json_Packet_Gen(void)
{
	//char* json_packet;
	
	char json_packet[300]; 
	char mac_addr[40];
	char request_serial_no[40];
	char check_Time[40];
	char check_data[150];
	
	int contentLength;
	
	sprintf((char*)mac_addr, "\"iot_mac_addr\":\"%02X:%02X:%02X:%02X:%02X:%02X\"", MAC_ADDR0, MAC_ADDR1,MAC_ADDR2,MAC_ADDR3,MAC_ADDR4,MAC_ADDR5);
	sprintf((char*)request_serial_no, "\"iot_request_history_id\":\"%d\"", recv_serial);
	sprintf((char*)check_Time, "\"check_time\":\"%.4d-%.2d-%.2d:%.2d:%.2d:%.2d\"", RTC_year,RTC_month, RTC_day, RTC_hour, RTC_minute, RTC_second);
	sprintf((char*)check_data, "\"check_data\":[{\"iot_item_id\":\"5\",\"data\":\"%f\"},{\"iot_item_id\":\"6\",\"data\":\"%f\"}]", iot_item_1, iot_item_2);
	sprintf((char*)json_packet, "{%s,%s,%s,%s}", mac_addr, request_serial_no, check_Time, check_data);
	contentLength = strlen((char *)json_packet);
	//sprintf((char*)post_header,  "POST HTTP/1.1\r\nHost: 192.168.25.36::25261\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %d\r\n", contentLength);
	sprintf((char*)post_packet, "%sEOF\r\n", json_packet);
	
	return 1;
}


int str2int(uint8_t *str)
{
	int result;
	int puiss;

	result = 0;
	puiss = 1;
	while (('-' == (*str)) || ((*str) == '+'))
	{
		if (*str == '-')
			puiss = puiss * -1;
		str++;
	}
	while ((*str >= '0') && (*str <= '9'))
	{
		result = (result * 10) + ((*str) - '0');
		str++;
	}
	
	return (result * puiss);
}


