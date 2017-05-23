#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <linux/tcp.h>
#include "open62541.h"
#include <sys/time.h>
#include <time.h>
#define WRONG 2
#define input 512
#define output 768
#define analog 640
#define opc_port 16664
#define MAX_NUM 1024
#define LINE_NUM 16
#define REG_NUM 4
#define ANA_NUM 2
pthread_mutex_t mutex;
UA_Server *server;
int data_r;
int sockfd,sockfd1;
int judge;
struct sockaddr_in local;
typedef struct _DATA_SOURCE{
	char* name;
	unsigned char type;   // 1 stand for DI, 2 stand for DO ,3 stand for analoy
	int state;			  // I\O鍙ｇ姸鎬侊紝楂?锛屼綆0
    float Idata;
    float Udata;
}DATA_SOURCE;

typedef struct _ANALOGY_SOURCE{
    char* opc1;
	char* opc2;
	unsigned char type;// 1 stands for DI, 2 stands for DO ,3 stands for analogy INPUT,4 stands for analogy OUTPUT.
    float Idata;
    float Udata;
}ANA_SOURCE;

DATA_SOURCE DI0[] = {
	{"DI0-00", 1,0,0,0},
	{"DI0-01", 1,0,0,0},
	{"DI0-02", 1,0,0,0},
	{"DI0-03", 1,0,0,0},
	{"DI0-04", 1,0,0,0},
	{"DI0-05", 1,0,0,0},
	{"DI0-06", 1,0,0,0},
	{"DI0-07", 1,0,0,0},
	{"DI0-08", 1,0,0,0},
	{"DI0-09", 1,0,0,0},
	{"DI0-10", 1,0,0,0},
	{"DI0-11", 1,0,0,0},
	{"DI0-12", 1,0,0,0},
	{"DI0-13", 1,0,0,0},
	{"DI0-14", 1,0,0,0},
	{"DI0-15", 1,0,0,0}
};
DATA_SOURCE DI1[] = {
    {"DI1-00", 1,0,0,0},
	{"DI1-01", 1,0,0,0},
	{"DI1-02", 1,0,0,0},
	{"DI1-03", 1,0,0,0},
	{"DI1-04", 1,0,0,0},
	{"DI1-05", 1,0,0,0},
	{"DI1-06", 1,0,0,0},
	{"DI1-07", 1,0,0,0},
	{"DI1-08", 1,0,0,0},
	{"DI1-09", 1,0,0,0},
	{"DI1-10", 1,0,0,0},
	{"DI1-11", 1,0,0,0},
	{"DI1-12", 1,0,0,0},
	{"DI1-13", 1,0,0,0},
	{"DI1-14", 1,0,0,0},
	{"DI1-15", 1,0,0,0}
};
DATA_SOURCE DI2[] = {
    {"DI2-00", 1,0,0,0},
	{"DI2-01", 1,0,0,0},
	{"DI2-02", 1,0,0,0},
	{"DI2-03", 1,0,0,0},
	{"DI2-04", 1,0,0,0},
	{"DI2-05", 1,0,0,0},
	{"DI2-06", 1,0,0,0},
	{"DI2-07", 1,0,0,0},
	{"DI2-08", 1,0,0,0},
	{"DI2-09", 1,0,0,0},
	{"DI2-10", 1,0,0,0},
	{"DI2-11", 1,0,0,0},
	{"DI2-12", 1,0,0,0},
	{"DI2-13", 1,0,0,0},
	{"DI2-14", 1,0,0,0},
	{"DI2-15", 1,0,0,0}
};
DATA_SOURCE DI3[] = {
    {"DI3-00", 1,0,0,0},
	{"DI3-01", 1,0,0,0},
	{"DI3-02", 1,0,0,0},
	{"DI3-03", 1,0,0,0},
	{"DI3-04", 1,0,0,0},
	{"DI3-05", 1,0,0,0},
	{"DI3-06", 1,0,0,0},
	{"DI3-07", 1,0,0,0},
	{"DI3-08", 1,0,0,0},
	{"DI3-09", 1,0,0,0},
	{"DI3-10", 1,0,0,0},
	{"DI3-11", 1,0,0,0},
	{"DI3-12", 1,0,0,0},
	{"DI3-13", 1,0,0,0},
	{"DI3-14", 1,0,0,0},
	{"DI3-15", 1,0,0,0}
};
DATA_SOURCE DO0[] = {
	{"DO0-00", 2,0,0,0},
	{"DO0-01", 2,0,0,0},
	{"DO0-02", 2,0,0,0},
	{"DO0-03", 2,0,0,0},
	{"DO0-04", 2,0,0,0},
	{"DO0-05", 2,0,0,0},
	{"DO0-06", 2,0,0,0},
	{"DO0-07", 2,0,0,0},
	{"DO0-08", 2,0,0,0},
	{"DO0-09", 2,0,0,0},
	{"DO0-10", 2,0,0,0},
	{"DO0-11", 2,0,0,0},
	{"DO0-12", 2,0,0,0},
	{"DO0-13", 2,0,0,0},
	{"DO0-14", 2,0,0,0},
	{"DO0-15", 2,0,0,0}
};
DATA_SOURCE DO1[] = {
	{"DO1-00", 2,0,0,0},
	{"DO1-01", 2,0,0,0},
	{"DO1-02", 2,0,0,0},
	{"DO1-03", 2,0,0,0},
	{"DO1-04", 2,0,0,0},
	{"DO1-05", 2,0,0,0},
	{"DO1-06", 2,0,0,0},
	{"DO1-07", 2,0,0,0},
	{"DO1-08", 2,0,0,0},
	{"DO1-09", 2,0,0,0},
	{"DO1-10", 2,0,0,0},
	{"DO1-11", 2,0,0,0},
	{"DO1-12", 2,0,0,0},
	{"DO1-13", 2,0,0,0},
	{"DO1-14", 2,0,0,0},
	{"DO1-15", 2,0,0,0}
};
DATA_SOURCE DO2[] = {
	{"DO2-00", 2,0,0,0},
	{"DO2-01", 2,0,0,0},
	{"DO2-02", 2,0,0,0},
	{"DO2-03", 2,0,0,0},
	{"DO2-04", 2,0,0,0},
	{"DO2-05", 2,0,0,0},
	{"DO2-06", 2,0,0,0},
	{"DO2-07", 2,0,0,0},
	{"DO2-08", 2,0,0,0},
	{"DO2-09", 2,0,0,0},
	{"DO2-10", 2,0,0,0},
	{"DO2-11", 2,0,0,0},
	{"DO2-12", 2,0,0,0},
	{"DO2-13", 2,0,0,0},
	{"DO2-14", 2,0,0,0},
	{"DO2-15", 2,0,0,0}
};
DATA_SOURCE DO3[] = {
	{"DO3-00", 2,0,0,0},
	{"DO3-01", 2,0,0,0},
	{"DO3-02", 2,0,0,0},
	{"DO3-03", 2,0,0,0},
	{"DO3-04", 2,0,0,0},
	{"DO3-05", 2,0,0,0},
	{"DO3-06", 2,0,0,0},
	{"DO3-07", 2,0,0,0},
	{"DO3-08", 2,0,0,0},
	{"DO3-09", 2,0,0,0},
	{"DO3-10", 2,0,0,0},
	{"DO3-11", 2,0,0,0},
	{"DO3-12", 2,0,0,0},
	{"DO3-13", 2,0,0,0},
	{"DO3-14", 2,0,0,0},
	{"DO3-15", 2,0,0,0}
};
ANA_SOURCE AI0[] = {
	{"AI0-1I","AI0-1U",3,0,0},
	{"AI0-2I","AI0-2U",3,0,0},
	{"AI0-3I","AI0-3U",3,0,0},
	{"AI0-4I","AI0-4U",3,0,0}
};
ANA_SOURCE AI1[] = {
	{"AI1-1I","AI1-1U",3,0,0},
	{"AI1-2I","AI1-2U",3,0,0},
	{"AI1-3I","AI1-3U",3,0,0},
	{"AI1-4I","AI1-4U",3,0,0}
};
ANA_SOURCE AO0[] = {
	{"AO0-1I","AO0-1U",4,0,0},
	{"AO0-2I","AO0-2U",4,0,0},
	{"AO0-3I","AO0-3U",4,0,0},
	{"AO0-4I","AO0-4U",4,0,0}
};
ANA_SOURCE AO1[] = {
	{"AO1-1I","AO1-1U",4,0,0},
	{"AO1-2I","AO1-2U",4,0,0},
	{"AO1-3I","AO1-3U",4,0,0},
	{"AO1-4I","AO1-4U",4,0,0}
};
UA_Boolean running = true;
static void stopHandler(int sig) {
	running = false;
}

