#!/usr/bin/env python2.7
import fileinput
import json 
from subprocess import call
import os
appDir = "."
protocolOpt = [("b", "11Mbps"), ("g", "9Mbps"), ("g", "54Mbps")]
#Network protocol configuration
#input: 
#   cli_num: int (2-10)
#   protocol: int (0-2)
def omnetpp_ini(cli_num, protocol):
   numCli=str(cli_num) 
   opMode=protocolOpt[protocol][0]
   bitRate=protocolOpt[protocol][1]
   fileToSearch = appDir+"/src/omnetpp.ini"
   opModeNToSearch = "**.wlan[*].mac.opMode"
   opModeToSearch = "**.wlan.mac.opMode"
   bitRateNToSearch = "**.wlan[*].mac.basicBitrate"
   bitRateToSearch = "**.wlan.mac.basicBitrate"
   numCliToSearch="Throughput.numCli"
   inifile = fileinput.FileInput(fileToSearch, inplace=True)
   for line in inifile:
     if opModeNToSearch in line:
       line = opModeNToSearch + "=" + "\"" + opMode + "\"" 
     if opModeToSearch in line:
       line = opModeToSearch + "=" + "\"" + opMode + "\"" 
     if bitRateNToSearch in line:
       line = bitRateNToSearch + "="  + bitRate
     if bitRateToSearch in line:
       line = bitRateToSearch + "="   + bitRate
     if numCliToSearch in line:
       line = numCliToSearch + "=" + numCli
     print line.rstrip()
   inifile.close()


#Dump a json file into 
#config_vector
#{
#    "protocol": config_vec[0],
#    "srv_type":config_vec[1],
#    "srv_core_num": config_vec[2],
#}

#Offloading list: [1,1,1,1,1,1] #number of  
def config_json(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list):
   config_dic={}
   config_dic["cli_num"] = cli_num
   config_dic["protocol"] = config_vec[0]
   config_dic["offloading"] = offload_list
   config_dic["cli_type"] = cli_type_list
   config_dic["cli_core_num"] = cli_core_num_list
   config_dic["srv_type"] = config_vec[1]
   config_dic["srv_core_num"] = config_vec[2]

   jsonConfigFile=appDir+"/src/conf.json"
   with open(jsonConfigFile,"w") as jFile:
      json.dump(config_dic, jFile, indent=4, separators=(',', ': '))
      #json.dump(config_dic, jFile)


#Evaluate the metrics of a configuration vector 
#config_vector
#{
#    "protocol": config_vec[0],
#    "srv_type":config_vec[1],
#    "srv_core_num": config_vec[2],
#}


#input: config_vector
#       cli_num
#	offloading_list
#	cli_type_list
#	cli_core_num_list



def evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list):
   omnetpp_ini(cli_num, config_vec[0])
   config_json(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list) 
   call(["make","test"])
   #os.system("make test")
   print "Simulation is finished"
   #{
   #  "lwip_core": 0.0810244,
   #  "app_core": 0.593459,
   #  "o2o_delay": 0.12266
   #}
   ####SRV result####
   srv_core_util_lwip=0.0
   srv_core_util_app=0.0
   o2o_delay = 0.0
   resultJsonFile=appDir+"/src/srv.json"
   with open(resultJsonFile) as jFile:
      result = json.load(jFile)   
   print result["lwip_core"] 
   print result["app_core"] 
   print result["o2o_delay"] 
   o2o_delay=result["o2o_delay"]
   srv_core_util_lwip = result["lwip_core"]
   srv_core_util_app = result["app_core"]
   srv_core_util = srv_core_util_lwip+srv_core_util_app
   ####CLI result####

   cli_core_util_lwip=0.0
   cli_core_util_app=0.0
   for cli in range(cli_num):   
     resultJsonFile=appDir+"/src/cli"+str(cli+1)+".json"
     print "=====", cli, "====="
     with open(resultJsonFile) as jFile:
       result = json.load(jFile)   
     cli_core_util_lwip = cli_core_util_lwip + result["lwip_core"]
     cli_core_util_app = cli_core_util_app + result["app_core"]
     print result["lwip_core"] 
     print result["app_core"] 
     print result["o2o_delay"] 
   cli_core_util_lwip = cli_core_util_lwip/cli_num
   cli_core_util_app = cli_core_util_app/cli_num    
   print cli_core_util_lwip
   print cli_core_util_app

   cli_core_util = cli_core_util_lwip+cli_core_util_app

   result_dic={}
   result_dic["cli"] = {"lwip_core" : cli_core_util_lwip, "app_core" : cli_core_util_app}
   result_dic["srv"] = {"lwip_core" : srv_core_util_lwip, "app_core" : srv_core_util_app}
   result_dic["o2o_delay"] = o2o_delay

   return result_dic





