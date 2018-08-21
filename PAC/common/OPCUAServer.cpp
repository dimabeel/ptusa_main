﻿#include <OPCUAServer.h>
#include "log.h"
#include <stdio.h>

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

#include "rm_manager.h"

void Transliterate(const char* str, char* newStr)
    {
    for (; *str != 0; str++)
        {
        switch (str[0])
            {
            case 'а': strcat(&newStr[0], "a"); break;
            case 'б': strcat(&newStr[0], "b"); break;
            case 'в': strcat(&newStr[0], "v"); break;
            case 'г': strcat(&newStr[0], "g"); break;
            case 'д': strcat(&newStr[0], "d"); break;
            case 'е': strcat(&newStr[0], "e"); break;
            case 'ё': strcat(&newStr[0], "ye"); break;
            case 'ж': strcat(&newStr[0], "zh"); break;
            case 'з': strcat(&newStr[0], "z"); break;
            case 'и': strcat(&newStr[0], "i"); break;
            case 'й': strcat(&newStr[0], "y"); break;
            case 'к': strcat(&newStr[0], "k"); break;
            case 'л': strcat(&newStr[0], "l"); break;
            case 'м': strcat(&newStr[0], "m"); break;
            case 'н': strcat(&newStr[0], "n"); break;
            case 'о': strcat(&newStr[0], "o"); break;
            case 'п': strcat(&newStr[0], "p"); break;
            case 'р': strcat(&newStr[0], "r"); break;
            case 'с': strcat(&newStr[0], "s"); break;
            case 'т': strcat(&newStr[0], "t"); break;
            case 'у': strcat(&newStr[0], "u"); break;
            case 'ф': strcat(&newStr[0], "f"); break;
            case 'х': strcat(&newStr[0], "ch"); break;
            case 'ц': strcat(&newStr[0], "z"); break;
            case 'ч': strcat(&newStr[0], "ch"); break;
            case 'ш': strcat(&newStr[0], "sh"); break;
            case 'щ': strcat(&newStr[0], "ch"); break;
            case 'ъ': strcat(&newStr[0], "''"); break;
            case 'ы': strcat(&newStr[0], "y"); break;
            case 'ь': strcat(&newStr[0], "''"); break;
            case 'э': strcat(&newStr[0], "e"); break;
            case 'ю': strcat(&newStr[0], "yu"); break;
            case 'я': strcat(&newStr[0], "ya"); break;
            case 'А': strcat(&newStr[0], "A"); break;
            case 'Б': strcat(&newStr[0], "B"); break;
            case 'В': strcat(&newStr[0], "V"); break;
            case 'Г': strcat(&newStr[0], "G"); break;
            case 'Д': strcat(&newStr[0], "D"); break;
            case 'Е': strcat(&newStr[0], "E"); break;
            case 'Ё': strcat(&newStr[0], "Ye"); break;
            case 'Ж': strcat(&newStr[0], "Zh"); break;
            case 'З': strcat(&newStr[0], "Z"); break;
            case 'И': strcat(&newStr[0], "I"); break;
            case 'Й': strcat(&newStr[0], "Y"); break;
            case 'К': strcat(&newStr[0], "K"); break;
            case 'Л': strcat(&newStr[0], "L"); break;
            case 'М': strcat(&newStr[0], "M"); break;
            case 'Н': strcat(&newStr[0], "N"); break;
            case 'О': strcat(&newStr[0], "O"); break;
            case 'П': strcat(&newStr[0], "P"); break;
            case 'Р': strcat(&newStr[0], "R"); break;
            case 'С': strcat(&newStr[0], "S"); break;
            case 'Т': strcat(&newStr[0], "T"); break;
            case 'У': strcat(&newStr[0], "U"); break;
            case 'Ф': strcat(&newStr[0], "F"); break;
            case 'Х': strcat(&newStr[0], "Ch"); break;
            case 'Ц': strcat(&newStr[0], "Z"); break;
            case 'Ч': strcat(&newStr[0], "Ch"); break;
            case 'Ш': strcat(&newStr[0], "Sh"); break;
            case 'Щ': strcat(&newStr[0], "Ch"); break;
            case 'Ъ': strcat(&newStr[0], "''"); break;
            case 'Ы': strcat(&newStr[0], "Y"); break;
            case 'Ь': strcat(&newStr[0], "''"); break;
            case 'Э': strcat(&newStr[0], "E"); break;
            case 'Ю': strcat(&newStr[0], "Yu"); break;
            case 'Я': strcat(&newStr[0], "Ya"); break;
            default: { char Temp[2] = { str[0], 0 }; strcat(&newStr[0], &Temp[0]); }
            }
        }
    }

