#This is a list of plots that the user can choose from (in gen_ShowerQuality_plots)
#Each plot has a function that takes as input a dataframe, and returns a matplotlib figure.

try:
	import numpy as np
	import pandas as pd
	import matplotlib.pyplot as plt
	from sq_fitutils import *
except ImportError:
	raise ImportError('Something didn\'t import correctly! Do you have numpy, pandas, matplotlib, and root_numpy? Get it!')


def energy_res_plot(df):
	#Plot of energy asymmetry
	fig = plt.figure(figsize=(10,6))
	plt.title("(MC Energy - Reco Energy)/MC Energy")
	bins = np.linspace(-1,1,100)
	curvebins = np.linspace(-1,1,300)
	eresU = (df['mc_energy']-df['reco_energy_U'])/df['mc_energy']
	eresV = (df['mc_energy']-df['reco_energy_V'])/df['mc_energy']
	eresY = (df['mc_energy']-df['reco_energy_Y'])/df['mc_energy']
	entriesU, bin_edgesU, patchesU = plt.hist(eresU,bins=bins,label='U',alpha=0.5,color='b',edgecolor=None)
	params, errors = getGaussFit(entriesU,bin_edgesU)
	plt.plot(curvebins,gauss(curvebins,*params),'b-',lw=3,label='U plane: mu = %.02f sigma = %.02f'%(params[1],params[2]))
	entriesV, bin_edgesV, patchesV = plt.hist(eresV,bins=bins,label='V',alpha=0.5,color='g',edgecolor=None)
	params, errors = getGaussFit(entriesV,bin_edgesV)
	plt.plot(curvebins,gauss(curvebins,*params),'g-',lw=3,label='V plane: mu = %.02f sigma = %.02f'%(params[1],params[2]))
	entriesY, bin_edgesY, patchesY = plt.hist(eresY,bins=bins,label='Y',alpha=0.5,color='r',edgecolor=None)
	params, errors = getGaussFit(entriesY,bin_edgesY)
	plt.plot(curvebins,gauss(curvebins,*params),'r-',lw=3,label='Y plane: mu = %.02f sigma = %.02f'%(params[1],params[2]))
	plt.grid(True)
	plt.legend(loc=2)
	return fig

def energy_asym_plot(df):
	#Plot of energy asymmetry
	fig = plt.figure(figsize=(10,6))
	plt.title("2*(MC Energy - Reco Energy)/(MC Energy + Reco Energy)")
	bins = np.linspace(-1,1,100)
	easymU = 2*(df['mc_energy']-df['reco_energy_U'])/(df['mc_energy']+df['reco_energy_U'])
	easymV = 2*(df['mc_energy']-df['reco_energy_V'])/(df['mc_energy']+df['reco_energy_V'])
	easymY = 2*(df['mc_energy']-df['reco_energy_Y'])/(df['mc_energy']+df['reco_energy_Y'])
	entriesU, bin_edgesU, patchesU = plt.hist(easymU,bins=bins,label='U',alpha=0.5,color='b',edgecolor=None)
	entriesV, bin_edgesV, patchesV = plt.hist(easymV,bins=bins,label='V',alpha=0.5,color='g',edgecolor=None)
	entriesY, bin_edgesY, patchesY = plt.hist(easymY,bins=bins,label='Y',alpha=0.5,color='r',edgecolor=None)
	plt.grid(True)
	plt.legend(loc=2)
	return fig

def energy_diff_plot(df):
	#Plot of energy asymmetry
	fig = plt.figure(figsize=(10,6))
	plt.title("Reco Energy - MC Energy")
	bins = np.linspace(-1000,1000,100)
	ediffU = df['reco_energy_U']-df['mc_energy']
	ediffV = df['reco_energy_V']-df['mc_energy']
	ediffY = df['reco_energy_Y']-df['mc_energy']
	entriesU, bin_edgesU, patchesU = plt.hist(ediffU,bins=bins,label='U',alpha=0.5,color='b',edgecolor=None)
	entriesV, bin_edgesV, patchesV = plt.hist(ediffV,bins=bins,label='V',alpha=0.5,color='g',edgecolor=None)
	entriesY, bin_edgesY, patchesY = plt.hist(ediffY,bins=bins,label='Y',alpha=0.5,color='r',edgecolor=None)
	plt.grid(True)
	plt.legend(loc=2)
	return fig

def energy_corr_plot(df,plane='Y'):
	#Plot of reco energy vs mc energy (2d histo)
	if plane not in ['U','V','Y']:
		raise ValueError('energy_corr_plot() function requires plane that is either \'U\', \'V\', or \'Y\'')

	# fig = plt.figure(figsize=(10,6))
	fig = df.plot(x='mc_energy',y='reco_energy_%s'%plane,
		kind='hexbin',colormap='jet',gridsize=100,
		title='MC Energy vs. Reco Energy',
		figsize=(10,6))
	plt.xlabel('MCShower Deposited Energy')
	plt.ylabel('Reconstructed Energy')
	return fig

