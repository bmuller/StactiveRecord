#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(std::string config) {
    std::vector<std::string> configparts = explode(config, "://");
    if(configparts.size() != 2)
      throw Sar_InvalidConfigurationException("Invalid database configuration string: " + config);

#ifdef HAVE_MYSQL 
    if(configparts[0] == "mysql")
      return new MySQLStorage(configparts[1]);
#endif

#ifdef HAVE_SQLITE3
    if(configparts[0] == "sqlite")
      return new SQLiteStorage(configparts[1]);
#endif    

#ifdef HAVE_POSTGRESQL
    //if(configparts[0] == "postgres")
    //  return new PostgresStorage(configparts[1]);
#endif    

    throw Sar_InvalidConfigurationException("DB type of \"" + configparts[0] + "\" not recognized.");
  };

  bool Sar_Dbi::table_is_initialized(std::string tablename) { 
    return initialized_tables.includes(tablename);
  };

  int Sar_Dbi::next_id(std::string classname) {
    SarVector<KVT> cols;
    cols << KVT("id", INTEGER);
    SarVector<Row> row = select("id_maximums", cols, Q("classname", classname));
    SarVector<KVT> values;
    if(row.size() == 0) {
      debug("could not find max id int for " + classname + " - starting at 0");
      values << KVT("id", 0);
      values << KVT("classname", classname);
      insert("id_maximums", values);
      return 0;
    }
    int maxid = row[0].get_int(0) + 1;
    values << KVT("id", maxid);
    update("id_maximums", values, Q("classname", classname));
    return maxid;
  };

  int Sar_Dbi::current_id(std::string classname) {
    SarVector<KVT> values;
    values << KVT("id", INTEGER);
    SarVector<Row> rows = select("id_maximums", values, Q("classname", classname));
    if(rows.size() == 0)
      return -1;
    return rows[0].get_int(0);
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap<std::string>& values) {
    std::string tablename = classname + "_s";
    SarVector<KVT> cols;
    cols << KVT("keyname", STRING);
    cols << KVT("value", STRING);
    SarVector<Row> rows = select(tablename, cols, Q("id", id));
    std::string key, value;
    for (unsigned int i = 0; i < rows.size(); i++) {
      rows[i].get_string(0, key);
      rows[i].get_string(1, value);
      values[key] = value;
    }
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap<int>& values) {
    std::string tablename = classname + "_i";
    SarVector<KVT> cols;
    cols << KVT("keyname", STRING);
    cols << KVT("value", INTEGER);
    SarVector<Row> rows = select(tablename, cols, Q("id", id));
    std::string key;
    int value;
    for (unsigned int i = 0; i < rows.size(); i++) {
      rows[i].get_string(0, key);
      value = rows[i].get_int(0);
      values[key] = value;
    }
  };

  void Sar_Dbi::set(int id, std::string classname, SarMap<std::string> values, bool isinsert) {
    std::string tablename = classname + "_s";
    for(std::map<std::string,std::string>::iterator i=values.begin(); i!=values.end(); ++i) {
      SarVector<KVT> values;
      values << KVT("value", std::string((*i).second));
      if(isinsert) {
        values << KVT("id", id);
	values << KVT("keyname", std::string((*i).first));
        insert(tablename, values);
      } else {
	update(tablename, values, Q("id", id) && Q("keyname", std::string((*i).first)));
      }
    }
  };

  void Sar_Dbi::set(int id, std::string classname, SarMap<int> values, bool isinsert) {
    std::string tablename = classname + "_i";
    for(std::map<std::string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
      SarVector<KVT> values;
      values << KVT("value", (*i).second);
      if(isinsert) {
        values << KVT("id", id);
	values << KVT("keyname", std::string((*i).first));
        insert(tablename, values);
      } else {
	update(tablename, values, Q("id", id) && Q("keyname", std::string((*i).first)));
      }
    }
  };

  void Sar_Dbi::del(int id, std::string classname, SarVector<std::string> keys, coltype ct) {
    std::string tablename = (ct == STRING) ? classname+"_s" : classname+"_i";
    for(unsigned int i=0; i < keys.size(); i++) 
      remove(tablename, Q("id", id) && Q("keyname", keys[i]));
  };
  
  void Sar_Dbi::delete_record(int id, std::string classname) {
    std::string tablename = classname + "_s";
    remove(tablename, Q("id", id));
    tablename = classname + "_i";
    remove(tablename, Q("id", id));

    tablename = "relationships";
    remove(tablename, Q("class_one", classname) && Q("class_one_id", id));
    remove(tablename, Q("class_two", classname) && Q("class_two_id ", id));
  };

  void Sar_Dbi::delete_records(std::string classname) {
    // delete string values table
    std::string tablename = classname + "_s";
    remove(tablename);

    // delete int values table
    tablename = classname + "_i";
    remove(tablename);

    std::string sclassname_where;
    tablename = "relationships";
    Where * classname_where = equals(classname);
    where_to_string(classname_where, sclassname_where);

    // delete entries from relationships
    remove(tablename, Q("class_one", classname) || Q("class_two", classname));

    // delete max id
    tablename = "id_maximums";
    remove(tablename, Q("classname", classname));
  };

  void Sar_Dbi::set(int id, std::string classname, SarVector<int> related, std::string related_classname) {
    std::string tablename = "relationships";
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    debug("Adding related " + related_classname + "s to a " + classname);
    for(SarVector<int>::size_type i=0; i<related.size(); i++) {
      SarVector<KVT> values;
      if(swap) {
        values << KVT("class_one", related_classname);
        values << KVT("class_one_id", related[i]);
        values << KVT("class_two", classname);
        values << KVT("class_two_id", id);
      } else {
        values << KVT("class_one", classname);
        values << KVT("class_one_id", id);
        values << KVT("class_two", related_classname);
        values << KVT("class_two_id", related[i]);
      }
      insert(tablename, values);
    }
  };

  void Sar_Dbi::get(int id, std::string classname, std::string related_classname, SarVector<int>& related) {
    std::string tablename = "relationships";
    debug("Getting related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    SarVector<KVT> cols;
    SarVector<Row> rows;

    if(swap) {
      cols << KVT("class_one_id", INTEGER);
      rows = select(tablename, cols, Q("class_one", related_classname) && Q("class_two_id", id) && Q("class_two", classname));
    } else {
      cols << KVT("class_two_id", INTEGER);
      rows = select(tablename, cols, Q("class_one", classname) && Q("class_one_id", id) && Q("class_two", related_classname));
    }

    for(unsigned int i = 0; i < rows.size(); i++)
      related << rows[i].get_int(0);
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap< SarVector<int> >& sm) {
    debug("Getting all related objects to a " + classname);
    std::string tablename = "relationships";
    SarVector<KVT> cols;
    cols << KVT("class_one", STRING);
    cols << KVT("class_one_id", INTEGER);
    SarVector<Row> rows = select(tablename, cols, Q("class_two", classname) && Q("class_two_id", id));
    for(unsigned int i = 0; i<rows.size(); i++) {
      std::string key;
      rows[i].get_string(0, key);
      if(!sm.has_key(key))
        sm[key] = SarVector<int>();
      sm[key] << rows[i].get_int(0);
    }

    cols.clear();
    cols << KVT("class_two", STRING);
    cols << KVT("class_two_id", INTEGER);
    rows = select(tablename, cols, Q("class_one", classname) && Q("class_one_id", id));
    for(unsigned int i = 0; i<rows.size(); i++) {
      std::string key;
      rows[i].get_string(0, key);
      if(!sm.has_key(key))
        sm[key] = SarVector<int>();
      sm[key] << rows[i].get_int(0);
    }
  };


};