def gen_data_3d():
#    O-3         O-2        O-1        O-0
#b11,g9,g54, b11,g9,g54, b11,g9,g54, b11,g9,g54
	s3d3=[]
	s0d3=[]
	s3s4=[]
	s3s3=[]
	s0s3=[]
        protocol_num=[0,1,2]
        offload_num=[2,1,0]
	cli_num=4


	config_vec=[0,1,1]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [0,0,0,0,0,0]
        for o_num in offload_num:
           for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s0s3.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])

	config_vec=[0,1,2]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [0,0,0,0,0,0]
        for o_num in offload_num:
           for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s0d3.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])

	config_vec=[0,1,2]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
        for o_num in offload_num:
           for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s3d3.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])

	config_vec=[0,2,1]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
        for o_num in offload_num:
           for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s3s4.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])
	config_vec=[0,1,1]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
        for o_num in offload_num:
           for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s3s3.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])

	print "s3d3 = ", s3d3
	print "s0d3 = ", s0d3
	print "s3s4 = ", s3s4
	print "s3s3 = ", s3s3
	print "s0s3 = ", s0s3
        z= s3d3 + s0d3 + s3s4 + s3s3 + s0s3
        print "z = ", z

def core_util():

	s3d3 = {0:[],1:[],2:[]}
	srv_app = {0:[],1:[],2:[]}
	srv_lwip = {0:[],1:[],2:[]}
	cli_app = {0:[],1:[],2:[]}
	cli_lwip = {0:[],1:[],2:[]}
        protocol_num=[0,1,2]
	cli_num_list=[6,4,2]
        offload_num=[2,1,0]
	config_vec=[0,1,2]
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]


	result={}
	for o_num in offload_num:
	    for cli_num in cli_num_list:
		for p_num in protocol_num:
		     config_vec[0]=p_num
		     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
		     result = evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
		     s3d3[o_num].append(result["o2o_delay"])	   
		     srv_app[o_num].append(result["srv"]["app_core"]) 
		     srv_lwip[o_num].append(result["srv"]["lwip_core"]) 
		     cli_app[o_num].append(result["cli"]["app_core"]) 
		     cli_lwip[o_num].append(result["cli"]["lwip_core"]) 	       

	print " Offloading level 2 "
	print "s3d3 = ", s3d3[2]
	print "srv_app = ", srv_app[2]
	print "srv_lwip = ", srv_lwip[2]
	print "cli_app = ", cli_app[2]
	print "cli_lwip = ", cli_lwip[2]

	print " Offloading level 1 "
	print "s3d3 = ", s3d3[1]
	print "srv_app = ", srv_app[1]
	print "srv_lwip = ", srv_lwip[1]
	print "cli_app = ", cli_app[1]
	print "cli_lwip = ", cli_lwip[1]

	print " Offloading level 0 "
	print "s3d3 = ", s3d3[0]
	print "srv_app = ", srv_app[0]
	print "srv_lwip = ", srv_lwip[0]
	print "cli_app = ", cli_app[0]
	print "cli_lwip = ", cli_lwip[0]
	import core_util_plot
	core_util_plot.plot(srv_app, srv_lwip, cli_app, cli_lwip)

