namespace stactiverecord {
  using namespace std;

  class CUDPropertyRegister {
  private:
    map<int, vector<string> > newprop;
    map<int, vector<string> > changedprop;
    map<int, vector<string> > deletedprop;
  protected:
    void register_new(string key, coltype ct);
    void register_change(string key, coltype ct);
    void register_delete(string key, coltype ct);
    void get_new(SarVector<string>& v, coltype ct);
    void get_changed(SarVector<string>& v, coltype ct);
    void get_deleted(SarVector<string>& v, coltype ct);
  };

};
