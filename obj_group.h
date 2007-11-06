namespace stactiverecord {
  class ObjGroup : public SarVector<Record> {
  public:
    ObjGroup() : SarVector<Record>() {};
    ObjGroup(SarVector<Record>& sr) : SarVector<Record>() {
      for(unsigned int i=0; i<sr.size(); i++)
	push_back(sr[i]);
    };
    SarVector<int> get_ids();
    bool has_id(int id);
  };
};
