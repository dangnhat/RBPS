/**
 * @file MB1_SPI.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 20-11-2013
 * @brief This is source file for ZbStack class.
 */

/**< includes and namespaces */
#include "ZigbeeStack.h"

using namespace ZbStack_ns;

/**< class ZbStack */

/**
  * @brief ZbStack constructor, init data members.
  * @return None.
  */
ZbStack::ZbStack (void){
    /**< init data members */
    aZNP_p = NULL;
}

/**
  * @brief init, map a ZNP object to this ZbStack instance.
  * @param CC2530ZNP *CC2530ZNP_p, pointer to a ZNP object.
  * @return ZbStack_ns::status_t.
  */
status_t ZbStack::init (CC2530ZNP *CC2530ZNP_p){
    aZNP_p = CC2530ZNP_p;

    return ZSuccess;
}

/**<----------------------- CONFIGURATION interface -----------------------*/

/**
  * @brief CONF_write, write conifguration params to CC2530 NV memory.
  * @param ZbStack_ns::ZCD_confID_t, can't use with configuration params have more than 4 bytes confValue.
  * @return ZbStack_ns::status_t.
  * @attention this function can not use with ZCD_NV_preCfgKey, ZCD_NV_userDesc. Use generic CONF_write instead.
  */
status_t ZbStack::CONF_write (ZCD_confID_t confID, uint32_t confValue){
    status_t retVal = ZFailure;
    ZCD_confValues_s confValues_s;

    switch (confID){

    /**< confID with 1 byte in confValue */
    case ZCD_NV_startupOption:
    case ZCD_NV_logicalType:
    case ZCD_NV_ZDODirectCB:
    case ZCD_NV_pollFailureRetries:
    case ZCD_NV_indirectMsgTimeout:
    case ZCD_NV_APSFrameRetries:
    case ZCD_NV_preCfgKeyEnable:
    case ZCD_NV_securityMode:
    case ZCD_NV_useDefaultTCLK:
    case ZCD_NV_passiveAckTimeout:
    case ZCD_NV_bcastDeliveryTime:
    case ZCD_NV_routeExpiryTime:

        confValues_s.len = 1;
        confValues_s.values_p [0] = (uint8_t) confValue;
        break;

    /**< confID with 2 bytes in confValue */
    case ZCD_NV_pollRate:
    case ZCD_NV_queuedPollRate:
    case ZCD_NV_responsePollRate:
    case ZCD_NV_APSAckWaitDuration:
    case ZCD_NV_bindingTime:
    case ZCD_NV_panID:
    case ZCD_NV_bcastRetries:

        confValues_s.len = 2;
        confValues_s.values_p [0] = (uint8_t) confValue;
        confValues_s.values_p [1] = (uint8_t) (confValue >> 8);
        break;

    /**< confID with 4 bytes in confValue */
    case ZCD_NV_chanList:

        confValues_s.len = 4;
        confValues_s.values_p [0] = (uint8_t) confValue;
        confValues_s.values_p [1] = (uint8_t) (confValue >> 8);
        confValues_s.values_p [2] = (uint8_t) (confValue >> 16);
        confValues_s.values_p [3] = (uint8_t) (confValue >> 24);
        break;

    default:
        return retVal;
    }


    retVal = CONF_write (confID, confValues_s);

    return retVal;
}

/**
  * @brief CONF_write, write conifguration params to CC2530 NV memory, generic function.
  * @param ZbStack_ns::ZCD_confID_t.
  * @param uZCD_confValues_s &confValue_s
  * @return ZbStack_ns::status_t.
  * @attention : this function will not change value in confValues_s struct.
  */
