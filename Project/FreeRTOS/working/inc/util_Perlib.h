#ifndef __UTIL_PERLIB_H_
#define __UTIL_PERLIB_H_

#include "main.h"
#include <stddef.h>
#include <stdint.h>
#include "jsmn.h"


#ifdef __cplusplus
extern "C" {
#endif
	
	
	
void LCD_Control_init(void);
void LCD_LINE_Control(int line, char* str);







	
#ifdef __cplusplus
}
#endif

#endif /* __UTIL_PERLIB_H_ */




