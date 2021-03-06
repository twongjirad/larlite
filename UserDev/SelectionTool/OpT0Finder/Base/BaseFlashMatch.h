/**
 * \file BaseFlashMatch.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class BaseFlashMatch
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef OPT0FINDER_BASEFLASHMATCH_H
#define OPT0FINDER_BASEFLASHMATCH_H

#include "BaseAlgorithm.h"

namespace flashana {
  /**
     \class BaseFlashMatch
     Algorithm base class for matching flashana::QCluster_t (TPC object) and \n
     flashana::Flash_t (flash). It creates flashana::FlashMatch_t which contains \n
     matching infomration.
  */
  class BaseFlashMatch : public BaseAlgorithm{
    
  public:
    
    /// Default constructor
    BaseFlashMatch() : BaseAlgorithm(kFlashMatch)
    {}
    
    /// Default destructor
    virtual ~BaseFlashMatch(){}

    /**
       CORE FUNCTION: takes in flashana::QCluster_t (TPC object) and flashana::Flash_t (flash) \n
       and inspect if two are consistent (i.t. matched) or not. Returns flashana::FlashMatch_t \n
       which represents the compatibility of two inputs. In particular the algorithm needs to  \n
       set the "score" and "QPoint_t" of the return object. The former represents the goodness \n
       of a match with a value larger than 0. Negative value is interpreted as no match.       \n
       The latter represents the matched TPC 3D point. The "tpc_id and "flash_id" of the return \n
       object is set by flashana::FlashMatchManager, the caller of the algorithm, as it manages \n
       the overall collection of user input flash and TPC objects. \n
       \n
       Note it is flashana::FlashMatchManager configuration option to allow an assignment of the \n
       same flash to multiple TPC object or not. If not allowed, a match with a higher "score"  \n
       in the return object is chosen.
     */
    virtual FlashMatch_t Match(const QCluster_t&, const Flash_t&) = 0;

  };
}

#endif
/** @} */ // end of doxygen group 

