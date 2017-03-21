#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
uint16_t tab_reg[128]={0};
void Modbus_read(modbus_t *p)
{  int regs1; 
   while(1)
    {
   regs1=modbus_read_input_registers(p,512,16,tab_reg);//function code is 0x04
    if(regs1==-1)
   {printf("Wrong!");
    return;
   }
}
}
void Modbus_write(modbus_t *p)
{   int a;
    int lost;
    int data;
    char b;
    printf("Let's begin to write register!\n");
    Re:printf("Please enter the number of register:\n");
    lost=scanf("%d",&a);
    if(lost==EOF){
        perror("Wrong Input");
    }
    printf("\nPlease enter the data for register:");
    Re1:lost=scanf("%d",&data);
    if(lost==EOF){
        perror("Wrong Input");
    }
    if(data==0xAA||data==0x55)
    {
       modbus_write_register(p, a, data);
    }
    else{
        printf("Wrong input");
        goto Re;
    }
    printf("confirm?(Y/N)\n");
    Re2:scanf("%c",&b);
    if(b=='Y')
    {
        return;
    }
    else if(b=='N')
    {
        goto Re1;
    }
    else{
        printf("Please enter Y or N!!\n");
        goto Re2;
    }
}
int Initialization(modbus_t *p)
{
    int j=0;
    p = modbus_new_tcp("192.168.198.1", 6666);
    if (p == NULL) {
       fprintf(stderr, "Unable to allocate libmodbus context\n");
       return;

    }
    modbus_set_slave(p,0x01);
    modbus_connect(p);
   if (modbus_connect(p) == -1) {
       fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
       modbus_free(p);
       return -1;
}
   else {printf("The Modbus Client is online\n");}

    struct timeval t;
      t.tv_sec=0;
      t.tv_usec=1000000;
    modbus_set_response_timeout(p,t.tv_sec,t.tv_usec);
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
{   /**
    char *s;
    s=(char*)malloc(sizeof(char)*100);
    **/
    char s[1000];
    int ret;
    modbus_t *ctx;
    pthread_t write;
    pthread_t read;
    Initialization(ctx);
    ret=pthread_create(&read,NULL,(void*)Modbus_read,ctx);
    if(ret!=0){
    printf("Create pthread error!\n");
    exit(1);
}
   while(1)
   {
       fgets(s,1000,stdin);
       if(s=="change")
       {
        pthread_create(&write,NULL,(void*)Modbus_write,ctx);
       }
       else if(s=="print")
        {printfdata();}
        else
            {
            printf("please type in change or print");
        }
       }
   }


