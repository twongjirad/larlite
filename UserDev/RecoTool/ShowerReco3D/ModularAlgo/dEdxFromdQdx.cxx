#ifndef DEDXFROMDQDX_CXX
#define DEDXFROMDQDX_CXX

#include "dEdxFromdQdx.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/LArProperties.h"

namespace showerreco{

  dEdxFromdQdx::dEdxFromdQdx()
  {
    _name = "dEdxFromdQdx";
    _use_pitch = true;
  }

  void dEdxFromdQdx::initialize()
  {

    return;
  }

  void dEdxFromdQdx::do_reconstruction(const ShowerClusterSet_t & inputShowers, Shower_t & resultShower){
    

    auto geomHelper = larutil::GeometryHelper::GetME();

    // get the 3D direction reconstructed hopefully in a previous step
    auto const& dir3D = resultShower.fDCosStart;

    // get the vector of dQdx filled by someone else
    auto const& dQdx = resultShower.fdQdx;

    // loop over all input cluster -> calculate a dQdx per plane
    for (size_t n=0; n < inputShowers.size(); n++){
      
      // get the plane associated with this cluster
      auto const& pl = inputShowers.at(n).plane_id.Plane;

      //*** we need to calculate a pitch for this plane
      double pitch = geomHelper->GetPitch(dir3D,(int)pl);

      double dEdx = 0.;

      double dqdx = dQdx[pl];

      if (_verbose) { std::cout << "dQdx on plane : " << pl << " -> " << dqdx << std::endl; }

      if (_use_pitch)
	dqdx *= (pitch/0.3);

      dEdx = larutil::LArProperties::GetME()->ModBoxCorrection(dqdx);

      if (_verbose) { std::cout << "dEdx on plane : " << pl << " -> " << dEdx << std::endl; }

      // take the dQdx measured on each plane and convert
      // to dEdx using a recombination model

      resultShower.fdEdx[pl] = dEdx;

    }

    return;
  }


} //showerreco

#endif
