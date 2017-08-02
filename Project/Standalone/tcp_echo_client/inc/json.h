#ifndef __JSON_H_
#define __JSON_H_

#include <stddef.h>
#include <stdint.h>
#include "jsmn.h"


#ifdef __cplusplus
extern "C" {
#endif


static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
uint8_t json_Packet_Gen(void);
	
int str2int(char *str);
	
	
	
	
	
extern int recv_serial;	
extern char post_packet[300];
	
	
	
	
	
	
	
	
	
	
	
#ifdef __cplusplus
}
#endif

#endif /* __JSON_H_ */




