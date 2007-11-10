#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  void CUDPropertyRegister::register_new(string key, coltype ct) {
    if(!newprop[ct].includes(key))
      newprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_change(string key, coltype ct) {
    if(!changedprop[ct].includes(key))
      changedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_delete(string key, coltype ct) {
    if(!deletedprop[ct].includes(key))
      deletedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::get_new(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < newprop[ct].size(); i++)
      v.push_back(newprop[ct][i]);
  };

  void CUDPropertyRegister::get_changed(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < changedprop[ct].size(); i++)
      v.push_back(changedprop[ct][i]);
  };

  void CUDPropertyRegister::get_deleted(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < deletedprop[ct].size(); i++)
      v.push_back(deletedprop[ct][i]);
  };

  bool CUDPropertyRegister::is_registered_new(string key, coltype ct) {
    return newprop[ct].includes(key);
  };

  bool CUDPropertyRegister::is_registered_changed(string key, coltype ct) {
    return changedprop[ct].includes(key);
  };

  bool CUDPropertyRegister::is_registered_deleted(string key, coltype ct) {
    return deletedprop[ct].includes(key);
  };

  void CUDPropertyRegister::unregister_new(string key, coltype ct) {
    newprop[ct].remove(key);
  };

  void CUDPropertyRegister::unregister_change(string key, coltype ct) {
    changedprop[ct].remove(key);
  };

  void CUDPropertyRegister::unregister_delete(string key, coltype ct) {
    deletedprop[ct].remove(key);
  };

  void CUDPropertyRegister::clear_registers() {
    map<int, SarVector<string> >::iterator i;
    for(i=newprop.begin(); i!=newprop.end(); ++i) 
      (*i).second.clear();
    for(i=changedprop.begin(); i!=changedprop.end(); ++i) 
      (*i).second.clear();
    for(i=deletedprop.begin(); i!=deletedprop.end(); ++i) 
      (*i).second.clear();
  };

  void CUDPropertyRegister::dump_registers() {
    cout << "New props:\n";
    cout << "Strings: ";
    newprop[STRING].dump();
    cout << "Integers: ";
    newprop[INTEGER].dump();

    cout << "\nChanged props:\n";
    cout << "Strings: ";
    changedprop[STRING].dump();
    cout << "Integers: ";
    changedprop[INTEGER].dump();

    cout << "\nDeleted props:\n";
    cout << "Strings: ";
    deletedprop[STRING].dump();
    cout << "Integers: ";
    deletedprop[INTEGER].dump();
  };

};
