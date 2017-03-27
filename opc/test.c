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
	unsigned char type;   // 1 stand for DI, 2 stand for DO ,3 stand for analoy
	int state;
	int data;
}DATA_SOURCE;

DATA_SOURCE source[] = {
	{"DI000", 1,1,0},
	{"DI001", 1,0,0},
	{"DI002", 1,0,0},
	{"DI003", 1,0,0},
	{"DI004", 1,0,0},
	{"DI005", 1,1,0},
	{"DI006", 1,0,0},
	{"DI007", 1,0,0},
	{"DI008", 1,0,0},
	{"DI009", 1,1,0},
	{"DI010", 1,0,0},
	{"DI011", 1,1,0},
	{"DI012", 1,0,0},
	{"DI013", 1,0,0},
	{"DI014", 1,0,0},
	{"DI015", 1,0,0},
	{"DO000", 2,1,0},
	{"DO001", 2,0,0},
	{"DO002", 2,0,0},
	{"DO003", 2,0,0},
	{"DO004", 2,1,0},
	{"DO005", 2,0,0},
	{"DO006", 2,0,0},
	{"DO007", 2,1,0},
	{"DO008", 2,0,0},
	{"DO009", 2,1,0},
	{"DO010", 2,0,0},
	{"DO011", 2,0,0},
	{"DO012", 2,1,0},
	{"DO013", 2,0,0},
	{"DO014", 2,0,0},
	{"DO015", 2,0,0},
	{"aaaaa", 3,0,10},
	{"bbbbb", 3,0,14},
	{"ccccc", 3,0,21},
	{"ddddd", 3,0,20}
};

#include <signal.h>
UA_Boolean running = true;
static void stopHandler(int sig) {    
	running = false;
}

/*通过名字读数据*/
void* nodeIdFindData(const UA_NodeId nodeId) 
{
	int i;
	for(i=0;i<sizeof(source)/sizeof(DATA_SOURCE);i++) 
	{
		if(strncmp((char*)nodeId.identifier.string.data, source[i].name, strlen(source[i].name)) == 0) 
		{
			if(source[i].type != 3) {
				return &source[i].state;
			}
			else if(source[i].type == 3) {
				return &source[i].data;
			}			
		}
	}
	printf("not find:%s!\n",nodeId.identifier.string.data);
	return NULL;
}

/*通过名字写数据*/
void* nodeIdwriteData(const UA_NodeId nodeId, const UA_Variant *data) 
{
	int i;
	for(i=0;i<sizeof(source)/sizeof(DATA_SOURCE);i++) 
	{
		if(strncmp((char*)nodeId.identifier.string.data, source[i].name, strlen(source[i].name)) == 0) 
		{
			if(source[i].type != 3) {
				source[i].state=data->data;
				return &source[i].state;
			}
			else if(source[i].type == 3) {
				 source[i].data=data->data;
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
	printf("read Value %d\n", temp);
    return UA_STATUSCODE_GOOD;
}

/*写数据*/
static UA_StatusCode
writeDataSource(void *handle, const UA_NodeId nodeId, const UA_Variant *data, 
			const UA_NumericRange *range) {
				UA_Int32 temp;
	if(UA_Variant_isScalar(data) && data->type == &UA_TYPES[UA_TYPES_INT32] && data->data)
	{
		if(nodeIdwriteData(nodeId, data)!=NULL)
			temp = *(UA_UInt32*)data->data;
	}
//	*ANALOY->state=*handle;
	printf("Node written %s\n", nodeId.identifier.string.data);
	printf("written value %d\n",  temp);
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
//			UA_Int32 intState = (UA_Int32)source[i].state;
//		UA_Variant_setScalar(&attr->value, &intState, &UA_TYPES[UA_TYPES_INT32]);
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
		.write = NULL};
		
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
//			UA_Int32 intState = (UA_Int32)source[i].state;
//		UA_Variant_setScalar(&attr->value, &intState, &UA_TYPES[UA_TYPES_INT32]);
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
		if(source[i].type == 3) {
					UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, 
		.write = writeDataSource};
		
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
//			UA_Int32 intData = (UA_Int32)source[i].data;
//		UA_Variant_setScalar(&attr->value, &intData, &UA_TYPES[UA_TYPES_INT32]);
		attr->description = UA_LOCALIZEDTEXT("en_US",source[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",source[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, source[i].name);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, source[i].name);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 3000);
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


	/* add a variable node Digital to the address space */
    UA_VariableAttributes attr;
    UA_VariableAttributes_init(&attr);
    attr.description = UA_LOCALIZEDTEXT("en_US","DI");
    attr.displayName = UA_LOCALIZEDTEXT("en_US","DI");

	
	/* Add the three variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_NUMERIC(1, 1000);
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "DI");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NULL, attr, NULL, NULL);
		
    UA_VariableAttributes_init(&attr);
    attr.description = UA_LOCALIZEDTEXT("en_US","DO");
    attr.displayName = UA_LOCALIZEDTEXT("en_US","DO");
    UA_Server_addVariableNode(server,  UA_NODEID_NUMERIC(1, 2000), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DO"),
                              UA_NODEID_NULL, attr, NULL, NULL);	
							  						  	
    UA_VariableAttributes_init(&attr);
    attr.description = UA_LOCALIZEDTEXT("en_US","Analoy");
    attr.displayName = UA_LOCALIZEDTEXT("en_US","Analoy");
    UA_Server_addVariableNode(server,  UA_NODEID_NUMERIC(1, 3000), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "Analoy"),
                              UA_NODEID_NULL, attr, NULL, NULL);
							  
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
