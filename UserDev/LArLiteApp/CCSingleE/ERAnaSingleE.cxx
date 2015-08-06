#ifndef ERTOOL_ERANASINGLEE_CXX
#define ERTOOL_ERANASINGLEE_CXX

#include "ERAnaSingleE.h"

namespace ertool {

  ERAnaSingleE::ERAnaSingleE(const std::string& name)
    : AnaBase(name)
    , _result_tree(nullptr)
    , fTPC(0.,-115.5,0.,254.8,117.5,1036.92)

  {


    PrepareTreeVariables();
    // keep track of number of events gone by
    _numEvts = 0;
    // keep track of singleEs found
    _singleE_ctr = 0;

    _debug = false;

    // set default energy cut (for counting) to 0
    _eCut = 0;

    // set initial efficiency to 0
    _eff = 0.;

    _h_e_nu_correlation = new TH2F("h_eNu_eNuReco","True Neutrino Energy vs. Reconstructed Neutrino Energy;True Neutrino Energy [MEV];Reconstructed Neutrino Energy [MEV]",100,0,3000,100,0,3000);

  }

  void ERAnaSingleE::Reset()
  {}

  bool ERAnaSingleE::Analyze(const EventData &data, const ParticleGraph &graph)
  {

    if (_debug)
      std::cout << "******  Begin ERAnaSingleE Analysis  ******" << std::endl;
    
    if (_debug){
      std::cout << "ParticleGraph Diagram: " << std::endl
		<< graph.Diagram() << std::endl;
    }
    
    // Get MC particle set
    auto const& mc_graph = MCParticleGraph();
    
    if (_debug){
      std::cout << "MC Particle Diagram: " << std::endl
		<< mc_graph.Diagram() << std::endl;
    }
    
    // Reset tree variables
    // Assume we will mis-ID
    ResetTreeVariables();
    
    _numEvts += 1;
    
    // Get MC EventData (showers/tracks...)
    //auto const& mc_data = MCEventData();
    
    // Keep track of all energy deposited in detector
    _EDep = 0;

    for (auto const s : data.Shower()){
      _EDep += s._energy;
      if (s._energy > _eCut) { _n_showers += 1; }
    }
    for (auto const &t : data.Track()){
      _EDep += t._energy;
      if (t._energy > _eCut) { _n_tracks += 1; }
    }
    if (_debug) { std::cout << "Total Energy deposited in detector: " << _EDep << std::endl; }
    
    // Loop through MC Particle Graph...not yet being made...
    // */ 
    // loop over all particles in MC particle set (made by Helper)
    for (auto &p : mc_graph.GetParticleArray()){
      // Find the Lepton and store its energy
      if (abs(p.PdgCode()) == 12 || abs(p.PdgCode()) == 14){
	_e_nu = p.Energy();
	_x_nu = p.Vertex()[0];
	_y_nu = p.Vertex()[1];
	_z_nu = p.Vertex()[2];
	_px_nu = p.Momentum()[0];
	_py_nu = p.Momentum()[1];
	_pz_nu = p.Momentum()[2];
	_pdg_nu = p.PdgCode();
	
	bool found_lepton_daughter = false;

	for (auto &nud : p.Children()){
	  auto d = mc_graph.GetParticle(nud) ;
	  if (abs(d.PdgCode()) == 11 || abs(d.PdgCode()) == 13){
	    if(found_lepton_daughter) 
	      std::cout<<"wtf already found daughter? overwriting variables..."<<std::endl;
	    found_lepton_daughter = true;
	    _e_lep = d.Energy();
	    _x_lep = d.Vertex()[0];
	    _y_lep = d.Vertex()[1];
	    _z_lep = d.Vertex()[2];
	    _px_lep = d.Momentum()[0];
	    _py_lep = d.Momentum()[1];
	    _pz_lep = d.Momentum()[2];
	    _theta_lep = (180./3.14) * acos( _pz_lep / sqrt( _px_lep*_px_lep + _py_lep*_py_lep + _pz_lep*_pz_lep ) );
	    _phi_lep   = (180./3.14) * asin( _py_lep / sqrt( _px_lep*_px_lep + _py_lep*_py_lep ) );
					     
	    _pdg_lep = d.PdgCode();
	  }
	}
      }
    }// for all particles in MC Particle Graph
    
    // find the number of tracks that start within 1 cm of the neutrino interaction
    //    _n_tracksInt = 0;
    geoalgo::Point_t nu_vtx(_x_nu,_y_nu,_z_nu);
    
    // size of ParticleSet should be the number of neutrinos found, each associated with a single electron
    _n_singleReco = graph.GetNumPrimaries();
    auto const& particles = graph.GetParticleArray();
    int neutrinos = 0;

    for ( auto const & p : particles ){
      if ( p.PdgCode() == 12 ){
	neutrinos += 1;
	// get all descendants of the neutrino
	_e_dep = 0;
	auto const descendants = graph.GetAllDescendantNodes(p.ID());
	for ( auto const & desc : descendants){
	  auto const & part = graph.GetParticle(desc);
	  // does this particle have a reco ID?
	  if (part.HasRecoObject() == true){
	    // get the reco object's dep. energy
	    // if shower
	    if (part.RecoType() == kShower)
	      _e_dep += data.Shower(part.RecoID())._energy;
	    if (part.RecoType() == kTrack)
	      _e_dep += data.Track(part.RecoID())._energy;
	  }// if the particle has a reco object
	}// for all neutrino descendants
      }// if we found the neutrino
      
    }

    // if only 1 CCSingleE interaction was found -> misID = 0
    _misID = 1;
    if (neutrinos == 1){
      _misID = 0;
      _singleE_ctr += 1;
    }
    // If no single electrons reconstructed -> fill tree anyway with misID info
    if (neutrinos == 0){
      _result_tree->Fill();
      return true;
    }
    
    for( auto const & p : graph.GetParticleArray() ){
      

      if(abs(p.PdgCode()) == 12 ){

	auto neutrino = p ;
	
	_x_nuReco = neutrino.Vertex()[0];
	_y_nuReco = neutrino.Vertex()[1];
	_z_nuReco = neutrino.Vertex()[2];
	_px_nuReco = neutrino.Momentum()[0];
	_py_nuReco = neutrino.Momentum()[1];
	_pz_nuReco = neutrino.Momentum()[2];

	// search for all energy deposited by something starting
	// within x centimeters of the neutrino vertex
	geoalgo::Point_t nu_vtx(_x_nuReco,_y_nuReco,_z_nuReco);
	_e_r = 0;
	// loop again over all particles
	for( auto const & p2 : graph.GetParticleArray() ){
	  // is the vertex within 10 cm from the nu vtx
	  if (p2.Vertex().Dist(nu_vtx) < 200.){
	    // does this particle have a reco ID?
	    if (p2.HasRecoObject() == true){
	      // get the reco object's dep. energy
	      // if shower
	      if (p2.RecoType() == kShower){
		_e_r += data.Shower(p2.RecoID())._energy;
	      }
	      if (p2.RecoType() == kTrack)
		_e_r += data.Track(p2.RecoID())._energy;
	    }// if the particle has a reco object
	  }// if start point within 10 cm of neutrino vertex
	}// loop over all particles a second time
	
	double momMag = 0;
	
	_e_trkIntReco = 0;
	_e_nuReco = 0;
	//find the neutrino daughter that is a lepton
	for (auto const& d : neutrino.Children()){
	  
	  auto daught = graph.GetParticle(d) ;
	  _e_nuReco += daught.KineticEnergy();
	  
	  // if not a lepton, add energy to tracks
	  if (abs(daught.PdgCode()) != 11)
	    _e_trkIntReco += daught.KineticEnergy();
	  
	  
	  if(abs(daught.PdgCode()) == 11 ){//|| abs(daught.PdgCode()) == 13){

	    _e_lepReco = daught.Energy();

	    // get shower's deposited energy
	    double e_lep_dep = data.Shower(daught.RecoID())._energy;
	    
	    //length of shower (geoalgo cone) associated with the electron
	    //_showerlength_lepReco = data.AllShower().at(daught.RecoObjID()).Length(); //kaleko
	    _x_lepReco = daught.Vertex()[0];
	    _y_lepReco = daught.Vertex()[1];
	    _z_lepReco = daught.Vertex()[2];
	    _px_lepReco = daught.Momentum()[0];
	    _py_lepReco = daught.Momentum()[1];
	    _pz_lepReco = daught.Momentum()[2];
	    double theta_lepReco_rad = acos( _pz_lepReco / sqrt( _px_lepReco*_px_lepReco + _py_lepReco*_py_lepReco + _pz_lepReco*_pz_lepReco ) );
	    _theta_lepReco = (180./3.14) * theta_lepReco_rad;
	    _phi_lepReco   = (180./3.14) * asin( _py_lepReco / sqrt( _px_lepReco*_px_lepReco + _py_lepReco*_py_lepReco ) );
	    _lep_vtxdist = sqrt( (_x_lep-_x_lepReco)*(_x_lep-_x_lepReco) +
				 (_y_lep-_y_lepReco)*(_y_lep-_y_lepReco) + 
				 (_z_lep-_z_lepReco)*(_z_lep-_z_lepReco) );
	    _lep_dot = ( ( _px_lep*_px_lepReco + _py_lep*_py_lepReco + _pz_lep*_pz_lepReco ) / 
			 ( sqrt( _px_lepReco*_px_lepReco + _py_lepReco*_py_lepReco + _pz_lepReco*_pz_lepReco ) * 
			   sqrt( _px_lep*_px_lep + _py_lep*_py_lep + _pz_lep*_pz_lep ) ) );

	    // try and get CCQE energy
	    _e_nuCCQE = CCQEEnergy(e_lep_dep,_pz_lepReco / sqrt( _px_lepReco*_px_lepReco + _py_lepReco*_py_lepReco + _pz_lepReco*_pz_lepReco ));
	    
	    ::geoalgo::HalfLine shr(daught.Vertex(),daught.Momentum());
	    
	    double detHalfHeight = 116.5 ;
	    
	    //If the lepton is contained, store the fiducial distance and the distance to top wall of shower.
	    //	  if( _x_lepReco > 0 && _x_lepReco < 256 && _y_lepReco > -116 
	    //	   && _y_lepReco < 116 && _z_lepReco > 0 && _z_lepReco < 1036){
	    //	    _distToTopWall = (_y_lepReco - detHalfHeight)*daught.Momentum().Length()/_py_lepReco ;
	    //	    _distToWall = sqrt(_geoAlgo.SqDist(daught.Vertex(),fTPC));
	    //	    }
	    if(_geoAlgo.Intersection(fTPC,shr,true).size() > 0){
	      _distBackAlongTraj = sqrt(daught.Vertex().SqDist(_geoAlgo.Intersection(fTPC,shr,true)[0])) ;
	      _distToTopWall     = (_y_lepReco - detHalfHeight)*daught.Momentum().Length()/_py_lepReco ;
	      _distToWall        = sqrt(_geoAlgo.SqDist(daught.Vertex(),fTPC));
	    }
	    else{
	      _distBackAlongTraj = -9999; 
	      _distToTopWall     = -9999;  
	      _distToWall        = -9999; 
	    }
	    
	    momMag = sqrt(_px_lepReco*_px_lepReco + _py_lepReco *_py_lepReco + _pz_lepReco*_pz_lepReco);
	    _px_lepNormReco = _px_lepReco / momMag ;
	    _py_lepNormReco = _py_lepReco / momMag ;
	    _pz_lepNormReco = _pz_lepReco / momMag ;
	    
	    double py = _py_lepNormReco ;
	    double pz = _pz_lepNormReco ;
	    double convert = 180.0/ 3.1415926525898 ; 
	    double quad = py/pz  ;
	    if(quad < 0)
	      quad *=-1; 
	    
	    _angle_Norm = atan(quad );
	    _angle_Norm*= convert ;
	    
	    if(py >=0 && pz <0)
	      _angle_Norm = 180 - _angle_Norm ; 
	    else if(py <0 && pz <0)
	      _angle_Norm = 180 + _angle_Norm ;
	    else if(py <0 && pz >=0)
	      _angle_Norm = 360 - _angle_Norm ;
	    
	  }// if daughter is electron
	}// for all children 
	_result_tree->Fill();
      }// if electorn neutrino  
    }// loop over all particles in graph

    _h_e_nu_correlation->Fill(_e_nu,_e_nuReco);
    
    if (_debug){
      std::cout << "Ana results:" << std::endl
		<< "Mis-ID                 : " << _misID << std::endl           
		<< "Neutrino E  [Mc,Reco]  : [" << _e_nu << ", " << _e_nuReco << "]" <<  std::endl
		<< "EDep (in interaction)  : [" << _e_dep << "]" <<  std::endl
		<< "Lepton E  [Mc,Reco]    : [" << _e_lep << ", " << _e_lepReco << "]" <<  std::endl
		<< "Neutrals E  [Mc,Reco]  : [" << _e_neutrals << "]" <<  std::endl
		<< "Nucl dE  [Mc,Reco]     : [" << _e_nucleus_diff << "]" <<  std::endl
		<< "Lepton Vtx dist Mc-Reco:  " << sqrt( (_x_lep-_x_lepReco)*(_x_lep-_x_lepReco) +
							 (_y_lep-_y_lepReco)*(_y_lep-_y_lepReco) +
							 (_z_lep-_z_lepReco)*(_z_lep-_z_lepReco) ) << std::endl
		<< "Lepton theta [Mc,Reco] : [" << _theta_lep << ", " << _theta_lepReco << "]" <<  std::endl
		<< "Lepton phi [Mc,Reco]   : [" << _phi_lep << ", " << _phi_lepReco << "]" <<  std::endl << std::endl;
    }
    
    return true;
}
  
