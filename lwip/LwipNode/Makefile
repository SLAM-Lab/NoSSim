#
# Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission. 
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
# SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
# OF SUCH DAMAGE.
#
# This file is part of the lwIP TCP/IP stack.
# 
# Author: Adam Dunkels <adam@sics.se>
#


CCDEP=g++
CC=g++



#Including SystemC
#SYSTEMC_DIR=/home/unga/group/gerstl/pkg/systemc-2.3.0
#TLM_INC=/home/unga/group/gerstl/pkg/systemc-2.3.0
SYSTEMC_DIR=/usr/local/packages/systemc-2.2.0
TLM_INC=/usr/local/packages/systemc-2.2.0
TARGET_ARCH = linux64
DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES
#Including SystemC


#To compile for linux: make ARCH=linux
#To compile for cygwin: make ARCH=cygwin
ARCH=unix

CFLAGS=-g -O2 -Wall $(DEFINES) -DIPv4 -DLWIP_DEBUG -pedantic -Werror \
	-Wparentheses -Wsequence-point -Wswitch-default \
	-Wextra -Wundef -Wshadow -Wpointer-arith \
	-Wwrite-strings -Wredundant-decls -fPIC -w
# not used for now but interesting:
# -Wpacked
# -Wunreachable-code
# -ansi
# -std=c89
#LDFLAGS=-lutil  -L.\
#	 -lpthread\
#	 -lsystemc -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc \
#	 -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64

LDFLAGS=-L. -L..\
	 -lsystemc -L$(SYSTEMC_DIR)/lib-linux64\
	# -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64

 

#LWIPARCH=$(CONTRIBDIR)/ports/unix
LWIPARCH=../LwipArch
ARFLAGS=rs

#Set this to where you have the lwip core module checked out from CVS
#default assumes it's a dir named lwip at the same level as the contrib module
LWIPDIR=../lwip/src

CFLAGS:=$(CFLAGS) \
	-I. -I..\
	-I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPDIR) -I$(LWIPARCH)/include\
	-I$(SYSTEMC_DIR)/include -I$(TLM_INC)


APPFILES=./tcpecho.c ./tcpechoclient.c ./image_file.c ./simhost.c\



APPLIB=liblwipapps.a
APPOBJS=$(notdir $(APPFILES:.c=.o))

%.o:
	$(CC) $(CFLAGS) -c $(<:.o=.c)

all: $(APPLIB) 
.PHONY: all

clean:
	rm -f *.o  $(APPLIB) *.s .depend* *.core core *~

depend dep: .depend

include .depend

$(APPLIB): $(APPOBJS)
	$(AR) $(ARFLAGS) $(APPLIB) $?

.depend: $(APPFILES)
	$(CCDEP) $(CFLAGS) -MM $^ > .depend || rm -f .depend





