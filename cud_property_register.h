namespace stactiverecord {
  using namespace std;

  class CUDPropertyRegister {
  private:
    map<int, SarVector<string> > newprop;
    map<int, SarVector<string> > changedprop;
    map<int, SarVector<string> > deletedprop;
  protected:
    void register_new(string key, coltype ct);
    void register_change(string key, coltype ct);
    void register_delete(string key, coltype ct);

    bool is_registered_new(string key, coltype ct);
    bool is_registered_changed(string key, coltype ct);
    bool is_registered_deleted(string key, coltype ct);

    void unregister_new(string key, coltype ct);
    void unregister_change(string key, coltype ct);
    void unregister_delete(string key, coltype ct);

    void get_new(SarVector<string>& v, coltype ct);
    void get_changed(SarVector<string>& v, coltype ct);
    void get_deleted(SarVector<string>& v, coltype ct);

    void clear_registers();
  };

};
