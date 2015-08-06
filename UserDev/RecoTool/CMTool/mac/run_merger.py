import ROOT,sys,os
from recotool.mergeDef import *
from ROOT import larlite as fmwk

mgr = fmwk.ana_processor()

#args should be input file name
for x in xrange(len(sys.argv)-2):

    mgr.add_input_file(sys.argv[x+1])

#last arg should be output file name
out_file = sys.argv[-1]
if os.path.isfile(out_file):
    print
    print 'ERROR: output file already exist...'
    print
    sys.exit(0)

mgr.set_output_file(out_file)

mgr.set_io_mode(fmwk.storage_manager.kBOTH)

mgr.set_ana_output_file("")

prelim = GetPrelimMergerInstance()
prelim.SetInputProducer("fuzzycluster")
prelim.SetOutputProducer("mergedfuzzy1")
second = GetSecondMergerInstance()
second.SetInputProducer("mergedfuzzy1")
second.SetOutputProducer("mergedfuzzy2")
mgr.add_process(prelim)
mgr.add_process(second)

#merge_all = GetMergeAllInstance()
#merge_all.SetOutputProducer("mergedfuzzy")
mgr.set_data_to_write(fmwk.data.kCluster,"mergedfuzzy1")
mgr.set_data_to_write(fmwk.data.kAssociation,"mergedfuzzy1")
mgr.set_data_to_write(fmwk.data.kCluster,"mergedfuzzy2")
mgr.set_data_to_write(fmwk.data.kAssociation,"mergedfuzzy2")
#mgr.add_process(merge_all)

mgr.run()


