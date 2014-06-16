/**
 * @file HA_ZigbeeStack.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 19-11-2013
 * @brief This is header file for ZbStack class.
 */

#ifndef ZIGBEESTACK_H_
#define ZIGBEESTACK_H_

#include "HA_Glb.h"
#include "CC2530ZNP.h"

namespace ZbStack_ns {
/**< ZbStack */
typedef enum : uint8_t {
    ZSuccess = 0x00,
    ZFailure = 0x01,
    ZInvalidParameter = 0x02,
    NV_ITEM_UNINIT = 0x09,
    NV_OPER_FAILED = 0x0a,
    NV_BAD_ITEM_LEN = 0x0c,
    ZMemError = 0x10,
    ZBufferFull = 0x11,
    ZUnsupportedMode = 0x12,
    ZMacMemError = 0x13,
    zdoInvalidEndpoint = 0x82,
    zdoUnsupported = 0x84,
    zdoTimeout = 0x85,
    zdoNoMatch = 0x86,
    zdoTableFull = 0x87,
    zdoNoBindEntry = 0x88,
    ZSecNoKey = 0xa1,
    ZSecMaxFrmCount = 0xa3,
    ZApsFail = 0xb1,
    ZApsTableFull = 0xb2,
    ZApsIllegalRequest = 0xb3,
    ZApsInvalidBinding = 0xb4,
    ZApsUnsupportedAttrib = 0xb5,
    ZApsNotSupported = 0xb6,
    ZApsNoAck = 0xb7,
    ZApsDuplicateEntry = 0xb8,
    ZApsNoBoundDevice = 0xb9,
    ZNwkInvalidParam = 0xc1,
    ZNwkInvalidRequest = 0xc2,
    ZNwkNotPermitted = 0xc3,
    ZNwkStartupFailure = 0xc4,
    ZNwkTableFull = 0xc7,
    ZNwkUnknownDevice = 0xc8,
    ZNwkUnsupportedAttribute = 0xc9,
    ZNwkNoNetwork = 0xca,
    ZNwkLeaveUnconfirmed = 0xcb,
    ZNwkNoAck = 0xcc,
    ZNwkNoRoute = 0xcd,
    ZMacNoACK = 0xe9
} status_t;

/**< ZbStack */

/**< configuration interface */
const uint8_t lenOfValuesBuff_max = 17;

typedef enum : uint8_t {
    ZCD_NV_startupOption = 0x03,
    ZCD_NV_logicalType = 0x87,
    ZCD_NV_ZDODirectCB = 0x8F,
    ZCD_NV_pollRate = 0x24,
    ZCD_NV_queuedPollRate = 0x25,
    ZCD_NV_responsePollRate = 0x26,
    ZCD_NV_pollFailureRetries = 0x29,
    ZCD_NV_indirectMsgTimeout = 0x2B,
    ZCD_NV_APSFrameRetries = 0x43,
    ZCD_NV_APSAckWaitDuration = 0x44,
    ZCD_NV_bindingTime = 0x46,
    ZCD_NV_userDesc = 0x81,
    ZCD_NV_panID = 0x83,
    ZCD_NV_chanList = 0x84,
    ZCD_NV_preCfgKey = 0x62,
    ZCD_NV_preCfgKeyEnable = 0x63,
    ZCD_NV_securityMode = 0x64,
    ZCD_NV_useDefaultTCLK = 0x6D,
    ZCD_NV_bcastRetries = 0x2E,
    ZCD_NV_passiveAckTimeout = 0x2F,
    ZCD_NV_bcastDeliveryTime = 0x30,
    ZCD_NV_routeExpiryTime = 0x2C
} ZCD_confID_t;

typedef struct {
    uint8_t len;
    uint8_t values_p [lenOfValuesBuff_max];
} ZCD_confValues_s;

/**< configuration interface special value */
enum ZCD_startOpt_e : uint8_t {
    ZCD_startOpt_clearState = 0x02,
    ZCD_startOpt_clearConfig = 0x01,
    ZCD_startOpt_noClear = 0x00
};

enum ZCD_logicalType_e : uint8_t {
    ZCD_coordinator = 0x00,
    ZCD_router = 0x01,
    ZCD_endDevice = 0x02,
};

enum ZCD_bool_e : uint8_t{
    ZCD_true = 0x01,
    ZCD_false = 0x00,
};

enum ZCD_sec_e : uint8_t{
    ZCD_sec_on = 0x01,
    ZCD_sec_off = 0x00,
};

enum ZCD_chanList_e : uint32_t {
    ZCD_chanList_11 = 0x00000800,
    ZCD_chanList_12 = 0x00001000,
    ZCD_chanList_13 = 0x00002000,
    ZCD_chanList_14 = 0x00004000,
    ZCD_chanList_15 = 0x00008000,
    ZCD_chanList_16 = 0x00010000,
    ZCD_chanList_17 = 0x00020000,
    ZCD_chanList_18 = 0x00040000,
    ZCD_chanList_19 = 0x00080000,
    ZCD_chanList_20 = 0x00100000,
    ZCD_chanList_21 = 0x00200000,
    ZCD_chanList_22 = 0x00400000,
    ZCD_chanList_23 = 0x00800000,
    ZCD_chanList_24 = 0x01000000,
    ZCD_chanList_25 = 0x02000000,
    ZCD_chanList_26 = 0x04000000,
    ZCD_chanList_allChannels = 0x07FFF800,
    ZCD_chanList_none = 0x00000000
};
/**< configuration interface */

/**< simple API interface (Zb_ prefix) */
const uint8_t Zb_numOfCmds_max = 16;

typedef struct {
    uint8_t appEndPoint;
    uint16_t appProfileID;
    uint16_t deviceID;
    uint8_t deviceVersion;

    uint8_t inputCmdsNum; // < Zb_numOfCmds_max
    uint16_t inputCmdsList [Zb_numOfCmds_max]; //TODO: clarify this, LSB first ???, low order byte first ?
    uint8_t outputCmdsNum;
    uint16_t outputCmdsList [Zb_numOfCmds_max];

} Zb_appInfo_s;

typedef struct {
    uint16_t destAddr; // use this when send data
    uint16_t srcAddr; // use this when receive data
    uint16_t cmdID;
    uint16_t len;
    uint8_t *data_p;
}Zb_dataStruct_s;

typedef enum : uint8_t {
    Zb_deviceState = 0x00,
    Zb_deviceIEEEAddr = 0x01,
    Zb_deviceShortAddr = 0x02,
    Zb_parentShortAddr = 0x03,
    Zb_parentIEEEAddr = 0x04,
    Zb_workingChannel = 0x05,
    Zb_panID = 0x06,
    Zb_extPanID = 0x07
} Zb_deviceInfo_t;

typedef struct {
    Zb_deviceInfo_t infoParam; // input param

    uint8_t valuesLen;   // output param
    uint8_t values[8];  // output param, low order byte first
} Zb_deviceInfoParams_s;