  void ERAnaSingleE::ProcessEnd(TFile* fout)
  {
    
    _eff = 100*_singleE_ctr/float(_numEvts);
    
    std::cout << "RESULTS: " << std::endl
	      << "Tot Events    : " << _numEvts << std::endl
	      << "SingleE Events: " << _singleE_ctr << std::endl
	      << "Eff           : " << _eff << " %" << std::endl;

    MakeEffPlot("e_lep",10,0,3000);
    MakeEffPlot("e_nu",10,0,3000);
    //MakeEffPlot("n_tracks",10,-0.5,9.5);
    //MakeEffPlot("n_tracksReco",10,-0.5,9.5);

    if (fout){
      fout->cd();
      _result_tree->Write();
      _h_e_nu_correlation->Write();
    }

    return;
  }


  void ERAnaSingleE::MakeEffPlot(std::string varname,
				 int nbins, double xmin, double xmax){

    // Efficiency vs. variable "varname"
    int Nall, Nok;
    TH1D *hist = new TH1D(Form("_hEffvs%s",varname.c_str()),Form("Efficiency vs %s",varname.c_str()),nbins,xmin,xmax);
    TH1D *All = new TH1D("All","All",nbins,xmin,xmax); // Histogram of all entries as a function of Elep
    TH1D *Ok = new TH1D("Ok","Ok",nbins,xmin,xmax);  // Histogram of all entries that are not mis-ID vs Elep
    Nall = _result_tree->Draw(Form("%s>>All",varname.c_str()),"");
    Nok = _result_tree->Draw(Form("%s>>Ok",varname.c_str()),"misID==0");
    for (int n=0; n < nbins; n++){
      if (All->GetBinContent(n+1) > 0){
	double eff = Ok->GetBinContent(n+1)/All->GetBinContent(n+1);
	hist->SetBinContent(n+1, eff);
	hist->SetBinError(n+1, sqrt( eff*(1-eff) / All->GetBinContent(n+1) ) );
      }
      else{
	hist->SetBinContent(n+1,0);
	hist->SetBinError(n+1,0.);
      }
    }
    delete All;
    delete Ok;
    hist->Write();

    return;
  }

