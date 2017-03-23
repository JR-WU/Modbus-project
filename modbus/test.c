#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
uint16_t tab_reg[1000000]={0};
int NO=768;
typedef struct _DATA_SOURCE{
	char* name;
	unsigned char type;   // 1 stand for digital, 2 stand for analog.
	int state;
	int data;
}DATA_SOURCE;
DATA_SOURCE source[] = {
	{"DI0", 1,0,0},
	{"DI1", 1,0,0},
	{"DI2", 1,0,0},
	{"DI3", 1,0,0},
	{"DI4", 1,0,0},
	{"DI5", 1,0,0},
	{"DI6", 1,0,0},
	{"DI7", 1,0,0},
	{"DI8", 1,0,0},
	{"DI9", 1,0,0},
	{"DI10", 1,0,0},
	{"DI11", 1,0,0},
	{"DI12", 1,0,0},
	{"DI13", 1,0,0},
	{"DI14", 1,0,0},
	{"DI15", 1,0,0},
	{"DO0", 1,0,0},
	{"DO1", 1,0,0},
	{"DO2", 1,0,0},
	{"DO3", 1,0,0},
	{"DO4", 1,0,0},
	{"DO5", 1,0,0},
	{"DO6", 1,0,0},
	{"DO7", 1,0,0},
	{"DO8", 1,0,0},
	{"DO9", 1,0,0},
	{"DO10", 1,0,0},
	{"DO11", 1,0,0},
	{"DO12", 1,0,0},
	{"DO13", 1,0,0},
	{"DO14", 1,0,0},
	{"DO15", 1,0,0},
	{"aaaa", 2,0,10},
	{"bbbb", 2,0,14},
	{"cccc", 2,0,21},
	{"dddd", 2,0,20},
	{"eeee", 2,0,33}
};
modbus_t *ctx;
void Modbus_read()
{
   int i;
   while(1)
    {
    modbus_read_registers(ctx,512,16,tab_reg);//function code is 0x04
   for(i=0;i<=16;i++)
   {
       source[i].state=tab_reg[i];
   }
}
}
/**
void Modbus_write(int i)
{   int a=768;
    /**
    int lost;
    int data;
    char b;
    printf("Please enter the number of register:\n");
    lost=scanf("%d",&a);
    if(lost==EOF){
        perror("Wrong Input");
    }
    printf("Please enter the data for register:\n");
    while(1){
    scanf("%d",&data);
    if(data==1||data==0)
    {
       modbus_write_register(ctx,a,data);
       return;
    }
    else{
        printf("Wrong input\n");
    }
    }
 
}
**/
int Initialization()
{

    ctx = modbus_new_tcp("192.168.198.1", 6666);
    if (ctx == NULL) {
       fprintf(stderr, "Unable to allocate libmodbus context\n");
       return -1;

    }
    modbus_set_slave(ctx,0x01);
    modbus_connect(ctx);
   if (modbus_connect(ctx) == -1) {
       fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
       modbus_free(ctx);
       return -1;
}
   else {printf("The Modbus Client is online\n");}

    struct timeval t;
      t.tv_sec=0;
      t.tv_usec=1000000;
    modbus_set_response_timeout(ctx,t.tv_sec,t.tv_usec);
    return 0;
}
void printfdata()
{
    int n=0;
    while(n<=32)
   {  if(n==32)
      {printf("%d\n",source[n].state); break;}
      printf("%d ",source[n].state);
      n++;
   }
}
int main()
{

    /**
    char s[1000];
    **/
    int ret;
    pthread_t read;
    Initialization();
    ret=pthread_create(&read,NULL,(void*)Modbus_read,NULL);
    if(ret!=0){
    printf("Create pthread error!\n");
    exit(1);
}
    int num[100],i;
    for(i=0;i<=16;i++)
    {
        num[i]=source[i+16].state;
    }
   while(1)
   {   /**
       scanf("%s",s);
       if(strcmp(s,"change")==0)
       {
        Modbus_write();
        pthread_create(&read,NULL,(void*)Modbus_read,NULL);
       }
       else
       if(strcmp(s,"print")==0)
        {printfdata();}
        else
            {
            printf("please type in change or print\n");
        }
       }
       **/
       for(i=0;i<=16;i++)
       {
           if(source[i+16].state!=num[i])
           {
                modbus_write_register(ctx,NO+i,source[i+16].state);
                pthread_create(&read,NULL,(void*)Modbus_read,NULL);
                num[i]=source[i+16].state;
                printfdata();
           }
       }


   }
}


