#include  "../stactive_record.h"

using namespace stactiverecord;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://./sqlitedb.db");

void assert(bool v, string msg) {
  if(v) return;
  throw Sar_AssertionFailedException("Failed when testing " + msg);
}

class TestClass : public Record {
public:
  TestClass() : Record("testclass") {};
  TestClass(int id) : Record("testclass", id) {};
};


int main() {
  debug("Testing sqlite..."); 
  Sar_Dbi *db = Sar_Dbi::dbi;
  string classname = "testclass";

  // delete all preexisting
  Record::delete_all<TestClass>();

  // recreate tables;
  db->initialize_tables(classname);
  
  // test id creation
  assert(db->next_id(classname) == 0, "next_id incrementing");
  assert(db->next_id(classname) == 1, "next_id incrementing");
  assert(db->next_id(classname) == 2, "next_id incrementing");
  assert(db->current_id(classname) == 2, "getting current id");
  
  SarMap<string> svalues;
  SarMap<int> ivalues;
  
  delete Sar_Dbi::dbi;
  return 0;
}