def gen_mixed_data_3d():
#    O-3         O-2        O-1        O-0
#b11,g9,g54, b11,g9,g54, b11,g9,g54, b11,g9,g54
	s3d3=[]
	s0d3=[]
	s3s4=[]
	s3s3=[]
	s0s3=[]

	cli_num=6
        protocol_num=[0,1,2]

	#
	#
	cli_core_list=[]
	QoS_list=[] 
	cli_core=[]
	QoS=[]
	offload_num=[2,1,0]
	data_dic={}


	config_vec=[0,1,1]#S0S3
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [0,0,0,0,0,0]
	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	     QoS.append (1/result["o2o_delay"])
	     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])

	cli_core_list.append(cli_core)
	QoS_list.append(QoS)
	data_dic["s0s3"] = {"cli_core":cli_core, "QoS":QoS}
	cli_core=[]
	QoS=[]

	config_vec=[0,1,1]#S3S3
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	     QoS.append (1/result["o2o_delay"])
	     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])
	cli_core_list.append(cli_core)
	QoS_list.append(QoS)
	data_dic["s3s3"] = {"cli_core":cli_core, "QoS":QoS}
	cli_core=[]
	QoS=[]


	config_vec=[0,1,2]#S3D3
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	     QoS.append (1/result["o2o_delay"])
	     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])
	cli_core_list.append(cli_core)
	QoS_list.append(QoS)
	data_dic["s3d3"] = {"cli_core":cli_core, "QoS":QoS}
	cli_core=[]
	QoS=[]

	config_vec=[0,2,1]#S3S4
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [1,1,1,1,1,1]
	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	     QoS.append (1/result["o2o_delay"])
	     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])
	cli_core_list.append(cli_core)
	QoS_list.append(QoS)
	data_dic["s3s4"] = {"cli_core":cli_core, "QoS":QoS}
	cli_core=[]
	QoS=[]

	config_vec=[0,1,2]#S0D3
	cli_core_num_list = [1,1,1,1,1,1]
	cli_type_list = [0,0,0,0,0,0]
	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	     QoS.append (1/result["o2o_delay"])
	     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])
	cli_core_list.append(cli_core)
	QoS_list.append(QoS)
	data_dic["s0d3"] = {"cli_core":cli_core, "QoS":QoS}
	cli_core=[]
	QoS=[]


	jsonConfigFile="./data.json"
   	with open(jsonConfigFile,"w") as jFile:
      		json.dump(data_dic, jFile, indent=4, separators=(',', ': '))



	import dse
	dse.plot(cli_core_list, QoS_list)

"""

	offload_list = [0,0,0,0,0,0]
	offload_list = [1,1,1,1,1,1]
	offload_list = [2,2,2,2,2,2]

	for o_num in offload_num:
          for p_num in protocol_num:
             config_vec[0]=p_num
	     offload_list = [o_num,o_num,o_num,o_num,o_num,o_num]
	     s0s3.append (evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)["o2o_delay"])
"""
import random

