/**
 * \file PSet.h
 *
 * \ingroup FhiclLite
 * 
 * \brief Class def header for a class PSet
 *
 * @author kazuhiro
 */

/** \addtogroup FhiclLite

    @{*/
#ifndef BASICTOOL_FHICLLITE_PSET_H
#define BASICTOOL_FHICLLITE_PSET_H

#include <iostream>
#include <string>
#include <map>
#include <TString.h>
#include "FhiclLiteException.h"
#include "UtilFunc.h"
namespace fcllite {
  /**
     \class PSet
     User defined class PSet ... these comments are used to generate
     doxygen documentation!
  */
  class PSet {
    
  public:
    
    /// Default constructor
    PSet(const std::string name="",
	 const std::string data="");

    /// Default destructor
    virtual ~PSet(){};

    /// Copy ctor
    PSet(const PSet& orig) : _name       ( orig._name       )
			   , _data_value ( orig._data_value )
			   , _data_pset  ( orig._data_pset  )
    {}

    /// name getter
    const std::string& name() const { return _name; }
    
    /// operator override
    inline bool operator==(const PSet& rhs) const
    {
      if(_name != rhs.name()) return false;
      auto const v_keys = this->value_keys();
      if(v_keys.size() != rhs.value_keys().size()) return false;
      for(auto const& key : v_keys) {
	if(!rhs.contains_value(key))
	  return false;
	if(this->get<std::string>(key) != rhs.get<std::string>(key))
	  return false;
      }
      auto const p_keys = this->pset_keys();
      if(p_keys.size() != rhs.pset_keys().size()) return false;
      for(auto const& key : p_keys) {
	if(!rhs.contains_pset(key))
	  return false;
	if(this->get_pset(key) != rhs.get_pset(key))
	  return false;
      }
      return true;
    }

    inline bool operator!=(const PSet& rhs) const
    { return !((*this) == rhs); }

    /// clear method
    void clear() 
    { _data_value.clear(); _data_pset.clear(); }

    /// Set data contents
    void add(const std::string& data);

    /// Insert method for a simple param
    void add_value(std::string key, std::string value);

    /// Insert method for a PSet rep
    void add_pset(const PSet& p);

    /// Insert method for a PSet rep
    void add_pset(std::string key,
		  std::string pset);
    
    /// Dump into a text format
    std::string dump(size_t indent_size=0) const;

    /// Template getter
    template <class T>
    T get(const std::string& key) const{
      auto iter = _data_value.find(key);
      if( iter == _data_value.end() ) {
	std::string msg;
	msg = "Key does not exist: \"" + key + "\"";
	throw FhiclLiteException(msg);
      }
      return FromString<T>((*iter).second);
    }

    const PSet& get_pset(const std::string& key) const;

    size_t size() const;
    const std::vector<std::string> keys() const;
    const std::vector<std::string> value_keys () const;
    const std::vector<std::string> pset_keys  () const;
    bool  contains_value (const std::string& key) const;
    bool  contains_pset  (const std::string& key) const;

  protected:

    enum KeyChar_t {
      kParamDef,
      kBlockStart,
      kBlockEnd,
      kNone
    };

    std::pair<PSet::KeyChar_t,size_t> search(const std::string& txt, const size_t start) const;
    void strip(std::string& str, const std::string& key);
    void rstrip(std::string& str, const std::string& key);
    void no_space(std::string& txt);

    std::string _name;

    std::map<std::string,std::string> _data_value;
    std::map<std::string,::fcllite::PSet> _data_pset;

  };

}

#endif
/** @} */ // end of doxygen group 

