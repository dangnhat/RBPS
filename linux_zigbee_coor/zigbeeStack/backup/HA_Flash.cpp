/**
 * @file HA_Flash.cpp
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 8-12-2013
 * @brief This is source file for HA_Flash.
 *
 * Format of Flash:
 * HA_flash_headerAddr
 *         |
 * |---- 16 ----|------- 128 -------|--- 4 -----|--- 4 -----|---- 104 ------|-- vary ---|
 * |    name    |   description     | dataLen   |   flags   |   Reversed    |   data    |
 * |------------------------------------------------------------------------------------|
 *
 *
 * Format of flag :
 * | 0 -| 1  ---------   3 |
 * | CR |    -reversed     |
 * CR = 0x01 means file has been created. Otherwise, when open, new file will be created
 * (set dataLen = 0, name : default, description : default);
 *
 * file format:
 * dataStartAddr ----> position: 0 1 ..... dataLen-1 dataLen (position = dataLen is the end of file position)
 *
 * Attention:
 * I assume that there is no write/read protection to STM32's flash.
 * We can use ST-link or Flash loader to remove protection.
 */

#include "HA_Flash.h"

using namespace HA_flash_ns;

/**
 * @brief HA_flash constructor. Init private vars.
 * putPosition = 0, getPosition = 0, position_max = 0.
 * errcode = successful,
 * inFlag = true, outFlag = true, binaryFlag = false, ateFlag = false, appFlag = false, truncFlag = false,
 * fileIsOpen = false.
 * buffer_isHoldingPage = false;
 * @return void
 */
HA_flash::HA_flash(void){

    putPosition = 0; getPosition = 0; position_max = 0;
    errcode = successful;
    inFlag = true; outFlag = true; binaryFlag = false; ateFlag = false; appFlag = false; truncFlag = false;
    file_isOpen = false;
    buffer_isHoldingPage = false;

    return;
}

/**
 * @brief open, open file with name and description.
 * @param char *fileName : name string, maximum is 15 characters.
 * @param char *fileDesc : description string, maximum is 127 characters.
 * @param HA_flash_ns::mode_t mode : mode to open file.
 * @return HA_flash_ns::status_t
 * This function will write name string (only first 15 characters) (included '\0')
 * and write description string to flash (only first 127 characters) (included '\0').
 * Then set some setting based on mode:
 * mode == in : can read from file.
 * mode == out: can write to file.
 * mode == binary : open file in binary mode, can use write and read member function.
 * mode == ate : set init position of get and put to the end of file instead of the beginning.
 * mode == app : append mode, output to file append to the end of file. (always write at the end of file)
 * mode == trunc : overwrite old data in file. (set file length to 0)
 * @attention
 * This function will overwrite old name and desc in flash.
 * Update position_max = dataLen.
 * app mode have not been implemented yet
 */
