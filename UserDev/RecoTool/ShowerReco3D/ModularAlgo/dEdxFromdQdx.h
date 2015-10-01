/**
 * \file dEdxFromdQdx.h
 *
 * \ingroup ModularAlgo
 * 
 * \brief Class def header for a class dQdx2DModule
 *
 * @authors David Caratelli
 */

/** \addtogroup ModularAlgo

    @{*/
#ifndef DEDXFROMDQDX_H
#define DEDXFROMDQDX_H

#include <iostream>
#include "ShowerRecoModuleBase.h"
/**
   \class dEdxFromdQdx : ShowerRecoModuleBase
   This is meant to compute the 2D dQdx along the start of the shower. 
 */
namespace showerreco {

  class dEdxFromdQdx : ShowerRecoModuleBase{

  public:

    /// Default constructor
    dEdxFromdQdx();

    /// Default destructor
    ~dEdxFromdQdx(){}

    void do_reconstruction(const ShowerClusterSet_t &, Shower_t &);
    
    void initialize();

    void SetUsePitch(bool on) { _use_pitch = on; }

  private:

    bool _use_pitch;

  };

} // showerreco

#endif
/** @} */ // end of doxygen group 

