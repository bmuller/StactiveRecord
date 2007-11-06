#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  SarVector<int> ObjGroup::get_ids() {
    SarVector<int> sv;
    for(unsigned int i=0; i < this->size(); i++)
      sv.push_back(this->at(i).id);
    return sv;
  };

  bool ObjGroup::has_id(int id) {
    return get_ids().includes(id);
  };
};

