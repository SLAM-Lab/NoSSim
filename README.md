# NoSSim
Network-of-Systems Simulator
============================

Network-of-Systems Simulator (NoSSim) [1] is a network/system co-simulation framework
that combines a fast and accurate host-compiled full system simulator [2] with a standard, 
reconfigurable OMNeT++ network simulation backplane. Detailed models of network interfaces 
and protocol stacks (lwIP) are integrated into host-compiled system and OS models to
allow accurately capturing network and system interactions.

NoSSim is demonstrated with two IoT application scenarios with different set of 
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
  inet_extra/                -- INET patch for power/energy estimation
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

Preparation before build:
  - For setting up HCSim (included as a submodule), please see [here](https://github.com/SLAM-Lab/HCSim)
  - Apply the INET patch (inet_extra) and rebuild INET framework

Preparation before running the example:
  - Set the OMNeT++ path
```bash
  pushd /home/slam/OMNET/omnetpp-5.0; . setenv; popd
```
  - Set the INET path by changing the INET_DIR in [Makefile](https://github.com/SLAM-Lab/NoSSim/blob/master/examples/ecg_diagnosis/Makefile) for each application


Running:
--------
In general, you should perform a function-level profiling and back-annotation 
for the lwIP, ECG and ezSIFT library with the provided LLVM pass and 
helper functions in InstrumentLLVM directory. For the case studies included in 
this repository, we include [profile data for different platforms](https://github.com/SLAM-Lab/NoSSim/tree/master/examples/ecg_diagnosis/src/profile) so that you can directly try them out of box. 

To build an example, change into the corresponding example
directory and run:
```bash
  cd examples/vision_graph
  make dependency
  make makefiles
  make
```
To run an example, you may want to first change [the application](https://github.com/SLAM-Lab/NoSSim/blob/master/examples/vision_graph/src/sim_config.json), [system](https://github.com/SLAM-Lab/NoSSim/blob/master/examples/vision_graph/src/Cluster.ned) and [network](https://github.com/SLAM-Lab/NoSSim/blob/master/examples/vision_graph/src/omnetpp.ini) configration files based on your choice, then run:
```bash
   cd examples/ecg_diagnosis
   make test
```



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


