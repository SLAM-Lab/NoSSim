#INET dir
INET_DIR = /home/slam/OMNET/inet
#OS model
OS_MODEL=../../../HCSim
#lwIP stack
LWIP=../../../lwip-hcsim
LWIP_LIB=../../../lwip-hcsim/ports
#Simulation contexts
HELPER=../../../lwip-hcsim/ports/hcsim/context
JSONPARSER=../../../json
#IoT Runtime library
DISTRIOT=../networking_api
#Application
ECG=../ecg

CONFIGNAME = clang-debug
PKG_NAME = NoS
MODEL_EXE = INET

.PHONY: all clean cleanall makefiles makefiles-so makefiles-lib makefiles-exe checkmakefiles doxy

all: checkmakefiles src/$(PKG_NAME)/features.h 
	cd src && $(MAKE) MODE=debug all

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@rm -f src/Makefile src/$(PKG_NAME)/features.h
	@cd src && rm -rf result.json nic_energy_device* out results *.log *.dat .cmdenv-log .tkenvlog .tkenvrc
	cd src && make -C $(ECG) clean
	cd src && make -C $(DISTRIOT) clean

MAKEMAKE_OPTIONS := -f --deep -o $(MODEL_EXE) -O out --no-deep-includes -I. -I.. -I./NoS/applications -I./NoS/applications/OSNode \
		 -I$(SYSTEMC)/include  -I./src/OSNode -I$(JSONPARSER) \
 		 -I$(OS_MODEL)/include -I$(LWIP)/src/include -I$(LWIP)/ports/port/include -I$(LWIP)/src/include/ipv4 -I$(LWIP)/lwip/src  -I$(LWIP)/ports/hcsim -I$(HELPER) \
		 -L$(LWIP_LIB)  -L$(HELPER)  -L$(OS_MODEL)/lib \
                 -I$(DISTRIOT)/include -I$(DISTRIOT)/src -I$(DISTRIOT)/hcsim -I$(DISTRIOT)/lwip \
                 -I$(ECG) \
                 -L$(DISTRIOT) -L$(ECG) \
		 -I$$\(INET_PROJ\)/src/inet/applications/ethernet \
		 -I$$\(INET_PROJ\)/src/inet/common \
		 -I$$\(INET_PROJ\)/src/inet/common/lifecycle \
		 -I$$\(INET_PROJ\)/src/ \
		 -I$$\(INET_PROJ\)/src/inet/linklayer/common \
		 -I$$\(INET_PROJ\)/src/inet/linklayer/contract \
		 -I$$\(INET_PROJ\)/src/inet/networklayer/common  \
		 -I$$\(INET_PROJ\)/src/inet/networklayer/contract  \
		 -I$$\(INET_PROJ\)/src/inet/networklayer/contract/ipv4  \
		 -I$$\(INET_PROJ\)/src/inet/networklayer/contract/ipv6 \
		 -L$$\(INET_PROJ\)/out/$(CONFIGNAME)/src -lINET 		\
		 -DSC_INCLUDE_DYNAMIC_PROCESSES -DINET_IMPORT 			\
		 -KINET_PROJ=$(INET_DIR)

makefiles: src/$(PKG_NAME)/features.h makefiles-exe

makefiles-so:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake --make-so $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-lib:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake --make-lib $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-exe:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '========================================================================'; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '========================================================================'; \
	echo; \
	exit 1; \
	fi

test:
	export NEDPATH=$$NEDPATH:.:$(INET_DIR)/src && \
	  cd src && ./$(MODEL_EXE)  -f omnetpp.ini -u Cmdenv

dependency:
	cd src && make -C $(ECG)
	cd src && make -C $(LWIP_LIB)
	cd src && make -C $(DISTRIOT)
	cd src && make -C $(OS_MODEL)

# generate an include file that contains all the WITH_FEATURE macros according to the current enablement of features
src/$(PKG_NAME)/features.h: $(wildcard .oppfeaturestate) .oppfeatures
	@chmod +x ./inet_featuretool
	@./inet_featuretool defines >src/$(PKG_NAME)/features.h

doxy:
	doxygen doxy.cfg

tcptut:
	cd doc/src/tcp && $(MAKE)