def gen_random_data_3d():
	cli_num=6
        protocol_num=[0,1,2]
	offload_level_opt=2
	cli_core_list=[]
	QoS_list=[] 
	cli_core=[]
	QoS=[]

	data_dic={}
	for i in range(1000):
		config_vec=[0,random.randint(1,2),random.randint(1,2)]
		cli_core_num_list = [1,1,1,1,1,1]
		cli_type_list = [random.randint(0,1),random.randint(0,1),random.randint(0,1),random.randint(0,1),random.randint(0,1),random.randint(0,1)]
		offload_list = [random.randint(0,offload_level_opt),random.randint(0,offload_level_opt),random.randint(0,offload_level_opt),random.randint(0,offload_level_opt),random.randint(0,offload_level_opt),random.randint(0,offload_level_opt)]
		for p_num in protocol_num:
		     config_vec[0]=p_num
		     result=evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
		     QoS.append (1/result["o2o_delay"])
		     cli_core.append (result["cli"]["lwip_core"] + result["cli"]["app_core"])
		cli_core_list.append(cli_core)
		QoS_list.append(QoS)
		data_dic[''.join(str(e) for e in config_vec)+"_"+''.join(str(e) for e in offload_list) + "_" + ''.join(str(e) for e in cli_type_list)] = {"cli_core":cli_core, "QoS":QoS}
		cli_core=[]
		QoS=[]
	jsonConfigFile="./data_rd2.json"
   	with open(jsonConfigFile,"w") as jFile:
      		json.dump(data_dic, jFile, indent=4, separators=(',', ': '))
	#import dse
	#dse.plot(cli_core_list, QoS_list)

if __name__ == "__main__":
	print "NoS v graph ... ..."
	gen_random_data_3d()

#latency data format 
#		Cli num
#     O-2        O-1         O-0
# b11,g9,g54, b11,g9,g54, b11,g9,g54

        #z6 = [3.76735, 3.64103, 3.62587, 2.215, 1.98424, 1.95581, 3.62907, 1.85211, 1.44108, 
		#3.76787, 3.64412, 3.62894, 2.21623, 1.98859, 1.96165, 3.64811, 2.47293, 2.3633, 
		#2.69659, 2.16988, 2.11767, 2.53516, 1.61182, 1.52116, 3.87885, 2.13405, 1.74993, 
		#4.47063, 3.9443, 3.87789, 3.43988, 2.5166, 2.4008, 4.40455, 2.62906, 2.21846, 
		#4.47384, 3.95521, 3.88965, 3.44712, 2.53563, 2.42313, 4.42359, 3.19796, 2.88952]
        #z6 = z6_new
	#z4 =  [2.49155, 2.39003, 2.3784, 1.51097, 1.08772, 1.06625, 2.66874, 1.2528, 1.05183, 
		#2.49181, 2.39249, 2.38094, 1.67693, 1.33685, 1.29729, 2.85699, 2.23224, 2.1869, 
		#1.88696, 1.46908, 1.42838, 1.75072, 1.01914, 0.952479, 2.74543, 1.35145, 1.15511, 
		#3.04527, 2.62618, 2.57628, 2.2261, 1.49416, 1.40894, 2.90333, 1.4889, 1.28761, 
		#3.047, 2.63423, 2.58599, 2.24827, 1.52951, 1.44612, 3.09053, 2.23891, 2.1869]
	#z4_new = [2.49155, 2.39003, 2.3784, 1.51097, 1.08772, 1.06625, 2.66874, 1.2528, 1.05183, 
		#2.49181, 2.39249, 2.38094, 1.67693, 1.33685, 1.29729, 2.87003, 2.23224, 2.1869, 
		#1.88696, 1.46908, 1.42838, 1.75072, 1.01914, 0.952479, 2.74543, 1.35145, 1.15511, 
		#3.04527, 2.62618, 2.57628, 2.2261, 1.49416, 1.40894, 2.90333, 1.4889, 1.28761, 
		#3.047, 2.63423, 2.58599, 2.24827, 1.52951, 1.44612, 3.10358, 2.23891, 2.1869]
 
	#z2 =  [0.922642, 0.864473, 0.858242, 0.840701, 0.567338, 0.537803, 1.54779, 0.942287, 0.888577, 
		#0.922826, 0.866017, 0.859765, 1.50812, 1.24612, 1.21712, 2.6981, 2.22863, 2.18287, 
		#0.821534, 0.584489, 0.562578, 0.91875, 0.567954, 0.544546, 1.54769, 0.949297, 0.901845, 
		#1.23306, 0.994674, 0.968444, 1.00107, 0.576116, 0.537734, 1.54773, 0.942218, 0.888508, 
		#1.23331, 0.999123, 0.974606, 1.50812, 1.24612, 1.21712, 2.6981, 2.22863, 2.18287]
	#z2_new =  [0.922642, 0.864473, 0.858242, 0.840701, 0.567338, 0.537803, 1.54841, 0.934865, 0.881155, 
		#0.922826, 0.866017, 0.859765, 1.50812, 1.24612, 1.21712, 2.67748, 2.20801, 2.16225, 
		#0.821534, 0.584489, 0.562578, 0.91875, 0.567954, 0.544546, 1.5483, 0.941875, 0.894423, 
		#1.23306, 0.994674, 0.968444, 1.00107, 0.576116, 0.537734, 1.54834, 0.934796, 0.881086, 
		#1.23331, 0.999123, 0.974606, 1.50812, 1.24612, 1.21712, 2.67748, 2.20801, 2.16225]

