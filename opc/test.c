#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/tcp.h>

#include "open62541.h"

#define opc_port 6666
#define S_PORT 5888
#define R_PORT 5222
#define BACKLOG 7
#define YES 1
UA_Server *server;
typedef struct _DATA_SOURCE{
	char* name;
	unsigned char type;   // 1 stand for shuzi, 2 stand for moni.
	int state;
	int data;
}DATA_SOURCE;

struct argument{
	int fd;
	int port;
	int serverfd;
};
struct argument sendOriginalDataFD;
struct argument recvDataFD;
int ret = 0;
DATA_SOURCE source[] = {
	{"1000", 1,0,0},
	{"1001", 1,0,0},
	{"1002", 1,1,0},
	{"1003", 1,1,0},
	{"1004", 1,0,0},
	{"aaaa", 2,0,10},
	{"bbbb", 2,0,14},
	{"cccc", 2,0,21},
	{"dddd", 2,0,20},
	{"eeee", 2,0,33}
};

#include <signal.h>

UA_Boolean running = true;
static void stopHandler(int sig) {    
	running = false;
}
#if 0
void creat_socket(int *sockfd, struct sockaddr_in *local, int portnum){
	int err;
	int optval = YES;
	int nodelay = YES;
	
	if ((*sockfd = socket(PF_INET6, SOCK_STREAM, 0)) == -1){
		perror("socket");  
        exit(1);  
    } 
	else  printf("socket created/n");
	
	err = setsockopt(*sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
	if(err){
		perror("setsockopt");
	}
	err = setsockopt(*sockfd,IPPROTO_TCP,TCP_NODELAY,&nodelay,sizeof(nodelay));
	if(err){
		perror("setsockopt");
	}
	
	memset(local, 0, sizeof(struct sockaddr_in));
	local->sin_family = AF_INET;
	local->sin_addr.s_addr = htonl(INADDR_ANY);
	local->sin_port = htons(portnum);
	if ( bind(*sockfd, (struct sockaddr*)local, sizeof(struct sockaddr_in))<0 )
	{
		perror("bind");  
        exit(1);  
    } 
	else  
        printf("binded/n");
	if (listen(*sockfd, BACKLOG) < 0) {  
        perror("listen");  
        exit(1);  
    } 
	else  
        printf("begin listen/n"); 
}

void creatserver(struct argument *p){
	char addrstr[100];
	int serverfd;
	struct sockaddr_in local_addr_s;
	struct sockaddr_in from;
	unsigned int len = sizeof(from);

	creat_socket(&serverfd,&local_addr_s,p->port);

	while(1)
	{
		p->fd = accept(serverfd, (struct sockaddr*)&from, &len);
		if(ret == -1){
			perror("accept");
			exit(EXIT_FAILURE);
		}
		struct timeval time;
		gettimeofday(&time, NULL);
		printf("time:%lds, %ldus\n",time.tv_sec,time.tv_usec);
		printf("a IPv4 client from:%s\n",inet_ntop(AF_INET, &(from.sin_addr), addrstr, INET_ADDRSTRLEN));
	}
}
#endif

void* nodeIdFindData(const UA_NodeId nodeId) 
{
	int i;
	for(i=0;i<sizeof(source)/sizeof(DATA_SOURCE);i++) 
	{
		if(strncmp((char*)nodeId.identifier.string.data, source[i].name, strlen(source[i].name)) == 0) 
		{
			if(source[i].type == 1) {
				return &source[i].state;
			}
			else if(source[i].type == 2) {
				return &source[i].data;
			}			
		}
	}
	printf("not find:%s!\n",nodeId.identifier.string.data);
	return NULL;
}

/*读数据*/
static UA_StatusCode
readDataSource(void *handle, const UA_NodeId nodeId, UA_Boolean sourceTimeStamp,
             const UA_NumericRange *range, UA_DataValue *dataValue) {
	if(range) {
        dataValue->hasStatus = true;
        dataValue->status = UA_STATUSCODE_BADINDEXRANGEINVALID;
        return UA_STATUSCODE_GOOD;
    }
	UA_Int32 temp;
	if(nodeIdFindData(nodeId) != NULL)
		temp = *(UA_Int32*)nodeIdFindData(nodeId);
	else 
		temp = 0;
	dataValue->sourceTimestamp = UA_DateTime_now();
	dataValue->hasValue = true;
	dataValue->hasSourceTimestamp = true;
    UA_Variant_setScalarCopy(&dataValue->value, &temp, &UA_TYPES[UA_TYPES_INT32]);
//	printf("Node read %s\n", nodeId.identifier.string.data);
//	printf("read Value %i\n", temp);
    return UA_STATUSCODE_GOOD;
}

/*写数据*/
static UA_StatusCode
writeDataSource(void *handle, const UA_NodeId nodeId, const UA_Variant *data, 
		 const UA_NumericRange *range) {
		 if(UA_Variant_isScalar(data) && data->type == &UA_TYPES[UA_TYPES_INT32] && data->data){
         *(UA_UInt32*)handle = *(UA_UInt32*)data->data;
    }
//		*ANALOY->state=*handle;
	printf("Node written %s\n", nodeId.identifier.string.data);
	printf("written value %d\n",  *(UA_UInt32*)handle);
	     return UA_STATUSCODE_GOOD;		
}
			 
void add_dataSource_to_opcServer()
{
	int i;
	UA_UInt32 m=10000,n=20000;
	for(i=0; i<sizeof(source)/sizeof(DATA_SOURCE);i++) {
		if(source[i].type == 1) {
			m++;
			UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
															.write = writeDataSource};
			UA_VariableAttributes attr;
			UA_VariableAttributes_init(&attr);
			attr.description =  UA_LOCALIZEDTEXT("en_US", source[i].name);
			attr.displayName =  UA_LOCALIZEDTEXT("en_US", source[i].name);
			attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
			
			UA_Int32 intData = (UA_Int32)source[i].state;
			UA_Variant_setScalarCopy(&attr.value, &intData, &UA_TYPES[UA_TYPES_INT32]);
			UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, m),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, source[i].name), UA_NODEID_NULL, attr, NULL, NULL);
		}
		if(source[i].type == 2) {
			n++;
			UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
															.write = writeDataSource};
			UA_VariableAttributes attr;
			UA_VariableAttributes_init(&attr);
			attr.description =  UA_LOCALIZEDTEXT("en_US", source[i].name);
			attr.displayName =  UA_LOCALIZEDTEXT("en_US", source[i].name);
			attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
			
			UA_Int32 intState = (UA_Int32)source[i].data;
			UA_Variant_setScalar(&attr.value, &intState, &UA_TYPES[UA_TYPES_INT32]);
			UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, n),
                              UA_NODEID_NUMERIC(1, 20000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASNOTIFIER),
                              UA_QUALIFIEDNAME(1, source[i].name), UA_NODEID_NULL, attr, NULL, NULL);	
		}										
	}
}

