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
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>

#include "lnk_bip_buf_internal.h"
/* -------------------------------------------------------------------------- */
lbb_handle lbb_create(int size)
{
	if(size < LBB_BUF_MIN_SZ) return NULL;
	
	/* allocate a buffer via malloc */
	u32 total = LBB_B2W_ALIGN(size)+sizeof(lbb_header);
	lbb_header* lbb = (lbb_header*)malloc(total*sizeof(u32));
	
	if(lbb != NULL){
		(void)memset((void*)lbb, 0, sizeof(lbb_header));
		
		lbb->end  = (size+3)/sizeof(u32);
		lbb->size = (size+3)/sizeof(u32);
		lbb->free = lbb->size - lbb->used;
	}

	return (lbb_handle)lbb;
}

/* -------------------------------------------------------------------------- */
lbb_handle lbb_create_to_ext_buf(int size, void* user_buffer)
{
	if(size < LBB_BUF_MIN_SZ) return NULL;
	
	lbb_header* lbb = (lbb_header*)user_buffer; 
	
	if(lbb != NULL){
		/* use the external user buffer */
		size = size - sizeof(lbb_header);

		/* align to the word boundary */
		size = size & (~3); 
		(void)memset((void*)lbb, 0, sizeof(lbb_header));
		lbb->extbuf = user_buffer;
		
		lbb->end  = (size+3)/sizeof(u32);
		lbb->size = (size+3)/sizeof(u32);
		lbb->free = lbb->size - lbb->used;
	}

	return (lbb_handle)lbb;
}

/* -------------------------------------------------------------------------- */
void lbb_destroy(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*)hdl;
	if(lbb->extbuf == 0) free(lbb);
}

/* -------------------------------------------------------------------------- */
static inline void lbb_add_dummy(lbb_header* const lbb)
{
	lbb->tailor = lbb->end-lbb->cur;

	/* and create a dummy block until the end */
	lbb_blk_hd* dummy = (lbb_blk_hd*) 
		(&(lbb->buffer[0])+lbb->cur);
	dummy->next  = 0; // pointer to the head
	dummy->status = LBB_BH_READY;
	dummy->size  = lbb->tailor;

	lbb->used += lbb->tailor;
	lbb->free  = lbb->size-lbb->used;
	lbb->cur   = dummy->next;
}

/* -------------------------------------------------------------------------- */
void* lbb_alloc(lbb_handle hdl, int size)
{
	lbb_header* lbb = (lbb_header*)hdl;
	void* ret = NULL;
	i32 total = LBB_BLK_SIZE(size);
	
	if(lbb == NULL || size <= 0 || (lbb->free < total)) 
		return ret;

	/* find the matching free area for allocation*/
	i32 tail = lbb->last;
	i32 head = lbb->cur;
	if(tail <= head)
	{
		if(lbb->end-lbb->cur >= (total))
		{
			tail = lbb->end;
		}
		else if(lbb->last >= (total))
		{
			head = 0;
			lbb_add_dummy(lbb);
		}
	}

	/* try to allocate memory from the free area */
	if((tail - head) >= total)
	{
		lbb_blk_hd* bh = (lbb_blk_hd*)&lbb->buffer[head];
		bh->status = LBB_BH_ALLOC;
		if(head + total + LBB_BH_WORDS < tail)
		{
			bh->size = total;
			bh->next = head + total;	
		}
		else 
		{
			bh->size = tail - head;
			lbb->tailor = bh->size - total;
			bh->next = (tail==lbb->end) ? 0 : tail;
		}
		lbb->cur = bh->next;
		lbb->used += bh->size;
		lbb->free = lbb->size - lbb->used;

		ret = &(lbb->buffer[head+LBB_BH_WORDS]);
	}
	
	return ret;
}


/* -------------------------------------------------------------------------- */
void lbb_free(lbb_handle hdl, void* ptr)
{
	lbb_header* lbb = (lbb_header*)hdl;
	lbb_blk_hd* bh = NULL;
	if(lbb == NULL || ptr == NULL) 
		return;

	/* if it's not a allocated block, return directly */
	bh = LBB_GET_BH_BY_PTR(ptr);
	if(bh->status == LBB_BH_FREE || bh->status == LBB_BH_READY) 
		return;

    /* if it's not the oldest block, set to READY to free*/
	if(((u32*)ptr-LBB_BH_WORDS)!=(&lbb->buffer[0]+lbb->last)){
		bh->status = LBB_BH_READY;	
		return;
	}
	
	LBB_FREE_THIS_BLOCK(lbb, bh);

	/* continure to check if any ready to free blocks */
	bh = LBB_GET_LAST_BLOCK_BH(lbb);
	while(bh->status == LBB_BH_READY && bh->size != 0)
	{
		LBB_FREE_THIS_BLOCK(lbb, bh);
		bh = LBB_GET_LAST_BLOCK_BH(lbb);
	}

	/* reset to the begin of the buffer while no usage */
	if(lbb->used == 0 && (lbb->last == lbb->cur))
	{
		lbb->cur    = 0;
		lbb->last   = 0;
		lbb->tailor = 0;
		lbb->free   = lbb->size;
	}
}

/* -------------------------------------------------------------------------- */
int lbb_get_max_used_space(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->max:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_used_space(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->used:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_free_space(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->free:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_total_space(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->size:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_block_count(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->cnt:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_head(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->cur:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_tail(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->last:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_tailor_words(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->tailor:0);
}

/* -------------------------------------------------------------------------- */
int lbb_get_end(lbb_handle hdl)
{
	lbb_header* lbb = (lbb_header*) hdl;
	
	return ((lbb!=NULL)?lbb->end:0);
}

