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
UA_Server *server;

typedef struct _DATA_SOURCE{
	char* name;
	unsigned char type;   // 1 stand for shuzi, 2 stand for moni.
	int state;
	int data;
}DATA_SOURCE;

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
	printf("Node read %s\n", nodeId.identifier.string.data);
	printf("read Value %i\n", temp);
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
	for(i=0; i<sizeof(source)/sizeof(DATA_SOURCE);i++) {
		if(source[i].type == 1) {
					UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
		.write = writeDataSource};
		
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
			UA_Int32 intData = (UA_Int32)source[i].state;
		UA_Variant_setScalar(&attr->value, &intData, &UA_TYPES[UA_TYPES_INT32]);
		attr->description = UA_LOCALIZEDTEXT("en_US",source[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",source[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, source[i].name);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, source[i].name);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 1000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL); 
		}
		if(source[i].type == 2) {
					UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
		.write = writeDataSource};
		
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
			UA_Int32 intData = (UA_Int32)source[i].data;
		UA_Variant_setScalar(&attr->value, &intData, &UA_TYPES[UA_TYPES_INT32]);
		attr->description = UA_LOCALIZEDTEXT("en_US",source[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",source[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, source[i].name);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, source[i].name);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 2000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL); 
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


	/* add a variable node to the address space */
    UA_VariableAttributes attr;
    UA_VariableAttributes_init(&attr);
    UA_Int32 myInteger = 42;
    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en_US","Digital");
    attr.displayName = UA_LOCALIZEDTEXT("en_US","Digital");

	
	/* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_NUMERIC(1, 1000);
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "Digital");
//		UA_DataSource dateDataSource = (UA_DataSource) {.handle =&myInteger, .read = readDataSource, 
//			.write = writeDataSource};
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NULL, attr, NULL, NULL);
							  
	UA_VariableAttributes oattr;	
    UA_VariableAttributes_init(&oattr);
    UA_Int32 Integer = 40;
    UA_Variant_setScalar(&oattr.value, &Integer, &UA_TYPES[UA_TYPES_INT32]);
    oattr.description = UA_LOCALIZEDTEXT("en_US","Analoy");
    oattr.displayName = UA_LOCALIZEDTEXT("en_US","Analoy");
    UA_Server_addVariableNode(server,  UA_NODEID_NUMERIC(1, 2000), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "Analoy"),
                              UA_NODEID_NULL, oattr, NULL, NULL);
							  
		
	add_dataSource_to_opcServer();


    UA_Server_run(server, &running);
	UA_Server_delete(server);
    nl.deleteMembers(&nl);  
}

int main()
{
	pthread_t opcua_server_id;
	pthread_create(&opcua_server_id,NULL,(void *)handle_opcua_server,NULL);
	while(1) {
		sleep(1);
	}
	return 0;
}

