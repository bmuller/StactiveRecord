#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  void CUDPropertyRegister::register_new(string key, coltype ct) {
    newprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_change(string key, coltype ct) {
    changedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_delete(string key, coltype ct) {
    deletedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::get_new(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < newprop.size(); i++)
      v.push_back(newprop[ct][i]);
  };

  void CUDPropertyRegister::get_changed(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < changedprop.size(); i++)
      v.push_back(changedprop[ct][i]);
  };

  void CUDPropertyRegister::get_deleted(SarVector<string>& v, coltype ct) {
    for(unsigned int i=0; i < deletedprop.size(); i++)
      v.push_back(deletedprop[ct][i]);
  };

};
