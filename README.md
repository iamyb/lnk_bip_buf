## What is lnk-bip-buf

lnk-bip-buf(Linked Bip Buffer, LBB) provides a lightweight and high-performance memory alloc/free mechanism for realtime stream processing. 

It requires the buffer **MUST** be in a continuous memory area to cater the cache policy. With this the successive stream data blocks could be allocated in adjacent memory area,  then reduce cache missing and thrashing during the data processing. It's **thread-unsafe** in current implementation. Therefore you have to add addtional locks to handle the usage in multi-thread processing.

#### Usage Example

	/* Keep this buf in a continuous physic memory location for best practice*/ 
	uint8_t buf[4096];

	/* Initalize the buffer */
	lbb_handle hdl = lbb_create_to_ext_buf(sizeof(buf), buf);

	/* Alloc one block from it*/
	void* ptr = lbb_alloc(hdl, size);

	/* Free the block */
	lbb_free(hdl, ptr);

#### Perfomance 
Below is a comparation between glibc malloc/free and lnk-bip-buf alloc/free. The upper one is to compare the consumed nanoseconds for alloc and free with series block size. And the bottom adds one time memset upon the first scenario. (Ubuntu 16.04, Intel i5-4300U @ 1.96GHz)

![](./doc/performance.png)

Please refer source code test/test_performance.c for the details. Note you have to execute setenv.sh before run test_performance to create hugepage in your environment.
 

## License
>This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.    

>This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