    /**< for callbacks */
typedef enum : uint16_t{
    Zb_noCallback = 0x0000,
    Zb_startConfirm = 0x8046,
    Zb_stateChangeInd = 0xC045,
    Zb_bindConfirm = 0x8146,
    Zb_allowBindConfirm = 0x8246,
    Zb_sendDataConfirm = 0x8346,
    Zb_receiveDataIndication = 0x8746,
    Zb_findDeviceConfirm = 0x8546,
    Zb_otherCallBack = 0xFFFF
}Zb_callbackType_t;

typedef enum : uint8_t {
    Zb_success = 0x00,
    Zb_init = 0x22,
}Zb_startConfirmStatus_t;

typedef enum : uint8_t {
    Zb_devHold = 0x00,
    Zb_devInit = 0x01,
    Zb_devNWKDisc = 0x02,
    Zb_devNWKJoining = 0x03,
    Zb_devNWKRejoin = 0x04,
    Zb_devEndDeviceUnauth = 0x05,
    Zb_devEndDevice = 0x06,
    Zb_devRouter = 0x07,
    Zb_devCoordStarting = 0x08,
    Zb_devZBCoord = 0x09,
    Zb_devNWKOrphan = 0x0A,
}Zb_stateChangeInd_t;
    /**< for callbacks */

/**< simple API interface */

}

/**< class ZbStack */

class ZbStack {
public :
    ZbStack ();
    ZbStack_ns::status_t init (CC2530ZNP *CC2530ZNP_p);

    /**< configuration interface */

    ZbStack_ns::status_t CONF_write (ZbStack_ns::ZCD_confID_t confID, uint32_t confValue);
    ZbStack_ns::status_t CONF_write (ZbStack_ns::ZCD_confID_t confID, ZbStack_ns::ZCD_confValues_s &confValues_s);

    ZbStack_ns::status_t CONF_read (ZbStack_ns::ZCD_confID_t confID, uint32_t &confValue);
    ZbStack_ns::status_t CONF_read (ZbStack_ns::ZCD_confID_t confID, ZbStack_ns::ZCD_confValues_s &confValues_s);
    /**< configuration interface */

    /**< simple API interface */
    ZbStack_ns::status_t Zb_appRegister_request (ZbStack_ns::Zb_appInfo_s &aAppInfo_s);
    ZbStack_ns::status_t Zb_start_request (void);

    ZbStack_ns::status_t Zb_permitJoining_request (uint16_t dest, uint8_t timeOut);
    ZbStack_ns::status_t Zb_bindDevice (bool create, uint16_t cmdID, uint8_t *dest_p);
    ZbStack_ns::status_t Zb_allowBind (uint8_t timeOut);
    ZbStack_ns::status_t Zb_sendData_request (ZbStack_ns::Zb_dataStruct_s &aDataStruct_s, uint8_t handle, bool ack, uint8_t radius);
    ZbStack_ns::status_t Zb_deviceInfo_get (ZbStack_ns::Zb_deviceInfoParams_s &aDeviceInfoParams_s);
    ZbStack_ns::status_t Zb_findDevice_request (uint8_t *searchKey_p);

    void Zb_callback_get (ZbStack_ns::Zb_callbackType_t &aCallbackType);
    void Zb_startConfirm_get (ZbStack_ns::Zb_startConfirmStatus_t &status);
    void Zb_stateChangeInd_get (ZbStack_ns::Zb_stateChangeInd_t &stateChange);
    void Zb_bindConfirm_get (uint16_t &cmdID, ZbStack_ns::status_t &status);
    void Zb_allowBindConfirm_get (uint16_t &source);
    void Zb_sendDataConfirm_get (uint8_t &handle, ZbStack_ns::status_t &status);
    void Zb_receiveDataIndication_get (ZbStack_ns::Zb_dataStruct_s &aDataStruct_s);
    void Zb_findDeviceConfirm_get (uint8_t *&searchKey_p, uint16_t &result);

    /**< simple API interface */

private:
    CC2530ZNP* aZNP_p;

    /**< simple API interface */
    uint8_t callbackCmdBuffer [2];
    uint8_t callbackDataLen;
    uint8_t callbackDataBuffer[100];

    /**< simple API interface */
};




#endif // ZIGBEESTACK_H_