HA_flash_ns::status_t HA_flash::open (const char *fileName, const char *fileDesc, HA_flash_ns::mode_t mode){
    /**< declare some vars */
    uint16_t temp, aCount;
    bool tempFlag;

    /**< check fileName and fileDesc */
    if ( (fileName == NULL) || (fileDesc == NULL) )
        return failed;

    /**< set flags */
    for (aCount = 0; aCount < 8; aCount++){
        tempFlag = ( (((uint8_t) mode) & (0x01 << aCount)) == 0x0000 ) ? false : true;

        switch ( aCount ){
        case 0:
            inFlag = tempFlag;
            break;
        case 1:
            outFlag = tempFlag;
            break;
        case 2:
            binaryFlag = tempFlag;
            break;
        case 3:
            ateFlag = tempFlag;
            break;
        case 4:
            appFlag = tempFlag;
            break;
        case 5:
            truncFlag = tempFlag;
            break;

        default:
            break;
        }
    }

    /**< write name and description to flash and set dataLen to 0 if truncFlag = true */
    if (buffer_readFromPage (fileStartAddr) != successful)
        return failed;

    /**< if CR flag != 0x01 or truncFlag = true then set dataLen to 0 */
    if ((pageBuffer[flagsStartAddr - fileStartAddr] != 0x01) || (truncFlag == true)){//file not created
        pageBuffer[dataLenStartAddr - fileStartAddr] = 0x00;
        pageBuffer[dataLenStartAddr + 1 - fileStartAddr] = 0x00;
        pageBuffer[dataLenStartAddr + 2 - fileStartAddr] = 0x00;
        pageBuffer[dataLenStartAddr + 3 - fileStartAddr] = 0x00;

        if (pageBuffer[flagsStartAddr - fileStartAddr] != 0x01){
            pageBuffer[flagsStartAddr - fileStartAddr] = 0x01;
        }
    }

    temp = strlen (fileName);
    if (temp > (fileNameLen-1) ){
        memcpy (pageBuffer + fileNameStartAddr - fileStartAddr, fileName, fileNameLen-1);
        pageBuffer[fileNameStartAddr - fileStartAddr + fileNameLen] = 0;
    }
    else{
        memcpy (pageBuffer + fileNameStartAddr - fileStartAddr, fileName, temp);
        pageBuffer[fileNameStartAddr - fileStartAddr + temp] = 0;
    }

    temp = strlen (fileDesc);
    if (temp > (fileDescLen-1) ){
        memcpy (pageBuffer + fileDescStartAddr - fileStartAddr, fileDesc, fileDescLen-1);
        pageBuffer[fileDescStartAddr - fileStartAddr + fileDescLen] = 0;
    }
    else{
        memcpy (pageBuffer + fileDescStartAddr - fileStartAddr, fileDesc, temp);
        pageBuffer[fileDescStartAddr - fileStartAddr + temp] = 0;
    }

    if (buffer_writeToPage()!= successful)
        return failed;

    /**< update positions */
    position_max = dataLen_read();
    putPosition = ateFlag ? dataLen_read() : 0;
    getPosition = ateFlag ? dataLen_read() : 0;

    /**< set file_isOpen */
    file_isOpen = true;

    return successful;
}

/**
 * @brief open, open file without name and description.
 * @param HA_flash_ns::mode_t mode : mode to open file.
 * @return HA_flash_ns::status_t
 * This function will open file in flash, if file is not created before then it will create, write default name
 * and description to it.
 * Set some setting based on mode:
 * mode == in : can read from file.
 * mode == out: can write to file.
 * mode == binary : open file in binary mode, can use write and read member function.
 * mode == ate : set init position of get and put to the end of file instead of the beginning.
 * mode == app : append mode, output to file append to the end of file. (always write at the end of file)
 * mode == trunc : overwrite old data in file. (set file length to 0)
 * @attention
 * This function will overwrite old name and desc in flash.
 * app mode have not been implemented yet
 */
HA_flash_ns::status_t HA_flash::open (HA_flash_ns::mode_t mode){
    /**< check CR flag */
    if (byte_read(flagsStartAddr - fileStartAddr) != 0x01){ //file not created.
        return open ("default", "default", mode);
    }
    /**< file is created */
    return open ("", "", mode);
}

/**
 * @brief close, close file and flush remain data in buffer to flash.
 * @return void
 */
HA_flash_ns::status_t HA_flash::close(void){
    /**< check if file is open or not */
    if (!file_isOpen)
        return failed;

    flush ();

    /**< update dataLen */
    dataLen_write (position_max);

    file_isOpen = false;

    return successful;
}

/**
 * @brief flush, flush data in buffer to the working page.
 * @return void
 * This function will check if buffer is holding data of a page or not. If it's true then
 * it will flush data in buffer to the page which data belong to. Otherwise it will do nothing.
 */
void HA_flash::flush (void){
    /**< check if file is open or not */
    if (!file_isOpen)
        return;

    if (buffer_isHoldingPage){
        buffer_writeToPage();
    }
}

/**
 * @brief tellp, return put position.
 * @return uint32_t
 */
uint32_t HA_flash::tellp (void){
    /**< check if file is open or not */
    if (!file_isOpen)
        return 0;

    return putPosition;
}

/**
 * @brief tellg, return get position.
 * @return uint32_t
 */
