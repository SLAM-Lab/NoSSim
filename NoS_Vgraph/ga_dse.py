#!/usr/bin/env python2.7
#   NoSSim exploration framework
#   Author: Zhuoran Zhao
#   Date: 2017/03/18 
#   This GA-based exploration is developed with DEAP library 
#   Single/Multi-Object GA are encapsulated in different functions main_ga, main_moga 

import random
import array
import json
import numpy
import pprint

from deap import algorithms
from deap import base
from deap import creator
from deap import tools
from deap import benchmarks
from deap.benchmarks.tools import diversity, convergence


import explore 
#cli_num = 6
#{
#    "protocol": config_vec[0], #0-2
#    "srv_type":config_vec[1],#0-2
#    "srv_core_num": config_vec[2],#1-2
#}#min [0,1,1] max [2,2,2]
#offload_list = #min [0,0,0,0,0,0]# max [2,2,2,2,2,2] 
#cli_type_list = #min [0,0,0,0,0,0]# max [1,1,1,1,1,1] 
#cli_core_num_list = [1,1,1,1,1,1]

#min = [0,1,1,  0,0,0,0,0,0,  0,0,0,0,0,0]
#max = [2,2,2,  2,2,2,2,2,2,  1,1,1,1,1,1]

#result = explore.evaluate(6, individual[0:3], individual[3:9], individual[9:15], [1,1,1,1,1,1])
#result = explore.evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
#return 1/result["o2o_delay"] , result["cli"]["lwip_core"]+result["cli"]["app_core"]

creator.create("FitnessMulti", base.Fitness, weights=(1.0, -1.0))
creator.create("Individual", list, fitness=creator.FitnessMulti)

toolbox = base.Toolbox()


knobs_low = [0,1,1,  0,0,0,0,0,0,  0,0,0,0,0,0]
knobs_up =  [2,2,2,  2,2,2,2,2,2,  1,1,1,1,1,1]
#customize the

ga_data={}


for i in range(len(knobs_low)):
   toolbox.register("attr_int"+str(i), random.randint, knobs_low[i], knobs_up[i])   

#toolbox.register("attr_int0", random.randint, 1, 5)
#toolbox.register("attr_int1", random.randint, 1, 5)
#toolbox.register("attr_int2", random.randint, 1, 5)
#toolbox.register("attr_int3", random.randint, 1, 5)
#toolbox.register("attr_int4", random.randint, 1, 5)
#toolbox.register("attr_int5", random.randint, 1, 5)
#toolbox.register("attr_int6", random.randint, 1, 5)
#toolbox.register("attr_int7", random.randint, 10, 20)
#toolbox.register("attr_int8", random.randint, 10, 20)
#toolbox.register("attr_int9", random.randint, 10, 20)
#toolbox.register("attr_int10", random.randint, 10, 20)
#toolbox.register("attr_int11", random.randint, 10, 20)


toolbox.register("individual", tools.initCycle, creator.Individual,
                 (toolbox.attr_int0, toolbox.attr_int1, toolbox.attr_int2, toolbox.attr_int3, toolbox.attr_int4, 
		  toolbox.attr_int5, toolbox.attr_int6, toolbox.attr_int7, toolbox.attr_int8, toolbox.attr_int9, 
		  toolbox.attr_int10, toolbox.attr_int11,toolbox.attr_int12, toolbox.attr_int13,toolbox.attr_int14), 
		 n=1)
toolbox.register("population", tools.initRepeat, list, toolbox.individual)


def evalOneMax(individual):
	result = explore.evaluate(6, individual[0:3], individual[3:9], individual[9:15], [1,1,1,1,1,1])
	#result = explore.evaluate(cli_num, config_vec, offload_list, cli_type_list, cli_core_num_list)
	return 1/result["o2o_delay"] , result["cli"]["lwip_core"]+result["cli"]["app_core"]
	#return sum(individual[0:8]), sum(individual[6:len(individual)])

#----------

