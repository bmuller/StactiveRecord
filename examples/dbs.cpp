#include "stactiverecord/stactive_record.h"
using namespace stactiverecord;
using namespace std;

/** 
    Here are some example database connections strings:
    
    // open an sqlite db in memory
    Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");

    // open PostgreSQL db with username and password on localhost, and make each table have a prefix
    Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("postgres://user:password@localhost/myonlydatabase", "stactive_record_");

    // open an sqlite db in the file /tmp/db
    Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite:///tmp/db");
**/

// here are some examples of executing SQL directly
Sar_Dbi * Sar_Dbi::dbi;
int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./simple <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(string(argv[1]));
  Sar_Dbi::dbi->execute("CREATE TABLE example (id INT, name VARCHAR(255))");
  Sar_Dbi::dbi->execute("INSERT INTO example (id, name) VALUES(0, \"Richard Stallman\")");

  // KVT's are Key/Value/Type objects - use a vector of them to tell the db which columns you want
  // to select 
  SarVector<KVT> cols;
  cols << KVT("name", STRING);
  // select name from example where id = 0
  SarVector<Row> rows = Sar_Dbi::dbi->select("example", cols, Q("id", 0));
  // get the first string column in the first row returned, and put it into "name"
  string name;
  rows[0].get_string(0, name);
  
  cout << "Next time you see " << name << " say hi to him for me\n";

  delete Sar_Dbi::dbi;
  return 0;
};