uint32_t HA_flash::tellg (void){
    /**< check if file is open or not */
    if (!file_isOpen)
        return 0;

    return getPosition;
}

/**
 * @brief seekp, seek put position.
 * @param uint32_t position, new position of put. position can be [0, position_max]
 * @return uint32_t
 */
void HA_flash::seekp(uint32_t position){

    /**< check if file is open or not */
    if ( (!file_isOpen) || (!binaryFlag) )
        return;

    /**< check conditions */
    if (position > position_max)
        return;

    /**< update new position */
    putPosition = position;
}

/**
 * @brief seekp, seek put position with offset.
 * @param int64_t offset
 * @param HA_flash_ns::direction_t direction, can be one of following values:
 * + beg : beginning of file. ( 0 <= offset <= position_max )
 * + end : end of file. (offset must be <= 0, and -offset <= position_max)
 * + cur : current position. ( 0 <= current position + offset <= position_max)
 * @return uint32_t
 */
void HA_flash::seekp(int64_t offset, HA_flash_ns::direction_t direction){
    uint32_t position;
    int64_t positionInt;
    bool isOkayToSeek = false;

    /**< check if file is open or not */
    if ( (!file_isOpen) || (!binaryFlag) )
        return;

    switch (direction){
    case beg:
        if ( (offset >= 0) && (offset <= (int64_t) position_max) ){
            position = (uint32_t) offset;
            isOkayToSeek = true;
        }
        break;
    case cur:
        positionInt = ((int64_t) putPosition) + offset;
        if ( (positionInt >= 0) && (positionInt < (int64_t) position_max) ){
            position = (uint32_t) positionInt;
            isOkayToSeek = true;
        }
        break;
    case end:
        if ( (offset <= 0) && ( (-offset) <= (int64_t) position_max ) ){
            position = (uint32_t) ((int64_t) position_max + offset);
            isOkayToSeek = true;
        }
        break;
    }

    if (isOkayToSeek)
        seekp (position);
}

/**
 * @brief seekg, seek get position.
 * @param uint32_t position, new position of get. position can be [0, position_max]
 * @return uint32_t
 */
void HA_flash::seekg (uint32_t position){
    /**< check if file is open or not */
    if ( (!file_isOpen) || (!binaryFlag) )
        return;

    /**< check conditions */
    if (position > position_max)
        return;

    /**< update new position */
    getPosition = position;
}

/**
 * @brief seekg, seek get position with offset.
 * @param int64_t offset
 * @param HA_flash_ns::direction_t direction, can be one of following values:
 * + beg : beginning of file. ( 0 <= offset <= position_max )
 * + end : end of file. (offset must be <= 0, and -offset <= position_max)
 * + cur : current position. ( 0 <= current position + offset <= position_max)
 * @return void.
 */
void HA_flash::seekg(int64_t offset, HA_flash_ns::direction_t direction){
    uint32_t position;
    int64_t positionInt;
    bool isOkayToSeek = false;

    /**< check if file is open or not */
    if ( (!file_isOpen) || (!binaryFlag) )
        return;

    switch (direction){
    case beg:
        if ( (offset >= 0) && (offset <= (int64_t) position_max) ){
            position = (uint32_t) offset;
            isOkayToSeek = true;
        }
        break;
    case cur:
        positionInt = ((int64_t) getPosition) + offset;
        if ( (positionInt >= 0) && (positionInt < (int64_t) position_max) ){
            position = (uint32_t) positionInt;
            isOkayToSeek = true;
        }
        break;
    case end:
        if ( (offset <= 0) && ( (-offset) <= (int64_t) position_max ) ){
            position = (uint32_t) ((int64_t) position_max + offset);
            isOkayToSeek = true;
        }
        break;
    }

    if (isOkayToSeek)
        seekg (position);
}

/**
 * @brief write, write a memory block to file begin from putPosition. (buffered)
 * @param uint8_t * memblock : pointer to memory block.
 * @param uint32_t len : number of byte to write.
 * @return uint32_t : actual number of bytes has been written.
 * @attention This function will not check the length of memblock, so if len > actual length of memblock, it will result in
 * hard fault.
 */
