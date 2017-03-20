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
	int state;
}DATA_SOURCE;

struct argument{
	int fd;
	int port;
	int serverfd;
};
struct argument sendOriginalDataFD;
struct argument recvDataFD;
int ret = 0;
DATA_SOURCE ANALOY[] = {
	{"1000", 0},
	{"1001", 0},
	{"1002", 1},
	{"1003", 0},
	{"1004", 0},
	{"1005", 0},
	{"1006", 1},
	{"1007", 0},
	{"1008", 0},
	{"1009", 0}
};

#include <signal.h>

UA_Boolean running = true;
static void stopHandler(int sig) {    
	running = false;
}

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

	creat_sockket(&serverfd,&local_addr_s,p->port);

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

void* nodeidFindData(const UA_NodeId nodeId) 
{
	int i;
	for(i=0;i<sizeof(ANALOY)/sizeof(DATA_SOURCE);i++) 
	{
		if(strncmp((char*)nodeId.identifier.string.data, ANALOY[i].name, strlen(ANALOY[i].name)) == 0) 
		{
				return &ANALOY[i].state;			
		}
	}
	printf("not find:%s!\n",nodeId.identifier.string.data);
	return NULL;
}

/*读数据*/
static UA_StatusCode
readDataSource(void *handle, const UA_NodeId nodeId, UA_Boolean sourceTimeStamp,
             const UA_NumericRange *range, UA_DataValue *value) {
	if(range) {
        value->hasStatus = true;
        value->status = UA_STATUSCODE_BADINDEXRANGEINVALID;
        return UA_STATUSCODE_GOOD;
    }
	UA_UInt32 temp;
	if(nodeidFindData(nodeId) != NULL)
		temp = *(UA_UInt32*)nodeidFindData(nodeId);
	else 
		temp = 0;
	value->sourceTimestamp = UA_DateTime_now();
	value->hasValue = true;
	value->hasSourceTimestamp = true;
    UA_Variant_setScalarCopy(&value->value, &temp, &UA_TYPES[UA_TYPES_INT32]);
//	printf("Node read %s\n", nodeId.identifier.string.data);
//	printf("read value %i\n", temp);
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
	for(i=0;i<sizeof(ANALOY)/sizeof(DATA_SOURCE);i++) {
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
		.write = writeDataSource};
	
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
			UA_Int32 intData = (UA_Int32)ANALOY[i].state;
		UA_Variant_setScalar(&attr->value, &intData, &UA_TYPES[UA_TYPES_INT32]);
		attr->description = UA_LOCALIZEDTEXT("en_US",ANALOY[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",ANALOY[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, ANALOY[i].name);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, ANALOY[i].name);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);                                     
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

	/* add a variable node to the address space */
    UA_VariableAttributes attr;
    UA_VariableAttributes_init(&attr);
    UA_Int32 myInteger = 42;
    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en_US","the answer");
    attr.displayName = UA_LOCALIZEDTEXT("en_US","the answer");

	
	/* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
	
		UA_DataSource dateDataSource = (UA_DataSource) {.handle =&myInteger, .read = readDataSource, 
			.write = writeDataSource};
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NULL, attr, NULL, NULL);
//		add_dataSource_to_opcServer();
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

