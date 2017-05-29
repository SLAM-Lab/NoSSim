#Top&App dir
TOP_DIR = ../../../NoS
MODEL_DIR = $(TOP_DIR)/NoS_ECG
MODEL_EXE = NoS_ECG
ECG_DIR=$(MODEL_DIR)/ecg

# SystemC Models
HCSIM_DIR	= $(TOP_DIR)/HCSim

# lwIP Models
LWIPARCH=$(TOP_DIR)/lwip/LwipArch
APPLWIP=$(TOP_DIR)/lwip/LwipNode
LWIPDIR=$(TOP_DIR)/lwip/lwip/src
LWIPLIBDIR=$(TOP_DIR)/lwip

# Annotation Wrappers 
HELPER_DIR=$(TOP_DIR)/helper

# Third party libs
JSONCPP = $(TOP_DIR)/config_json

# OMNeT's INET framework
OMNET_DIR	= $$omnetpp_root
INET_DIR	= $(OMNET_DIR)/inet
CONFIGNAME	= gcc-debug

# configuration
#DEFAULT_CONFIG	=  
#DEFAULT_FRAMES	= 

# compilation parameters
#DEFINES		= -DDEMO

all: checkmakefiles 
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

#makefiles: simulations/run
makefiles: 
	cd src && opp_makemake -f --deep -I.. 	-I$(HCSIM_DIR)/include\
	 -I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 \
	 -I$(LWIPDIR) -I$(LWIPARCH)/include -I$(APPLWIP)\
	 -I$(APPLWIP) 		\
	 -I$(ECG_DIR)		\
	 -I$(HELPER_DIR)	\
	 -I$(JSONCPP)		\
	 -L$(LWIPLIBDIR)	\
	 -L$(ECG_DIR)		\
	 -L$(HELPER_DIR)	\
	 -L$(HCSIM_DIR)/lib						\
	 -I$$\(INET_PROJ\)/src/linklayer/ieee80211/radio 		\
	 -I$$\(INET_PROJ\)/src/linklayer/ethernet 			\
	 -I$$\(INET_PROJ\)/src/networklayer/common 			\
	 -I$$\(INET_PROJ\)/src 						\
	 -I$$\(INET_PROJ\)/src/status 					\
	 -I$$\(INET_PROJ\)/src/networklayer/icmpv6 			\
	 -I$$\(INET_PROJ\)/src/world/obstacles 				\
	 -I$$\(INET_PROJ\)/src/networklayer/manetrouting/base 		\
	 -I$$\(INET_PROJ\)/src/networklayer/contract 			\
	 -I$$\(INET_PROJ\)/src/mobility 				\
	 -I$$\(INET_PROJ\)/src/networklayer/xmipv6 			\
	 -I$$\(INET_PROJ\)/src/linklayer/ieee80211/mgmt 		\
	 -I$$\(INET_PROJ\)/src/util 					\
	 -I$$\(INET_PROJ\)/src/transport/contract 			\
	 -I$$\(INET_PROJ\)/src/linklayer/radio/propagation 		\
	 -I$$\(INET_PROJ\)/src/linklayer/ieee80211/radio/errormodel 	\
	 -I$$\(INET_PROJ\)/src/world/powercontrol 			\
	 -I$$\(INET_PROJ\)/src/linklayer/radio 				\
	 -I$$\(INET_PROJ\)/src/util/headerserializers/tcp 		\
	 -I$$\(INET_PROJ\)/src/networklayer/ipv4 			\
	 -I$$\(INET_PROJ\)/src/util/headerserializers/ipv4 		\
	 -I$$\(INET_PROJ\)/src/base 					\
	 -I$$\(INET_PROJ\)/src/util/headerserializers 			\
	 -I$$\(INET_PROJ\)/src/world/radio 				\
	 -I$$\(INET_PROJ\)/src/linklayer/ieee80211/mac 			\
	 -I$$\(INET_PROJ\)/src/networklayer/ipv6 			\
	 -I$$\(INET_PROJ\)/src/transport/sctp 				\
	 -I$$\(INET_PROJ\)/src/util/headerserializers/udp 		\
	 -I$$\(INET_PROJ\)/src/networklayer/ipv6tunneling 		\
	 -I$$\(INET_PROJ\)/src/battery/models 				\
	 -I$$\(INET_PROJ\)/src/applications/pingapp 			\
	 -I$$\(INET_PROJ\)/src/linklayer/contract 			\
	 -I$$\(INET_PROJ\)/src/util/headerserializers/sctp 		\
	 -I$$\(INET_PROJ\)/src/networklayer/arp 			\
	 -I$$\(INET_PROJ\)/src/transport/tcp_common 			\
	 -I$$\(INET_PROJ\)/src/transport/udp 				\
	 -L$$\(INET_PROJ\)/out/$(CONFIGNAME)/src -linet 		\
	 -DSC_INCLUDE_DYNAMIC_PROCESSES -DINET_IMPORT 			\
	 -KINET_PROJ=$(INET_DIR)

##	 -DTOTAL_FRAMES=$$\(TOTAL_FRAMES\) -D$$\(CONFIG\)		\
##         -KTOTAL_FRAMES=$(DEFAULT_FRAMES) 	\
##	 -KCONFIG=$(DEFAULT_CONFIG) $(DEFINES)				

test:
	echo  export NEDPATH=$$NEDPATH:.:$(INET_DIR)/src
	export NEDPATH=$$NEDPATH:.:$(INET_DIR)/src && \
	  cd src && ../out/$(CONFIGNAME)/src/$(MODEL_EXE) -u Cmdenv -f omnetpp.ini

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi

#checklinks: simulations/server_fs simulations/client_fs

#simulations/server_fs:
#	mkdir simulations/server_fs

#simulations/client_fs:
#	mkdir simulations/client_fs
#	cd simulations/client_fs && ln -sf ../../src/client_fs/*[0-9].bmp .

#simulations/run: simulations/run.in
#	sed -e "s|@INET_DIR@|$(INET_DIR)|" < $< > $@ && chmod +x $@
