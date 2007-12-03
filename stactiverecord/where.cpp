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

#include "stactive_record.h"

namespace stactiverecord {

  Where * startswith(std::string value) {
    return new Where(value, STARTSWITH);
  };

  Where * endswith(std::string value) {
    return new Where(value, ENDSWITH);
  };

  Where * contains(std::string value) {
    return new Where(value, CONTAINS);
  };
  
  Where * greaterthan(int value) {
    return new Where(value, GREATERTHAN);
  };

  Where * lessthan(int value) {
    return new Where(value, LESSTHAN);
  };

  Where * between(int value, int valuetwo) {
    return new Where(value, valuetwo, BETWEEN);
  };

  Where * equals(int value) {
    return new Where(value, EQUALS);
  };

  Where * equals(std::string value) {
    return new Where(value, EQUALS);
  };

  Where * equals(bool value) {
    if(value)
      return new Where(0, EQUALS, true);
    return new Where(0, EQUALS);
  };

  Where * hasobject(Record r) {
    return new Where(r.classname, r.id, OBJECTRELATION);
  };

  Where * in(std::vector<int> values) {
    return new Where(values, IN);
  };

  Where * isnull() {
    return new Where(ISNULL);
  };

  // negated values 

  Where * nstartswith(std::string value) {
    return new Where(value, STARTSWITH, true);
  };

  Where * nendswith(std::string value) {
    return new Where(value, ENDSWITH, true);
  };

  Where * ncontains(std::string value) {
    return new Where(value, CONTAINS, true);
  };
  
  Where * ngreaterthan(int value) {
    return new Where(value, GREATERTHAN, true);
  };

  Where * nlessthan(int value) {
    return new Where(value, LESSTHAN, true);
  };

  Where * nbetween(int value, int valuetwo) {
    return new Where(value, valuetwo, BETWEEN, true);
  };

  Where * nequals(int value) {
    return new Where(value, EQUALS, true);
  };

  Where * nequals(std::string value) {
    return new Where(value, EQUALS, true);
  };

  Where * nequals(bool value) {
    return equals(!value);
  };

  Where * nin(std::vector<int> values) {
    return new Where(values, IN, false);
  };

  Where * nisnull() {
    return new Where(ISNULL, false);
  };

};
