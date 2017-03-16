#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <errno.h> 
#include <malloc.h>
#include <stdarg.h> 
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h> 
#include <sys/ioctl.h>
#include <pthread.h>
#include <linux/tcp.h>
#include "open62541.h"

#define opc_port 6666
UA_Server * server;
typedef struct _DATA_SOURCE{
	char* name;
	int state;
}DATA_SOURCE;
DATA_SOURCE ANALOY[] = {
	{"100", 0},
	{"101", 0},
	{"102", 1},
	{"103", 0},
	{"104", 0},
	{"105", 0},
	{"106", 1},
	{"107", 0},
	{"108", 0},
	{"109", 0}
};

#include <signal.h>

UA_Boolean running = true;
static void stopHandler(int sig) {    
	running = false;
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
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
writeDataSource(void *handle, const UA_NodeId nodeid, const UA_Variant *data, 
		 const UA_NumericRange *range) {
		 if(UA_Variant_isScalar(data) && data->type == &UA_TYPES[UA_TYPES_INT32] && data->data){
         *(UA_UInt32*)handle = *(UA_UInt32*)data->data;
     }
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
		UA_Int32 intData = ANALOY[i].state;
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
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NULL, attr, NULL, NULL);
	add_dataSource_to_opcServer();
    UA_Server_run(server, &running);
	UA_Server_delete(server);
    nl.deleteMembers(&nl);  
}

int main()
{
	// for (int i = 0;i<sizeof(ANALOY)/sizeof(DATA_SOURCE);i++)
	// {
		// printf("%s ", ANALOY[i].name);
	// }
		pthread_t opcua_server_id;
		pthread_create(&opcua_server_id,NULL,(void *)handle_opcua_server,NULL);
	while(1) {
		sleep(1);
	}
	return 0;
}

