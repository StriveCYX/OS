#include "stdint.h"

void Myputs(char* str)
{
	while(*str!='\0')
	{
		put_char(*str);
		++str;	
	}
		
}