  double ERAnaSingleE::CCQEEnergy(const double& Elep, const double& theta){
    

    double Mp = 938.28; // proton mass [MeV]
    double Mn = 939.57; // neutron mass [MeV]
    double Eb = 30;     // binding energy [MeV]
    double PZlep = Elep*theta; // lepton z-component momentum [MeV]
    // assume lepton mass irrelevant (i.e. 0)

    double ECCQE = ((Mp*Mp) - (Mn-Eb)*(Mn-Eb) + 2*(Mn-Eb)*Elep) / (2*(Mn-Eb-Elep+PZlep));
      
    return ECCQE;
  }


  void ERAnaSingleE::PrepareTreeVariables(){

    if (_result_tree) { delete _result_tree; }
    
    _result_tree = new TTree("_result_tree","Result Tree");
    _result_tree->Branch("_numEvts",&_numEvts,"numEvts/I");
    _result_tree->Branch("_EDep",&_EDep,"EDep/D");
    _result_tree->Branch("_angle_Norm",&_angle_Norm,"_angle_Norm/D");
    _result_tree->Branch("_n_singleReco",&_n_singleReco,"n_singleReco/I");
    _result_tree->Branch("_distToTopWall",&_distToTopWall,"distToTopWall/D");
    _result_tree->Branch("_distToWall",&_distToWall,"distToWall/D");
    _result_tree->Branch("_distBackAlongTraj",&_distBackAlongTraj,"distBackAlongTraj/D");
    _result_tree->Branch("_e_nu",&_e_nu,"e_nu/D");
    _result_tree->Branch("_e_nuReco",&_e_nuReco,"e_nuReco/D");
    _result_tree->Branch("_e_nuCCQE",&_e_nuCCQE,"e_nuCCQE/D");
    _result_tree->Branch("_e_dep",&_e_dep,"e_dep/D");
    _result_tree->Branch("_e_r",&_e_r,"e_r/D");
    _result_tree->Branch("_x_nu",&_x_nu,"x_nu/D");
    _result_tree->Branch("_y_nu",&_y_nu,"y_nu/D");
    _result_tree->Branch("_z_nu",&_z_nu,"z_nu/D");
    _result_tree->Branch("_px_nu",&_px_nu,"px_nu/D");
    _result_tree->Branch("_py_nu",&_py_nu,"py_nu/D");
    _result_tree->Branch("_pz_nu",&_pz_nu,"pz_nu/D");
    _result_tree->Branch("_x_nuReco",&_x_nuReco,"x_nuReco/D");
    _result_tree->Branch("_y_nuReco",&_y_nuReco,"y_nuReco/D");
    _result_tree->Branch("_z_nuReco",&_z_nuReco,"z_nuReco/D");
    _result_tree->Branch("_px_nuReco",&_px_nuReco,"px_nuReco/D");
    _result_tree->Branch("_py_nuReco",&_py_nuReco,"py_nuReco/D");
    _result_tree->Branch("_pz_nuReco",&_pz_nuReco,"pz_nuReco/D");
    _result_tree->Branch("_e_trkIntReco",&_e_trkIntReco,"e_trkIntReco/D");
    _result_tree->Branch("_pdg_nu",&_pdg_nu,"pdg_nu/I");
    _result_tree->Branch("_e_lep",&_e_lep,"e_lep/D");
    _result_tree->Branch("_pdg_lep",&_pdg_lep,"pdg_lep/I");
    _result_tree->Branch("_x_lep",&_x_lep,"x_lep/D");
    _result_tree->Branch("_y_lep",&_y_lep,"y_lep/D");
    _result_tree->Branch("_z_lep",&_z_lep,"z_lep/D");
    _result_tree->Branch("_px_lep",&_px_lep,"px_lep/D");
    _result_tree->Branch("_py_lep",&_py_lep,"py_lep/D");
    _result_tree->Branch("_pz_lep",&_pz_lep,"pz_lep/D");
    _result_tree->Branch("_theta_lep",&_theta_lep,"theta_lep/D");
    _result_tree->Branch("_phi_lep",&_phi_lep,"phi_lep/D");
    _result_tree->Branch("_e_lepReco",&_e_lepReco,"e_lepReco/D");
    _result_tree->Branch("_theta_lepReco",&_theta_lepReco,"theta_lepReco/D");
    _result_tree->Branch("_phi_lepReco",&_phi_lepReco,"phi_lepReco/D");
    _result_tree->Branch("_x_lepReco",&_x_lepReco,"x_lepReco/D");
    _result_tree->Branch("_y_lepReco",&_y_lepReco,"y_lepReco/D");
    _result_tree->Branch("_z_lepReco",&_z_lepReco,"z_lepReco/D");
    _result_tree->Branch("_px_lepReco",&_px_lepReco,"px_lepReco/D");
    _result_tree->Branch("_py_lepReco",&_py_lepReco,"py_lepReco/D");
    _result_tree->Branch("_pz_lepReco",&_pz_lepReco,"pz_lepReco/D");
    _result_tree->Branch("_px_lepNormReco",&_px_lepNormReco,"px_lepNormReco/D");
    _result_tree->Branch("_py_lepNormReco",&_py_lepNormReco,"py_lepNormReco/D");
    _result_tree->Branch("_pz_lepNormReco",&_pz_lepNormReco,"pz_lepNormReco/D");
    _result_tree->Branch("_misID",&_misID,"misID/I");
  }
  