status_t ZbStack::CONF_write (ZbStack_ns::ZCD_confID_t confID, ZCD_confValues_s &confValues_s){
    uint8_t len = confValues_s.len;
    uint8_t *values_p = confValues_s.values_p;
    uint16_t cmd = 0x0526;
    uint8_t dataLen = len + 2;
    uint8_t dataBuffer [len+2];
    uint8_t *retDataBuffer = NULL, *retCmdBuffer = NULL;
    status_t retVal = ZFailure;

    dataBuffer[0] = confID;
    dataBuffer[1] = len;
    memcpy ( (void*) (dataBuffer+2), (const void*) values_p, len );

    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    retDataBuffer = aZNP_p->cmd_dataBuffer_get ();
    dataLen = aZNP_p->cmd_dataLen_get ();
    retCmdBuffer = aZNP_p->cmd_cmdBuffer_get ();

    if ( (dataLen == 1) && (retCmdBuffer[0] == 0x66) && (retCmdBuffer[1] == 0x05) ){
        retVal = ((status_t) retDataBuffer[0]);
    }

    return retVal;
}

/**
  * @brief CONF_read, read conifguration params from CC2530 NV memory.
  * @param ZbStack_ns::ZCD_confID_t, can't use with configuration params have more than 4 bytes confValue.
  * @param uint32_t &confValue, return configuration value.
  * @return ZbStack_ns::status_t.
  * @attention
  * + This function can not use with ZCD_NV_preCfgKey, ZCD_NV_userDesc. Use generic CONF_read instead.
  * + confValue is used as output so it will be changed after this function.
  */
status_t ZbStack::CONF_read (ZCD_confID_t confID, uint32_t &confValue){
    status_t retVal = ZFailure;

    ZCD_confValues_s confValues_s;

    if ( (confID == ZCD_NV_preCfgKey) || (confID == ZCD_NV_userDesc) )
        return ZFailure;

    retVal = CONF_read (confID, confValues_s);

    switch (confValues_s.len){
    case 1:
    case 2:
    case 4:
        confValue = 0;

        for (uint8_t i = confValues_s.len; i > 0 ; i--){
            confValue = confValue << 8;
            confValue = confValue + confValues_s.values_p [i - 1];
        }

        break;

    default:
        return ZFailure;
    }

    return retVal;
}

/**
  * @brief CONF_read, read conifguration params from CC2530 NV memory, generic function
  * @param ZbStack_ns::ZCD_confID_t.
  * @param uint8_t &len : len of data in values_p. (output value).
  * @param ZCD_confValues_s &confValues_s : output data struct.
  * @return ZbStack_ns::status_t.
  * @attention
  * + confValues_s will use as output data struct, it will be changed.
  */
status_t ZbStack::CONF_read (ZCD_confID_t confID, ZCD_confValues_s &confValues_s){
    status_t retVal = ZFailure;

    uint16_t cmd = 0x0426;
    uint8_t dataLen = 1;
    uint8_t dataBuffer[1] = {confID};

    uint8_t *retDataBuffer;
    uint8_t *retCmdBuffer;

    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    dataLen = aZNP_p->cmd_dataLen_get ();
    retDataBuffer = aZNP_p->cmd_dataBuffer_get ();
    retCmdBuffer = aZNP_p->cmd_cmdBuffer_get ();

    /**< check return values */
    if ( (retCmdBuffer[0] == 0x66) && (retCmdBuffer[1] == 0x04) && (retDataBuffer[1] == confID) ){
        retVal = (status_t) retDataBuffer[0];

        confValues_s.len = retDataBuffer[2];
        memcpy (confValues_s.values_p, retDataBuffer+3, confValues_s.len);
    }

    return retVal;
}

/**<----------------------- CONFIGURATION interface -----------------------*/

/**<----------------------- SIMPLE API interface --------------------------*/

/**
  * @brief Zb_appRegister_request, register app information to an endpoint.
  * @param ZbStack_ns::Zb_appInfo_s &aAppInfo_s.
  * @return ZbStack_ns::status_t.
  * @attention
  * + aAppInfo_s will not be changed, pass by reference only for saving time and space.
  */
