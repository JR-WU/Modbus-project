#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
#define uint16_t tab_reg[128]={0};
int Modbus_read(*p)
{   while(1)
    {
    int regs1=modbus_read_input_registers(p,512,16,tab_reg);//function code is 0x04
    if(regs1==-1)
   {printf("Wrong!");
    return -1;
   }
   printfdata(regs1);
}
}
int Modbus_write(*p)
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
        int modbus_write_register(p, a, data);
    }
    else{
        printf("Wrong input");
        goto Re;
    }
    printf("confirm?(Y/N)\n");
    Re2:scanf("%c",&b);
    if(b=='Y')
    {
        return 1;
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
int Initialization(*p)
{
    int j=0;
    p = modbus_new_tcp("192.168.198.1", 6666);
    if (p == NULL) {
       fprintf(stderr, "Unable to allocate libmodbus context\n");
       return -1;
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
int printfdata(int a)
{
    int j=0;
    int i=0;
    usleep(500000);
    printf("%d:\n",j);
    printf("The number of input registers is:%d\n",a);
    while(i<=16)
   {  if(i==16)
      {printf("%d\n",tab_reg[i]); break;}
      printf("%d ",tab_reg[i]);
      i++;
   }
   usleep(500000);
   j++;

}
void main()
{   char *s;
    modbus_t *ctx;
    pthread_t write;
    pthread_t read;
    Initialization(ctx);
    ret=pthread_create(&read,NULL,(int *) Modbus_read,ctx);
　　 if(ret!=0) {
　　　　printf ("Create pthread error!\n");
　　　　exit (1);
　　}
   while(1)
   {
       scanf("&s",*s);
       if(*s=="change")
       {
           pthread_create(&write,NULL,(int *) Modbus_write,ctx);
       }
       else {
        printf("If you want to change data please type change\n");
       }
   }

}
