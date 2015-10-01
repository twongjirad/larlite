#ifndef SELECTIONTOOL_ERTOOL_ISTREAMBASE_CXX
#define SELECTIONTOOL_ERTOOL_ISTREAMBASE_CXX

#include "IStreamBase.h"

namespace ertool {
  namespace io {
    IStreamBase::IStreamBase(const std::string& name)
      : MessageUtil()
      , _n_entries ( 0    )
      , _name      ( name )
    {}
    
    const std::string& IStreamBase::Name() const
    { return _name; }

    size_t IStreamBase::NumEntries() const
    { return _n_entries; }
    
  }
}
#endif