status_t ZbStack::Zb_appRegister_request (Zb_appInfo_s &aAppInfo_s){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0A26;
    uint8_t len = 9 + 2 * (aAppInfo_s.inputCmdsNum + aAppInfo_s.outputCmdsNum);
    uint8_t dataBuffer [9 + 2 * (Zb_numOfCmds_max)];

    dataBuffer[0] = aAppInfo_s.appEndPoint;

    dataBuffer[1] = (uint8_t) aAppInfo_s.appProfileID;
    dataBuffer[2] = (uint8_t) (aAppInfo_s.appProfileID >> 8);

    dataBuffer[3] = (uint8_t) aAppInfo_s.deviceID;
    dataBuffer[4] = (uint8_t) (aAppInfo_s.deviceID >> 8);

    dataBuffer[5] = aAppInfo_s.deviceVersion;

    dataBuffer[7] = aAppInfo_s.inputCmdsNum;
    memcpy (dataBuffer + 8, aAppInfo_s.inputCmdsList, aAppInfo_s.inputCmdsNum);

    dataBuffer[8 + aAppInfo_s.inputCmdsNum] = aAppInfo_s.outputCmdsNum;
    memcpy (dataBuffer + 9 + aAppInfo_s.inputCmdsNum, aAppInfo_s.outputCmdsList, aAppInfo_s.outputCmdsNum);

    /**< send cmd, len, dataBuffer to ZNP */
    aZNP_p->cmd_SREQ (cmd, len, dataBuffer);

    /**< check returned SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t *retData_p = aZNP_p->cmd_dataBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0x01) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x0A) ){
        retVal = (status_t) retData_p[0];
    }

    return retVal;
}

/**
  * @brief Zb_start_request, start Zigbee stack.
  * @return ZbStack_ns::status_t.
  * @attention procedure before call Zb_start_request.
  * + Write configuration params for logical type, pan ID, channels list.
  * + call Zb_appRegister_request.
  * + Then, call Zb_start_request and wait for Zb_start_confirm.
  */
status_t ZbStack::Zb_start_request (void){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0026;

    /**< send start request to ZNP */
    aZNP_p->cmd_SREQ (cmd, 0, NULL);

    /**< check SRSP */
    uint8_t *retCmd_p;
    uint8_t retDataLen;

    retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0x00) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x00) )
        retVal = ZSuccess;

    return retVal;
}

/**
  * @brief Zb_permitJoining_request, control joining permissions for new devices (always permit joining by default).
  * @param uint16_t dest : short address of a device or broadcast address (0xFFFC : all routers and coordinator).
  * @param uint8_t timeOut : amount of time in seconds for the joining permisson to turn on.
  * (0x00 : turn off, 0xFF : turn on indefinitely)
  * @return ZbStack_ns::status_t.
  * @attention Zigbee stack in CC2530ZNP must be started before.
  */
status_t ZbStack::Zb_permitJoining_request (uint16_t dest, uint8_t timeOut){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0826;
    uint8_t len = 3;
    uint8_t dataBuffer[3];

    /**< wrap data in dataBuffer */
    dataBuffer [0] = (uint8_t) dest;
    dataBuffer [1] = (uint8_t) (dest >> 8);
    dataBuffer [2] = timeOut;

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, len, dataBuffer);

    /**< check SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t *retData_p = aZNP_p->cmd_dataBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 1) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x08) ){
        retVal = (status_t) retData_p[0];
    }

    return retVal;
}

/**
  * @brief Zb_bindDevice, create/remove a binding to another device.
  * @param bool create : true to create a binding, false to remove.
  * @param uint16_t cmdID : command ID - the identifier of the binding.
  * @param uint8_t *dest_p : pointer to array of 64 bit IEEE address (low order byte first)
  * (dest_p = NULL means unknow destination add and instead the destination device is in allow bind mode)
  * @return ZbStack_ns::status_t.
  * @attention Zigbee stack in CC2530ZNP must be started before.
  */
