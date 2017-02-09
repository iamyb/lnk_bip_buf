# Copyright (C) 2016 b20yang 
# ---
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.

# This program is distributed in the hope that it will be useful,but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
################################################################################
include ../rules.mk

CFLAGS_LOCAL = -I$(SRC_PATH)
CFLAGS_LOCAL += -DLBB_TEST

CPPFLAGS = -I$(INC_PATH) 
CPPFLAGS+= -I$(SRC_PATH)

TEST_LBB_SRC=test_lnk_bip_buf.c 
TEST_LBB_OBJ=$(TEST_LBB_SRC:.c=.o)
TEST_LBB_TGT=$(TEST_LBB_SRC:.c=.out)

TEST_LBB_CPP_SRC=test_performance.c
TEST_LBB_CPP_OBJ=$(TEST_LBB_CPP_SRC:.c=.o)
TEST_LBB_CPP_TGT=$(TEST_LBB_CPP_SRC:.c=.out)

LDFLAGS_LOCAL=-llbb 
LDFLAGS =$(LDFLAGS_LOCAL)
LDFLAGS+=$(LDFLAGS_GLOBAL)

all: $(TEST_LBB_TGT) $(TEST_LBB_CPP_TGT)

#.PHONY:$(TEST_LBB_TGT) $(TEST_LBB_CPP_TGT)

$(TEST_LBB_TGT): $(LIB_LBB) $(TEST_LBB_OBJ)
	$(CC) -o $@ $(TEST_LBB_OBJ) -L$(LIB_PATH) $(LDFLAGS) 

$(TEST_LBB_OBJ): $(TEST_LBB_SRC)  
	$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -c $? -o $@  

$(TEST_LBB_CPP_TGT): $(LIB_LBB) $(TEST_LBB_CPP_OBJ)
	$(CC) -o $@ $(TEST_LBB_CPP_OBJ) -L$(LIB_PATH) $(LDFLAGS)  

$(TEST_LBB_CPP_OBJ): $(TEST_LBB_CPP_SRC)
	$(CC) $(CFLAGS_GLOBAL) $(CFLAGS_LOCAL) -c $? -o $@  

clean:
	$(RM) $(TEST_LBB_OBJ) $(TEST_LBB_CPP_OBJ)
	$(RM) $(TEST_LBB_TGT) $(TEST_LBB_CPP_TGT)
