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
SYSTEMC_DIR=/usr/local/packages/systemc-2.2.0
TLM_INC=/usr/local/packages/systemc-tlm-2.0.1/include/tlm
#SYSTEMC_DIR=/home/unga/group/gerstl/pkg/systemc-2.3.0
#TLM_INC=/home/unga/group/gerstl/pkg/systemc-2.3.0
TARGET_ARCH = linux64
DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES
HCOS_DIR=../HCSim
HELPER_DIR = ../helper_multiprof

#LLVM profiling paths
INS_LIB = ../InstrumentLLVM/build/FunctionInstr/libLLVMFunctionInstr.so
LLVM_INSTR_PASS = -functionInstr
INS_STAGE=annot#preprof#prof#annot 
LIB_ID=-libname=12







LFLAGS=-L$(SYSTEMC_DIR)/lib-linux64 -lsystemc -L$(HCOS_DIR)/lib -lhcsim \
	 -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64 


#To compile for linux: make ARCH=linux
#To compile for cygwin: make ARCH=cygwin
ARCH=unix

CFLAGS=-g -O2 -Wall $(DEFINES) -DIPv4 -DLWIP_DEBUG -pedantic -Werror \
	-Wparentheses -Wsequence-point -Wswitch-default \
	-Wextra -Wundef -Wshadow -Wpointer-arith \
	-Wwrite-strings -Wredundant-decls -fPIC -w

 
#CONTRIBDIR=../../..
#LWIPARCH=$(CONTRIBDIR)/ports/unix
LWIPARCH=./LwipArch
APPLWIP=./LwipNode

ARFLAGS=rs

#Set this to where you have the lwip core module checked out from CVS
#default assumes it's a dir named lwip at the same level as the contrib module
LWIPDIR=./lwip/src

CFLAGS:=$(CFLAGS) \
	-I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPDIR) -I$(LWIPARCH)/include -I$(APPLWIP)\
	-I$(SYSTEMC_DIR)/include -I$(TLM_INC)\
	-I$(HCOS_DIR)/include

# COREFILES, CORE4FILES: The minimum set of files needed for lwIP.
COREFILES=$(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c \
	$(LWIPDIR)/core/pbuf.c $(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c \
	$(LWIPDIR)/core/sys.c $(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
	$(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c \
	$(LWIPDIR)/core/init.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/def.c
CORE4FILES=$(wildcard $(LWIPDIR)/core/ipv4/*.c) 
#$(LWIPDIR)/core/ipv4/inet.c $(LWIPDIR)/core/ipv4/inet_chksum.c


# SNMPFILES: Extra SNMPv1 agent
SNMPFILES=$(LWIPDIR)/core/snmp/asn1_dec.c $(LWIPDIR)/core/snmp/asn1_enc.c \
	$(LWIPDIR)/core/snmp/mib2.c $(LWIPDIR)/core/snmp/mib_structs.c \
	$(LWIPDIR)/core/snmp/msg_in.c $(LWIPDIR)/core/snmp/msg_out.c


APIFILES=$(LWIPDIR)/api/api_lib.c $(LWIPDIR)/api/api_msg.c $(LWIPDIR)/api/tcpip.c \
	$(LWIPDIR)/api/err.c $(LWIPDIR)/api/netbuf.c $(LWIPDIR)/api/netdb.c

NETIFFILES=$(LWIPDIR)/netif/etharp.c $(LWIPDIR)/netif/slipif.c

ARCHFILES=$(wildcard $(LWIPARCH)/*.c $(LWIPARCH)/netif/OmnetIf_server.c $(LWIPARCH)/netif/list.c $(LWIPARCH)/netif/tcpdump.c)

#$(LWIPARCH)/netif/OmnetIf_pkt.c 


# LWIPFILES: All the above.
LWIPFILES=$(COREFILES) $(CORE4FILES) $(SNMPFILES) $(APIFILES) $(NETIFFILES) #$(ARCHFILES)
LWIPFILESW=$(wildcard $(LWIPFILES))
LWIPOBJS=$(notdir $(LWIPFILESW:.c=.o))

# APPFILES:
APPFILES=$(APPLWIP)/tcpecho.c $(APPLWIP)/tcpechoclient.c $(APPLWIP)/image_file.c #$(APPLWIP)/simhost.c
APPOBJS=$(notdir $(APPFILES:.c=.o))


LWIPLIB=liblwip4.a
APPLIB=liblwipapps.a



# Including OMNET++ and INET interface 
OMNETPP_ROOT = $$omnetpp_root
OMNETPP_INC = $(OMNETPP_ROOT)/include
CFLAGS:=$(CFLAGS) -I$(OMNETPP_INC)

INET_DIR = $(OMNETPP_ROOT)/inet
INET_ETHERNET = ../NoS_Simple/src/ethernet
CFLAGS:=$(CFLAGS) -I$(INET_DIR)/src/base -I$(INET_ETHERNET)



# Including helper files 
OSAPP_DIR = /home/local/grimlock/zzhao/NoS/VisionGraphApps/OSNode

CFLAGS:=$(CFLAGS) -I$(HELPER_DIR) -I$(OSAPP_DIR)



#Profiling variables

LWIPALL=liblwip4
LWIPOBJ=$(LWIPALL).o
COPTS=-emit-llvm
LLVM_CC=clang++


LWIPBCFILES=$(notdir $(LWIPFILES:.c=.bc))



ARCHFILESW=$(wildcard $(ARCHFILES))
ARCHOBJS=$(notdir $(ARCHFILESW:.c=.o))




all: $(LWIPLIB) $(APPLIB)
.PHONY: all

clean:
	rm -f *.bc *.o $(LWIPLIB) $(APPLIB) *.s .depend* *.core core *~

depend dep: .depend

include .depend

$(LWIPOBJ): $(LWIPFILES)
	$(LLVM_CC) $(COPTS) $(CFLAGS) -c $(LWIPFILES) 
	llvm-link $(LWIPBCFILES) -o $(LWIPALL).all.bc 
	#llc -filetype=obj $(LWIPALL).all.bc -o $(LWIPALL).o
	opt -load $(INS_LIB) $(LLVM_INSTR_PASS) -libname=12 -InstruProf=$(INS_STAGE) < $(LWIPALL).all.bc > $(LWIPALL).instru.bc
	llc -filetype=obj $(LWIPALL).instru.bc -o $(LWIPALL).o

#$(): 
%.o:
	$(CC) $(CFLAGS) -c $(<:.o=.c) $(LFLAGS)



$(APPLIB): $(APPOBJS)
	$(AR) $(ARFLAGS) $(APPLIB) $?

$(LWIPLIB): $(LWIPOBJ) $(ARCHOBJS)
	echo $(AR) $(ARFLAGS) $(LWIPLIB) $?
	$(AR) $(ARFLAGS) $(LWIPLIB) $?

.depend: $(LWIPFILES) $(ARCHFILES) $(APPFILES)
	$(CCDEP) $(CFLAGS) -MM $^ > .depend || rm -f .depend