status_t ZbStack::Zb_bindDevice (bool create, uint16_t cmdID, uint8_t *dest_p){
    uint16_t cmd = 0x0126;
    uint8_t dataLen = 11;
    uint8_t dataBuffer[11];
    uint8_t aCount;

    status_t retVal = ZFailure;

    /**< wrap data in dataBuffer */
    dataBuffer[0] = (create == true) ? ZCD_true : ZCD_false;
    dataBuffer[1] = (uint8_t) cmdID;
    dataBuffer[2] = (uint8_t) (cmdID >> 8);

    for (aCount = 0; aCount < dataLen; aCount++){
        if (dest_p != NULL)
            dataBuffer[aCount+3] = dest_p[aCount];
        else
            dataBuffer[aCount+3] = 0;
    }

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    /**< check SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x01) )
        retVal = ZSuccess;

    return retVal;
}

/**
  * @brief Zb_allowBind, put device in allow binding mode.
  * @param uint8_t *timeOut : amount of time for device to remain in allow binding mode.
  * timeOut = 0 : set allow binding mode false.
  * timeOut > 64 : allow binding mode will be true indefinitely.
  * @return ZbStack_ns::status_t.
  * @attention Zigbee stack in CC2530ZNP must be started before.
  */
status_t ZbStack::Zb_allowBind (uint8_t timeOut){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0226;
    uint8_t dataLen = 1;
    uint8_t dataBuffer[1];

    /**< wrap data in dataBuffer */
    dataBuffer[0] = timeOut;

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    /**< check SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x02) )
        retVal = ZSuccess;

    return retVal;
}

/**
  * @brief Zb_sendData_request,send data to another device in the network.
  * @param Zb_dataStruct_s aDataStruct_s
  * + aDataStruct_s.destAddr    = 0-0xFFF7 : short address for the destination.
  *                             = 0xFFFC : group of all router and coordinator
  *                             = 0xFFFD : group of all devices with receiver turned on
  *                             = 0xFFFE : binding address
  *                             = 0xFFFF : boardcast
  * + aDataStruct_s.srcAddr is not used in this function.
  * + aDataStruct_s.cmdID : command ID to send with the message.
  * + aDataStruct_s.data_p : pointer to an array of data, 0-99 bytes without any security, 0-81 bytes with NWK security,
  * 0-64 bytes with NWK and APS security.
  * @param uint8_t handle : used to identify the send data request.
  * @param bool ack : true if requesting APS ack from destination.
  * @param uint8_t radius : means time to live in number of hops.
  * @return ZbStack_ns::status_t.
  * @attention + Zigbee stack in CC2530ZNP must be started before.
  * + dataStruct should be configured properly.
  */

status_t ZbStack::Zb_sendData_request (Zb_dataStruct_s &aDataStruct_s, uint8_t handle, bool ack, uint8_t radius){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0326;
    uint8_t dataLen = 8 + aDataStruct_s.len;
    uint8_t dataBuffer [8 + aDataStruct_s.len];
    uint8_t aCount;

    /**< wrap data in dataBuffer */
    dataBuffer[0] = (uint8_t) aDataStruct_s.destAddr;
    dataBuffer[1] = (uint8_t) (aDataStruct_s.destAddr >> 8);
    dataBuffer[2] = (uint8_t) (aDataStruct_s.cmdID);
    dataBuffer[3] = (uint8_t) (aDataStruct_s.cmdID >> 8);
    dataBuffer[4] = handle;
    dataBuffer[5] = ack ? ZCD_true : ZCD_false;
    dataBuffer[6] = radius;
    dataBuffer[7] = (uint8_t) aDataStruct_s.len;

    for (aCount = 0; aCount < aDataStruct_s.len; aCount++){
        dataBuffer[8+aCount] = aDataStruct_s.data_p[aCount];
    }

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    /**< check SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x03) )
        retVal = ZSuccess;

    return retVal;
}

/**
  * @brief Zb_deviceInfo_get, get infomation of device.
  * @param Zb_deviceInfoParams_s &aDeviceInfoParams_s : input/output struct.
  * + aDeviceInfoParams_s.infoParam : input parameter.
  * + aDeviceInfoParams_s.valueLen : output.
  * + aDeviceInfoParams_s.value [] : output.
  * @return ZbStack_ns::status_t.
  * @attention + Zigbee stack in CC2530ZNP must be started before.
  * + aDeviceInfoParams_s.infoParam should be configured properly.
  */
status_t ZbStack::Zb_deviceInfo_get (Zb_deviceInfoParams_s &aDeviceInfoParams_s){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0626;
    uint8_t dataLen = 0x01;
    uint8_t dataBuffer[1] = {aDeviceInfoParams_s.infoParam};

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, dataLen, dataBuffer);

    /**< get data in SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t *retData_p = aZNP_p->cmd_dataBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0x09) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x06)
        && (retData_p[0] == aDeviceInfoParams_s.infoParam) ){

        /**< find aDeviceInfoParams_s.valueLen */
        switch (aDeviceInfoParams_s.infoParam){
        case Zb_deviceState:
        case Zb_workingChannel:
            aDeviceInfoParams_s.valuesLen = 0x01;
            break;
        case Zb_deviceIEEEAddr:
        case Zb_parentIEEEAddr:
        case Zb_extPanID:
            aDeviceInfoParams_s.valuesLen = 0x08;
            break;
        case Zb_deviceShortAddr:
        case Zb_parentShortAddr:
        case Zb_panID:
            aDeviceInfoParams_s.valuesLen = 0x02;
            break;
        default:
            break;
        }

        /**< get aDeviceInfoParams_s.values */
        for (uint8_t aCount = 0; aCount <  8; aCount++){
            aDeviceInfoParams_s.values[aCount] = retData_p[aCount+1];
        }

        retVal = ZSuccess;
    }

    return retVal;
}


/**
  * @brief Zb_findDevice_request, find short address of a device with IEEE address.
  * @param uint8_t* searchKey_p : pointer to array holding IEEE address as a search key.
  * @return ZbStack_ns::status_t.
  * @attention + Zigbee stack in CC2530ZNP must be started before.
  * + searchKey_p must point to an array which has been allocated at least 8 bytes.
  */
status_t ZbStack::Zb_findDevice_request (uint8_t *searchKey_p){
    status_t retVal = ZFailure;
    uint16_t cmd = 0x0726;
    uint8_t dataLen = 0x08;

    /**< send cmd to ZNP */
    aZNP_p->cmd_SREQ (cmd, dataLen, searchKey_p);

    /**< check SRSP */
    uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
    uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

    if ( (retDataLen == 0x00) && (retCmd_p[0] == 0x66) && (retCmd_p[1] == 0x07) )
        retVal = ZSuccess;

    return retVal;
}

/**
  * @brief Zb_callback_get, check for new callback from CC2530, and then get data of callback to callback buffers.
  * @return ZbStack_ns::Zb_callbackType_t &aCallbackType (return by reference).
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + get data from callback buffer before call Zb_callback_get again, values callback buffers will be destroyed
  * after call Zb_callback_get.
  */

void ZbStack::Zb_callback_get (Zb_callbackType_t &aCallbackType){
    aCallbackType = Zb_noCallback;
    uint16_t retCmd;

    if (aZNP_p->cmd_isNewMessage() == true){
        /**< check new callback */
        uint8_t *retCmd_p = aZNP_p->cmd_cmdBuffer_get ();
        uint8_t *retData_p = aZNP_p->cmd_dataBuffer_get ();
        uint8_t retDataLen = aZNP_p->cmd_dataLen_get ();

        retCmd = (uint16_t) retCmd_p[1];
        retCmd = (retCmd << 8) | ( (uint16_t) retCmd_p[0] );

        switch (retCmd){
        case Zb_startConfirm:
        case Zb_stateChangeInd:
        case Zb_bindConfirm:
        case Zb_allowBindConfirm:
        case Zb_sendDataConfirm:
        case Zb_receiveDataIndication:
        case Zb_findDeviceConfirm:
            aCallbackType = (Zb_callbackType_t)retCmd;

            /**< get callback data to callback buffers */
            callbackCmdBuffer[0] = retCmd_p[0];
            callbackCmdBuffer[1] = retCmd_p[1];
            callbackDataLen = retDataLen;
            memcpy (callbackDataBuffer, retData_p, retDataLen);

            break;
        default:
            aCallbackType = Zb_otherCallBack;
            break;
        }
    }
}

/**
  * @brief Zb_startConfirm_get, get startConfirm data.
  * @return Zb_startConfirmStatus_t &status (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_startConfirm.
  */
void ZbStack::Zb_startConfirm_get (Zb_startConfirmStatus_t &status){
    status = (Zb_startConfirmStatus_t) callbackDataBuffer[0];
}

/**
  * @brief Zb_stateChangeInd_get, get stateChangeInd data.
  * @return Zb_stateChangeInd_t &stateChange (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_stateChangeInd.
  */
void ZbStack::Zb_stateChangeInd_get (Zb_stateChangeInd_t &stateChange){
    stateChange = (Zb_stateChangeInd_t) callbackDataBuffer[0];
}

/**
  * @brief Zb_bindConfirm_get, get bindConfirm data.
  * @return uint16_t &cmdID (return by reference)
  * @return status_t &status (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_bindConfirm.
  */
void ZbStack::Zb_bindConfirm_get (uint16_t &cmdID, status_t &status){
    cmdID = ( ((uint16_t) callbackDataBuffer[1]) << 8 ) | ( (uint16_t) callbackDataBuffer[0] );

    status = (status_t) callbackDataBuffer[2];
}

/**
  * @brief Zb_allowBindConfirm_get, get allowBindConfirm data.
  * @return uint16_t &source (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_allowBindConfirm.
  */
void ZbStack::Zb_allowBindConfirm_get (uint16_t &source){
    source = ( ((uint16_t) callbackDataBuffer[1]) << 8 ) | ( (uint16_t) callbackDataBuffer[0] );
}

/**
  * @brief Zb_sendDataConfirm_get, get sendDataConfirm data.
  * @return uint8_t &handle (return by reference)
  * @return status_t &status (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_sendDataConfirm.
  */
void ZbStack::Zb_sendDataConfirm_get (uint8_t &handle, status_t &status){
    handle = callbackDataBuffer[0];
    status = (status_t) callbackDataBuffer[1];
}

/**
  * @brief Zb_receiveDataIndication_get, get receiveDataIndication data.
  * @return Zb_dataStruct_s &aDataStruct_s (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_receiveDataIndication.
  * + for this function, data_p just a pointer to data in callback buffer, save data before call Zb_callback_get.
  */
void ZbStack::Zb_receiveDataIndication_get (Zb_dataStruct_s &aDataStruct_s){
    aDataStruct_s.srcAddr = ( ((uint16_t) callbackDataBuffer[1]) << 8 ) | ( (uint16_t) callbackDataBuffer[0] );
    aDataStruct_s.cmdID = ( ((uint16_t) callbackDataBuffer[3]) << 8 ) | ( (uint16_t) callbackDataBuffer[2] );
    aDataStruct_s.len = (callbackDataBuffer [5] << 8) | (callbackDataBuffer [4]);
    aDataStruct_s.data_p = callbackDataBuffer + 6; //TODO : test this.
}

/**
  * @brief Zb_findDeviceConfirm_get, get findDeviceConfirm data.
  * @return uint8_t *&searchKey_p (return by reference)
  * @attention
  * + Zigbee stack in CC2530ZNP must be started before.
  * + Zb_callback_get shoud be called before with the return value is Zb_findDeviceConfirm.
  * + for this function, searchKey_p just a pointer to data in callback buffer, save data before call Zb_callback_get
  */
void ZbStack::Zb_findDeviceConfirm_get (uint8_t *&searchKey_p, uint16_t &result){
    searchKey_p = callbackDataBuffer+3;
    result = ( ((uint16_t) callbackDataBuffer[2]) << 8 ) | ( (uint16_t) callbackDataBuffer[1] );
}

/**<----------------------- SIMPLE API interface --------------------------*/

/**< class ZbStack */