uint32_t HA_flash::write (uint8_t *memblock, uint32_t len){
    uint32_t currentPageAddr = ((dataStartAddr + putPosition) / pageLen) * pageLen;
    uint16_t aCount = 0;

    /**< check condition */
    if ((!file_isOpen) || (!binaryFlag) || (!outFlag))
        return 0;

    /**< check if we need to change holding-page in buffer or not */
    if (!buffer_isHoldingPage) { //need to change holding-page in buffer.
        if (buffer_readFromPage(currentPageAddr) != successful)
            return 0;
    }
    else if (currentPageAddr != buffer_holdingPageAddr){
        if (buffer_writeToPage() != successful)
            return 0;

        if (buffer_readFromPage(currentPageAddr) != successful)
            return 0;
    }

    /**< okay, buffer is holding current working page  */

    /**< write data to buffer */
    for (aCount = 0; aCount < len; aCount++){
        pageBuffer[putPosition + dataStartAddr - currentPageAddr] = memblock[aCount];

        putPosition++;

        /**< check if putPosition > dataSize_max */
        if (putPosition >= dataSize_max)
            return aCount + 1;

        /**< update position_max if needed */
        if (putPosition > position_max)
            position_max = putPosition;

        /**< change holding page if needed */
        currentPageAddr = ((dataStartAddr + putPosition) / pageLen) * pageLen;
        if (currentPageAddr != buffer_holdingPageAddr){
            if (buffer_writeToPage() != successful)
                return aCount + 1;
            if (buffer_readFromPage(currentPageAddr) != successful)
                return aCount + 1;
        }
    }// end for all data has been write to buffer or flash. (dataLen is not updated yet).

    return aCount;
}


/**
 * @brief read, read data from file begin with getPosition to memblock.
 * @param uint8_t * memblock : pointer to memory block.
 * @param uint32_t len : number of byte to read.
 * @return uint32_t : actual number of bytes has been read.
 * @attention
 * This function will not check the length of memblock, so if len > actual length of memblock, it will result in
 * hard fault.
 * Only read to position_max.
 * If read perform on a page of which data is being hold in buffer, then data will be read from buffer instead of
 * real flash page.
 */
uint32_t HA_flash::read (uint8_t *memblock, uint32_t len){
    uint16_t aCount;
    uint32_t pageReadAddr = ( (getPosition + dataStartAddr) / pageLen) * pageLen;

    /**< check condition */
    if ((!file_isOpen) || (!binaryFlag) || (!inFlag))
        return 0;

    /**< read data to memblock */
    for (aCount = 0; aCount < len; aCount++){
        if ( (buffer_isHoldingPage) && (pageReadAddr == buffer_holdingPageAddr) ) // Read data in buffer instead of flash page.
            memblock[aCount] = pageBuffer[getPosition + dataStartAddr - pageReadAddr];
        else
            memblock[aCount] = byte_read(getPosition + dataStartAddr);

        getPosition++;
        pageReadAddr = ( (getPosition + dataStartAddr) / pageLen) * pageLen;

        if (getPosition >= position_max)
            return aCount + 1;
    }

    return aCount;
}


/***************************** PRIVATE FUNCTIONS ************************************************************/

/**
 * @brief dataLen_write, write new data to dataLen.
 * @param uint32_t dataLen : new dataLen to be written;
 * @return void
 * This function will call flush to flush data in buffer to flash if buffer is holding data of a page.
 * Then read whole page which hold header, update dataLen and flush buffer to flash.
 */
void HA_flash::dataLen_write (uint32_t dataLen){

    /**< flush data in buffer to holding page */
    flush();

    /**< read whole page holding header to buffer */
    buffer_readFromPage(dataLenStartAddr);

    /**< write new dataLen */
    pageBuffer[dataLenStartAddr - fileStartAddr] = (uint8_t) dataLen;
    pageBuffer[dataLenStartAddr - fileStartAddr + 1] = (uint8_t) (dataLen >> 8);
    pageBuffer[dataLenStartAddr - fileStartAddr + 2] = (uint8_t) (dataLen >> 16);
    pageBuffer[dataLenStartAddr - fileStartAddr + 3] = (uint8_t) (dataLen >> 24);

    /**< flush data back */
    buffer_writeToPage();
 }

 /**
 * @brief dataLen_read, read dataLen.
 * @return uint32_t
 */
