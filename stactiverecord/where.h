namespace stactiverecord {
  using namespace std;

  class Where {
  public:
    int ivalue, ivaluetwo;
    string svalue;
    wheretype type;
    coltype ct;
    Where(int _ivalue, wheretype _wt) : 
      ivalue(_ivalue), type(_wt), ct(INTEGER) {};
    Where(int _ivalue, int _valuetwo, wheretype _wt) :
      ivalue(_ivalue), ivaluetwo(_valuetwo), type(_wt), ct(INTEGER) {};
    Where(string _svalue, wheretype _wt) :
      svalue(_svalue), type(_wt), ct(STRING) {};
  };

  Where * startswith(string value);
  Where * endswith(string value);
  Where * contains(string value);
  Where * lessthan(int value);
  Where * greaterthan(int value);
  Where * between(int value, int valuetwo);
};
