import ROOT,sys
from recotool.mergeDef import *
from ROOT import larlite as fmwk
import os

mgr = fmwk.ana_processor()

#args should be input file name
for x in xrange(len(sys.argv)-1):
    mgr.add_input_file(sys.argv[x+1])

#last arg should be output file name
#out_file = sys.argv[-1]
#if os.path.isfile(out_file):
#    print
#    print 'ERROR: output file already exist...'
#    print
#    sys.exit(0)

mgr.set_io_mode(fmwk.storage_manager.kBOTH)

mgr.set_output_file("merge_all.root")

merger = GetMergeAllInstance("cccluster")
merger.SetOutputProducer("mergeall")

mgr.add_process(merger)

mgr.run()


