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

  Q& Q::operator||(Q other) { 
    ored << other; 
    return (*this);
  };

  Q& Q::operator&&(Q other) { 
    anded << other; 
    return (*this); 
  };     

  SarVector<int> Q::test(std::string classname) {
    SarVector<int> results;
    Sar_Dbi::dbi->get_where(classname, key, where, results);

    // if none matching were found, see if any of our ors match
    for(unsigned int i=0; i<ored.size(); i++) {
      results.unionize(ored[i].test(classname));
    }

    // if we have no results by now, just return
    if(results.size() == 0)
      return results;	  

    SarVector<int> anded_results;
    for(unsigned int i=0; i<anded.size(); i++) {
      anded_results = anded[i].test(classname);
      if(anded_results.size() == 0)
	return anded_results;
    }

    // if there are anded values, only return the ones that
    // intersect the original results - otherwise, return first
    // results
    if(anded.size() > 0)
      return results.intersects(anded_results);
    return results;
  };

  void Q::to_string(std::string& query) {
    std::string swhere;
    Sar_Dbi::dbi->where_to_string(where, swhere);
    query = "(" + key + " " + swhere;
    
    std::string sored;
    for(unsigned int i=0; i<ored.size(); i++) {
      ored[i].to_string(sored);
      query += " OR " + sored; 
    }
    
    std::string sanded;
    for(unsigned int i=0; i<anded.size(); i++) {
      anded[i].to_string(sanded);
      query += " AND " + sanded; 
    }
    query += ")";
  };

  void Q::dump() {
    std::cout << key << ": " << value << "\n";
    if(anded.size() > 0) {
      std::cout << "\tanded:\n";
      for(unsigned int i=0; i<anded.size(); i++) {
	std::cout << "\t";
	anded[i].dump();
      }
    }
    if(ored.size() > 0) {
      std::cout << "\tored:\n";
      for(unsigned int i=0; i<ored.size(); i++) {
	std::cout << "\t";
	ored[i].dump();
      }
    }
  };

  void Q::free() {
    for(unsigned int i=0; i<ored.size(); i++)
      ored[i].free();
    for(unsigned int i=0; i<anded.size(); i++)
      anded[i].free();
    delete where;
  };
  
};
