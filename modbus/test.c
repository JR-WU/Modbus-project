#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "modbus/modbus.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define WRONG 2
#define input 512
#define output 768
#define analog 640
uint16_t tab_reg[1000000]={0};
unsigned char tab_ana[1000000]={0};
modbus_t *ctx;
typedef struct _DATA_SOURCE{
	char* name;
	unsigned char type;   // 1 stand for input, 2 stand for output,3 stand for analog.
	int state;//only two states,one is 0xAA,another is 0x55.
	unsigned char datah8;//only for analog.High 8 bit.
	unsigned char datal8;//Low 8 bit.
}DATA_SOURCE;
DATA_SOURCE source[] = {
	{"DI000", 1,0,0},
	{"DI001", 1,0,0},
	{"DI002", 1,0,0},
	{"DI003", 1,0,0},
	{"DI004", 1,0,0},
	{"DI005", 1,0,0},
	{"DI006", 1,0,0},
	{"DI007", 1,0,0},
	{"DI008", 1,0,0},
	{"DI009", 1,0,0},
	{"DI010", 1,0,0},
	{"DI011", 1,0,0},
	{"DI012", 1,0,0},
	{"DI013", 1,0,0},
	{"DI014", 1,0,0},
	{"DI015", 1,0,0},
	{"DO000", 2,0,0},
	{"DO001", 2,0,0},
	{"DO002", 2,0,0},
	{"DO003", 2,0,0},
	{"DO004", 2,0,0},
	{"DO005", 2,0,0},
	{"DO006", 2,0,0},
	{"DO007", 2,0,0},
	{"DO008", 2,0,0},
	{"DO009", 2,0,0},
	{"DO010", 2,0,0},
	{"DO011", 2,0,0},
	{"DO012", 2,0,0},
	{"DO013", 2,0,0},
	{"DO014", 2,0,0},
	{"DO015", 2,0,0},
	{"AIO001", 3,0,0,0},
	{"AIO002", 3,0,0,0},
	{"AIO003", 3,0,0,0},
	{"AIO004", 3,0,0,0}
};
void Modbus_read_DI()
{
   int i,j;
   j=0;
   while(1)
    {
    modbus_read_registers(ctx,input,15,tab_reg);//function code is 0x04
    modbus_read_registers(ctx,analog,3,tab_ana);//function code is 0x04
   for(i=0;i<=15;i++)
   {
       if(tab_reg[i]==85)
       {
           source[i].state=0;
       }
       else if(tab_reg[i]==170)
       {
           source[i].state=1;
       }
       else
       {
           source[i].state=WRONG;
       }
   }
   for(i=0;i<=7;i=i+2)
   {
       source[j+32].datah8=tab_ana[i];
       source[j+32].datal8=tab_ana[i+1];
       j++;
   }
}
return;
}
void Modbus_read_DO()
{
   int i;
    modbus_read_registers(ctx,output,16,tab_reg);//function code is 0x04
   for(i=0;i<=15;i++)
   {
       if(tab_reg[i]==85)
       {
           source[i+16].state=0;
       }
       else if(tab_reg[i]==170)
       {
           source[i+16].state=1;
       }
       else
       {
           source[i+16].state=WRONG;
       }
   }
}
void Modbus_write(int i)
{   if(i>=16)
    {
        modbus_write_register(ctx,analog+i-16,source[i+16].data);
        return;
    }
    else if(source[i+16].state==1)
    {
        modbus_write_register(ctx,output+i,0xAA);
    }
    else if(source[i+16].state==0)
    {
        modbus_write_register(ctx,output+i,0x55);
    }
}
/**
void Modbus_write(int i)
{   int a=768;
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
    modbus_set_slave(ctx,0x0001);
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
    Modbus_read_DO();
    ret=pthread_create(&read,NULL,(void*)Modbus_read_DI,NULL);
    if(ret!=0){
    printf("Create pthread error!\n");
    exit(1);
}
    int num[100],i;
    for(i=0;i<=15;i++)
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
       for(i=0;i<=19;i++)
       {   /**
           if(i>=16)
            {
                if(source[i+16].data!=num[i])
                {
                    Modbus_write(i);
                    pthread_create(&read,NULL,(void*)Modbus_read_DI,NULL);
                    num[i]=source[i+16].data;
                    printfdata();
                }
            }
            **/
            if(source[i+16].state!=num[i])
           {
                Modbus_write(i);
                pthread_create(&read,NULL,(void*)Modbus_read_DI,NULL);
                num[i]=source[i+16].state;
                printfdata();
           }
       }


   }
}


