/**
 *******************************************************************************
 * @file       OsTime.c
 * @version   V1.1.4
 * @date      2011.04.20
 * @brief      Header file related to time management
 * @details    Thie file including some data declare related to time managment.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 *
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */

#ifndef _TIME_H
#define _TIME_H

/**< add extern C to compile with C++ */
#ifdef __cplusplus
extern "C" {
#endif
/**< add extern C to compile with C++ */


/*---------------------------- Variable declare ------------------------------*/
extern P_OSTCB  DlyList;            /*!< A pointer to ther delay list.        */

/*---------------------------- Function declare ------------------------------*/
extern void  TimeDispose(void);     /*!< Time dispose function.               */
extern void  isr_TimeDispose(void);
extern void  RemoveDelayList(P_OSTCB ptcb);
extern void  InsertDelayList(P_OSTCB ptcb,U32 ticks);

/**< add extern C to compile with C++ */
#ifdef __cplusplus
}
#endif
/**< add extern C to compile with C++ */

#endif
