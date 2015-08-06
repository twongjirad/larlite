#ifndef RECOTOOL_PANDORASHOWER_CXX
#define RECOTOOL_PANDORASHOWER_CXX

#include "PandoraShower.h"
#include <map>
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/pfpart.h"
namespace larlite {

   PandoraShower::PandoraShower() : ana_base(), fShowerAlgo( nullptr ) {
      _name = "PandoraShower";
      fHitProducer = "pandoraCosmicKHitRemoval";
      fClusterProducer = "pandoraNu";
      fPFParticleProducer = "pandoraNu";

      /// Get the geometry of the detector, eps. the number of the wire planes
      auto geom = ::larutil::Geometry::GetME();
      fNPlanes = geom->Nplanes();
      // for ( auto iview : geom->Views() ) fViews.insert( iview );
      fViews   = geom->Views();
   }

   bool PandoraShower::initialize() {
      _mgr = 0;

      if( !fShowerAlgo ) {
         throw ::cluster::CRUException("Shower reco algorithm not attached... aborting.");
         return false;
      }

      return true;
   }

   bool PandoraShower::analyze( storage_manager* storage ) {
      _mgr = storage;

      // Re-initialize tools
      fShowerAlgo->Reset();

      // Retrieve hits, clusters and PF particles
      auto hits = ( event_hit* )( storage->get_data( data::kHit, fHitProducer ) );
      auto clusters = ( event_cluster* )( storage->get_data( data::kCluster, fClusterProducer ) );
      auto pfparts = ( event_pfpart* )( storage->get_data( data::kPFParticle, fPFParticleProducer ) );

      if ( !hits->size() || !clusters->size() || !pfparts->size() ) return true;

      // Prepare output storage
      auto showers = storage->get_data<event_shower>("pandorashower");
      showers->clear();
      showers->reserve( pfparts->size() );

      // set event ID through storage manager
      storage->set_id(pfparts->run(),pfparts->subrun(),pfparts->event_id());
      
      //std::cout << "Event " << pfparts->event_id() << ": " << std::endl;
      // if ( pfparts->event_id() < 304 || pfparts->event_id() > 304 ) return true;

      // Associate PF particles and clusters
      AssSet_t ass_clusters;
      ass_clusters.reserve( pfparts->size() );
      auto pfparts_ass = storage->get_data<event_ass>(pfparts->name());
      ass_clusters = pfparts_ass->association( pfparts->id(), clusters->id() );

      // Generate the ClusterParamsAlg vector
      std::vector< ::cluster::ClusterParamsAlg > cpans;
      fCRUHelper.GenerateCPAN( storage, fClusterProducer, cpans );
      // FIX ME: temporarily forcing the cluster plane type (by default it's set from hits, but seems there's an issue for stored value there)
      for(size_t i=0; i<clusters->size(); ++i) {
	cpans[i].SetVerbose(false);
	cpans[i].SetPlane((*clusters)[i].View());
	cpans[i].DisableFANN();
	cpans[i].FillParams(false,false,false,false,false,false);
	cpans[i].FillPolygon();
      }
      // Create association holder
      AssSet_t iassociations;
      iassociations.reserve( pfparts->size() );

      // Calculate total hit charges
      std::map< larlite::geo::View_t, double > TotalHitCharges;
      for ( auto const& iview : fViews ) TotalHitCharges[iview] = 0.;
      CalculateTotalHitCharge( hits, TotalHitCharges );
      bool process = false;

      for ( size_t ipart = 0; ipart < pfparts->size(); ipart++ ) {
         if ( std::abs( pfparts->at(ipart).PdgCode() ) != 11 ) break;
         // std::cout << "PF particle " << ipart << " is associated with ..." << std::endl;

         std::map< larlite::geo::View_t, double > TotalClusterCharges;
         for ( auto const& iview : fViews ) TotalClusterCharges[iview] = 0.;

         // Create a cpan holder
         std::vector< ::cluster::ClusterParamsAlg > cpan_holder;
         cpan_holder.reserve( ass_clusters[ipart].size() );

         // Create an association holder
         AssUnit_t iass;
         iass.reserve( ass_clusters[ipart].size() );

         for ( auto const& c_index : ass_clusters[ipart] ) {
            auto iview = clusters->at(c_index).View();
            // std::cout << "   cluster " << c_index << " in Plane " 
            //           << iview << ", charge = "
            //           << clusters->at(c_index).Charge() << std::endl;
            TotalClusterCharges[iview] += clusters->at(c_index).Integral();
	    //std::cout<<iview<<std::endl;
            // Fill the clusters associated to a certain PF particle
            cpan_holder.push_back( cpans.at(c_index) );
            iass.push_back( c_index );
         }

         // Simple check
         // for ( auto const& iview : fViews ) {
         //    std::cout << "   in Plane " << iview << ", total cluster charge = "
         //              << TotalClusterCharges[iview] << ", total hit charge = "
         //              << TotalHitCharges[iview] << ", efficiency = "
         //              << TotalClusterCharges[iview]/TotalHitCharges[iview] << std::endl;
         // }

         // For each PF particle, fill the associated clusters into the
         // input object of the shower reconstruction
         fShowerAlgo->AppendInputClusters( cpan_holder );
         iassociations.push_back( iass );
	 process = true;
      }

      if(!process) return false;
      // Call the shower reconstruction
      auto results = fShowerAlgo->Reconstruct();

      // Make sure result has the same size
      if ( results.size() != iassociations.size() )
         throw ::showerreco::ShowerRecoException("Mismatch in # of showers from algorithm's return!");

      for ( size_t ires = 0; ires < results.size(); ires++ ) {

         auto& result = results.at( ires );

         // Set ID
         result.set_id( showers->size() );

         // Store
         showers->push_back( result );

      } // done looping over matched cluster pairs

      auto shower_ass = storage->get_data<event_ass>(showers->name());
      shower_ass->set_association(showers->id(),product_id(data::kCluster,fClusterProducer),iassociations);
      //showers->set_association( data::kCluster, fClusterProducer, iassociations );
      return true;
   }

   bool PandoraShower::finalize() {
      return true;
   }

   void PandoraShower::CalculateTotalHitCharge( larlite::event_hit const* hits, std::map< larlite::geo::View_t, double >& charges ) {
      for ( auto const& hit : ( *hits ) ) {
         charges[hit.View()] += hit.Integral();
      }
      return;
   }

}
#endif
