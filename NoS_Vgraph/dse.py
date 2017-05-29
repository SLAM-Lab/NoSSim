import numpy as np
import matplotlib.pyplot as plt
from numpy.random import random



def plot(cli_core_list, QoS_list):
	colorsred = [[1,0.9,0.9], [1,0.8,0.8], [1,0.7,0.7], [1,0.5,0.5], [1,0.3,0.3],[1,0.1,0.1], [0.9,0,0]]
	colorsgreen = ['darkgreen', 'seagreen', 'limegreen', 'springgreen']
	colorspurple =['indigo', 'purple', 'blueviolet', 'mediumorchid']
	colorsblue =[[0.8,0.8,0.9],[0.6,0.6,0.9], [0.4,0.4,0.9], [0.2,0.2,0.9],[0,0,0.9]]
	#colors = ['brown', 'red', 'tomato', 'lightsalmon', 'darkgreen', 'seagreen', 'limegreen', 'springgreen']
	#colorsred = ['brown', 'red', 'tomato', 'lightsalmon']
	#colorsgreen = ['darkgreen', 'seagreen', 'limegreen', 'springgreen']
	#colorsblue =['navy', 'blue', 'steelblue', 'lightsteelblue']
	#colorspurple =['indigo', 'purple', 'blueviolet', 'mediumorchid']

	colors = colorsred+colorsgreen+colorspurple+colorsblue
	label_size=30
	font_size=22
	legend_size=16


	fig = plt.figure()
	ax = fig.add_subplot(1, 1, 1)





	#RPi3
	#cli_core3 = [0.07242829, 0.081542199, 0.094914539, 0.17552642699999998, 0.14891839299999998, 0.152300863, 0.204632127, 0.4366954, 0.150342575, 0.15358281099999999, 0.206639919, 0.56286176]
	#QoS3 = [6.024265137547527, 6.770714493188323, 6.827961320964709, 7.126897982018836, 12.386401217830969, 12.646062016288127, 14.72077631485974, 17.845511407743167, 12.504837809127407, 12.752499489900021, 14.865257588899816, 24.22480620155039]

	i=0
	for cli_core, QoS in zip(cli_core_list, QoS_list):
		d1 = ax.scatter (cli_core,    QoS,    s=20,    marker="s",    color=colors[i%(len(colors))])
		i=i+1
	#d2 = ax.scatter(cli_core3[1], QoS3[1], s=200, marker='s', color=colorsred[1])
	#d3 = ax.scatter(cli_core3[2], QoS3[2], s=200, marker='s', color=colorsred[2])
	#d4 = ax.scatter(cli_core3[3], QoS3[3], s=200, marker='s', color=colorsred[3])

	#e1 = ax.scatter(cli_core3[4], QoS3[4], s=200, marker="s", color=colorsgreen[0])
	#e2 = ax.scatter(cli_core3[5], QoS3[5], s=200, marker='s', color=colorsgreen[1])
	#e3 = ax.scatter(cli_core3[6], QoS3[6], s=200, marker='s', color=colorsgreen[2])
	#e4 = ax.scatter(cli_core3[7], QoS3[7], s=200, marker='s', color=colorsgreen[3])

	#f1 = ax.scatter(cli_core3[8], QoS3[8], s=200, marker="s", color=colorsblue[0])
	#f2 = ax.scatter(cli_core3[9], QoS3[9], s=200, marker='s', color=colorsblue[1])
	#f3 = ax.scatter(cli_core3[10], QoS3[10], s=200, marker='s', color=colorsblue[2])
	#f4 = ax.scatter(cli_core3[11], QoS3[11], s=200, marker='s', color=colorsblue[3])

	#find pareto front
	#QoS = QoS0+QoS3
	#cli_core = cli_core0+cli_core3

	#flag = 0;
	#for i in range(len(QoS)):
	#   flag = 1
	#   for j in range(len(QoS)):
	#	if QoS[j]>QoS[i] and cli_core[j]<cli_core[i]:
			#print "==="
			#print i
			#print "==="
	#		flag = 0;
	#   if flag==1:
	#	print i




	plt.gca().invert_yaxis()
	#plt.gca().invert_xaxis()
	plt.ylabel('Throughput       ', fontsize=label_size)
	#plt.xlabel('Client core utilization', fontsize=24)
	plt.xticks(fontsize=font_size)
	plt.yticks(fontsize=font_size)
	plt.xlim([0,0.7])
	plt.ylim([1.5,0])

	vals = ax.get_xticks()
	ax.set_xticklabels(['{:3.0f}%'.format(x*100) for x in vals])
	ax.set_xlabel('Client core utilization', fontsize=label_size)

	#fig.savefig('dse_all_ECG.pdf', bbox_inches='tight')




	#fig.legend(
	#	    (
	#	     a1, a2, a3, a4,
	#	     b1, b2, b3, b4,
	#	     c1, c2, c3, c4,
	#	     d1, d2, d3, d4,
	#	     e1, e2, e3, e4,
	#	     f1, f2, f3, f4
	#	    ),
	#            (
	#		channels[0]+Offloadings[3]+cli_type[0], channels[0]+Offloadings[2]+cli_type[0], channels[0]+Offloadings[1]+cli_type[0], channels[0]+Offloadings[0]+cli_type[0],
	#		channels[1]+Offloadings[3]+cli_type[0], channels[1]+Offloadings[2]+cli_type[0], channels[1]+Offloadings[1]+cli_type[0], channels[1]+Offloadings[0]+cli_type[0],
	#		channels[2]+Offloadings[3]+cli_type[0], channels[2]+Offloadings[2]+cli_type[0], channels[2]+Offloadings[1]+cli_type[0], channels[2]+Offloadings[0]+cli_type[0],
	#		channels[0]+Offloadings[3]+cli_type[1], channels[0]+Offloadings[2]+cli_type[1], channels[0]+Offloadings[1]+cli_type[1], channels[0]+Offloadings[0]+cli_type[1],
	#		channels[1]+Offloadings[3]+cli_type[1], channels[1]+Offloadings[2]+cli_type[1], channels[1]+Offloadings[1]+cli_type[1], channels[1]+Offloadings[0]+cli_type[1],
	#		channels[2]+Offloadings[3]+cli_type[1], channels[2]+Offloadings[2]+cli_type[1], channels[2]+Offloadings[1]+cli_type[1], channels[2]+Offloadings[0]+cli_type[1]
	#	    ),
	#           scatterpoints=1,
	# 	    loc='upper center',
	#           ncol=3,
	#	    prop={'size':legend_size})


	#plt.xlim([max(cli_core),0])
	#plt.ylim([min(QoS),max(QoS)])

	plt.show()


if __name__ == "__main__":
	
	import json
	from pprint import pprint
	cli_core_list=[[],[]]
	QoS_list=[[],[]]

	with open('./data_rd2.json') as data_file:    
	    data = json.load(data_file)


        for key in data.keys():
	    cli_core_list[0] = cli_core_list[0] + data[key]["cli_core"]
    	    QoS_list[0] = QoS_list[0] + data[key]["QoS"]

#	with open('./data.json') as data_file:    
#	    data = json.load(data_file)


#       for key in data.keys():
#	    cli_core_list[1] = cli_core_list[1] + data[key]["cli_core"]
#    	    QoS_list[1] = QoS_list[1] + data[key]["QoS"]

	plot(cli_core_list, QoS_list) 
	


