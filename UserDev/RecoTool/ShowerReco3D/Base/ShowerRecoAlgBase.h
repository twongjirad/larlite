/**
 * \file ShowerRecoAlgBase.h
 *
 * \ingroup ShowerReco3D
 * 
 * \brief Class def header for a class ShowerRecoAlgBase
 *
 * @author kazuhiro
 */

/** \addtogroup ShowerReco3D

    @{*/
#ifndef RECOTOOL_SHOWERRECOALGBASE_H
#define RECOTOOL_SHOWERRECOALGBASE_H

#include <iostream>
#include "DataFormat/shower.h"
#include "AnalysisAlg/CalorimetryAlg.h"
#include "ClusterRecoUtil/ClusterParamsAlg.h"
#include "ShowerRecoException.h"
#include <limits>
#include <climits>
#include "TTree.h"

namespace showerreco {

  struct ShowerCluster_t {
    ::larutil::PxPoint start_point;
    ::larutil::PxPoint end_point;
    double             angle_2d;
    double             opening_angle;
    unsigned short     plane_id;
    std::vector< ::larutil::PxHit> hit_vector;
    ShowerCluster_t() : hit_vector()
    {}
  };

  typedef std::vector<showerreco::ShowerCluster_t> ShowerClusterSet_t;
  
  /**
     \class ShowerRecoAlgBase
     User defined class ShowerRecoAlgBase ... these comments are used to generate
     doxygen documentation!
  */
  class ShowerRecoAlgBase{
    
  public:
    
    /// Default constructor
    ShowerRecoAlgBase();
    
    /// Default destructor
    virtual ~ShowerRecoAlgBase(){}

    /// Function to reset algorithm, to be called @ beginning of each event
    virtual void Reset() = 0;

    /// Setter for a matched combination of clusters
    virtual void AppendInputClusters(const std::vector<cluster::ClusterParamsAlg>& cpan_v);

    /// Execute reconstruction
    std::vector<larlite::shower> Reconstruct();

    /// Verbosity switch
    virtual void Verbose(bool on=true) { fVerbosity=on; }

    /// Calorimetry algorithm setter
    void CaloAlgo(::calo::CalorimetryAlg* alg) { fCaloAlg = alg; }

    /// Function to get algorithm's TTree
    TTree* getTree() { return _alg_tree; }

  protected:

    /// Function to reorganize input cluster information
    virtual void ProcessInputClusters()
    { return; }

    /// Function to reconstruct one shower
    virtual ::larlite::shower RecoOneShower(const ShowerClusterSet_t& clusters) = 0;

    /// Verbosity flag
    bool fVerbosity;

    /// Calorimetry algorithm
    ::calo::CalorimetryAlg *fCaloAlg;

    /// Input clusters
    std::vector<showerreco::ShowerClusterSet_t> fInputClusters;

    /// TTree for algorithm's stuff
    TTree *_alg_tree;

  };
}

#endif
/** @} */ // end of doxygen group 

