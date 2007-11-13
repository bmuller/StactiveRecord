namespace stactiverecord {
  using namespace std;

  class Where {
  public:
    int ivalue, ivaluetwo;
    string svalue;
    wheretype type;
    coltype ct;
    bool isnot;
    Where(int _ivalue, wheretype _wt, bool _isnot=false) : 
      ivalue(_ivalue), type(_wt), ct(INTEGER), isnot(_isnot) {};
    Where(int _ivalue, int _valuetwo, wheretype _wt, bool _isnot=false) :
      ivalue(_ivalue), ivaluetwo(_valuetwo), type(_wt), ct(INTEGER), isnot(_isnot) {};
    Where(string _svalue, wheretype _wt, bool _isnot=false) :
      svalue(_svalue), type(_wt), ct(STRING), isnot(_isnot) {};
  };

  Where * startswith(string value);
  Where * endswith(string value);
  Where * contains(string value);
  Where * lessthan(int value);
  Where * greaterthan(int value);
  Where * between(int value, int valuetwo);
  Where * equals(int value);
  Where * equals(string value);

  // negated values
  Where * nstartswith(string value);
  Where * nendswith(string value);
  Where * ncontains(string value);
  Where * nlessthan(int value);
  Where * ngreaterthan(int value);
  Where * nbetween(int value, int valuetwo);
  Where * nequals(int value);
  Where * nequals(string value);
};