void handle_opcua_server(void * arg){
		//signal(SIGINT,  stopHandler);
    //signal(SIGTERM, stopHandler);

	UA_ServerConfig config = UA_ServerConfig_standard;
    UA_ServerNetworkLayer nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, opc_port);
	config.networkLayers = &nl;
    config.networkLayersSize = 1;
	server = UA_Server_new(config);

	/* Create a rudimentary objectType*/
	UA_ObjectTypeAttributes otAttr;
    UA_ObjectTypeAttributes_init(&otAttr);						  						  
	otAttr.description = UA_LOCALIZEDTEXT("en_US", "the answer");
    otAttr.displayName = UA_LOCALIZEDTEXT("en_US", "the answer");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, 10000),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                UA_QUALIFIEDNAME(1, "ANALOG"), otAttr, NULL, NULL);						  
		
	
		
	UA_ObjectTypeAttributes_init(&otAttr);
    otAttr.description = UA_LOCALIZEDTEXT("en_US", "the answer");
    otAttr.displayName = UA_LOCALIZEDTEXT("en_US", "the answer");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, 20000),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                UA_QUALIFIEDNAME(1, "DIGITAL"), otAttr, NULL, NULL);						  
		
	add_dataSource_to_opcServer();					  

	/*分模拟量数字量*/
	UA_ObjectAttributes oAttr;
	UA_ObjectAttributes_init(&oAttr);
    oAttr.description = UA_LOCALIZEDTEXT("en_US", "A DATA");
    oAttr.displayName = UA_LOCALIZEDTEXT("en_US", "MONI");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 0),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
							UA_QUALIFIEDNAME(1, "MONI"), UA_NODEID_NUMERIC(1, 10000), oAttr, NULL, NULL);
	
	UA_ObjectAttributes_init(&oAttr);
    oAttr.description = UA_LOCALIZEDTEXT("en_US", "A DATA");
    oAttr.displayName = UA_LOCALIZEDTEXT("en_US", "SHUZI");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 0),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "SHUZI"), UA_NODEID_NUMERIC(1, 20000), oAttr, NULL, NULL);

    UA_Server_run(server, &running);
	UA_Server_delete(server);
    nl.deleteMembers(&nl);  
}

int main()
{
/*	for (int i = 0;i<sizeof(ANALOY)/sizeof(DATA_SOURCE);i++)
	{
		printf("%s ", ANALOY[i].name);
	}*/
	
/*	pthread_t r_id;	
	recvDataFD.port = R_PORT;
	pthread_create(&r_id,NULL,(void *)creatserver,&recvDataFD);*/
	
	pthread_t opcua_server_id;
	pthread_create(&opcua_server_id,NULL,(void *)handle_opcua_server,NULL);
	while(1) {
		sleep(1);
	}
	return 0;
}

