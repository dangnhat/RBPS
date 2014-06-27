/**
 *******************************************************************************
 * @file       OsMm.h
 * @version   V1.1.4
 * @date      2011.04.20
 * @brief      Header file	related to memory management
 * @details    This file including some defines and function declare related to
 *             memory management.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 *
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */


#ifndef  _MM_H
#define  _MM_H

/**< add extern C to compile with C++ */
#ifdef __cplusplus
extern "C" {
#endif
/**< add extern C to compile with C++ */

typedef struct Memory
{
    U8*   memAddr;
    U8*   freeBlock;
    U32   blockSize;
    U32   blockNum;
}MM,*P_MM;


typedef struct MemoryBlock
{
    struct MemoryBlock* nextBlock;
}MemBlk,*P_MemBlk;


extern U32  MemoryIDVessel;

/**< add extern C to compile with C++ */
#ifdef __cplusplus
}
#endif
/**< add extern C to compile with C++ */

#endif   /* _MM_H */

