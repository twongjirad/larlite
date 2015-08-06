import sys, os
from ROOT import gSystem
from ROOT import ertool
from ROOT import larlite as fmwk
#from seltool.ccsingleeDef import GetCCSingleEInstance
from seltool.erviewer import ERViewer
from seltool.algoviewer import viewAll, view
import basictool.geoviewer as gv

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

# Create ana_processor instance
my_proc = fmwk.ana_processor()
my_proc.enable_filter(False)


# Get Default CCSingleE Algorithm instance
# this sets default parameters
# this information is loaded from:
# $LARLITE_BASEDIR/python/seltool/GetCCSingleEInstance
# and the algorithm instance is the return of the
# function GetCCSingleEInstance()
#my_algo = GetCCSingleEInstance()
#~my_algo.setVerbose(True)


# Create algorithm
my_algo = ertool.AlgoPi0()
my_algo.setVerbose(False)
#my_algo.setMinShrEnergy(10)
my_algo.setMinShrEnergy(0)
my_algo.setMaxShrEnergy(1000)
my_algo.setIPMax(10)
my_algo.setMinFitMass(50)
my_algo.setMaxFitMass(200)
my_algo.setAngleMax(3.14)

my_algo2 = ertool.AlgoPrimaryFinder()
my_algo2.setVtxToTrkStartDist(1)
my_algo2.setVtxToTrkDist(1)
my_algo2.setVtxToShrStartDist(1)
my_algo2.setMaxIP(5)

my_algo3 = ertool.ERAlgoPrimaryPi0()
my_algo3.SetMinDistVtx(7)
my_algo3.SetMinDistEnd(7)
my_algo3.SetVerbose(True)

# First lets make a filter that looks for a certain events
pi0_topo = fmwk.effpi0()
# 0 == inclusive 1 == 1pi0&&nopi+/-
pi0_topo.SetTopology(1)
# 0 == ntsignal 1 == signal
pi0_topo.SignalTopology(1)
# 0 == CC 1 == NC 
pi0_topo.SetCCNC(1)
pi0_topo.SetFVCut(0)
pi0_topo.SetEnergyCut(20);
pi0_topo.SetContainment(0.0);


# Create ERTool filter
# This filter removes any track that
# is less than 3 mm in length
# these tracks exist in "perfect reco"
# but at this stage it is unreasonable
# to assume we will be able to
# reconstruct them

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("pi0_selection.root")

# here set E-cut for Helper & Ana modules
#This cut is applied in helper... ertool showers are not made if the energy of mcshower or reco shower
#is below this threshold. This has to be above 0 or else the code may segfault. This is not a "physics cut".
#Do not change this value unless you know what you are doing.
Ecut = 20 # in MeV

my_ana = ertool.ERAnaPi0All() #ERAnaSingleE()
my_ana.SetVerbose(True)
#my_ana.SetECut(Ecut)

my_anaunit = fmwk.ExampleERSelection()
my_anaunit._mgr.AddAlgo(my_algo)
my_anaunit._mgr.AddAlgo(my_algo2)
my_anaunit._mgr.AddAlgo(my_algo3)
my_anaunit._mgr.AddAna(my_ana)
#my_anaunit._mgr.AddCfgFile('new_empart.txt')
my_anaunit.SetMinEDep(Ecut)
my_anaunit._mgr._mc_for_ana = True

# ***************  Set Producers  ****************
# First Argument: True = MC, False = Reco
my_anaunit.SetShowerProducer(True,"mcreco");
my_anaunit.SetTrackProducer(True,"mcreco");
#my_anaunit.SetVtxProducer(True,"generator");
#my_anaunit.SetShowerProducer(False,"mergeall");
#my_anaunit.SetShowerProducer(False,"newdefaultreco");
#my_anaunit.SetShowerProducer(True,"mcreco");
#my_anaunit.SetShowerProducer(False,"showerreco");
#my_anaunit.SetShowerProducer(False,"showerrecofuzzy");
#my_anaunit.SetShowerProducer(False,"pandoraNuShower");
#my_anaunit.SetTrackProducer(False,"stitchkalmanhit");
# ************************************************


my_proc.add_process(pi0_topo)
my_proc.add_process(my_anaunit)

mcviewer   = ERViewer('MC Info')
recoviewer = ERViewer('RECO Info')

# Start event-by-event loop
counter = 0
while (counter < 11700):
    try:
        counter = input('Hit Enter to continue to next evt, or type in an event number to jump to that event:')
    except SyntaxError:
        counter = counter + 1
    print "Event number: ", counter

    display_mc = gv.GeoViewer()
    display_reco = gv.GeoViewer()

    my_proc.process_event(counter)
    print "Processing event {0}".format(counter) 
    # get objets and display
    data_reco = my_anaunit.GetData()
    part_reco = my_anaunit.GetParticles()
    data_mc   = my_anaunit.GetData(True)
    part_mc   = my_anaunit.GetParticles(True)
    view(display_mc,data_mc,part_mc)
#    view(display_reco,data_reco,part_reco)

#    viewAll(mcviewer  , data_mc  , part_mc,
#	    recoviewer, data_reco, part_reco)

  #  dist = shower.Start().Dist(track.back())

    #for x in xrange(part_mc.size()):
    #    print part_mc[x].Diagram()



# done!
print
print "Finished running ana_processor event loop!"
print

#my_algo.StoreParams()
sys.exit(0)