/* predefined identifier for later use */
UA_NodeId folderDevicesTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1001 } };
UA_NodeId folderPumpsTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1002 } };
UA_NodeId folderValvesTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1003 } };
UA_NodeId pumpTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1101 } };
UA_NodeId valveTypeId = { 1, UA_NODEIDTYPE_NUMERIC,{ 1102 } };



void OPCUAServer::Init(int port)
{
    config = UA_ServerConfig_new_default();
    nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_default, 4840, NULL);
    config->networkLayers = &nl;
    config->networkLayersSize = 1;
    server = UA_Server_new(config);
   
}


void OPCUAServer::UserInit()
    {
    //G_LUA_MANAGER->void_exec_lua_method(0, "OPCUA_userinit", 0);
    lua_State* L = G_LUA_MANAGER->get_Lua();
    lua_getfield(L, LUA_GLOBALSINDEX, "OPCUA_userinit");
    if (lua_isfunction(L, -1))
        {
        lua_pcall(L, 0, 0, 0);
        }
    else
        {
        lua_remove(L, -1);
        }
    }

UA_StatusCode OPCUAServer::Start()
{
    return UA_Server_run_startup(server);
}

void OPCUAServer::Shutdown()
{
    UA_Server_run_shutdown(server);
    UA_Server_delete(server);
    nl.deleteMembers(&nl);
}

void OPCUAServer::Evaluate()
{
    UA_Server_run_iterate(server, true);
}


void OPCUAServer::BaseConfig(bool readonly)
{
    CreateFolders();
    CreateDeviceTypes(readonly);
    
}

///Создаем структуру папок.
void OPCUAServer::CreateFolders()
    {
    //папка Objects\Devices
    UA_ObjectAttributes devicesFolderAttr = UA_ObjectAttributes_default;
    devicesFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Devices");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Devices"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        devicesFolderAttr, NULL, &folderDevicesTypeId);
    //папка Objects\Devices\Pumps
    UA_ObjectAttributes pumpsFolderAttr = UA_ObjectAttributes_default;
    pumpsFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Pumps");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderDevicesTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Pumps"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        pumpsFolderAttr, NULL, &folderPumpsTypeId);
    //папка Objects\Devices\Valves
    UA_ObjectAttributes valvesFolderAttr = UA_ObjectAttributes_default;
    valvesFolderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Valves");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderDevicesTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "Valves"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        valvesFolderAttr, NULL, &folderValvesTypeId);
    }