  void ERAnaSingleE::ResetTreeVariables(){
    
    //_misID = 1;

    _n_singleReco  = -1;
    _n_showers     = -1;
    _n_showersReco = -1;
    _n_tracks      = -1;
    _n_tracksReco  = -1;
    _n_tracksInt   = -1;
    _n_tracksIntReco = -1;
    _n_protons     = -1;
    _n_neutrons    = -1;
    _n_piplus      = -1;
    _n_pi0         = -1;

    _e_nu          = -1000;
    _e_nuReco      = -1000;
    _e_nuCCQE     = -1000;
    _e_dep        = -1000;
    _e_r          = -1000;
    _x_nu         = -1000;
    _y_nu         = -1000;
    _z_nu         = -1000;
    _x_nuReco     = -1000;
    _y_nuReco     = -1000;
    _z_nuReco     = -1000;
    _px_nu         = -1000;
    _py_nu         = -1000;
    _pz_nu         = -1000;
    _px_nuReco     = -1000;
    _py_nuReco     = -1000;
    _pz_nuReco     = -1000;
    _pdg_nu        = -1;
    _e_lep         = -1000;
    _x_lep         = -1000;
    _y_lep         = -1000;
    _z_lep         = -1000;
    _pdg_lep       = -1;
    _theta_lep     = -360;
    _phi_lep       = -360;
    _e_lepReco     = -1000;

    _showerlength_lepReco     = -1000;
    _x_lepReco     = -1000;
    _y_lepReco     = -1000;
    _z_lepReco     = -1000;
    _theta_lepReco = -360;
    _phi_lepReco   = -360;
    _lep_dot       = -2;
    _lep_vtxdist   = -1000;
    _distToTopWall = -9999;
    _distToWall    = -9999;
    _distBackAlongTraj = -9999;
    _angle_Norm    = -9999;

    _e_trkInt = -1;
    _e_trkIntReco = -1;

    return;
  }

  }

#endif
