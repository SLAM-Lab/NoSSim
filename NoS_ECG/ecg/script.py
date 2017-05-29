#! /usr/bin/env python
import sys
import subprocess

application = './flow'

for i in {38}:
	subprocess.call(['make','clean'])
	subprocess.call(['make'])
	for j in {101}:
		subprocess.call([application, 'configs/config_%d.txt' % i, 'conf%d_rec%d_filt_detect.txt' % (i,j), 'conf%d_rec%d_dwt_svm.txt' % (i,j), 'data_%d.txt' % j, 'model_config_%d.txt' % i]) 