/*通过名字读数据*/
void* nodeIdFindData(const UA_NodeId nodeId)//void nodeIdFindDataANA()
{
	int i;
	for(i=0;i<sizeof(DI0)/sizeof(DATA_SOURCE);i++)
	{
		if(strncmp((char*)nodeId.identifier.string.data, DI0[i].name, strlen(DI0[i].name)) == 0)
		{
			return &DI0[i].state;
		}
		 if(strncmp((char*)nodeId.identifier.string.data, DI1[i].name, strlen(DI1[i].name)) == 0)
		{
			return &DI1[i].state;
		}
		 if(strncmp((char*)nodeId.identifier.string.data, DI2[i].name, strlen(DI2[i].name)) == 0)
		{
			return &DI2[i].state;
		}
		 if(strncmp((char*)nodeId.identifier.string.data, DI3[i].name, strlen(DI3[i].name)) == 0)
		{
			return &DI3[i].state;
        }
        if(strncmp((char*)nodeId.identifier.string.data, DO0[i].name, strlen(DO0[i].name)) == 0)
		{
			return &DO0[i].state;
		}
		if(strncmp((char*)nodeId.identifier.string.data, DO1[i].name, strlen(DO1[i].name)) == 0)
		{
			return &DO1[i].state;
		}
		if(strncmp((char*)nodeId.identifier.string.data, DO2[i].name, strlen(DO2[i].name)) == 0)
		{
			return &DO2[i].state;
		}
		if(strncmp((char*)nodeId.identifier.string.data, DO3[i].name, strlen(DO3[i].name)) == 0)
		{
			return &DO3[i].state;
		}
	}
	for(i=0;i<sizeof(AI0)/sizeof(ANA_SOURCE);i++)
	{
		if(strncmp((char*)nodeId.identifier.string.data, AI0[i].opc1, strlen(AI0[i].opc1)) == 0)
		{
			return &AI0[i].Idata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AI1[i].opc1, strlen(AI1[i].opc1)) == 0)
		{
			return &AI1[i].Idata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AO0[i].opc1, strlen(AO0[i].opc1)) == 0)
		{
			return &AO0[i].Idata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AO1[i].opc1, strlen(AO1[i].opc1)) == 0)
		{
			return &AO1[i].Idata;
		}
	}
	for(i=0;i<sizeof(AI0)/sizeof(ANA_SOURCE);i++)
	{
		if(strncmp((char*)nodeId.identifier.string.data, AI0[i].opc2, strlen(AI0[i].opc2)) == 0)
		{
			return &AI0[i].Udata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AI1[i].opc2, strlen(AI1[i].opc2)) == 0)
		{
			return &AI1[i].Udata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AO0[i].opc2, strlen(AO0[i].opc2)) == 0)
		{
			return &AO0[i].Udata;
		}
		if(strncmp((char*)nodeId.identifier.string.data, AO1[i].opc2, strlen(AO1[i].opc2)) == 0)
		{
			return &AO1[i].Udata;
		}
	}
	printf("not find:%s!\n",nodeId.identifier.string.data);
	return NULL;
}


