import sys, os
from ROOT import gSystem
from ROOT import ertool
from ROOT import larlite as fmwk
from seltool.ccsingleeDef import GetCCSingleEInstance
from seltool.algoviewer import viewAll, getViewer

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
# $LARLITE_BASEDIR/python/seltool/ccsingleeDef
# and the algorithm instance is the return of the
# function GetCCSingleEInstance()
my_algo = GetCCSingleEInstance()
my_algo.setVerbose(True)

# Create MC Filter
MCfilter = fmwk.MC_CC1E_Filter();
MCfilter.flip(False)

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("singleE_selection.root")

# here set E-cut for Helper & Ana modules
#This cut is applied in helper... ertool showers are not made if the energy of mcshower or reco shower
#is below this threshold. This has to be above 0 or else the code may segfault. This is not a "physics cut".
#Do not change this value unless you know what you are doing.
Ecut = 20 # in MeV

my_ana = ertool.ERAnaSingleE()
my_ana.SetDebug(True)
my_ana.SetECut(Ecut)

my_anaunit = fmwk.ExampleERSelection()
my_anaunit._mgr.AddAlgo(my_algo)
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
#my_anaunit.SetShowerProducer(False,"pandoraNuShower");
#my_anaunit.SetTrackProducer(False,"stitchkalmanhit");
# ************************************************

my_proc.add_process(MCfilter)
my_proc.add_process(my_anaunit)


#create instance of mc and reco viewer
mcviewer   = getViewer('mc info')
recoviewer = getViewer('reco info')

# Start event-by-event loop
counter = 0
while (counter < 11700):
    try:
        counter = input('Hit Enter to continue to next evt, or type in an event number to jump to that event:')
    except SyntaxError:
        counter = counter + 1
    print "Event number: ", counter
    my_proc.process_event(counter)
    print "Processing event {0}".format(counter) 
    # get objets and display
    data_reco = my_anaunit.GetData()
    part_reco = my_anaunit.GetParticles()
    data_mc   = my_anaunit.GetData(True)
    part_mc   = my_anaunit.GetParticles(True)
    #viewAll(mcviewer,data_mc,part_mc,
    #        recoviewer,data_reco,part_reco)

    #for x in xrange(part_mc.size()):
    #    print part_mc[x].Diagram()



# done!
print
print "Finished running ana_processor event loop!"
print

#my_algo.StoreParams()
sys.exit(0)

