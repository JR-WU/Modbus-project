#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
uint16_t tab_reg[1000000]={0};
modbus_t *ctx;
void Modbus_read()
{  int regs1=0;
   while(1)
    {
   regs1=modbus_read_registers(ctx,512,16,tab_reg);//function code is 0x04
}
}
void Modbus_write()
{   int a;
    int lost;
    int data;
    char b;
    printf("Let's begin to write a register!\n");
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
int Initialization()
{
    int j=0;
    ctx = modbus_new_tcp("192.168.198.1", 6666);
    if (ctx == NULL) {
       fprintf(stderr, "Unable to allocate libmodbus context\n");
       return;

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
}
void printfdata()
{
    int i=0;
    while(i<=16)
   {  if(i==16)
      {printf("%d\n",tab_reg[i]); break;}
      printf("%d ",tab_reg[i]);
      i++;
   }
}
void main()
{
    char *s;
    s=(char*)malloc(sizeof(char)*1000);
    /**
    char s[1000];
    **/
    int ret;
    pthread_t write;
    pthread_t read;
    Initialization();
    ret=pthread_create(&read,NULL,(void*)Modbus_read,NULL);
    if(ret!=0){
    printf("Create pthread error!\n");
    exit(1);
}
   while(1)
   {
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
   }


