import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)


from ROOT import gSystem,TMath
from larlite import larlite as fmwk
from larlite import larutil
from recotool import cmtool, showerreco
from recotool.showerDef import DefaultShowerReco3D, PandoraShowerReco3D

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

# Specify analysis output root file name
my_proc.set_ana_output_file("");

# Specify data output root file name
my_proc.set_output_file("showerreco_batch_out.root")

ana_unit=DefaultShowerReco3D()

shrname = 'showerreco'

ana_unit.SetInputProducer("mergedfuzzy2")
ana_unit.SetOutputProducer(shrname)

my_proc.set_data_to_write(fmwk.data.kShower,shrname)
my_proc.set_data_to_write(fmwk.data.kAssociation,shrname)
my_proc.add_process(ana_unit)

print
print  "Finished configuring ana_processor. Start event loop!"
print

#my_proc.run(0,5)
my_proc.run()

sys.exit()

