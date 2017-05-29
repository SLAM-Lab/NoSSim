import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from mpl_toolkits.axes_grid.parasite_axes import SubplotHost
from mpl_toolkits.axes_grid1 import host_subplot
import mpl_toolkits.axisartist as AA


def plot(srv_app, srv_lwip, cli_app, cli_lwip):



	colorsred = ['brown', 'red', 'tomato', 'lightsalmon']
	colorsgreen = ['darkgreen', 'seagreen', 'limegreen', 'springgreen']
	colorsblue =['navy', 'blue', 'steelblue', 'lightsteelblue']

	hatches = ['//', '++', 'xxx',  'oo','\\\\\\', 'OO', '..' , '---', "**"]
	label_size=15
	font_size=15



	N = 3
	width = 0.2       # the width of the bars
	xtra_space = 0.01


	ind1 = np.arange(N) + 2 - (width*N+xtra_space*2)/2 # the x locations for the groups
	ind2 = np.arange(N) + 2+(N+1) - (width*N+xtra_space*2)/2 # the x locations for the groups
	ind3 = np.arange(N) + 2+N+1+N+1 - (width*N+xtra_space*2)/2 # the x locations for the groups


	ind = np.append(ind1, ind2)
	ind = np.append(ind, ind3)


	O3comp = cli_app[3]
	O3lwip = cli_lwip[3]
	O2comp = cli_app[2]
	O2lwip = cli_lwip[2]
	O1comp = cli_app[1]
	O1lwip = cli_lwip[1]
	O0comp = cli_app[0]
	O0lwip = cli_lwip[0]



	fig, ax = plt.subplots(2)
	a1 = ax[0].bar(ind, O3comp, width, color=[0,0.5,1]) 
	a2 = ax[0].bar(ind, O3lwip, width, fill=False, hatch=hatches[0], edgecolor=[0,0.5,1], bottom=O3comp) 

	b1 = ax[0].bar(ind+ width + xtra_space, O2comp, width, color=[0,1,0.5]) 
	b2 = ax[0].bar(ind+ width + xtra_space, O2lwip, width, fill=False, hatch=hatches[0], edgecolor=[0,1,0.5], bottom=O2comp) 

	c1 = ax[0].bar(ind+ 2*(width + xtra_space), O1comp, width, color=[1,0.5,0]) 
	c2 = ax[0].bar(ind+ 2*(width + xtra_space), O1lwip, width, fill=False, hatch=hatches[0], edgecolor=[1,0.5,0], bottom=O1comp) 

	d1 = ax[0].bar(ind+ 3*(width + xtra_space), O0comp, width, color=[0.6,0,0.8]) 
	d2 = ax[0].bar(ind+ 3*(width + xtra_space), O0lwip, width, fill=False, hatch=hatches[0], edgecolor=[0.6,0,0.8], bottom=O0comp) 


	OLevel = ["O-0", "O-1", "O-2", "O-3"]
	channels = ["b@11Mbps", "g@9Mbps", "g@54Mbps"]
	duration_type = [" - lwIP", " - App."]
	legend_size=16
	plt.figlegend(
		    (
		     a1, a2,
		     b1, b2,
		     c1, c2,
		     d1, d2
		    ),
		    (
			OLevel[3]+duration_type[1], OLevel[3]+duration_type[0], 
			OLevel[2]+duration_type[1], OLevel[2]+duration_type[0], 
			OLevel[1]+duration_type[1], OLevel[1]+duration_type[0],
			OLevel[0]+duration_type[1], OLevel[0]+duration_type[0]
		    ),
		   scatterpoints=1,
		   loc='upper center',
		   ncol=4,
		   prop={'size':legend_size})





	xticks = [ 2,3.2,3.1,4.2,   6,7.2,7.1,8.2,   10,11.2,11.1,12.2 ]
	xticks_minor = [ 1,  5,  9, 13 ]#longer
	xlbls = [channels[0], '6-Cli.', channels[1], channels[2], 
	channels[0], '4-Cli.', channels[1], channels[2], 
	channels[0], '2-Cli.', channels[1], channels[2] ]

	ax[0].set_xticks( xticks )
	ax[0].set_xticks( xticks_minor, minor=True )
	ax[0].set_xticklabels( xlbls )
	ax[0].set_xlim( 1, 13 )
	ax[0].tick_params(axis='x', labelsize=20)
	ax[0].grid( 'off', axis='x' )
	ax[0].grid( 'off', axis='x', which='minor' )

	# vertical alignment of xtick labels
	#va = [ 0, -.05, 0, -.05, -.05, -.05 ]
	va = [ 0, -.1, 0, 0,  0, -.1, 0, 0,  0,  -.1, 0, 0]
	for t, y in zip( ax[0].get_xticklabels( ), va ):
	    t.set_y( y )

	ax[0].tick_params( axis='x', which='minor', direction='out', length=40 , top='off')
	#ax.tick_params( axis='x', which='major', direction='out', length=10 )
	ax[0].tick_params( axis='x', which='major', bottom='off', top='off' )
	vals = ax[0].get_yticks()
	ax[0].set_yticklabels(['{:3.0f}%'.format(x*100) for x in vals])


	O3comp = srv_app[3]
	O3lwip = srv_lwip[3]
	O2comp = srv_app[2]
	O2lwip = srv_lwip[2]
	O1comp = srv_app[1]
	O1lwip = srv_lwip[1]
	O0comp = srv_app[0]
	O0lwip = srv_lwip[0]




	a1 = ax[1].bar(ind, O3comp, width, color=[0,0.5,1]) 
	a2 = ax[1].bar(ind, O3lwip, width, fill=False, hatch=hatches[0], edgecolor=[0,0.5,1], bottom=O3comp) 

	b1 = ax[1].bar(ind+ width + xtra_space, O2comp, width, color=[0,1,0.5]) 
	b2 = ax[1].bar(ind+ width + xtra_space, O2lwip, width, fill=False, hatch=hatches[0], edgecolor=[0,1,0.5], bottom=O2comp) 

	c1 = ax[1].bar(ind+ 2*(width + xtra_space), O1comp, width, color=[1,0.5,0]) 
	c2 = ax[1].bar(ind+ 2*(width + xtra_space), O1lwip, width, fill=False, hatch=hatches[0], edgecolor=[1,0.5,0], bottom=O1comp) 

	d1 = ax[1].bar(ind+ 3*(width + xtra_space), O0comp, width, color=[0.6,0,0.8]) 
	d2 = ax[1].bar(ind+ 3*(width + xtra_space), O0lwip, width, fill=False, hatch=hatches[0], edgecolor=[0.6,0,0.8], bottom=O0comp) 












	xticks = [ 2,3.2,3.1,4.2,   6,7.2,7.1,8.2,   10,11.2,11.1,12.2 ]
	xticks_minor = [ 1,  5,  9, 13 ]#longer
	xlbls = [channels[0], '6-Cli.', channels[1], channels[2], 
	channels[0], '4-Cli.', channels[1], channels[2], 
	channels[0], '2-Cli.', channels[1], channels[2] ]

	ax[1].set_xticks( xticks )
	ax[1].set_xticks( xticks_minor, minor=True )
	ax[1].set_xticklabels( xlbls )
	ax[1].set_xlim( 1, 13 )

	ax[1].grid( 'off', axis='x' )
	ax[1].grid( 'off', axis='x', which='minor' )

	# vertical alignment of xtick labels
	#va = [ 0, -.05, 0, -.05, -.05, -.05 ]
	va = [ 0, -.1, 0, 0,  0, -.1, 0, 0,  0,  -.1, 0, 0]
	for t, y in zip( ax[1].get_xticklabels( ), va ):
	    t.set_y( y )

	ax[1].tick_params( axis='x', which='minor', direction='out', length=40 , top='off')
	#ax.tick_params( axis='x', which='major', direction='out', length=10 )
	ax[1].tick_params( axis='x', which='major', bottom='off', top='off' )
	vals = ax[1].get_yticks()
	ax[1].set_yticklabels(['{:3.0f}%'.format(x*100) for x in vals])






	# add some text for labels, title and axes ticks
	ax[0].set_ylabel('Core Utilization', fontsize=label_size)
	ax[0].set_xlabel('Client', fontsize=label_size)
	ax[1].set_ylabel('Core Utilization', fontsize=label_size)
	ax[1].set_xlabel('Server', fontsize=label_size)

	#ax[0].set_yticks(fontsize=font_size)
	#ax[1].set_yticks(fontsize=font_size)
	#ax[0].set_xticks(fontsize=font_size)
	#ax[1].set_xticks(fontsize=font_size)
	ax[0].tick_params(axis='y', labelsize=font_size)
	ax[1].tick_params(axis='y', labelsize=font_size)
	ax[0].tick_params(axis='x', labelsize=font_size)
	ax[1].tick_params(axis='x', labelsize=font_size)



	plt.show()