void OPCUAServer::CreateDeviceTypes(bool readonly)
{
    /* Объявляем базовый класс устройства */
    UA_NodeId deviceTypeId; /* get the nodeid assigned by the server */
    UA_ObjectTypeAttributes dtAttr = UA_ObjectTypeAttributes_default;
    dtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "DeviceType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "DeviceType"), dtAttr,
        NULL, &deviceTypeId);
    /* Добавляем поле "Description" - описание устройства*/
    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Description");
    UA_NodeId deviceDescription;
    UA_Server_addVariableNode(server, UA_NODEID_NULL, deviceTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Description"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, &deviceDescription);
    /* Делаем поле обязательным */
    UA_Server_addReference(server, deviceDescription,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    //====================================================================
    /* Объявление подкласса - мотор/насос "PumpType" */
    UA_ObjectTypeAttributes ptAttr = UA_ObjectTypeAttributes_default;
    ptAttr.displayName = UA_LOCALIZEDTEXT("en-US", "PumpType");
    UA_Server_addObjectTypeNode(server, pumpTypeId,
        deviceTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "PumpType"), ptAttr,
        NULL, NULL);

    UA_DataSource stateDataSource;
    stateDataSource.read = readState;
    stateDataSource.write = writeState;
    UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
    statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", "State");
    statusAttr.valueRank = -1;
    if (!readonly) statusAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_NodeId statusId;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, pumpTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "State"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), statusAttr, stateDataSource, NULL, &statusId);
    /* Make the status variable mandatory */
    UA_Server_addReference(server, statusId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    
  
    UA_DataSource frequencyDataSource;
    frequencyDataSource.read = readValue;
    frequencyDataSource.write = writeValue;
    UA_VariableAttributes rpmAttr = UA_VariableAttributes_default;
    rpmAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Frequency");
    rpmAttr.valueRank = -1;
    if (!readonly) rpmAttr.accessLevel = UA_ACCESSLEVELMASK_WRITE + UA_ACCESSLEVELMASK_READ;
    UA_NodeId motorRpm;
    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL, pumpTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Frequency"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), rpmAttr, frequencyDataSource, NULL, &motorRpm);
    ///* Make the status variable mandatory */
    UA_Server_addReference(server, motorRpm,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    UA_NodeTypeLifecycle lifecycle;
    lifecycle.constructor = pumpTypeConstructor;
    lifecycle.destructor = NULL;
    UA_Server_setNodeTypeLifecycle(server, pumpTypeId, lifecycle);

    }

void OPCUAServer::addPumpObject(device * dev)
    {
    char* name = strdup(dev->get_name());
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        folderPumpsTypeId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, name),
        pumpTypeId, /* this refers to the object type
                    identifier */
        oAttr, dev, NULL);
    free(name);
    }

UA_StatusCode OPCUAServer::readState(UA_Server * server, const UA_NodeId * sessionId, void * sessionContext, const UA_NodeId * nodeId, void * nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange * range, UA_DataValue * dataValue)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        UA_Int32 state = dev->get_state();
        UA_Variant_setScalarCopy(&dataValue->value, &state, &UA_TYPES[UA_TYPES_INT32]);
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeState(UA_Server *server,
    const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *nodeId, void *nodeContext,
    const UA_NumericRange *range,
    const UA_DataValue *value)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        if (value->hasValue)
            {
            UA_Int32 state = ((UA_Int32*)(value->value.data))[0];
            dev->set_state(state);
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::readValue(UA_Server * server, const UA_NodeId * sessionId, void * sessionContext, const UA_NodeId * nodeId, void * nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange * range, UA_DataValue * dataValue)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        UA_Float newvalue = dev->get_value();
        UA_Variant_setScalarCopy(&dataValue->value, &newvalue, &UA_TYPES[UA_TYPES_FLOAT]);
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::writeValue(UA_Server *server,
    const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *nodeId, void *nodeContext,
    const UA_NumericRange *range,
    const UA_DataValue *value)
    {
    if (nodeContext != nullptr)
        {
        device* dev = (device*)nodeContext;
        if (value->hasValue)
            {
            UA_Float newvalue = ((UA_Float*)(value->value.data))[0];
            dev->set_value(newvalue);
            }
        }
    return UA_STATUSCODE_GOOD;
    }

UA_StatusCode OPCUAServer::pumpTypeConstructor(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext, const UA_NodeId *typeId, void *typeContext, const UA_NodeId *nodeId, void **nodeContext)
    {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "New pump created");

    device* dev = (device*)(nodeContext[0]);

    /* Find the NodeId of the status child variable */
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, "State");

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = *nodeId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;

    UA_Int32 status = 0;
    UA_Variant value;
    UA_Variant_setScalar(&value, &status, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Frequency");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    UA_Server_setNodeContext(server, bpr.targets[0].targetId.nodeId, nodeContext[0]);
    UA_BrowsePathResult_deleteMembers(&bpr);

    rpe.targetName = UA_QUALIFIEDNAME(1, "Description");
    bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;
    int myddlen = strlen(dev->get_description());
    char mydd[100];
    strcpy(mydd, "");
    Transliterate(dev->get_description(), mydd);
    UA_String devdescr = UA_String_fromChars(mydd);
    UA_Variant_setScalar(&value, &devdescr, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
    UA_BrowsePathResult_deleteMembers(&bpr);

    return UA_STATUSCODE_GOOD;
    }

OPCUAServer& OPC_UA_SERV()
    {
    return OPCUAServer::getInstance();
    }


