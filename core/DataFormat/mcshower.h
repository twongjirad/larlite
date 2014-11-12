/**
 * \file mcshower.h
 *
 * \ingroup DataFormat
 * 
 * \brief Class def header for mcshower data container
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup DataFormat
    
@{*/

#ifndef LARLITE_MCSHOWER_H
#define LARLITE_MCSHOWER_H

#include "mctrajectory.h"

namespace larlite {
  /**
     \class mcshower
     LArSoft utility class (not data container) MCShowerProfile 
     implemented as a data product in Larlite.
  */
  class mcshower : public data_base {
    
  public:

    /// Default constructor
    mcshower() : data_base(data::kMCShower) {clear_data();}

    /// Copy constructor
    mcshower(const mcshower& origin) : data_base(origin),
				       fOrigin(origin.fOrigin),
                                       fPDGCode(origin.fPDGCode),
				       fTrackID(origin.fTrackID),
				       fProcess(origin.fProcess),
				       fStart(origin.fStart),
				       fEnd(origin.fEnd),
				       fMotherPDGCode(origin.fMotherPDGCode),
				       fMotherTrackID(origin.fMotherTrackID),
				       fMotherProcess(origin.fMotherProcess),
				       fMotherStart(origin.fMotherStart),
				       fMotherEnd(origin.fMotherEnd),
				       fAncestorPDGCode(origin.fAncestorPDGCode),
				       fAncestorTrackID(origin.fAncestorTrackID),
				       fAncestorProcess(origin.fAncestorProcess),
                                       fAncestorStart(origin.fAncestorStart),
				       fAncestorEnd(origin.fAncestorEnd),
                                       fDaughterTrackID(origin.fDaughterTrackID),
                                       fDetProfile(origin.fDetProfile),
                                       fPlaneCharge(origin.fPlaneCharge)
    {}

    /// Default destructor
    virtual ~mcshower(){}

    /// Clear method
    virtual void clear_data();

    //--- Getters ---//

    simb::Origin_t     Origin    () const { return fOrigin;    }

    int                PdgCode   () const { return fPDGCode;   }
    unsigned int       G4TrackID () const { return fTrackID; } 
    const std::string& Process   () const { return fProcess;   }
    const mcstep&      G4Start   () const { return fStart;   }
    const mcstep&      G4End     () const { return fEnd;     }

    int                MotherPdgCode   () const { return fMotherPDGCode;   }
    unsigned int       MotherG4TrackID () const { return fMotherTrackID; }
    const std::string& MotherProcess   () const { return fMotherProcess;   }
    const mcstep&      MotherG4Start   () const { return fMotherStart;   }
    const mcstep&      MotherG4End     () const { return fMotherEnd;     }

    int                AncestorPdgCode   () const { return fAncestorPDGCode;   }
    unsigned int       AncestorG4TrackID () const { return fAncestorTrackID; }
    const std::string& AncestorProcess   () const { return fAncestorProcess;   }
    const mcstep&      AncestorG4Start   () const { return fAncestorStart;   }
    const mcstep&      AncestorG4End     () const { return fAncestorEnd;     }

    const mcstep& DetProfile () const { return fDetProfile; }
    
    const std::vector<unsigned int>&  DaughterTrackID() const { return fDaughterTrackID; }

    double Charge(const size_t plane) const;

    //--- Setters ---//
    void Origin    ( simb::Origin_t o ) { fOrigin    = o;    }

    void PdgCode   ( int id                  ) { fPDGCode   = id;   }
    void G4TrackID ( unsigned int id         ) { fTrackID = id;   }
    void Process   ( const std::string &name ) { fProcess   = name; }
    void G4Start   ( const mcstep &s         ) { fStart   = s;    }
    void G4End     ( const mcstep &s         ) { fEnd     = s;    }

    void MotherPdgCode   ( int id                  ) { fMotherPDGCode   = id;   }
    void MotherG4TrackID ( unsigned int id         ) { fMotherTrackID = id;   }
    void MotherProcess   ( const std::string& name ) { fMotherProcess   = name; }
    void MotherG4Start   ( const mcstep& s         ) { fMotherStart   = s;    }
    void MotherG4End     ( const mcstep& s         ) { fMotherEnd     = s;    }

    void AncestorPdgCode   ( int id                  ) { fAncestorPDGCode   = id;   }
    void AncestorG4TrackID ( unsigned int id         ) { fAncestorTrackID = id;   }
    void AncestorProcess   ( const std::string& name ) { fAncestorProcess   = name; }
    void AncestorG4Start   ( const mcstep& s         ) { fAncestorStart   = s;    }
    void AncestorG4End     ( const mcstep& s         ) { fAncestorEnd     = s;    }

    void DetProfile ( const mcstep& s) { fDetProfile = s; }

    void DaughterTrackID ( const std::vector<unsigned int>& id_v ) { fDaughterTrackID = id_v; }

    void Charge (const std::vector<double>& q) { fPlaneCharge = q; }

  protected:

    //---- Origin info ----//
    simb::Origin_t fOrigin;    ///< Origin information

    //---- Shower particle info ----//
    int          fPDGCode;     ///< Shower particle PDG code
    unsigned int fTrackID;   ///< Shower particle G4 track ID
    std::string  fProcess;     ///< Shower particle's creation process
    mcstep       fStart;     ///< Shower particle's G4 start point
    mcstep       fEnd;       ///< Shower particle's G4 end point

    //---- Mother's particle info ---//
    int          fMotherPDGCode;   ///< Shower's mother PDG code   
    unsigned int fMotherTrackID; ///< Shower's mother G4 track ID
    std::string  fMotherProcess;   ///< Shower's mother creation process
    mcstep       fMotherStart;   ///< Shower's mother G4 start point
    mcstep       fMotherEnd;     ///< Shower's mother G4 end point

    //---- Ancestor's particle info ---//
    int          fAncestorPDGCode;   ///< Shower's ancestor PDG code   
    unsigned int fAncestorTrackID; ///< Shower's ancestor G4 track ID
    std::string  fAncestorProcess;   ///< Shower's ancestor creation process
    mcstep       fAncestorStart;   ///< Shower's ancestor G4 start point
    mcstep       fAncestorEnd;     ///< Shower's ancestor G4 end point

    //---- Energy deposition info ----//
    std::vector<unsigned int>  fDaughterTrackID; ///< Daughters' track ID
    mcstep                     fDetProfile;      ///< Combined energy deposition information

    //---- Charge per plane ----//
    std::vector<double> fPlaneCharge; ///< Charge deposit per plane

    ////////////////////////
    ClassDef(mcshower,6)
    ////////////////////////
      
  };

  /**
     \class event_mcshower
     A collection storage class of multiple mcshower
  */
  class event_mcshower : public std::vector<larlite::mcshower>,
			 public event_base {
    
  public:
    
    /// Default constructor
    event_mcshower(std::string name="noname") : event_base(data::kMCShower,name) {clear_data();}
    
    /// Default copy constructor
    event_mcshower(const event_mcshower& original) : std::vector<larlite::mcshower>(original),
						     event_base(original)
    {}
    
    /// Default destructor
    ~event_mcshower(){}
    
    /// Method to clear currently held data contents in the buffer
    virtual void clear_data(){event_base::clear_data(); clear();}
    
  private:
    
    //////////////////////////
    ClassDef(event_mcshower,2)
    //////////////////////////
  };
  
}
#endif

/** @} */ // end of doxygen group 
