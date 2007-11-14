/*
Copyright (C) 2007 Butterfat, LLC (http://butterfat.net)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Created by bmuller <bmuller@butterfat.net>
*/

namespace stactiverecord {
  
  class Sar_Exception : public std::exception {
  public:
    std::string _w;
    explicit Sar_Exception(const std::string &s) : _w(s) {};
    virtual ~Sar_Exception() throw() {};
    virtual const char * what() const throw() { return _w.c_str(); };
  };

  /** \class Sar_DBException
   * \brief Database exception - can be thrown by any Sar_Dbi implementors.
   */
  class Sar_DBException : public Sar_Exception {
  public:
    Sar_DBException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_NoSuchObjectException
   * \brief Exception thrown if object is created with a non existant id
   */
  class Sar_NoSuchObjectException : public Sar_Exception {
  public:
    Sar_NoSuchObjectException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_NoSuchPropertyException
   * \brief Exception thrown if get is called for a property that doesn't exist.
   */
  class Sar_NoSuchPropertyException : public Sar_Exception {
  public:
    Sar_NoSuchPropertyException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_InvalidConfigurationException
   * \brief Exception thrown if invalid Sar_Dbi configuration is given.
   */
  class Sar_InvalidConfigurationException : public Sar_Exception {
  public:
    Sar_InvalidConfigurationException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_RecordNotFoundException
   * \brief Exception thrown if a record can not be found.
   */
  class Sar_RecordNotFoundException : public Sar_Exception {
  public:
    Sar_RecordNotFoundException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_AssertionFailedException
   * \brief Exception thrown in testing if an assertion fails.
   */
  class Sar_AssertionFailedException : public Sar_Exception {
  public:
    Sar_AssertionFailedException(std::string s) : Sar_Exception(s) {};
  };  

  /** \class Sar_InvalidClassnameException
   * \brief Exception thrown if an invalid classname is used.
   */
  class Sar_InvalidClassnameException : public Sar_Exception {
  public:
    Sar_InvalidClassnameException(std::string s) : Sar_Exception(s) {};
  };  
};