uint32_t HA_flash::dataLen_read (void){
    uint32_t retVal;
    uint16_t aCount;

    retVal = 0;
    for (aCount = 4; aCount > 0; aCount--){
        retVal = (retVal << 8) | (byte_read(dataLenStartAddr+aCount-1));
    }

    return retVal;
}

/**
 * @brief buffer_writeToPage, write data in buffer to page which data is holding in buffer.
 * @return HA_flash_ns::status_t
 * This function will check if buffer is holding data of a page of not. (by calling buffer_readFromPage)
 * If it's true, this func will erase page and the write all data in buffer to the same page which is read by calling
 * buffer_readFromPage.
 */
HA_flash_ns::status_t HA_flash::buffer_writeToPage (void){
    /**< declare vars */
    uint16_t aCount;
    uint32_t temp, pageAddr = buffer_holdingPageAddr;
    FLASH_Status aFlashStatus;

    /**< check condition */
    if (!buffer_isHoldingPage)
        return failed;

    /**< Unlock FPEC */
    FLASH_Unlock();

    /**< check busy */
    while (FLASH_GetFlagStatus (FLASH_FLAG_BSY) != RESET);


    /**< Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    /**< erase page */
    pageAddr = (pageAddr / pageLen) * pageLen; // pageAddr now = start addr of a page.

    aFlashStatus = FLASH_ErasePage (pageAddr);
    if (aFlashStatus != FLASH_COMPLETE)
        return failed;

    /**< write buffer to page */
    for (aCount = 0; aCount < pageLen; aCount = aCount + 2){

        temp = ((pageBuffer[aCount+1]) << 8) | (pageBuffer[aCount]);

        aFlashStatus = FLASH_ProgramHalfWord (pageAddr + aCount, temp);

        if (aFlashStatus != FLASH_COMPLETE)
            return failed;

    }/**< end for, all data in buffer programmed successful */

    /**< lock FPEC */
    FLASH_Lock();

    /**< update buffer_isHoldingPage */
    buffer_isHoldingPage = false;

    return successful;
}

/**
 * @brief buffer_readFromPage, read page data to buffer.
 * @param uint32_t pageAddr : address of a page, pageAddr in [fileStartAddr, fileEndAddr]
 * @return HA_flash_ns::status_t
 * This function will read data in pageAddr to buffer, update buffer_isHoldingPage and buffer_holdingPageAddr.
 */
status_t HA_flash::buffer_readFromPage (uint32_t pageAddr){
    /**< declare vars */
    uint16_t aCount;

    /**< check condition */
    if ( (pageAddr < fileStartAddr) || (pageAddr > fileEndAddr) )
        return failed;

    /**< Unlock FPEC */
    FLASH_Unlock();

    /**< check busy */
    while (FLASH_GetFlagStatus (FLASH_FLAG_BSY) != RESET);

    /**< Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    /**< read all data in page to buffer */
    pageAddr = (pageAddr / pageLen) * pageLen; // pageAddr = start addr of the page

    for (aCount = 0; aCount < pageLen; aCount++){
        pageBuffer[aCount] = *( (__IO uint32_t*) (pageAddr + aCount) );
    }/**< end for, all data in buffer programmed successful */

    /**< lock FPEC */
    FLASH_Lock();

    /**< update buffer_isHoldingPage and buffer_holdingPageAddr */
    buffer_isHoldingPage = true;
    buffer_holdingPageAddr = pageAddr;

    return successful;
}

/**
 * @brief byte_read, read and return one byte in flash.
 * @param uint32_t startAddr in [fileStartAddr, fileEndAddr]
 * @return uint8_t
 */
uint8_t HA_flash::byte_read (uint32_t startAddr){
    /**< check condition */
    if ( (startAddr < fileStartAddr) || (startAddr > fileEndAddr) )
        return failed;

    return *( (__IO uint32_t*) startAddr );
}

