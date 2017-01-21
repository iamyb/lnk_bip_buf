##General  

lnk-bip-buf(Linked Bip Buffer, LBB) is a straightforward implementation of circular buffer. It aims to support the memory allocation/de-allocation based on circualr buffer in **NO STRICT FIFO realtime stream** scenarios with high performance. 

To achieve that, it links the allocated blocks one by one and introduce 3 additional states for each block. Once one block need be freed earlier than the tail of circular buffer, then just mark the block **"READY be freed"** and it will be finally committed to "free" state until the tail arriving it. 

Besides that, it also supports **VARIABLE-SIZE** blocks allocation and **NO-WRAP-AROUND** allocation in the end of buffer while there's no enough left space to fullfill the request. It will extend current block to the buffer end or add one dummy block which will link to the begin of the buffer.

it requires the buffer **MUST** be in a continuous memory area to cater the cache policy so it can reduce cache miss while handling stream data. And it's **thread-unsafe**, therefore you have to add addtional locks to handle the usage in multi-thread processing.

##More Details 
###1. Block Allocation 

     (1) free tail == buffer end

              free head --->                         free tail=buffer 
     _____________|____________________________________________
     |            |          |                                |
     |////////////|++++++++++|                                |
     |____________|__________|________________________________|

     (2.1) if Enough memory between free head and buffer end
     (2.1.1) the left memory is less than(eq) the LBB_BH_WORDS, 
             output: extend the block to the end and alloc sucess

                        free tail        free head --->                        
     ________________________|______________|_________________
     |                       |              |                 |
     |                       |//////////////|+++++++++++++////|
     |_______________________|______________|_________________|

     (2.1.2) the left memory is greater than the LBB_BH_WORDS, 
             output: alloc sucess
    
                        free tail        free head --->     
     ________________________|______________|_________________
     |                       |              |          |      |
     |                       |//////////////|++++++++++|      |
     |_______________________|______________|__________|______|

     (2.2) if NO enough memory between head and buffer end
             output: alloc a dummy block and extend to the end
                     reset the head to the begin of buffer 

                        free tail                free head --->     
     _________________________|________________________|_______
     |                        |                        |      |
     |                        |////////////////////////|++++++|
     |________________________|________________________|______|
                                                       |
                                                      dummy block

     (2.2.1) if enough memory between NEW head and tail
             output: alloc success 

                        free tail                free head --->     
     _________________________|________________________|______
     |       |                |                        |      |
     |+++++++|                |////////////////////////|++++++|
     |_______|________________|________________________|______|
       |                                                  |
     user block                                        dummy block

### License
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.    

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
