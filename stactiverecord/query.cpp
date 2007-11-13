#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  Q& Q::operator||(Q other) { 
    ored << other; 
    return (*this);
  };

  Q& Q::operator&&(Q other) { 
    anded << other; 
    return (*this); 
  };     

  // using the db, find all records that match
  SarVector<int> Q::test(string classname, Sar_Dbi *db) {
    SarVector<int> results;
    db->get_where(classname, key, where, results);

    // if none matching were found, see if any of our ors match
    for(unsigned int i=0; i<ored.size(); i++) {
      results.unionize(ored[i].test(classname, db));
    }

    // if we have no results by now, just return
    if(results.size() == 0)
      return results;	  

    SarVector<int> anded_results;
    for(unsigned int i=0; i<anded.size(); i++) {
      anded_results = anded[i].test(classname, db);
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

  void Q::dump() {
    cout << key << ": " << value << "\n";
    if(anded.size() > 0) {
      cout << "\tanded:\n";
      for(unsigned int i=0; i<anded.size(); i++) {
	cout << "\t";
	anded[i].dump();
      }
    }
    if(ored.size() > 0) {
      cout << "\tored:\n";
      for(unsigned int i=0; i<ored.size(); i++) {
	cout << "\t";
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
