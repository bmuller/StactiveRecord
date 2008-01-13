#include  "../stactiverecord/stactive_record.h"

using namespace stactiverecord;
using namespace std;

Sar_Dbi * Sar_Dbi::dbi;

void assert(bool v, string msg) {
  if(v) return;
  throw Sar_AssertionFailedException("Failed when testing " + msg);
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./db_test <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));
  Sar_Dbi *db = Sar_Dbi::dbi;
  string classname = "testclass";
  string related_classname = "testclass_related";

  // recreate tables;
  db->initialize_tables(classname);
  
  // test id creation
  debug("testing id creation");
  assert(db->next_id(classname) == 0, "next_id incrementing");
  assert(db->next_id(classname) == 1, "next_id incrementing");
  assert(db->next_id(classname) == 2, "next_id incrementing");
  assert(db->current_id(classname) == 2, "getting current id");
  
  int id = 2;
  SarMap<string> svalues;
  SarMap<int> ivalues;
  SarMap<DateTime> dtvalues;
  SarMap<string> sresults;
  SarMap<int> iresults;
  SarMap<DateTime> dtresults;
  SarVector<int> ovalues;
  SarVector<int> oresults;
  SarMap< SarVector<int> > all_relationships;

  // test inserts
  debug("testing inserts");
  svalues["foo"] = "bar";
  svalues["baz"] = "bang";
  ivalues["bar"] = 55;
  ivalues["zoop"] = 1234;
  dtvalues["shebang"] = DateTime(1,1,2001);

  db->set(id, classname, svalues, true);
  db->set(id, classname, ivalues, true);
  db->set(id, classname, dtvalues, true);

  db->get(id, classname, sresults);
  db->get(id, classname, iresults);
  db->get(id, classname, dtresults);
  assert(sresults == svalues, "storing/retrieving string values");
  assert(iresults == ivalues, "storing/retrieving int values");
  assert(dtresults == dtvalues, "storing/retrieving datetime values");

  // now test updates
  debug("testing updates");
  svalues["foo"] = "oof";
  svalues["baz"] = "foo";
  ivalues["bar"] = 998;
  ivalues["zoop"] = 5364;
  dtvalues["shebang"] = DateTime(2,2,2002);

  db->set(id, classname, svalues, false);
  db->set(id, classname, ivalues, false);
  db->set(id, classname, dtvalues, false);

  sresults.clear();
  iresults.clear();
  dtresults.clear();
  db->get(id, classname, sresults);
  db->get(id, classname, iresults);
  db->get(id, classname, dtresults);
  assert(sresults == svalues, "updating/retrieving string values");
  assert(iresults == ivalues, "updating/retrieving int values");
  assert(dtresults == dtvalues, "updating/retrieving DateTime values");

  // now test record relations
  debug("testing record relations");
  ovalues << 4;
  ovalues << 5;
  ovalues << 6;
  db->set(id, classname, ovalues, related_classname);
  db->get(id, classname, related_classname, oresults);
  assert(oresults == ovalues, "storing/retrieving object relationship");
  db->get(id, classname, all_relationships);
  assert(all_relationships[related_classname] == ovalues, "storing/retrieving all object relationships");

  // now test backwards finding of relation
  debug("test backwards finding of relation");
  oresults.clear();
  db->get(4, related_classname, classname, oresults);
  assert((oresults.size() == 1 && oresults[0] == id), "retrieving object relationships backwards");

  // test deleting values
  debug("testing deleting values");
  svalues.remove("foo");
  sresults.clear();
  SarVector<string> toremove;
  toremove << "foo";
  db->del(id, classname, toremove, STRING);
  db->get(id, classname, sresults);
  assert(sresults == svalues, "deleting string value");

  ivalues.remove("zoop");
  iresults.clear();
  toremove.clear();
  toremove << "zoop";
  db->del(id, classname, toremove, INTEGER);
  db->get(id, classname, iresults);
  assert(iresults == ivalues, "deleting int value");

  // test deleting association
  debug("testing deleting association");
  SarVector<int> itoremove;
  itoremove << 4;
  ovalues.remove(4);
  oresults.clear();
  db->del(id, classname, itoremove, related_classname);
  db->get(id, classname, related_classname, oresults);
  assert(oresults == ovalues, "deleting object relationship");

  // test searching
  debug("testing searching");
  // get all
  oresults.clear();
  db->get(classname, oresults);
  assert((oresults.size() == 1 && oresults[0] == id), "getting all objects of a type");
  // get all with string value
  oresults.clear();
  Where *where = startswith("fo");
  db->get_where(classname, "baz", where, oresults);
  assert((oresults.size() == 1 && oresults[0] == id), "getting all objects with matching string prop value");
  // get all related
  oresults.clear();
  where = new Where(classname, 2, OBJECTRELATION);
  db->get_where(related_classname, "unused: ignore", where, oresults);
  assert(oresults.size() == 2, "searching for all objects with a certain other object relation");
  // get all with int value
  oresults.clear();
  where = greaterthan(990);
  db->get_where(classname, "bar", where, oresults);
  delete where;
  assert((oresults.size() == 1 && oresults[0] == id), "getting all objects with matching int prop value");

  // test delete record
  debug("testing deleting record");
  db->delete_record(id, classname);
  oresults.clear();
  db->get(classname, oresults);
  assert((oresults.size() == 0), "deleting one object");

  // test delete all records
  debug("testing deleting all records");
  db->set(id, classname, svalues, true);
  oresults.clear();
  db->delete_records(classname);
  db->get(classname, oresults);
  assert((oresults.size() == 0), "deleting all objects of a type");

  delete Sar_Dbi::dbi;
  std::cout << "No errors were found.\n";
  return 0;
}