def mainNSGA(seed=None):
    toolbox.register("evaluate", evalOneMax)
    toolbox.register("mate", tools.cxTwoPoint)
    toolbox.register("mutate", tools.mutUniformInt, low=knobs_low, up=knobs_up, indpb=0.05)
    toolbox.register("select", tools.selNSGA2)
    random.seed(seed)




    # MU  is the size of population, total number of individuals
    #	    in each generation 
    #
    # CXPB  is the probability with which two individuals
    #       are crossed
    #
    # MUTPB is the probability for mutating an individual
    #
    # NGEN  is the number of generations for which the
    #       evolution runs


    MU = 200
    CXPB = 0.8
    MUTPB =  0.8
    NGEN = 20

    pop = toolbox.population(n=MU)

    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in pop if not ind.fitness.valid]
    fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
    for ind, fit in zip(invalid_ind, fitnesses):
        ind.fitness.values = fit

    # This is just to assign the crowding distance to the individuals
    # no actual selection is done
    pop = toolbox.select(pop, len(pop))
    


    # Begin the generational process
    for gen in range(1, NGEN):
        # Vary the population
        print(" ======Beginning %i th generation======: " % gen)
        offspring = tools.selTournamentDCD(pop, len(pop))
        offspring = [toolbox.clone(ind) for ind in offspring]
        
        for ind1, ind2 in zip(offspring[::2], offspring[1::2]):
            if random.random() <= CXPB:
                toolbox.mate(ind1, ind2)
            	del ind1.fitness.values, ind2.fitness.values
            if random.random() <= MUTPB:
                toolbox.mutate(ind1)
                toolbox.mutate(ind2)
            	del ind1.fitness.values, ind2.fitness.values
        
        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
        fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        # Select the next generation population
        pop = toolbox.select(pop + offspring, MU)

	fitness_list = []
    	fronts_lists = tools.sortNondominated(pop, len(pop), first_front_only=True)[0]	
	fronts=[]



	for i in range(len(fronts_lists)):
	   if fronts_lists[i] not in fronts:
		fronts.append(fronts_lists[i])
		fitness_list.append(fronts_lists[i].fitness.values)
        print " Pareto front is:"
	ga_data[gen]={"fitness":fitness_list,"front":fronts}
	pprint.pprint( fitness_list )
	pprint.pprint( fronts )

        print("  Evaluated %i individuals\n" % len(invalid_ind))


    jsonConfigFile="./ga_data.json"
    with open(jsonConfigFile,"w") as jFile:
      json.dump(ga_data, jFile, indent=4, separators=(',', ': '))


    print("-- End of (successful) evolution --")
    
    return pop

import matplotlib.pyplot as plt

def plot(cli_core_list, QoS_list):

	colorsred = [[1,0.96,0.96], [1,0.9,0.9], [1,0.7,0.7], [1,0.5,0.5], [1,0.3,0.3],[1,0.1,0.1], [0.9,0,0]]
	colorsgreen = ['darkgreen', 'seagreen', 'limegreen', 'springgreen']
	colorsblue =[[0.8,0.8,0.9],[0.6,0.6,0.9], [0.4,0.4,0.9], [0.2,0.2,0.9],[0,0,0.9]]
	colorspurple =['indigo', 'purple', 'blueviolet', 'mediumorchid']
	colors = colorsred+colorsgreen+colorsblue+colorspurple

	label_size=30
	font_size=22
	legend_size=16


	fig = plt.figure()
	ax = fig.add_subplot(1, 1, 1)


	i=0
	for cli_core, QoS in zip(cli_core_list, QoS_list):
		d1 = ax.scatter (cli_core,    QoS,    s=20,    marker="s",    color=colors[i%(len(colors))])
		i=i+1



	plt.gca().invert_yaxis()
	#plt.gca().invert_xaxis()
	plt.ylabel('Throughput', fontsize=label_size)
	#plt.xlabel('Client core utilization', fontsize=24)
	plt.xticks(fontsize=font_size)
	plt.yticks(fontsize=font_size)
	#plt.xlim([0,0.8])
	#plt.ylim([30,0])

	vals = ax.get_xticks()
	#ax.set_xticklabels(['{:3.0f}%'.format(x*100) for x in vals])
	#ax.set_xlabel('Client core utilization', fontsize=label_size)

	plt.show()


if __name__ == "__main__":

#    pop = mainNSGA()

    metricx_list=[]
    metricy_list=[]
    with open('./ga_data.json') as data_file:    
	    data = json.load(data_file)
    for key in range(len(data.keys())):
	#print data[key]["fitness"]
	#print data[key]["front"]
	metricx=[]
	metricy=[]
	for item in data[str(key+1)]["fitness"]:
	   metricx.append(item[1])
	   metricy.append(item[0])
        metricx_list.append(metricx)
	metricy_list.append(metricy)

    #dse.plot(metricx_list, metricy_list) 


    cli_core_list=[[]]
    QoS_list=[[]]
    with open('./data_rd2.json') as data_file:    
	    data = json.load(data_file)
    for key in data.keys():
	    cli_core_list[0] = cli_core_list[0] + data[key]["cli_core"]
    	    QoS_list[0] = QoS_list[0] + data[key]["QoS"]


    import dse
    dse.plot(cli_core_list+metricx_list, QoS_list+metricy_list) 



