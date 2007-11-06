namespace stactiverecord {
  using namespace std;
  
  class Sar_Exception : public exception {
  public:
    string _w;
    explicit Sar_Exception(const string &s) : _w(s) {};
    virtual ~Sar_Exception() throw() {};
    virtual const char * what() const throw() { return _w.c_str(); };
  };

  class Sar_DBException : public Sar_Exception {
  public:
    Sar_DBException(string s) : Sar_Exception(s) {};
  };  

  class Sar_NoSuchObjectException : public Sar_Exception {
  public:
    Sar_NoSuchObjectException(string s) : Sar_Exception(s) {};
  };  

  class Sar_NoSuchPropertyException : public Sar_Exception {
  public:
    Sar_NoSuchPropertyException(string s) : Sar_Exception(s) {};
  };  

  class Sar_InvalidConfigurationException : public Sar_Exception {
  public:
    Sar_InvalidConfigurationException(string s) : Sar_Exception(s) {};
  };  
};
