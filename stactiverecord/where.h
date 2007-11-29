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

  class Where {
  public:
    int ivalue, ivaluetwo;
    std::string svalue;
    std::vector<int> ivalues;
    wheretype type;
    coltype ct;
    bool isnot;
    Where(int _ivalue, wheretype _wt, bool _isnot=false) : 
      ivalue(_ivalue), type(_wt), ct(INTEGER), isnot(_isnot) {};
    Where(std::vector<int> _ivalues, wheretype _wt, bool _isnot=false) : 
      ivalues(_ivalues), type(_wt), ct(INTEGER), isnot(_isnot) {};
    Where(int _ivalue, int _valuetwo, wheretype _wt, bool _isnot=false) :
      ivalue(_ivalue), ivaluetwo(_valuetwo), type(_wt), ct(INTEGER), isnot(_isnot) {};
    Where(std::string _svalue, wheretype _wt, bool _isnot=false) :
      svalue(_svalue), type(_wt), ct(STRING), isnot(_isnot) {};
    // in the case of a record search - i.e., Q(hasobject(someobject))
    Where(std::string _svalue, int _ivalue, wheretype _wt, bool _isnot=false) :
      svalue(_svalue), ivalue(_ivalue), type(_wt), ct(RECORD), isnot(_isnot) {};
  };

  Where * startswith(std::string value);
  Where * endswith(std::string value);
  Where * contains(std::string value);
  Where * lessthan(int value);
  Where * greaterthan(int value);
  Where * between(int value, int valuetwo);
  Where * equals(int value);
  Where * equals(std::string value);
  Where * in(std::vector<int> values);

  // negated values
  Where * nstartswith(std::string value);
  Where * nendswith(std::string value);
  Where * ncontains(std::string value);
  Where * nlessthan(int value);
  Where * ngreaterthan(int value);
  Where * nbetween(int value, int valuetwo);
  Where * nequals(int value);
  Where * nequals(std::string value);
  Where * nin(std::vector<int> values);
};
