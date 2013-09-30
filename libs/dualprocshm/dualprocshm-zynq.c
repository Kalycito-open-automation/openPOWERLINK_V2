/**
********************************************************************************
\file   dualprocshm-zynq.c

\brief  Dual Processor Library Support File - For Zynq target

This file provides specific function definition for Zynq to support shared memory
interface using dual processor library

\ingroup module_dualprocshm
*******************************************************************************/
/*------------------------------------------------------------------------------
Copyright (c) 2012 Kalycito Infotech Private Limited
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include "dualprocshm-target.h"
#include <string.h>
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define DEFAULT_LOCK_ID             0x00
#define TARGET_LOCK                 0xFF
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static UINT8*      masterLock = (UINT8*) COMMON_MEM_BASE;

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
//------------------------------------------------------------------------------
/**
\brief  Target specific to routine to retrieve the base address of
        common memory between dual processors

\param  pSize_p      minimum size of the common memory, returns the
                     actual size of common memory
\return pointer to base address of common memory

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
UINT8* dualprocshm_getCommonMemAddr(UINT16* pSize_p)
{
    UINT8* pAddr;

    if(*pSize_p > MAX_COMMON_MEM_SIZE )
    {
        TRACE("Common memory size exceeds Size: %d Avail: %d\n",*pSize_p,MAX_COMMON_MEM_SIZE);
        return NULL;
    }

    pAddr = (UINT8 *) (COMMON_MEM_BASE + 1);

    memset(pAddr,0,MAX_COMMON_MEM_SIZE - 1);

    *pSize_p = MAX_COMMON_MEM_SIZE - 1;
    *masterLock = 0;
    return pAddr;
}
//------------------------------------------------------------------------------
/**
\brief  Target specific to routine to release the base address of
        common memory between dual processors

\param  pSize_p      size of the common memory

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_releaseCommonMemAddr(UINT16 pSize_p)
{
    // nothing to do be done on zynq
}
//------------------------------------------------------------------------------
/**
\brief  Target specific to routine to retrieve the base address for storing
        dynamic mapping table

\return pointer to base address of dynamic mapping table

\ingroup module_dualprocshm
*/
//------------------------------------------------------------------------------
UINT8* dualprocshm_getDynMapTableAddr(void)
{
    UINT8* pAddr;
    pAddr = (UINT8 *) MEM_ADDR_TABLE_BASE;

    memset(pAddr,0,(MAX_DYNAMIC_BUFF_COUNT * 4));
    return pAddr;
}
//------------------------------------------------------------------------------
/**
\brief  Target specific to routine to retrieve the base address for storing
        dynamic mapping table

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_releaseDynMapTableAddr()
{
    // nothing to be done on zynq
}
//------------------------------------------------------------------------------
/**
\brief  Target specific memory read routine

\param  pBase_p    base address to be read
\param  Size_p     No of bytes to be read
\param  pData_p    Pointer to receive the read data

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_targetReadData(UINT8* pBase_p, UINT16 Size_p, UINT8* pData_p)
{
    if(pBase_p == NULL || pData_p == NULL)
    {
        return;
    }

    DUALPROCSHM_INVALIDATE_DCACHE_RANGE((UINT32)pBase_p,Size_p);

    memcpy(pData_p,pBase_p,Size_p);

}
//------------------------------------------------------------------------------
/**
\brief  Target specific memory write routine

\param  pBase_p      base address to be written
\param  Size_p       No of bytes to write
\param  pData_p      Pointer to memory containing data to written

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_targetWriteData(UINT8* pBase_p, UINT16 Size_p, UINT8* pData_p)
{
    if(pBase_p == NULL || pData_p == NULL)
    {
        return;
    }

    memcpy(pBase_p,pData_p,Size_p);

    DUALPROCSHM_FLUSH_DCACHE_RANGE((UINT32)pBase_p,Size_p);

}
//------------------------------------------------------------------------------
/**
\brief  Target specific memory lock routine(acquire)

This routine provides support for a token based lock using the common memory.
The caller needs to pass the base address and the token for locking a resource
such as memory buffers

\param  pBase_p         base address of the lock memory
\param  lockToken_p     token to be used for locking
\param  pData_p   Pointer to memory containing data to written

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_targetAcquireLock(UINT8* pBase_p, UINT8 lockToken_p,UINT8 id)
{
    UINT8 lock = 0;
int count =0;
    if(pBase_p == NULL)
    {
        return;
    }

    // spin till the passed token is written into memory
    do{
        DUALPROCSHM_INVALIDATE_DCACHE_RANGE((UINT32)pBase_p,sizeof(UINT8));
        lock = DPSHM_READ8((UINT32)pBase_p);

        if(lock == DEFAULT_LOCK_ID)
        {
            DPSHM_WRITE8((UINT32)pBase_p,lockToken_p);
            DUALPROCSHM_FLUSH_DCACHE_RANGE((UINT32)pBase_p,sizeof(UINT8));
            continue;
        }
/*        else
        {
            if(lock != lockToken_p)
            {
                count++;
                if(count > 500)
                {
#ifdef __MICROBLAZE__
          //          printf("Lock %x %d t %x\n",*pBase_p,id,lockToken_p);
#endif
                    count = 0;

                }

            }
         //
        }*/
      }while(lock != lockToken_p);

}

//------------------------------------------------------------------------------
/**
\brief  Target specific memory lock routine(release)

This routine is used to release a lock acquired before at a address specified

\param  pBase_p         base address of the lock memory

\ingroup module_dualprocshm
 */
//------------------------------------------------------------------------------
void dualprocshm_targetReleaseLock(UINT8* pBase_p)
{
    UINT8   defaultlock = DEFAULT_LOCK_ID;
    if(pBase_p == NULL)
    {
        return;
    }
  //  printf("Ulk %x\n",*pBase_p);
    DPSHM_WRITE8((UINT32)pBase_p,defaultlock);

    DUALPROCSHM_FLUSH_DCACHE_RANGE((UINT32)pBase_p,sizeof(UINT8));
}
/**
 * EOF
 */
void target_acquireMasterLock(UINT8 token_p,UINT8 id)
{
    UINT8 lock = 0;
    int count = 0;
    do{
            DUALPROCSHM_INVALIDATE_DCACHE_RANGE((UINT32)masterLock,sizeof(UINT8));
            lock = DPSHM_READ8((UINT32)masterLock);
//printf("Lock %d\n",lock);
            if(lock == DEFAULT_LOCK_ID)
            {

                DPSHM_WRITE8((UINT32)masterLock, token_p);
                DUALPROCSHM_FLUSH_DCACHE_RANGE((UINT32)masterLock,sizeof(UINT8));
                continue;
            }
 /*           else
            {
                if(lock != token_p)
                {
                    count++;
                    if(count > 500)
                    {
#ifdef __MICROBLAZE__
     //                   printf("mLock %x %d t %x\n",*masterLock,id,token_p);
#endif
                        count = 0;
                    }

                }
             //
            }*/
       }while(lock != token_p);
}
void target_releaseMasterLock(void)
{
    UINT8 lock = DEFAULT_LOCK_ID;
   // printf("mUlk %x\n",*masterLock);
    DPSHM_WRITE8((UINT32)masterLock,lock);
    DUALPROCSHM_FLUSH_DCACHE_RANGE((UINT32)masterLock,sizeof(UINT8));
}
