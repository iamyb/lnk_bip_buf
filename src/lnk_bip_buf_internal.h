/**
********************************************************************************
LBB: linked bip buffer with variant block size

Copyright (C) 2016 b20yang
---
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "lnk_bip_buf.h"

#ifndef __LNK_BIP_BUF_INTERNAL_H__
#define  __LNK_BIP_BUF_INTERNAL_H__

#define LBB_TRUE      (1)
#define LBB_FALSE     (0)

#define LBB_BH_WORDS  (sizeof(lbb_blk_hd)/sizeof(u32))

/* The Magic Number for block status */
#define LBB_BH_FREE   (0xCDABCDAB)
#define LBB_BH_ALLOC  (0xABCDABCD)
#define LBB_BH_READY  (0xA9A9B9B9)

#define LBB_B2W_ALIGN(a) ((u32)(a+3)/sizeof(u32))
#define LBB_BLK_SIZE(a)  ((u32)(LBB_BH_WORDS+LBB_B2W_ALIGN(a)))

#define LBB_FREE_THIS_BLOCK(lbb, bh) \
	({ \
	 lbb->tailor = (bh->next==0)?0:lbb->tailor;\
     lbb->last = bh->next;\
	 bh->next = 0xFFFFFFFF;\
	 lbb->used -= bh->size;\
	 bh->size = 0;\
	 lbb->free = lbb->size-lbb->used;\
	 bh->status = LBB_BH_FREE;\
	 })

#define LBB_GET_BH_BY_PTR(ptr) \
	((lbb_blk_hd*)((u32*)ptr - LBB_BH_WORDS))

#define LBB_GET_LAST_BLOCK_BH(lbb) \
	((lbb_blk_hd*)(lbb->last+&lbb->buffer[0]))

#define LBB_BUF_MIN_SZ 1024 /* at least 1KB */

/*----------------------------------------------------------------------------*/
typedef unsigned int u32;
typedef int i32;

typedef struct lbb_blk_hd
{
	u32   status;         /* block status    */
	u32   size;           /* block size      */
	u32   next;           /* next block      */
}lbb_blk_hd;

typedef struct lbb_header
{
	u32   size;           /* total size      */
	u32   used;           /* alloc size      */
	u32   free;           /* free size       */
	u32   tailor;         /* tail words      */
	u32   max;            /* max used        */ /*not supported yet*/
	u32   cnt;            /* block count     */ /*not supported yet*/
	
	u32   cur;            /* current position*/
	u32   last;           /* last free       */
	u32   end;            /* buffer end      */

	void *extbuf;         /* external buffer */
	u32   buffer[1];  	  /* buffer          */
}lbb_header;

#if 0
/**
********************************************************************************
*
* @brief  get the max used space from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the max used space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_max_used_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_max_used_space(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get used space from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the used space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_used_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_used_space(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get free space from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the free space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_free_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_free_space(lbb_handle hdl);
#endif
	
/**
********************************************************************************
*
* @brief  get total space from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the total space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_total_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_total_space(lbb_handle hdl);

#if 0
/**
********************************************************************************
*
* @brief  get block count from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the block count of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_block_count(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_block_count(lbb_handle hdl);
#endif

/**
********************************************************************************
*
* @brief  get the head from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the head of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_head(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_head(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get the tail offset from specied lbb handle
*
* @param[in] cb specify the linked bip buffer handle
*
* @return return the tail offset space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024, NULL);
*    int used = lbb_get_tail(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
int lbb_get_tail(lbb_handle hdl);
#endif
