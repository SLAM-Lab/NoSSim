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

   result_dic={}
   result_dic["cli"] = {"lwip_core" : cli_core_util_lwip, "app_core" : cli_core_util_app}
   result_dic["srv"] = {"lwip_core" : srv_core_util_lwip, "app_core" : srv_core_util_app}
   result_dic["o2o_delay"] = o2o_delay

   return result_dic


#{
#    "protocol": config_vec[0],
#    "srv_type":config_vec[1],
#    "srv_core_num": config_vec[2],
#}




def gen_data_3d():
#    O-3         O-2        O-1        O-0
#b11,g9,g54, b11,g9,g54, b11,g9,g54, b11,g9,g54
	s3d3=[]
	s0d3=[]
	s3s4=[]
	s3s3=[]
	s0s3=[]
        protocol_num=[0,1,2]
        offload_num=[3,2,1,0]
	cli_num=2


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

def gen_mixed_data_3d():

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
	offload_num=[3,2,1,0]
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





import random

def gen_random_data_3d():
	cli_num=6
        protocol_num=[0,1,2]
	offload_level_opt=3
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
	print "NoS ecg ... ..."
	gen_random_data_3d()



	#gen_mixed_data_3d()

"""
	s3d3 = {0:[],1:[],2:[],3:[]}
	srv_app = {0:[],1:[],2:[],3:[]}
	srv_lwip = {0:[],1:[],2:[],3:[]}
	cli_app = {0:[],1:[],2:[],3:[]}
	cli_lwip = {0:[],1:[],2:[],3:[]}
        protocol_num=[0,1,2]
	cli_num_list=[6,4,2]
        offload_num=[3,2,1,0]
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
	print " Offloading level 3 "
	print "s3d3 = ", s3d3[3]
	print "srv_app = ", srv_app[3]
	print "srv_lwip = ", srv_lwip[3]
	print "cli_app = ", cli_app[3]
	print "cli_lwip = ", cli_lwip[3]

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
"""


"""
#SAMOS reference data
#6 Clis 
s3d3 =  [0.157711, 0.0763945, 0.0756297, 0.140647, 0.0735205, 0.0722096, 0.141803, 0.063772, 0.062461, 0.140932, 0.0566548, 0.0439586]
s0d3 =  [0.158757, 0.0764987, 0.0758213, 0.141418, 0.073601, 0.0723992, 0.142777, 0.0638524, 0.0626507, 0.143551, 0.0709747, 0.0633875]
s3s4 =  [0.192202, 0.0988086, 0.0875637, 0.172974, 0.0897292, 0.0795642, 0.169139, 0.0858933, 0.0757282, 0.140907, 0.0566298, 0.045928]
s3s3 =  [0.228513, 0.135135, 0.121004, 0.207537, 0.124292, 0.111641, 0.198708, 0.115463, 0.102812, 0.140918, 0.056641, 0.0439448]
s0s3 =  [0.229275, 0.135559, 0.122992, 0.208794, 0.124805, 0.113348, 0.200167, 0.116178, 0.104721, 0.143551, 0.0709747, 0.0633875]
#4 Clis
s3d3 =  [0.109286, 0.0509768, 0.050478, 0.0970985, 0.0490613, 0.0482004, 0.0979384, 0.0425623, 0.0417014, 0.0977583, 0.0422334, 0.0361759]
s0d3 =  [0.109527, 0.0510108, 0.0505914, 0.0978341, 0.049132, 0.0483751, 0.0988757, 0.042633, 0.041876, 0.100497, 0.0603281, 0.0555053]
s3s4 =  [0.131655, 0.0670828, 0.0593083, 0.118281, 0.0606126, 0.0538583, 0.115792, 0.0581241, 0.0513698, 0.0977358, 0.0422022, 0.037414]
s3s3 =  [0.155888, 0.0913109, 0.0816269, 0.141352, 0.083684, 0.0751563, 0.135535, 0.077867, 0.0693393, 0.0977445, 0.0422196, 0.0361656]
s0s3 =  [0.156311, 0.0912516, 0.0828435, 0.142178, 0.083717, 0.0762527, 0.136562, 0.0781017, 0.0706374, 0.100497, 0.0603281, 0.0555053]
#2 Clis
s3d3 =  [0.0564701, 0.0254942, 0.0252618, 0.0504425, 0.024573, 0.0241369, 0.0509657, 0.0218514, 0.0210474, 0.053122, 0.0310592, 0.0279574]
s0d3 =  [0.0566749, 0.025523, 0.0253367, 0.0506949, 0.0246507, 0.0244228, 0.0514197, 0.024785, 0.0234933, 0.0717801, 0.0497914, 0.0484519]
s3s4 =  [0.0663916, 0.0335643, 0.0298551, 0.0597075, 0.0305824, 0.0271925, 0.0585799, 0.0295636, 0.0264175, 0.053092, 0.0310293, 0.0284247]
s3s3 =  [0.0785474, 0.0457201, 0.0409847, 0.0713546, 0.0421628, 0.037988, 0.0685493, 0.0394823, 0.0356656, 0.0531082, 0.0310454, 0.0279437]
s0s3 =  [0.078766, 0.0456782, 0.0429859, 0.0716208, 0.0425682, 0.0403376, 0.0690171, 0.0412256, 0.0390962, 0.0717801, 0.0497968, 0.0484519]

"""
"""
#mixed results
s3d3 =  [0.146617, 0.0630461, 0.0582846, 0.146617, 0.0630461, 0.0582846, 0.146617, 0.0630461, 0.0582846, 0.146617, 0.0630461, 0.0582846]
s0d3 =  [0.149373, 0.0717551, 0.0654588, 0.149373, 0.0717551, 0.0654588, 0.149373, 0.0717551, 0.0654588, 0.149373, 0.0717551, 0.0654588]
s3s4 =  [0.172572, 0.0864567, 0.076122, 0.172572, 0.0864567, 0.076122, 0.172572, 0.0864567, 0.076122, 0.172572, 0.0864567, 0.076122]
s3s3 =  [0.200299, 0.114186, 0.101028, 0.200299, 0.114186, 0.101028, 0.200299, 0.114186, 0.101028, 0.200299, 0.114186, 0.101028]
s0s3 =  [0.203214, 0.120045, 0.110047, 0.203214, 0.120045, 0.110047, 0.203214, 0.120045, 0.110047, 0.203214, 0.120045, 0.110047]

s3d3 =  [0.146399, 0.0635268, 0.0582892, 0.146399, 0.0635268, 0.0582892, 0.146399, 0.0635268, 0.0582892, 0.146399, 0.0635268, 0.0582892]
s0d3 =  [0.14711, 0.0716962, 0.0656394, 0.14711, 0.0716962, 0.0656394, 0.14711, 0.0716962, 0.0656394, 0.14711, 0.0716962, 0.0656394]
s3s4 =  [0.172211, 0.0859152, 0.0748392, 0.172211, 0.0859152, 0.0748392, 0.172211, 0.0859152, 0.0748392, 0.172211, 0.0859152, 0.0748392]
s3s3 =  [0.199912, 0.113632, 0.100262, 0.199912, 0.113632, 0.100262, 0.199912, 0.113632, 0.100262, 0.199912, 0.113632, 0.100262]
s0s3 =  [0.200556, 0.11834, 0.108665, 0.200556, 0.11834, 0.108665, 0.200556, 0.11834, 0.108665, 0.200556, 0.11834, 0.108665]

"""