def startpoint_acc_plot(df):
	fig = plt.figure(figsize=(10,6))
	xmin, xmax, nbins = 0., 5., 100
	plt.title('Distance Between (3D) MC Start Point and Reco Start Point')
	plt.xlabel('Distance in 3D [cm]')
	plt.ylabel('Counts')
	plt.yscale('log')
	n_total = len(df['mc_reco_dist'])
	n_underflow = len(df.query('mc_reco_dist < %f'%xmin))
	n_overflow  = len(df.query('mc_reco_dist > %f'%xmax))
	df['mc_reco_dist'].hist(bins=np.linspace(xmin, xmax, nbins),label='Total entries: %d (%d underflow, %d overflow)'%(n_total,n_underflow,n_overflow))
	plt.legend(loc=1)
	return fig

def anglediff_3d_plot(df):
	fig = plt.figure(figsize=(10,6))
	xmin, xmax, nbins =0., 180., 100
	angular_res = df['mc_reco_anglediff']
	bins = np.linspace(xmin, xmax, nbins)
	plt.grid(True)
	plt.title('3D Direction MC - Reco Angular Difference')
	plt.xlabel('3D Angle Difference [degrees]')
	plt.ylabel('Counts')
	n_total = len(df['mc_reco_anglediff'])
	n_underflow = len(df.query('mc_reco_anglediff < %f'%xmin))
	n_overflow  = len(df.query('mc_reco_anglediff > %f'%xmax))
	plt.hist(angular_res,bins,edgecolor=None,label='Total entries: %d (%d underflow, %d overflow)'%(n_total,n_underflow,n_overflow))
	plt.legend(loc=1)
	plt.yscale('log')
	return fig

def dEdx_plot(df):
	fig = plt.figure(figsize=(10,6))
	dedx_u = df['reco_dedx_U']
	dedx_v = df['reco_dedx_V']
	dedx_y = df['reco_dedx_Y']
	bins = np.linspace(0,10,100)
	plt.hist(dedx_u,bins=bins,edgecolor=None,alpha=0.5,color='b',label='U Plane')
	plt.hist(dedx_v,bins=bins,edgecolor=None,alpha=0.5,color='g',label='V Plane')
	plt.hist(dedx_y,bins=bins,edgecolor=None,alpha=0.5,color='r',label='Y Plane')
	plt.grid(True)
	plt.legend(loc=1)
	plt.title('dE/dx [ MeV/cm ] in each Plane')
	plt.xlabel('dE/dx [ MeV / cm ]')
	plt.ylabel('Count')
	return fig

def reco_efficiency_vsenergy_plot(df):
	fig = plt.figure(figsize=(10,6))
	plt.grid(True)
	binmin, binmax, binwidth = 0, 1000, 100
	bins = np.arange(binmin, binmax, binwidth)
	effs_x = np.arange(binmin + (binwidth/2), binmax-binwidth, binwidth)
	effs_y = np.zeros(len(effs_x))
	errs_y = np.zeros(len(effs_x))
	for i in xrange(len(bins)-1):
		this_df = df.query('mcs_E > %d and mcs_E < %d' % (bins[i], bins[i+1]))
		effs_y[i] = float(np.sum(this_df['n_recoshowers']))/np.sum(this_df['n_mcshowers'])
		errs_y[i] = np.sqrt(effs_y[i]*(1-effs_y[i])/np.sum(this_df['n_mcshowers']))
	plt.errorbar(effs_x,effs_y, yerr=errs_y, color='blue', linestyle = ':', marker='*', mfc='blue', mec='blue', label='Shower Reco Efficiency')
	plt.legend(loc=4)
	plt.title('Shower Reconstruction Efficiency vs True Shower Deposited Energy')
	plt.xlabel('True Shower Deposited Energy [MeV]')
	plt.ylabel('N Reco Showers / N True Showers per bin')
	return fig

def cluster_efficiency_perplane_plot(df):
	fig = plt.figure(figsize=(10,6))
	cluseff_u = df['cluster_eff_U']
	cluseff_v = df['cluster_eff_V']
	cluseff_y = df['cluster_eff_Y']
	bins = np.linspace(0,1,100)
	plt.hist(cluseff_u,bins=bins,edgecolor=None,alpha=0.5,color='b',label='U Plane')
	plt.hist(cluseff_v,bins=bins,edgecolor=None,alpha=0.5,color='g',label='V Plane')
	plt.hist(cluseff_y,bins=bins,edgecolor=None,alpha=0.5,color='r',label='Y Plane')
	plt.grid(True)
	plt.legend(loc=1)
	plt.title('Shower\'s Associated Cluster\'s Efficiency in each Plane')
	plt.xlabel('Cluster Summed Hit Charge / Total Summed Hit Charge, per Plane')
	plt.ylabel('Counts')
	return fig


