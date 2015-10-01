#ifndef DQDXMODULE_CXX
#define DQDXMODULE_CXX

#include "dQdxModule.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "ClusterRecoUtil/Base/ClusterParams.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
#include "math.h"
#include <algorithm>
#include <functional>

bool larger( std::pair<int,double> a,std::pair<int,double> b){
  return a.second<b.second;
}

namespace showerreco{

  dQdxModule::dQdxModule()
  {
    _name = "dQdxModule";
  }

  void dQdxModule::initialize()
  {

    if (_tree) delete _tree;
    _tree = new TTree(_name.c_str(),"dQdx Info Tree");
    _tree->Branch("_n_hits",&_n_hits,"n_hits/I");
    _tree->Branch("_n_hits_radius",&_n_hits_radius,"n_hits_radius/I");
    _tree->Branch("_n_hits_poly",&_n_hits_poly,"n_hits_poly/I");
    _tree->Branch("_n_hits_sigma",&_n_hits_sigma,"n_hits_sigma/I");
    _tree->Branch("_shrs_w",&_shrs_w,"shrs_w/D");
    _tree->Branch("_shrs_t",&_shrs_t,"shrs_t/D");
    _tree->Branch("_length",&_length,"length/D");
    _tree->Branch("_pl",&_pl,"pl/I");
    _tree->Branch("_pitch",&_pitch,"pitch/D");
    _tree->Branch("_dQ",&_dQ,"dQ/D");
    _tree->Branch("_dQQ",&_dQQ,"dQQ/D");
    _tree->Branch("_dQdx",&_dQdx,"dQdx/D");
    _tree->Branch("_dQdx_pitch",&_dQdx_pitch,"dQdx_p/D");
    _tree->Branch("_dQdx_pitch_pl2",&_dQdx_pitch_pl2,"dQdx_p_pl2/D");
    _tree->Branch("_rms",&_rms,"rms/D");
    //_tree->Branch("_sigma",&_sigma,"sigma/D");
    _tree->Branch("_pl_best",&_pl_best,"pl_best/I");
    _tree->Branch("_median",&_median,"median/D");
    
    _fC_to_e = 6250.; // a fC in units of the electron charge
    _ADC_to_mV = 0.5; // ADC -> mV conversion from gain measurements
    // to go from mV to fC the ASIC gain and Shaping time
    // must be considered
    // fC -> mV *= ( shaping time * ASIC gain )
    _shp_time  = 2.; // in usec
    _asic_gain = 7.8; // in mV/fC
    _charge_conversion = _ADC_to_mV * _fC_to_e / ( _shp_time * _asic_gain ) ;

    return;
  }

