#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define FLAG 0x7E
#define A 0x03
#define CONTROL_SET 0x03
#define CONTROL_UA 0x07


char* get_msg(int fd){
    int state=0;
	char c;
    char store_A, store_C;

	char* msg = malloc(sizeof(char)*2);

	while (1) {      
		
		switch (state){
			case 0:
				if (c==FLAG){
                    state = 1; 
                    }
                else    state=0;
			break;
			case 1:
				store_A=c; 
				if(c==A)  /*BYTE A*/
					state = 2;
                else if(c==FLAG)
					state = 1;
				else
					state = 0;
			break;
			case 2:
				store_C=c; 
				if(c==CONTROL_SET) /*BYTE C*/
					state=3;
                else if(c==A)
					state=2;
				else
					state=0;
			break;
			case 3: 
				if((store_A ^ store_C)==c)
					state=4;
                else if (c==FLAG)
                    state=1;
				else
					state=0;
			break;
			case 4:
				if(c==FLAG){
					msg[0]=store_A;
					msg[1]=store_C;
					return msg;
				}
                else state=0;
							
			break;

		}	
    }
	return msg;
}