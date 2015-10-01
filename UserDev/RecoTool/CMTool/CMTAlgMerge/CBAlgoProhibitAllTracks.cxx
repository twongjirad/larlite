#ifndef RECOTOOL_CBALGOPROHIBITALLTRACKS_CXX
#define RECOTOOL_CBALGOPROHIBITALLTRACKS_CXX

#include "CBAlgoProhibitAllTracks.h"

namespace cmtool {

  //-------------------------------------------------------
  CBAlgoProhibitAllTracks::CBAlgoProhibitAllTracks() : CBoolAlgoBase()
  //-------------------------------------------------------
  {
    SetMinEP(.990000);
  }

  //-----------------------------
  void CBAlgoProhibitAllTracks::Reset()
  //-----------------------------
  {

  }

  //------------------------------------------------------------------------------------------
  //void CBAlgoProhibitAllTracks::EventBegin(const std::vector<cluster::ClusterParamsAlg> &clusters)
  //------------------------------------------------------------------------------------------
  //{
  //
  //}

  //-------------------------------
  //void CBAlgoProhibitAllTracks::EventEnd()
  //-------------------------------
  //{
  //
  //}

  //-----------------------------------------------------------------------------------------------
  //void CBAlgoProhibitAllTracks::IterationBegin(const std::vector<cluster::ClusterParamsAlg> &clusters)
  //-----------------------------------------------------------------------------------------------
  //{
  //
  //}

  //------------------------------------
  //void CBAlgoProhibitAllTracks::IterationEnd()
  //------------------------------------
  //{
  //
  //}
  
  //----------------------------------------------------------------
  bool CBAlgoProhibitAllTracks::Bool(const ::cluster::cluster_params &cluster1,
                                     const ::cluster::cluster_params &cluster2)
  //----------------------------------------------------------------
  {
    //return true means don't prohibit these two clusters
    if(cluster1.eigenvalue_principal > _min_EP ||
       cluster2.eigenvalue_principal > _min_EP) 
      {
        if(_verbose) 
          std::cout<<"Prohibiting clusters with EP's of "
                   <<cluster1.eigenvalue_principal
                   <<" and "
                   <<cluster2.eigenvalue_principal
                   <<std::endl;
        return true;
      }
    return false;
  }

  //------------------------------
  void CBAlgoProhibitAllTracks::Report()
  //------------------------------
  {

  }
    
}
#endif
