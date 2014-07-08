/**
 * @file HA_Flash.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 8-12-2013
 * @brief This is header file for HA_Flash.
 * Format of Flash:
 * HA_flash_headerAddr
 *         |
 * |---- 16 ----|------- 128 -------|--- 4 -----|--- 4 -----|---- 104 ------|-- vary ---|
 * |    name    |   description     | dataLen   |   flags   |   Reversed    |   data    |
 * |------------------------------------------------------------------------------------|
 *
 * Format of flag :
 * | 0 --------------   3 |
 * | cr | -reversed       |
 * cr = 0x01 means file has been created. Otherwise, when open, new file will be create
 * (set dataLen = 0, name : default, description : default);
 *
 * beginning of file : 0 (offset from dataStartAddr).
 * end of file : dataLen, position where next byte will be written.
 *
 * Attention:
 * I assume that there is no write/read protection to STM32's flash.
 * We can use ST-link or Flash loader to remove protection.
 *
 * Buffering:
 * Flash usually can be read every one byte but it must be erased, and programmed in block, page or many byte at once.
 * For STM32 : flash can be read one byte at a time, but it must be erased whole page, and programmed two or four byte at a time.
 * Therefore, there is no buffering for read. But when read data in a page of which data is being hold in buffer,
 * data will be read from buffer instead of real flash page.
 * Buffering happens with write:
 * pageBuffer's size = flash's page size, every write operation on flash, this lib will:
 * - Read whole page pointed by putPosition pointer, then save working page's address, set flag to indicate buffer is holding a
 * page.
 * - Data will be written to right position in buffer.
 * When buffer will be flushed to flash :
 * - Write data need to be performed in different page.
 * - Close file.
 * - Call flush. (flush will do nothing if it knows buffer is not holding a page)
 */

#ifndef HA_FLASH_H_
#define HA_FLASH_H_

#include "HA_Glb.h"

namespace HA_flash_ns {

/**< config (compile-time) */
const uint32_t fileStartAddr = 0x08040000; //absolute addr of page 128.
const uint32_t fileEndAddr = 0x0805FFFF; // end of page 191.
const uint32_t fileHeaderLen = 256;
const uint32_t pageLen = 2048; // in bytes

const uint32_t fileSize_max = (fileEndAddr - fileStartAddr) + 1; // include header, unit: bytes.
const uint32_t dataStartAddr = fileStartAddr + fileHeaderLen; //absolute addr.
const uint32_t dataSize_max = fileSize_max - fileHeaderLen;
const uint32_t fileNameStartAddr = fileStartAddr;
const uint16_t fileNameLen = 16;
const uint32_t fileDescStartAddr = fileStartAddr + fileNameLen;
const uint16_t fileDescLen = 128;
const uint32_t dataLenStartAddr = fileStartAddr + fileNameLen + fileDescLen;
const uint16_t dataLenLen = 4;
const uint32_t flagsStartAddr = fileStartAddr + fileNameLen + fileDescLen + dataLenLen;
const uint16_t flagsLen = 4;
/**< config (compile-time) */

typedef enum {
    successful,
    failed,
} status_t;

typedef struct{
    char fileName[fileNameLen];
    char fileDesc[fileDescLen];
}fileInfo_s;

typedef enum : uint8_t {
    in = 0x0001,
    out = 0x0002,
    binary = 0x0004,
    ate = 0x0008,
    app = 0x0010,
    trunc = 0x0020
} mode_t;

typedef enum {
    beg, // offset from the beginning of file.
    cur, // offset from current position of get of put position
    end, // offset from the end of file (end of file = dataLen + 1, position of next byte to be written)
} direction_t;

};

class HA_flash {

public:
    HA_flash (void);

    HA_flash_ns::status_t open (const char *fileName, const char *fileDesc, HA_flash_ns::mode_t mode);
    HA_flash_ns::status_t open (HA_flash_ns::mode_t mode);
    HA_flash_ns::status_t close (void);

    /**< extra function */
    void fileInfo_write (HA_flash_ns::fileInfo_s &aStruct); //TODO
    void fileInfo_read (HA_flash_ns::fileInfo_s &aStruct); //TODO
    /**< extra function */

    /**< binary mode */
    uint32_t write (uint8_t *memblock, uint32_t len);
    uint32_t read (uint8_t *memblock, uint32_t len);
    /**< binary mode */

    uint32_t tellp (void);
    uint32_t tellg (void);
    void seekp (uint32_t position);
    void seekg (uint32_t position);
    void seekp (int64_t offset, HA_flash_ns::direction_t direction);
    void seekg (int64_t offset, HA_flash_ns::direction_t direction);

    void flush (void);

private:
    uint8_t pageBuffer[HA_flash_ns::pageLen];
    uint32_t putPosition, getPosition, position_max;

    /**< errcode */
    HA_flash_ns::status_t errcode;

    /**< flags */
    bool file_isOpen;
    bool inFlag, outFlag, appFlag, ateFlag, binaryFlag, truncFlag; // modes

    /**< functions */
    void dataLen_write (uint32_t newDataLen);
    uint32_t dataLen_read (void);

    /****** physical layer **************************************/
    bool buffer_isHoldingPage;
    uint32_t buffer_holdingPageAddr; //start addr of holding-page in buffer.

    HA_flash_ns::status_t buffer_writeToPage (void);
    HA_flash_ns::status_t buffer_readFromPage (uint32_t pageAddr);
    uint8_t byte_read (uint32_t startAddr);

    /****** physical layer **************************************/
};

#endif /* HA_FLASH_H_ */

