# NoSSim
Network/System Co-Simulation Platform
=========================================

This folder contains the prototype for a network/system co-simulation approach
that combines a host-compiled SystemC simulation of device-internal SoC 
platforms with a simulation of network interactions using the OMNeT++ network 
simulation framework [1]. 

The approach is demonstrated on two IoT application scenarios with different set of 
network/system configurations. 


IoT scenarios:
------------------
```
  NoS_Vgraph       -- Vision graph discovery, where the relative position and 
		      orientation among a network of smart cameras are estimated
		      
  NoS_ECG          -- ECG monitoring, where raw ECG signals are used to 
		      detect heart arrhythmia
```

Directories:
------------
```
  config_json/      -- JSON lib and configuration file template
  helper/	    -- Shared library for profiling and back-annotation
  InstrumentLLVM/   -- LLVM function-level instrumentation pass.
  lwip/             -- lwIP stacks integrated with SystemC OS model
  NoS_Vgraph/       -- The vision graph discovery example
     src/             - source code
     out/             - compiled code
     input/           - client device input data set
     output/             - intermediate data and results

  NoS_ECG/          -- The ECG monitoring example
     ecg/     	      - ECG diagnosis C library
     src/             - source code
     out/             - compiled code
     input/           - client device input data set
     output/             - intermediate data and results
```

Building and installing:
------------------------
Build requirements:
  - SystemC version 2.2.0 http://www.accellera.org/
  - OMNEST version 4.3  http://www.omnest.com/
    (alternatively OMNeT++ with SystemC integration http://www.omnetpp.org)
  - OMNeT++ INET framework version 2.2.0 http://inet.omnetpp.org/  
  - HCSim host-compiled simulator version 1.0 [2]
    http://www.ece.utexas.edu/~gerstl/releases/HCSim-1.0.tar.gz
  - OpenCV 3.1.0 http://opencv.org/

Configuration of the package is done by manually editing the Makefile. The
following macros need to be adjusted to point to the locations of the
pre-installed packages listed above:
  HCSIM_DIR -- Host-compiled SystemC simulator
  OMNET_DIR -- OMNEST/OMNeT++ including SystemC integration
               (default is the $omnetpp_root environment variable)
  INET_DIR  -- INET framework


Running:
--------
Before running the simulation, perform a function-level profiling and back-annotation 
by compiling the lwIP, ECG and OpenCV library with the provided LLVM pass and 
helper functions in InstrumentLLVM and helper directory, and properly set the 
all the library pathes variables in the application Makefile 


To build each example locally, change into the corresponding example
directory and run:
  % cd <example>
  % make 

Then to run an example locally:
  % cd <example>
  % make test


In all cases, <example> can be one of:
  NoS_ECG
  NoS_Vgraph


References:
-----------
[1] Z. Zhao, V. Tsoutsouras, D. Soudris and A. Gerstlauer, "Network/System 
    Co-Simulation for Design Space Exploration of IoT Applications," SAMOS, July 2017.
[2] P. Razaghi, A. Gerstlauer, "Host-Compiled Multi-Core System Simulation
    for Early Real-Time Performance Evaluation," ACM Transactions on Embedded
    Computer Systems, 2014.


Contact: 
--------
     Zhuoran Zhao <zhuoran@utexas.edu>