  void dQdxModule::do_reconstruction(const ShowerClusterSet_t & inputShowers, Shower_t & resultShower){
    

    auto geom = larutil::Geometry::GetME();
    auto geomHelper = larutil::GeometryHelper::GetME();
    
    // get the 3D direction reconstructed hopefully in a previous step
    auto const& dir3D = resultShower.fDCosStart;
    double dQ[3]={};
    double dQQ[3]={};//dQ w/ sigma selection
    double dx[3]={};
    double dx_p[3]={};
    double trunk_length[3]={};
    double dQdx[3]={};
    double dQdx_pitch[3] ={};
    int n_hits[3]={};
    int n_hits_radius[3] = {};
    int n_hits_poly[3]={};
    int n_hits_sigma[3]={};
    double rms[3];//charge rms
    double mean[3];
    double sigma[3];
    std::vector<double> hits_poly[3];
    double median[3]={};
    
    //// loop over all input cluster -> calculate a dQdx per plane
    for (size_t n=0; n < inputShowers.size(); n++){
      
      // get the hits associated with this cluster
      auto const& hits = inputShowers.at(n).hit_vector;
      
      // get the plane associated with this cluster
      auto const& pl = inputShowers.at(n).plane_id.Plane;
      
      // get the start point for this cluster
      auto const& start = inputShowers.at(n).start_point;
      
      // get the showering point for this cluster
      auto const& shr_start = inputShowers.at(n).showering_point;
      
      // cluster open angle
      auto const& clu_an = inputShowers.at(n).opening_angle;
      
      // start direction
      auto const& start_dir = inputShowers.at(n).start_dir;
      
      //*** we need to calculate a pitch for this plane
      double pitch = geomHelper->GetPitch(dir3D,(int)pl);

      // BEGIN ALGORITHM DEVELOPMENT
      
      // STEP 1 : find list of hits that are along the trunk of
      // the shower. Find these hits by looking around the segment
      // connecting the start point and the beginning of the
      // showering point along the start_dir direction

       trunk_length[pl] = sqrt ( (shr_start.w - start.w) * (shr_start.w - start.w) + 
				   (shr_start.t - start.t) * (shr_start.t - start.t) );
       
       double sum = 0;//sum of charge square
       double hit_length;
       double factor=pitch/0.3;
       n_hits[pl] = hits.size();       
       dx[pl]=trunk_length[pl];
       dx_p[pl]=std::abs((shr_start.w - start.w)*factor);//3D length abs w/ pitch correction
       
       dQ[pl]=0;//sum of dQ
       
       for (int i=0;i<n_hits[pl];i++){
	 hit_length = sqrt((hits[i].w-start.w)*(hits[i].w-start.w)+
			   (hits[i].t-start.t)*(hits[i].t-start.t));
	 //dx[pl]=hit_length;
	 //Radius limit
	 if (hit_length<trunk_length[pl]){
	   n_hits_radius[pl]++;//select hits inside circle
	   std::vector<std::pair<float,float>> p_hit;//find triangle
	   p_hit.resize(3);
	   p_hit.at(0).first=start.w;
	   p_hit.at(0).second=start.t;
	   p_hit.at(1).first=shr_start.w;
	   p_hit.at(1).second=shr_start.t;
	   p_hit.at(2).first=hits[i].w;
	   p_hit.at(2).second=hits[i].t;	  
	   
	   float check_l1,check_l2;
	   check_l1=sqrt((p_hit.at(2).second-p_hit.at(0).second)*(p_hit.at(2).second-p_hit.at(0).second)+
			 (p_hit.at(2).first-p_hit.at(0).first)*(p_hit.at(2).first-p_hit.at(0).first));
	   check_l2=sqrt((p_hit.at(2).second-p_hit.at(1).second)*(p_hit.at(2).second-p_hit.at(1).second)+
			 (p_hit.at(2).first-p_hit.at(1).first)*(p_hit.at(2).first-p_hit.at(1).first));
	   
	   //Get the Triangle
	   Polygon2D poly(p_hit);
	   
	   if(check_l1*check_l2>0){
	     float in_an=poly.InteriorAngle(0);//hits NOT overlapping cluster/shower start point
	     if(tan(in_an)<=tan(clu_an)){//hits w/ cluster open angle
	       double Q = hits[i].charge * _charge_conversion;//!!!!charge is indeed charge/dx!!!!
	       sum += Q*Q;
	       dQ[pl] += Q;
	       n_hits_poly[pl]++;
	       hits_poly[pl].push_back(Q/pitch);///!!!!!!!!!!!!!!!!!!!!
	     }
	   }
	   else if(check_l1*check_l2==0){//hits overlaping cluster/shower start point
	     double Q = hits[i].charge * _charge_conversion;
	     sum += Q*Q;
	     dQ[pl] += Q;
	     n_hits_poly[pl]++;
	     hits_poly[pl].push_back(Q/pitch);
	   }
	 }
       }
       
       rms[pl]=sqrt(sum/n_hits_poly[pl]);
       mean[pl]=dQ[pl]/n_hits_poly[pl];
       sigma[pl]=sqrt(rms[pl]*rms[pl]-mean[pl]*mean[pl]);
       
       //std::cout<<"sigma:"<<sigma[pl]<<"\n";
       //std::cout<<"+:"<<(mean[pl]+sigma[pl])<<"\n";
       //std::cout<<"-:"<<(mean[pl]-sigma[pl])<<"\n";
       // n_hits_sigma[pl]=hits_poly[pl].size();
       //std::cout<<n_hits_sigma[pl]-n_hits_poly[pl]<<"\n";
       //std::cout<<hits_poly[pl].size()<<"\n";
       
       /*Get Median*/
       std::nth_element(hits_poly[pl].begin(),hits_poly[pl].begin() + hits_poly[pl].size()/2, hits_poly[pl].end());
       //std::cout<<"pl:"<<pl<<"look@me,size is:"<<hits_poly[pl].size()<<"median is:";
       //if(hits_poly[pl].at(hits_poly[pl].size()/2))median[pl]= hits_poly[pl].at((hits_poly[pl].size()/2-1));
       if(hits_poly[pl].size()>0)median[pl]= hits_poly[pl].at((hits_poly[pl].size()/2)); 
       //std::cout<<median[pl]<<"\n";
       
       
       for(size_t j=0; j<hits_poly[pl].size(); j++){
	 //if(hits_poly[pl].at(j)<=(mean[pl]+0.5*sigma[pl])&&hits_poly[pl].at(j)>=(mean[pl]-0.5*sigma[pl])){
	 if(hits_poly[pl].at(j)<median[pl]){///??????????????  
	   dQQ[pl] +=hits_poly[pl].at(j);
	   n_hits_sigma[pl]++;
	 }
       }
       
       //dQdx[pl]=dQ[pl]/n_hits_poly[pl];///dx[pl];
       //dQdx_pitch[pl]=dQ[pl]/pitch/n_hits_poly[pl];///dx_p[pl];

       dQdx[pl]=dQQ[pl]/n_hits_sigma[pl];///dx[pl];
       dQdx_pitch[pl]=dQQ[pl]/pitch/n_hits_sigma[pl];///dx_p[pl];
       
       if(pl==2){//select best plane
	 //int pl_bbest;
	 
	 //if(trunk_length[pl]>trunk_length[pl-1]&&trunk_length[pl]<10)pl_best=pl;
	 //else pl_best = pl-1;
	 
	 //if(trunk_length[pl_best]<trunk_length[pl-2]&&trunk_length[pl-2]<10)pl_best = pl-2;
	 //_pl_best =pl_best;//tree viriable
	 
	 //if(trunk_length[pl_best]>10)break;
	 //resultShower.fBestdQdxPlane = pl_best;///best plane go to showerquality
	 
	 std::vector<std::pair<int,double>> pl_best;
	 pl_best.resize(3);
	 pl_best.at(0).first=0;
	 pl_best.at(0).second=trunk_length[0];
	 pl_best.at(1).first=1;
         pl_best.at(1).second=trunk_length[1];
	 pl_best.at(2).first=2;
         pl_best.at(2).second=trunk_length[2];
	 std::sort(pl_best.begin(),pl_best.end(),larger);
	 //std::cout<<"length0:"<<pl_best.at(0).second<<"\n";
	 //std::cout<<"length1:"<<pl_best.at(1).second<<"\n";
	 //std::cout<<"length2:"<<pl_best.at(2).second<<"\n";
	 if(pl_best.at(2).second<10)_pl_best=pl_best.at(2).first;
	 else if(pl_best.at(1).second<10)_pl_best=pl_best.at(1).first;
	 else if(pl_best.at(0).second<10)_pl_best=pl_best.at(0).first;
	 else break;
	 //std::cout<<"best_pl:"<<_pl_best<<"\n";
       }      
       
       //tree variables w/ _ 
       _dQ   = dQ[_pl_best];
       _dQQ  = dQQ[_pl_best];
       _dQdx = dQdx[_pl_best];
       _dQdx_pitch =dQdx_pitch[_pl_best];
       _dQdx_pitch_pl2= dQdx_pitch[2];
       _rms = rms[_pl_best];
       _n_hits =n_hits[_pl_best];
       _n_hits_radius = n_hits_radius[_pl_best];
       _n_hits_poly= n_hits_poly[_pl_best];
       _n_hits_sigma=n_hits_sigma[_pl_best];
       _pl = pl;
       _pitch = pitch;
       _shrs_w=shr_start.w;
       _shrs_t=shr_start.t;
       _length= trunk_length[_pl_best];
       _median=median[pl];

       resultShower.fdQdx[pl] = dQdx_pitch[_pl_best];///dQdx go to showerquality      
       
       //std::cout<<"#########################################################\n";
       
       if(pl==2){//save the best plane
	 _tree->Fill();
       }
       
       //test------------------------------------------------------------------------
       //if (_verbose) 
       //play polygon
           }
    return;
  }
} //showerreco

#endif
