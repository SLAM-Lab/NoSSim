# NoSSim
Network-of-Systems Simulator
============================

Network-of-Systems Simulator (NoSSim) is a network/system co-simulation infrastructure
that combines a host-compiled SystemC simulation of device-internal SoC 
platforms with a simulation of network interactions using the OMNeT++ network 
simulation framework [1]. 

The approach is demonstrated on two IoT application scenarios with different set of 
network/system configurations. 


IoT scenarios:
------------------
```
  examples/vision_graph       -- Vision graph discovery, where the relative position and 
		                 orientation among a network of smart cameras are estimated
		      
  examples/ecg_diagnosis      -- ECG monitoring, where raw ECG signals are used to 
		                 detect heart arrhythmia
```

Directories:
------------
```
  json/                      -- JSON lib and configuration file template
  InstrumentLLVM/            -- LLVM function-level instrumentation pass, shared library for profiling and back-annotation
  lwip-hcsim/                -- lwIP stacks ported on SystemC-based OS model (HCSim)
  examples/vision_graph/     -- The vision graph discovery example
     ezSIFT/                     - The SIFT (scale-invariant feature transform) algorithm library
     networking_api/             - Networking APIs and runtime libraries (based on lwip and HCSim) 
     src/                        - Source code for application, system and network models

  examples/ecg_diagnosis/    -- The ECG monitoring example
     ecg/     	                 - ECG diagnosis library
     networking_api/             - Networking APIs and runtime libraries (based on lwip and HCSim) 
     src/                        - Source code for application, system and network models
```

Building and installing:
------------------------
Build requirements:
  - SystemC version 2.3.1 http://www.accellera.org/
  - OMNEST version 5.0  http://www.omnest.com/
    (alternatively OMNeT++ version 5.0 with SystemC integration http://www.omnetpp.org)
  - INET framework version 3.4.0 http://inet.omnetpp.org/


Configuration of the package is done by manually editing the Makefile. The
following macros need to be adjusted to point to the locations of the
pre-installed packages listed above:
  - HCSIM_DIR -- Host-compiled SystemC simulator
  - OMNET_DIR -- OMNEST/OMNeT++ including SystemC integration
               (default is the $omnetpp_root environment variable)
  - INET_DIR  -- INET framework


Running:
--------
Before running the simulation, perform a function-level profiling and back-annotation 
by compiling the lwIP, ECG and ezSIFT library with the provided LLVM pass and 
helper functions in InstrumentLLVM directory, and properly set the 
all the library pathes variables in the application Makefile 


To build each example locally, change into the corresponding example
directory and run:
  - % cd examples/vision_graph
  - % make 

Then to run an example locally:
  - % cd examples/ecg_diagnosis
  - % make test




References:
-----------
- [1] Z. Zhao, V. Tsoutsouras, D. Soudris and A. Gerstlauer, "Network/System 
    Co-Simulation for Design Space Exploration of IoT Applications," SAMOS, July 2017.
- [2] P. Razaghi, A. Gerstlauer, "Host-Compiled Multi-Core System Simulation
    for Early Real-Time Performance Evaluation," ACM Transactions on Embedded
    Computer Systems, 2014.


Contact: 
--------
     Zhuoran Zhao <zhuoran@utexas.edu>


