#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
int Modbus()
{
    int j=0;
    modbus_t *ctx;
    uint16_t tab_reg[32]={0};

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
    while(1)
    {
    int regs1=modbus_read_input_registers(ctx,512,16,tab_reg);//function code is 0x04
    if(regs1==-1)
   {printf("Wrong!");
    return -1;
   }
    int i=0;
    usleep(500000);
    printf("%d:\n",j);
    printf("The number of input registers is:%d\n",regs1);
    while(i<=16)
   {  if(i==16)
      {printf("%d\n",tab_reg[i]); break;}
      printf("%d ",tab_reg[i]);
      i++;
   }
   usleep(500000);
   j++;

}
}