/*通过名字写数据*/
void* nodeIdwriteData(const UA_NodeId nodeId, const UA_Variant *data)
{
	int i;
	for(i=0;i<sizeof(DO0)/sizeof(DATA_SOURCE);i++)
	{
		if(strncmp(DO0[i].name, (char*)nodeId.identifier.string.data, strlen(DO0[i].name)) == 0)
		{
			DO0[i].state=*(int*)data->data;
			return &DO0[i].state;
		}
		if(strncmp(DO1[i].name, (char*)nodeId.identifier.string.data, strlen(DO1[i].name)) == 0)
		{
			DO1[i].state=*(int*)data->data;
			return &DO1[i].state;
		}
		if(strncmp(DO2[i].name, (char*)nodeId.identifier.string.data, strlen(DO2[i].name)) == 0)
		{
			DO2[i].state=*(int*)data->data;
			return &DO2[i].state;
		}
		if(strncmp(DO3[i].name, (char*)nodeId.identifier.string.data, strlen(DO3[i].name)) == 0)
		{
			DO3[i].state=*(int*)data->data;
			return &DO3[i].state;
		}

	}
	for(i=0;i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		if(strncmp(AO0[i].opc1,(char*)nodeId.identifier.string.data, strlen(AO0[i].opc1)) == 0)
		{
			AO0[i].Idata =*(float*)data->data;
			return &AO0[i].Idata;
		}
		if(strncmp(AO1[i].opc1, (char*)nodeId.identifier.string.data, strlen(AO1[i].opc1)) == 0)
		{
			AO1[i].Idata =*(float*)data->data;
			return &AO1[i].Idata;
		}
	}
	for(i=0;i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		if(strncmp(AO0[i].opc2,(char*)nodeId.identifier.string.data, strlen(AO0[i].opc2)) == 0)
		{
			AO0[i].Udata =*(float*)data->data;
			return &AO0[i].Udata;
		}
		if(strncmp(AO1[i].opc2, (char*)nodeId.identifier.string.data, strlen(AO1[i].opc2)) == 0)
		{
			AO1[i].Udata =*(float*)data->data;
			return &AO1[i].Udata;
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
	UA_Int16 temp;
	if(nodeIdFindData(nodeId) != NULL)
		temp = *(UA_Int16*)nodeIdFindData(nodeId);
	else
		temp = 0;
	dataValue->sourceTimestamp = UA_DateTime_now();
	dataValue->hasValue = true;
	dataValue->hasSourceTimestamp = true;
    UA_Variant_setScalarCopy(&dataValue->value, &temp, &UA_TYPES[UA_TYPES_INT16]);
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
readDataSourceANA(void *handle, const UA_NodeId nodeId, UA_Boolean sourceTimeStamp,
             const UA_NumericRange *range, UA_DataValue *dataValue) {
	if(range) {
        dataValue->hasStatus = true;
        dataValue->status = UA_STATUSCODE_BADINDEXRANGEINVALID;
        return UA_STATUSCODE_GOOD;
    }
	UA_Float temp;
	if(nodeIdFindData(nodeId) != NULL)
		temp = *(UA_Float*)nodeIdFindData(nodeId);
	else
		temp = 0;
	dataValue->sourceTimestamp = UA_DateTime_now();
	dataValue->hasValue = true;
	dataValue->hasSourceTimestamp = true;
    UA_Variant_setScalarCopy(&dataValue->value, &temp, &UA_TYPES[UA_TYPES_FLOAT]);
	//printf("Node read %s\n", nodeId.identifier.string.data);
	//printf("read Value %d\n", temp);
    return UA_STATUSCODE_GOOD;
}


/*写数据*/
static UA_StatusCode
writeDataSource(void *handle, const UA_NodeId nodeId, const UA_Variant *data,
			const UA_NumericRange *range) {
				UA_Int16 temp;
	if(UA_Variant_isScalar(data) && data->type == &UA_TYPES[UA_TYPES_INT16] && data->data)
	{
		if(nodeIdwriteData(nodeId, data)!=NULL)
			temp = *(UA_Int16*)data->data;
	}
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
writeDataSourceANA(void *handle, const UA_NodeId nodeId, const UA_Variant *data,
			const UA_NumericRange *range) {
				UA_Float temp;
	if(UA_Variant_isScalar(data) && data->type == &UA_TYPES[UA_TYPES_FLOAT] && data->data)
	{
		if(nodeIdwriteData(nodeId, data)!=NULL)
			temp = *(UA_Float*)data->data;
	}
    return UA_STATUSCODE_GOOD;
}

void add_dataSource_to_opcServer()
{
	int i;
	for(i=0; i<sizeof(DI0)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DI0[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DI0[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DI0[i].name),UA_NODEID_NUMERIC(1, 1000),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DI0[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(DI1)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DI1[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DI1[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DI1[i].name),UA_NODEID_NUMERIC(1, 1001),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DI1[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
		for(i=0; i<sizeof(DI2)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DI2[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DI2[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DI2[i].name),UA_NODEID_NUMERIC(1, 1002),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DI2[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
		for(i=0; i<sizeof(DI3)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DI3[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DI3[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DI3[i].name),UA_NODEID_NUMERIC(1, 1003),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DI3[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	
	for(i=0; i<sizeof(DO0)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = writeDataSource};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DO0[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DO0[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DO0[i].name),UA_NODEID_NUMERIC(1, 2000),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DO0[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(DO1)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = writeDataSource};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DO1[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DO1[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DO1[i].name),UA_NODEID_NUMERIC(1, 2001),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DO1[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(DO2)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = writeDataSource};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DO2[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DO2[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DO2[i].name),UA_NODEID_NUMERIC(1, 2002),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DO2[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
		for(i=0; i<sizeof(DO3)/sizeof(DATA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSource, .write = writeDataSource};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",DO3[i].name);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",DO3[i].name);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;;
		UA_Server_addDataSourceVariableNode(server, UA_NODEID_STRING(1, DO3[i].name),UA_NODEID_NUMERIC(1, 2003),
	                              					UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(1, DO3[i].name),
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}

	for(i=0; i<sizeof(AI0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AI0[i].opc1);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AI0[i].opc1);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AI0[i].opc1);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AI0[i].opc1);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 3000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}

    for(i=0; i<sizeof(AI1)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AI0[i].opc2);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AI0[i].opc2);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AI0[i].opc2);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AI0[i].opc2);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 3000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(AI0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AI1[i].opc1);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AI1[i].opc1);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AI1[i].opc1);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AI1[i].opc1);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 3001);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}

	for(i=0; i<sizeof(AI0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = NULL};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AI1[i].opc2);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AI1[i].opc2);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AI1[i].opc2);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AI1[i].opc2);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 3001);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = writeDataSourceANA};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AO0[i].opc1);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AO0[i].opc1);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AO0[i].opc1);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AO0[i].opc1);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 4000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}

    for(i=0; i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = writeDataSourceANA};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AO0[i].opc2);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AO0[i].opc2);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AO0[i].opc2);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AO0[i].opc2);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 4000);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}
	for(i=0; i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = writeDataSourceANA};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AO1[i].opc1);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AO1[i].opc1);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AO1[i].opc1);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AO1[i].opc1);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 4001);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
		UA_Server_addDataSourceVariableNode(server, myIntegerNodeId,parentNodeId,
	                              					parentReferenceNodeId, myIntegerName,
                                                UA_NODEID_NULL, *attr, dateDataSource, NULL);
	}

    for(i=0; i<sizeof(AO0)/sizeof(ANA_SOURCE);i++)
	{
		UA_DataSource dateDataSource = (UA_DataSource) {.handle = NULL, .read = readDataSourceANA,.write = writeDataSourceANA};
		UA_VariableAttributes *attr = UA_VariableAttributes_new();
    	UA_VariableAttributes_init(attr);
		attr->description = UA_LOCALIZEDTEXT("en_US",AO1[i].opc2);
	    attr->displayName = UA_LOCALIZEDTEXT("en_US",AO1[i].opc2);
		attr->accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, AO1[i].opc2);
	    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, AO1[i].opc2);
	    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, 4001);
	    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY);
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


	/* add type to object */
	UA_ObjectAttributes object_attr;
    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DI0");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DI0");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 1000),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DI0"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DI1");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DI1");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 1001),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DI1"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DI2");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DI2");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 1002),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DI2"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DI3");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DI3");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 1003),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DI3"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DO0");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DO0");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 2000),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DO0"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DO1");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DO1");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 2001),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DO1"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);
	
    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DO2");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DO2");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 2002),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DO2"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);
	
    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "DO3");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "DO3");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 2003),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "DO3"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "AI0");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "AI0");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 3000),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "AI0"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);
		
	    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "AI1");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "AI1");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 3001),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "AI1"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);	

    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "AO0");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "AO0");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 4000),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "AO0"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);
		
	    UA_ObjectAttributes_init(&object_attr);
    object_attr.description = UA_LOCALIZEDTEXT("en_US", "AO1");
    object_attr.displayName = UA_LOCALIZEDTEXT("en_US", "AO1");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 4001),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, "AO1"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);	

	add_dataSource_to_opcServer();

    UA_Server_run(server, &running);
	UA_Server_delete(server);
    nl.deleteMembers(&nl);
}

