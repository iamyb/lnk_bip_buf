/**
********************************************************************************
LBB: linked bip buffer with variant block size

Copyright (C) 2016. b20yang 
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
#ifndef __LINKED_BIP_BUFFER_H__
#define __LINKED_BIP_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif 

typedef void*  lbb_handle;

/**
********************************************************************************
*
* @brief  create linked bip buffer for specied parameters 
*
* @param[in] size   specify the linked bip buffer size
*
* @return A pointer to the handle of the cicurlar buffer. 
*         in case of errors NULL returned. 
*
* @par Example:
*  @code
*    lbb_handle hdl_0 = lbb_create(1024);
*
*  @endcode
*
*******************************************************************************/
lbb_handle lbb_create(int size);

/**
********************************************************************************
*
* @brief  create linked bip buffer for specied parameters 
*
* @param[in] size   specify the linked bip buffer size
*
* @param[in] buffer specify the pointer to buffer memory
*                   if set to NULL, default buffer allocated.
*
* @return A pointer to the handle of the cicurlar buffer. 
*         in case of errors NULL returned. 
*
* @par Example:
*  @code
*
*    u32 size = 1024;
*    void* buffer = (void*)malloc(size);
*    lbb_handle hdl_1 = lbb_create_to_ext_buf(buffer_size, buffer);
*  @endcode
*
*******************************************************************************/
lbb_handle lbb_create_to_ext_buf(int size, void* user_buffer);

/**
********************************************************************************
*
* @brief  destroy linked bip buffer with specied lbb handle
*
* @param[in] hdl specify the linked bip buffer handle
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*
*    if(hdl != NULL)
*        lbb_destroy(hdl);
*  @endcode
*
*******************************************************************************/
void  lbb_destroy(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  allocate a block from specied lbb handle
*
* @param[in] hdl specify the linked bip buffer  handle
*
* @param[in] size specify the required block size 
*
* @return return the pointer of allocated block. NULL if fail.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    void* ptr = lbb_alloc(hdl, size);
*
*  @endcode
*
*******************************************************************************/
void* lbb_alloc(lbb_handle hdl, int size);

/**
********************************************************************************
*
* @brief  free a block from specied lbb handle
*
* @param[in] lbb specify the linked bip buffer handle
*
* @param[in] ptr specify the block ptr need be freed
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    void* ptr = lbb_alloc(hdl, size);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
void  lbb_free(lbb_handle hdl, void* ptr);

/**
********************************************************************************
*
* @brief  get the max used space from specied lbb handle
*
* @param[in] lbb specify the linked bip buffer handle
*
* @return return the max used space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    int used = lbb_get_max_used_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
unsigned int lbb_get_max_used_space(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get used space from specied lbb handle
*
* @param[in] lbb specify the linked bip buffer handle
*
* @return return the used space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    int used = lbb_get_used_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
unsigned int lbb_get_used_space(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get free space from specied lbb handle
*
* @param[in] lbb specify the linked bip buffer handle
*
* @return return the free space of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    int used = lbb_get_free_space(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
unsigned int lbb_get_free_space(lbb_handle hdl);

/**
********************************************************************************
*
* @brief  get block count from specied lbb handle
*
* @param[in] lbb specify the linked bip buffer handle
*
* @return return the block count of the buffer.
*
* @par Example:
*  @code
*    lbb_handle hdl = lbb_create(1024);
*    int used = lbb_get_block_count(hdl);
*    lbb_free(ptr);
*
*  @endcode
*
*******************************************************************************/
unsigned int lbb_get_block_count(lbb_handle hdl);
#ifdef __cplusplus
}
#endif 
#endif 
