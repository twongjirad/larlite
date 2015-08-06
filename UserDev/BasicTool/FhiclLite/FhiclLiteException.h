/**
 * \file FhiclLiteException.h
 *
 * \ingroup FhiclLite
 * 
 * \brief Class def header for exception classes in FhiclLite package
 *
 * @author kazuhiro
 */

/** \addtogroup FhiclLite

    @{*/
#ifndef BASICTOOL_FHICLLITE_EXCEPTION_H
#define BASICTOOL_FHICLLITE_EXCEPTION_H

#include <iostream>
#include <exception>

namespace fcllite {
  /**
     \class FhiclLiteException
     Generic (base) exception class
  */
  class FhiclLiteException : public std::exception{

  public:

    FhiclLiteException(std::string msg="") : std::exception(), _msg(msg)
    {}

    virtual ~FhiclLiteException() throw(){};
    virtual const char* what() const throw() 
    {return _msg.c_str(); }

  private:

    std::string _msg;
  };

}
#endif
/** @} */ // end of doxygen group 