int Initialization(int fd,struct sockaddr_in *local){
    int err;
    fd=socket(AF_INET,SOCK_STREAM, 0);
    if(fd < 0){
		perror("socket");
		exit(EXIT_FAILURE);
	}
    memset(local, 0, sizeof(struct sockaddr_in));
    local->sin_family = AF_INET;
	local->sin_addr.s_addr= inet_addr("127.0.0.1");
	local->sin_port = htons(6666);//重新设置local的值,并转换格式
//	if(bind(fd, (struct sockaddr*)local, sizeof(struct sockaddr_in))<0);
//  printf("Socket has been created.\n");

	err = connect(fd,(struct sockaddr*)local, sizeof(struct sockaddr_in));//connect with server.
	if(err < 0){
		perror("connect");
		exit(EXIT_FAILURE);
	} else {
		return fd;
	}
}

int Merge(int a,uint8_t b, uint8_t c)
{
	a = (b << 8) | c;
	return a;
}

/* 写寄存器函数,a 为要写的值,b为寄存器的地址。*/
void SendData(int a,int b,int j)       //
{
    printf("enter into SendData\n");
    int err,e,f;
    e = b >> 8;
	f = b & 0x0ff;
    if(a==1)
    {
        uint8_t Send_buf[] = {j, 0x06, e,f, 0xAA, 0x00,0x00};
        uint8_t Recv_buf[1024];
        printf("%d,%d,%d,%d,%d,%d,%d\n",Send_buf[0],Send_buf[1],Send_buf[2],Send_buf[3],Send_buf[4],Send_buf[5],Send_buf[6]);
        printf("%d\n",sockfd1);
        err=send(sockfd1,Send_buf,7,0);
        recv(sockfd1,Recv_buf,7,0);
        printf("%d,%d,%d,%d,%d,%d,%d\n",Recv_buf[0],Recv_buf[1],Recv_buf[2],Recv_buf[3],Recv_buf[4],Recv_buf[5],Recv_buf[6]);
        printf("%d\n",sockfd1);
        if(err < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        close(sockfd1);
        sockfd1=Initialization(sockfd1,&local);
        printf("%d\n",sockfd1);
    }
    else if(a==0)
    {
        uint8_t Send_buf[] = {j, 0x06, e,f, 0x55, 0x00,0x00};
        uint8_t Recv_buf[1024];
        printf("%d,%d,%d,%d,%d,%d,%d\n",Send_buf[0],Send_buf[1],Send_buf[2],Send_buf[3],Send_buf[4],Send_buf[5],Send_buf[6]);
        printf("%d\n",sockfd1);
        err=send(sockfd1,Send_buf,7,0);
        recv(sockfd1,Recv_buf,7,0);
        printf("%d,%d,%d,%d,%d,%d,%d\n",Recv_buf[0],Recv_buf[1],Recv_buf[2],Recv_buf[3],Recv_buf[4],Recv_buf[5],Recv_buf[6]);
        printf("%d\n",sockfd1);
        if(err < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        close(sockfd1);
        sockfd1=Initialization(sockfd1,&local);
        printf("%d\n",sockfd1);
    }
}

void SendDataAO(float a,int b,int j)
{
	int val;
	val=(int)a*65536/5000;
	uint8_t H=((val>>8)&0xff);
	uint8_t L=(val&0xff);
	
    int err,e,f;
    e = b >> 8;
	f = b & 0x0ff;
    uint8_t Send_buf[] = {j, 0x06, e,f, H, 0x00,0x00};
    uint8_t Recv_buf[1024];
    err=send(sockfd1,Send_buf,7,0);
    recv(sockfd1,Recv_buf,7,0);
    if(err < 0)
	{
       perror("send");
       exit(EXIT_FAILURE);
    }
	Send_buf[2] = (b+1) >> 8;
    Send_buf[3] = (b+1) & 0x0ff;
	err=send(sockfd,Send_buf,7,0);
    recv(sockfd1,Recv_buf,7,0);
    if(err < 0)
	{
       perror("send");
       exit(EXIT_FAILURE);
    }		
	close(sockfd);
	sockfd=Initialization(sockfd,&local);
}

void ReadData()
{
    printf("enter into ReadData\n");
    int err,i,e,f,ret,j,VAL1;
	float U1,I1;
	uint8_t Send_buf1[]={j, 0x04, e,f,0x00,0x00};
    uint8_t Recv_buf1[MAX_NUM];
    while(1)
    {
        pthread_mutex_lock(&mutex);
        sockfd=Initialization(sockfd,&local);
        for(i=0;i<LINE_NUM;i++)
        {
            for(j=0;j<REG_NUM;j++)
            {
            e = (input+i) >> 8;
            f = (input+i) & 0x0ff;
            Send_buf1[2] = e;
			Send_buf1[3] = f;
            ret=send(sockfd,Send_buf1,6,0);
            recv(sockfd,Recv_buf1,MAX_NUM,0);
            close(sockfd);
            sockfd=Initialization(sockfd,&local);
            if(Recv_buf1[2]==170)
            {
                switch(j)
                {

                case 0:DI0[i].state=1;break;
                case 1:DI1[i].state=1;break;
                case 2:DI2[i].state=1;break;
                case 3:DI3[i].state=1;break;
                }

            }
            else if(Recv_buf1[2]==85)
            {
                switch(j)
                {

                case 0:DI0[i].state=0;break;
                case 1:DI1[i].state=0;break;
                case 2:DI2[i].state=0;break;
                case 3:DI3[i].state=0;break;
                }
            }
            else
            {
                switch(j)
                {

                case 0:DI0[i].state=WRONG;break;
                case 1:DI1[i].state=WRONG;break;
                case 2:DI2[i].state=WRONG;break;
                case 3:DI3[i].state=WRONG;break;
                }
            }
			}
		}
		for(i=0;i<16;i++)
		{
			for(j=8;j<10;j++)
			{
				e = (input+2*i) >> 8;
				f = (input+2*i) & 0x0ff;
				Send_buf1[2] = e;
				Send_buf1[3] = f;
				ret=send(sockfd,Send_buf1,6,0);
				recv(sockfd,Recv_buf1,MAX_NUM,0);
				uint8_t L1=Recv_buf1[2];
				
				e = (input+2*i+1) >> 8;
				f = (input+2*i+1) & 0x0ff;
				Send_buf1[2] = e;
				Send_buf1[3] = f;
				ret=send(sockfd,Send_buf1,6,0);
				recv(sockfd,Recv_buf1,MAX_NUM,0);
				uint8_t H1=Recv_buf1[2];
				
				VAL1=(H1<<8)|L1;
				U1=(float)VAL1*0.1875;
				I1=U1/250;
				close(sockfd);
				sockfd=Initialization(sockfd,&local);
				switch(j)
				{
					case 8:AI0[i].Udata=U1, AI0[i].Idata=I1;break;
					case 9:AI1[i].Udata=U1, AI1[i].Idata=I1;break;
				}
				
			}
			
		}

    close(sockfd);
    pthread_mutex_unlock(&mutex);
    sleep(1);
	}
}
void ReadDataDO()
{
    pthread_mutex_lock(&mutex);
    printf("enter into ReadDataDo\n");
    int err,i,c,d,e,f,k,j, VAL2;
	float U2,I2;
	uint8_t Send_buf2[6]={k, 0x04, c,d,0x00,0x00};
    uint8_t Recv_buf2[MAX_NUM];
    {
        sockfd1=Initialization(sockfd1,&local);
        for(i=0;i<LINE_NUM;i++)
        {
            for(k=6;k<REG_NUM+6;k++)
                {
                    c = (output+i) >> 8;
                    d = (output+i) & 0x0ff;
					Send_buf2[2] = c;
					Send_buf2[3] = d;
                    send(sockfd1,Send_buf2,6,0);
                    recv(sockfd1,Recv_buf2,MAX_NUM,0);
                    printf("%d",Recv_buf2[2]);
                    close(sockfd1);
                    sockfd1=Initialization(sockfd1,&local);
                    if(Recv_buf2[2]==170)
                        {
                            switch(k)
                                {
                                    case 4:DO0[i].state=1;break;
                                    case 5:DO1[i].state=1;break;
                                    case 6:DO2[i].state=1;break;
                                    case 7:DO3[i].state=1;break;
                                }
                        }
                    else if(Recv_buf2[2]==85)
                        {
                            switch(k)
                                {
                                    case 4:DO0[i].state=0;break;
                                    case 5:DO1[i].state=0;break;
                                    case 6:DO2[i].state=0;break;
                                    case 7:DO3[i].state=0;break;
                                }
                        }
                    else
                        {
                            switch(k)
                                {
                                    case 4:DO0[i].state=1;break;
                                    case 5:DO1[i].state=1;break;
                                    case 6:DO2[i].state=1;break;
                                    case 7:DO3[i].state=1;break;
                                }
                        }

                }		
        }
		
		for(i=0;i<16;i++)
		{
			for(j=10;j<12;j++)
			{
				e = (output+2*i) >> 8;
				f = (output+2*i) & 0x0ff;
				Send_buf2[2] = e;
				Send_buf2[3] = f;
				send(sockfd,Send_buf2,6,0);
				recv(sockfd,Recv_buf2,MAX_NUM,0);
				uint8_t L2=Recv_buf2[2];
				
				e = (output+2*i+1) >> 8;
				f = (output+2*i+1) & 0x0ff;
				Send_buf2[2] = e;
				Send_buf2[3] = f;
				send(sockfd,Send_buf2,6,0);
				recv(sockfd,Recv_buf2,MAX_NUM,0);
				uint8_t H2=Recv_buf2[2];
				
				VAL2=(H2<<8)|L2;
				U2=(float)VAL2*(5000/65536);
				I2=U2/250;
				close(sockfd);
				sockfd=Initialization(sockfd,&local);
				switch(j)
				{
					case 10:AO0[i].Udata=U2, AO0[i].Idata=I2;break;
					case 11:AO1[i].Udata=U2, AO1[i].Idata=I2;break;
				}
				
			}
			
		}
    }
    pthread_mutex_unlock(&mutex);
    close(sockfd1);
}

void Write()
{
    printf("enter into Write\n");
	int num0[LINE_NUM]={0},num1[LINE_NUM]={0},num2[LINE_NUM]={0},num3[LINE_NUM]={0};
	float num4[LINE_NUM]={0.0},num5[LINE_NUM]={0.0},num6[LINE_NUM]={0.0},num7[LINE_NUM]={0.0};
    int i=0,j=4;
    ReadDataDO();
    for(i=0;i<LINE_NUM;i++)
    {
        num0[i]=DO0[i].state;
    }
    for(i=0;i<LINE_NUM;i++)
    {
        num1[i]=DO1[i].state;
    }
    for(i=0;i<LINE_NUM;i++)
    {
        num2[i]=DO2[i].state;
    }
    for(i=0;i<LINE_NUM;i++)
    {
        num3[i]=DO3[i].state;
    }
    for(i=0;i<LINE_NUM;i++)
    {
        num4[i]=AO0[i].Udata;
		num5[i]=AO0[i].Idata;
    }
    for(i=0;i<LINE_NUM;i++)
    {
        num6[i]=AO1[i].Udata;
		num7[i]=AO1[i].Idata;
    }
    while(1)
    {
        pthread_mutex_lock(&mutex);
        sockfd1=Initialization(sockfd1,&local);
        switch(j)
        {
            case 4:for(i=0;i<LINE_NUM;i++)
                    {
                        if(DO0[i].state!=num0[i])
                        {
                        SendData(DO0[i].state,output+i,j);
                        num0[i]=DO0[i].state;
                        }
                    }
                    j++;
            case 5:for(i=0;i<LINE_NUM;i++)
                    {
                        if(DO1[i].state!=num1[i])
                        {
                        SendData(DO1[i].state,output+i,j);
                        num1[i]=DO1[i].state;
                        }
                    }
                    j++;
            case 6:for(i=0;i<LINE_NUM;i++)
                    {
                        if(DO2[i].state!=num2[i])
                        {
                        SendData(DO2[i].state,output+i,j);
                        num2[i]=DO2[i].state;
                        }
                    }
                    j++;
            case 7:for(i=0;i<LINE_NUM;i++)
                    {
                        if(DO3[i].state!=num3[i])
                        {
                        SendData(DO3[i].state,output+i,j);
                        num3[i]=DO3[i].state;
                        }
                    }
                    j=10;
            case 10:for(i=0;i<LINE_NUM;i++)
                    {
                        if(AO0[i].Udata!=num4[i])
                        {
							SendDataAO(AO0[i].Udata,output+2*i,j);
							num4[i]=AO0[i].Udata;
                        }
						if(AO0[i].Idata!=num5[i])
                        {
							
							SendDataAO(AO0[i].Idata*250,output+2*i,j);
							num4[i]=AO0[i].Idata;
                        }
                    }
                    j++;
            case 11:for(i=0;i<LINE_NUM;i++)
                    {
                        if(AO1[i].Udata!=num6[i])
                        {
							SendDataAO(AO1[i].Udata,output+2*i,j);
							num6[i]=AO1[i].Udata;
                        }
						if(AO1[i].Idata!=num7[i])
                        {
							
							SendDataAO(AO1[i].Idata*250,output+2*i,j);
							num7[i]=AO1[i].Idata;
                        }
                    }
                    j=4;break;
        }
        close(sockfd1);
        pthread_mutex_unlock(&mutex);
    }
}
int main()
{
		int ret,ret1,ret2,res;
		pthread_t read;
		pthread_t write;
		pthread_t opcua_server_id;
		res = pthread_mutex_init(&mutex, NULL); //init mutex 初始化互斥锁
        if (res != 0)
        {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
        }
       // ret=pthread_create(&read,NULL,(void*)ReadData,NULL);
     //   ret1=pthread_create(&write,NULL,(void*)Write,NULL);
		ret2=pthread_create(&opcua_server_id,NULL,(void *)handle_opcua_server,NULL);
		if(ret!=0||ret1!=0||ret2!=0)
        {
		printf("Create pthread error!\n");
     //   exit(1);
        }
        while(1);
        return 0;

}