#core utilization data format
#	  Offloading level
#   cli 6       cli 4       cli 2
# b11,g9,g54, b11,g9,g54, b11,g9,g54


#config_vector
#{
#    "protocol": config_vec[0],
#    "srv_type":config_vec[1],
#    "srv_core_num": config_vec[2],
#}

	#gen_data_3d()




"""
	#Offloading level 2 
	s3d3[2] =  [3.76735, 3.64103, 3.62587, 2.49155, 2.39003, 2.3784, 0.922642, 0.864473, 0.858242]
	srv_app[2] = [0.941674, 0.974344, 0.978418, 0.929617, 0.969103, 0.973841, 0.891641, 0.951639, 0.958548]
	srv_lwip[2] = [0.0142069, 0.0146998, 0.0147612, 0.0166109, 0.0173165, 0.0174011, 0.0249993, 0.0266815, 0.0268752]
	cli_app[2] = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
	cli_lwip[2] = [0.002430515, 0.002514838333333333, 0.002525356666666666, 0.0030004025000000003, 0.003127845, 0.0031431375, 0.00756259, 0.00807147, 0.00813007]
	#Offloading level 1 
	s3d3[1] =  [2.215, 1.98424, 1.95581, 1.51097, 1.08772, 1.06625, 0.840701, 0.567338, 0.537803]
	srv_app[1] = [0.816711, 0.911689, 0.924945, 0.628942, 0.873678, 0.891268, 0.25544, 0.378519, 0.399307]
	srv_lwip[1] = [0.042305, 0.0472247, 0.0479114, 0.0473343, 0.0657531, 0.0670769, 0.0488817, 0.0724346, 0.0764126]
	cli_app[1] = [0.13082028333333334, 0.14603375, 0.14815728333333333, 0.22599199999999997, 0.3139305, 0.320251, 0.36155349999999997, 0.535763, 0.565186]
	cli_lwip[1] = [0.006769156666666667, 0.007556358333333335, 0.0076662300000000004, 0.0078818225, 0.010948824999999999, 0.01116925, 0.01386855, 0.02055095, 0.02167955]
	#Offloading level 0 
	s3d3[0] =  [3.62907, 1.85211, 1.44108, 2.66874, 1.2528, 1.05183, 1.54841, 0.934865, 0.881155]
	srv_app[0] = [0.289595, 0.567437, 0.729283, 0.118181, 0.251752, 0.299852, 0, 0, 0]
	srv_lwip[0] = [0.0402318, 0.0788308, 0.101315, 0.0416136, 0.0886465, 0.105584, 0.0410682, 0.0680209, 0.072167]
	cli_app[0] = [0.11466006666666667, 0.22466733333333336, 0.2887475, 0.18742874999999998, 0.399266, 0.47555149999999996, 0.334994, 0.5548465, 0.588667]
	cli_lwip[0] = [0.011493885000000002, 0.022521299999999998, 0.028944916666666664, 0.0129832575, 0.027657325000000003, 0.032941625, 0.0208563, 0.03454405, 0.036649650000000006]


"""



