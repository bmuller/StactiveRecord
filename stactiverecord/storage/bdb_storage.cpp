#include "../stactive_record.h"

namespace stactiverecord {
  using namespace std;

  BDBStorage::BDBStorage(string location) : db(NULL, 0) {
    debug("Attempting to open BDB DB at " + location);
    is_closed = false;
    u_int32_t oFlags = DB_CREATE; // Open flags;
    try {
      db_.open(NULL, location.c_str(), "id_maximums", DB_BTREE, oFlags, 0);
      db_.open(NULL, location.c_str(), "relationships", DB_BTREE, oFlags, 0);
      //db_.set_errpfx("stactiverecord BDB error: ");
      db_.set_error_stream(&cerr);
    } catch(DbException &e) {
      throw Sar_DBException("Database open failed for " + location);
    } 
    debug("BDB database opened successfully");
  };

  void BDBStorage::close() {
    if(is_closed)
      return;
    is_closed = true;
    try {
      db_.close(0);
    } catch(DbException &e) {
      throw Sar_DBException("Database close failed");
    }
  };
  /*
  int BDBStorage::next_id(string classname) {
    sqlite3_stmt *pSelect;
    string query = "SELECT id FROM id_maximums WHERE classname = \"" + classname + "\"";
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    if(rc != SQLITE_ROW){
      debug("could not find max id int for " + classname + " - starting at 0");
      rc = sqlite3_finalize(pSelect);
      query = "INSERT INTO id_maximums (id,classname) VALUES(0, \"" + classname + "\")";
      execute(query);
      return 0;
    }
    int maxid = sqlite3_column_int(pSelect, 0) + 1;
    rc = sqlite3_finalize(pSelect);
    string maxid_s;
    int_to_string(maxid, maxid_s);
    execute("UPDATE id_maximums SET id = " + maxid_s + " WHERE classname = \"" + classname + "\"");
    return maxid;
  };

  int BDBStorage::current_id(string classname) {
    sqlite3_stmt *pSelect;
    string query = "SELECT id FROM id_maximums WHERE classname = \"" + classname + "\"";
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    if(rc != SQLITE_ROW)
      return -1;
    int id = sqlite3_column_int(pSelect, 0);
    rc = sqlite3_finalize(pSelect);
    return id;
  };
 
  void BDBStorage::initialize_tables(string classname) {
    char *errMsg;
    string tablename;
    int rc;

    // make table for string values
    tablename = classname + "_s";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      string query = "CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), "
	"value VARCHAR(" + VALUE_MAX_SIZE_S + "))";
      execute(query);
      initialized_tables.push_back(tablename);
    }

    // make table for string values
    tablename = classname + "_i";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      execute("CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), value INT)");
      initialized_tables.push_back(tablename);
    }

    debug("Finished initializing tables for class " + classname);
  };

  void BDBStorage::get(int id, string classname, SarMap<string>& values) {
    sqlite3_stmt *pSelect;
    string tablename = classname + "_s";
    string id_s;
    int_to_string(id, id_s);
    string query = "SELECT keyname,value FROM " + tablename + " WHERE id = " + id_s;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    char c_key[255];
    char c_value[VALUE_MAX_SIZE + 1];
    while(rc == SQLITE_ROW){
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      snprintf(c_value, VALUE_MAX_SIZE, "%s", sqlite3_column_text(pSelect, 1));
      values[string(c_key)] = string(c_value);
      rc = sqlite3_step(pSelect);
    }    
    rc = sqlite3_finalize(pSelect);
  };

  void BDBStorage::get(int id, string classname, SarMap<int>& values) {
    sqlite3_stmt *pSelect;
    string tablename = classname + "_i";
    string id_s;
    int_to_string(id, id_s);
    string query = "SELECT keyname,value FROM " + tablename + " WHERE id = " + id_s;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      char c_key[255];
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      int value = sqlite3_column_int(pSelect, 1);
      values[string(c_key)] = value;
      rc = sqlite3_step(pSelect);
    }    
    rc = sqlite3_finalize(pSelect);
  };

  void BDBStorage::set(int id, string classname, SarMap<string> values, bool insert) {
    string query;
    string tablename = classname + "_s";
    string id_s;
    int_to_string(id, id_s);
    if(insert) {
      for(map<string,string>::iterator i=values.begin(); i!=values.end(); ++i) {
	query = "INSERT INTO " + tablename + " (id,keyname,value) VALUES (" + id_s + ",\"" + string((*i).first) + "\",\"" + string((*i).second) + "\")";
	execute(query);
      }
    } else {
      for(map<string,string>::iterator i=values.begin(); i!=values.end(); ++i) {
	query = "UPDATE " + tablename + " SET value=\"" + string((*i).second) + "\" WHERE id=" + id_s \
	  + " AND keyname=\"" + string((*i).first) + "\"";
	execute(query);
      }
    }
  };

  void BDBStorage::set(int id, string classname, SarMap<int> values, bool insert) {
    string query;
    string tablename = classname + "_i";
    string id_s;
    string value_s;
    int_to_string(id, id_s);
    if(insert) {
      for(map<string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
	int_to_string((*i).second, value_s);
	query = "INSERT INTO " + tablename + " (id,keyname,value) VALUES (" + id_s + ",\"" + string((*i).first) + "\"," + value_s + ")";
	execute(query);
      }
    } else {
      for(map<string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
	int_to_string((*i).second, value_s);
	query = "UPDATE " + tablename + " SET  value=" + value_s + " WHERE id=" + id_s \
	  + " AND keyname=\"" + string((*i).first) + "\"";
	execute(query);
      }
    }
  };

  void BDBStorage::del(int id, string classname, SarVector<string> keys, coltype ct) {
    string tablename = (ct == STRING) ? classname+"_s" : classname+"_i";
    string id_s;
    int_to_string(id, id_s);
    for(unsigned int i=0; i < keys.size(); i++)
      execute("DELETE FROM " + tablename + " WHERE id = " + id_s + " AND keyname = \"" + keys[i] + "\"");
  };

  void BDBStorage::delete_record(int id, string classname) {
    char *errMsg;
    int rc;
    string id_s, query, tablename;
    int_to_string(id, id_s);

    // delete all string values
    tablename = classname + "_s";
    execute("DELETE FROM " + tablename + " WHERE id = " + id_s);

    // delete all int values
    tablename = classname + "_i";
    execute("DELETE FROM " + tablename + " WHERE id = " + id_s);

    execute("DELETE FROM relationships WHERE class_one = \"" + classname + "\" AND class_one_id=" + id_s);
    execute("DELETE FROM relationships WHERE class_two = \"" + classname + "\" AND class_two_id=" + id_s);
  };

  void BDBStorage::delete_records(string classname) {
    char *errMsg;
    int rc;
    string query, tablename;

    // delete string values table
    tablename = classname + "_s";
    execute("DELETE FROM " + tablename);

    // delete int values table
    tablename = classname + "_i";
    execute("DELETE FROM " + tablename);

    // delete entries from relationships
    execute("DELETE FROM relationships WHERE class_one = \"" + classname + "\" OR class_two = \"" + classname + "\"");

    // delete max id
    execute("DELETE FROM id_maximums WHERE classname = \"" + classname + "\"");
  };

  void BDBStorage::set(int id, string classname, SarVector<int> related, string related_classname) {
    string s_id, related_id;
    int_to_string(id, s_id);
    debug("Adding related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    for(SarVector<int>::size_type i=0; i<related.size(); i++) {
      int_to_string(related[i], related_id);
      if(swap)
	execute("INSERT INTO relationships (class_one, class_one_id, class_two, class_two_id) VALUES(\"" + \
		related_classname + "\", " + related_id + ", \"" + classname + "\", " + s_id + ")");
      else
	execute("INSERT INTO relationships (class_two, class_two_id, class_one, class_one_id) VALUES(\"" + \
		related_classname + "\", " + related_id + ", \"" + classname + "\", " + s_id + ")");
    }
  };

  void BDBStorage::get(int id, string classname, string related_classname, SarVector<int>& related) {
    sqlite3_stmt *pSelect;
    string s_id, query;
    int_to_string(id, s_id);
    debug("Getting related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    if(swap)
      query = "SELECT class_one_id FROM relationships WHERE class_one = \"" + related_classname + "\" AND class_two_id = " 
	+ s_id + " AND class_two = \"" + classname + "\"";
    else
      query = "SELECT class_two_id FROM relationships WHERE class_two = \"" + related_classname + "\" AND class_one_id = " 
	+ s_id + " AND class_one = \"" + classname + "\"";

    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      related << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
  };
  
  void BDBStorage::get(int id, string classname, SarMap< SarVector<int> >& sm) {
    sqlite3_stmt *pSelect;
    string s_id, query, key;
    int_to_string(id, s_id);
    debug("Getting all related objects to a " + classname);

    query = "SELECT class_one, class_one_id FROM relationships WHERE class_two=\"" + classname + "\" AND class_two_id=" + s_id;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      char c_key[255];
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      key = string(c_key);
      if(!sm.has_key(key))
	sm[key] = SarVector<int>();
      sm[key] << sqlite3_column_int(pSelect, 1);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);    

    query = "SELECT class_two, class_two_id FROM relationships WHERE class_one=\"" + classname + "\" AND class_one_id=" + s_id;
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      char c_key[255];
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      key = string(c_key);
      if(!sm.has_key(key))
	sm[key] = SarVector<int>();
      sm[key] << sqlite3_column_int(pSelect, 1);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);    
  };

  void BDBStorage::del(int id, string classname, SarVector<int> related, string related_classname) {
    if(related.size() == 0)
      return;
    string s_id, related_id;
    int_to_string(id, s_id);
    debug("Deleting some related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;

    string idlist = "(";
    for(SarVector<int>::size_type i=0; i<related.size(); i++) {
      int_to_string(related[i], related_id);
      idlist += related_id;
      if(i!=(related.size() - 1))
	idlist += ",";
    }
    idlist += ")";

    if(swap)
      execute("DELETE FROM relationships WHERE class_two=\"" + classname + "\" AND class_two_id=" + s_id \
	      + " AND class_one=\"" + related_classname + "\" and class_one_id IN " + idlist);
    else
      execute("DELETE FROM relationships WHERE class_one=\"" + classname + "\" AND class_one_id=" + s_id \
	      + " AND class_two=\"" + related_classname + "\" and class_two_id IN " + idlist);
  };
  
  // some searching stuff
  void BDBStorage::get(string classname, SarVector<int>& results) {
    sqlite3_stmt *pSelect;
    SarVector<int> others;
    string tablename = classname + "_s";
    debug("Getting all objects of type " + classname);
    string query = "SELECT DISTINCT id FROM " + tablename;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      results << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
   
    tablename = classname + "_i";
    query = "SELECT DISTINCT id FROM " + tablename;
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();

    query = "SELECT DISTINCT class_one_id FROM relationships WHERE class_one = \"" + classname + "\"";
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();

    query = "SELECT DISTINCT class_two_id FROM relationships WHERE class_two = \"" + classname + "\"";
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();
  };

  void BDBStorage::get(string classname, string key, string value, SarVector<int>& results) {
    sqlite3_stmt *pSelect;
    string tablename = classname + "_s";
    debug("Getting all objects of type " + classname + " with key " + key + " and value " + value);
    string query = "SELECT id FROM " + tablename + " WHERE keyname=\"" + key + "\" AND value=\"" + value +"\"";
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      results << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
  };

  void BDBStorage::get(string classname, string key, int value, SarVector<int>& results) {
    sqlite3_stmt *pSelect;
    string tablename = classname + "_i";
    string svalue;
    int_to_string(value, svalue);
    debug("Getting all objects of type " + classname + " with key " + key + " and value " + svalue);
    string query = "SELECT id FROM " + tablename + " WHERE keyname=\"" + key + "\" AND value=" + svalue;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      results << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
  };
*/
};
