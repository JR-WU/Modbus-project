#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
void main()
{
modbus_t *ctx;
uint16_t tab_reg[32]={0};
int i=0;

ctx = modbus_new_tcp("192.168.198.1", 6666);
if (ctx == NULL) {
    fprintf(stderr, "Unable to allocate libmodbus context\n");
    return -1;
}

if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
}
modbus_set_slave(ctx,0x01);
modbus_connect(ctx);
struct timeval t;
  t.tv_sec=0;
  t.tv_usec=1000000;
modbus_set_response_timeout(ctx,t.tv_sec,t.tv_usec);
int regs=modbus_read_input_registers(ctx,512,16,tab_reg);//function code is 0x04
regs=modbus_write_register(ctx,768,0xAA);//function code is 0x06
printf("Chenggong? %d\n",regs);
while(i<=16)
{
    printf("%d\n",tab_reg[i]);
i++;
}
modbus_close(ctx);
modbus_free(ctx);
  return 0;
}
