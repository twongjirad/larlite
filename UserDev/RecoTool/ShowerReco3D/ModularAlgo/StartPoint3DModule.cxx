#ifndef STARTPOINT3DMODULE_CXX
#define STARTPOINT3DMODULE_CXX

#include "StartPoint3DModule.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/GeometryUtilities.h"
#include "LArUtil/DetectorProperties.h"

namespace showerreco {

  void StartPoint3DModule::do_reconstruction(const ShowerClusterSet_t & inputShowers, Shower_t & resultShower) {
    
    /**
    *  Use 2 best planes to calculate Y, Z of start point.  
    *  Use their time average to calculate X. 
    *  First calculate the worst plane and don't use that guy.
    */
    float minClusDist      =  999999 ;
    int worstPlane  = -1      ; 
    int planeTemp   =  0      ;
    
    if( inputShowers.size() > 2 ){
        for( auto const c : inputShowers ){
	   float distTemp = abs ( c.start_point.w - c.end_point.w );
    
    	   if( distTemp < minClusDist ){
    	       minClusDist = distTemp ;
    	       worstPlane = planeTemp;
    	       }
    
    	       planeTemp ++ ;
    	   }
	}

    /**
    *  Store planes and wire start, calculate average time for best 2 clusters.
    */
    double sX = 0 ;
    double sY ; 
    double sZ ; 
    
    std::vector<int> wireStarts(0) ;
    std::vector<int> planes(0) ;
    
    for( auto const c : inputShowers ){
        
        if(c.plane_id.Plane != worstPlane){
	   wireStarts.emplace_back( int(c.start_point.w/0.3) ) ;
    	   planes.emplace_back( c.plane_id.Plane ) ;
    	   sX += c.start_point.t;	
    	   }
	}

    /**
    *  Caluclate intersection point in Y,Z of the 2 best planes.
    *  Calculate their average start time
    */
    auto geom  = larutil::Geometry::GetME();
    geom->IntersectionPoint(wireStarts[0],wireStarts[1],planes[0],planes[1],sY,sZ ); 
    
    resultShower.fXYZStart = {sX/2,sY,sZ} ; 

    }

} //showerreco

#endif
