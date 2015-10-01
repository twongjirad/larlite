
export USER_MODULE="BasicTool RecoTool"
# Find the location of this script:
me="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Find the directory one above.
export LARLITE_BASEDIR="$( cd "$( dirname "$me" )" && pwd )"

source $LARLITE_BASEDIR/config/setup.sh

# Set up the event display if possible:
source $LARLITE_USERDEVDIR/DisplayTool/setup_evd.sh
if [ $BUILD_LARLITE_EVD ]; then
  USER_MODULE="${USER_MODULE} DisplayTool "
fi


