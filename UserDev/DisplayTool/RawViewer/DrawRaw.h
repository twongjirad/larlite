/**
 * \file DrawRaw.h
 *
 * \ingroup EventViewer
 * 
 * \brief Class def header for a class DrawRaw
 *
 * @author cadams
 */

/** \addtogroup EventViewer

    @{*/

#ifndef LARLITE_DRAWRAW_H
#define LARLITE_DRAWRAW_H

#include "Analysis/ana_base.h"
#include "LArUtil/Geometry.h"
#include "RawBase.h"

namespace evd {
  /**
     \class DrawRaw
     User custom analysis class made by SHELL_USER_NAME
   */
  class DrawRaw : public larlite::ana_base, public RawBase{
  
  public:

    /// Default constructor
    DrawRaw();

    /// Default destructor
    virtual ~DrawRaw(){}

    /** IMPLEMENT in DrawRaw.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in DrawRaw.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(larlite::storage_manager* storage);

    /** IMPLEMENT in DrawRaw.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();


  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
