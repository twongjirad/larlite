/**
 * \file CBAlgoHighQLineCompat.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CBAlgoHighQLineCompat
 *
 * @author davidkaleko
 */

/** \addtogroup ClusterRecoUtil

    @{*/
#ifndef RECOTOOL_CBALGOHIGHQLINECOMPAT_H
#define RECOTOOL_CBALGOHIGHQLINECOMPAT_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "TGraphErrors.h"
#include "TFitResult.h"
#include <algorithm> //std::sort
#include "TH1F.h"
 
namespace cmtool {
  /**
     \class CBAlgoHighQLineCompat
     User defined class CBAlgoAngleCompat ... these comments are used to generate
     doxygen documentation!
     This function takes the highest charge hits in each cluster and fits them to
     a line in wire-time space. It compares the slopes (angles) of these lines to determine
     if clusters should be merged. Each point has errors related to the charge of the
     hit, to aid in fitting.
     Note: if this becomes useful, we should probably compute the slope (as done
     in this class) for each cluster and save it in cluster params. Currently,
     this re-fits a line to the same cluster each time it is compared to another,
     which is overkill.
  */
  class CBAlgoHighQLineCompat : public CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoHighQLineCompat();
    
    /// Default destructor
    virtual ~CBAlgoHighQLineCompat(){};
        
    /// Overloaded (from CBoolAlgoBase) Bool function
    virtual bool Bool(const ::cluster::cluster_params &cluster1,
                      const ::cluster::cluster_params &cluster2);


    /// Method to set debug mode
    void SetDebug(bool on) { _debug = on; }

    /// Method to set cut value in degrees for angle compatibility test
    void SetAngleCutDegrees(double kaleko) { _max_angle_sep = kaleko; }

    void SetMinNHits(size_t n) { _min_n_hits = n; }

    void SetQFracCut(double kaleko) { _frac_hits_to_keep = kaleko; }

    TH1F* GetDebugHisto() const{ return _debug_histo; };

  protected:

    /// Function to take a list of pxhits, fit to a line, and return the slope of the line
    double ComputeSlope(const std::vector<larutil::PxHit> &pxhit_vector);

    /// Function to take in a list of pxhits and return a list of 
    /// highest "frac" fraction of those hits (by charge)
    /// This is super inefficient. I copy the hit vector, sort it, then make ANOTHER hit vector w/ reduced size
    /// Can definitely be rewritten faster. I don't care enough right now.
    const std::vector<larutil::PxHit> GetHighQHits(const std::vector<larutil::PxHit> &pxhit_vector, double frac);

    /// Function needed to sort pxhit vector by charge (static b/c that's the only way it'll compile...)
    static bool SortPxHitsMethod(const larutil::PxHit &h1, const larutil::PxHit &h2);

    /// Obligatory debug flag
    bool _debug;
    /// Minimum number of hits for cluster to be considered
    size_t _min_n_hits;    
    /// Fraction of hits to consider (highest charge hits)
    double _frac_hits_to_keep; 
    /// Fit line angle difference between two clusters (in degrees)
    double _max_angle_sep;

    TGraphErrors* _tge;

    TFitResultPtr _frp;


    TH1F* _debug_histo;
  };
  
} // end namespace cmtool

#endif
  /** @} */ // end of doxygen group 
  
