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
    Where * where = equals(classname);
    SarVector<Row> row = select("id_maximums", cols, "classname", where);
    delete where;
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
    update("id_maximums", values, "classname = \"" + classname + "\"");
    return maxid;
  };

  int Sar_Dbi::current_id(std::string classname) {
    SarVector<KVT> values;
    values << KVT("id", INTEGER);
    Where * where = equals(classname);
    SarVector<Row> rows = select("id_maximums", values, "classname", where);
    delete where;
    if(rows.size() == 0)
      return -1;
    return rows[0].get_int(0);
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap<std::string>& values) {
    std::string tablename = classname + "_s";
    SarVector<KVT> cols;
    cols << KVT("keyname", STRING);
    cols << KVT("value", STRING);
    Where * where = equals(id);
    SarVector<Row> rows = select(tablename, cols, "id", where);
    delete where;
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
    Where * where = equals(id);
    SarVector<Row> rows = select(tablename, cols, "id", where);
    delete where;
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
        Where * id_where = equals(id);
        Where * keyname_where = equals(std::string((*i).first));
	std::string sid_where, skeyname_where;
        where_to_string(id_where, sid_where);
        where_to_string(keyname_where, skeyname_where);
	std::string where = "id " + sid_where + " AND keyname " + skeyname_where;
        update(tablename, values, where);
        delete id_where;
        delete keyname_where;
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
        Where * id_where = equals(id);
        Where * keyname_where = equals(std::string((*i).first));
	std::string sid_where, skeyname_where;
        where_to_string(id_where, sid_where);
        where_to_string(keyname_where, skeyname_where);
	std::string where = "id " + sid_where + " AND keyname " + skeyname_where;
        update(tablename, values, where);
        delete id_where;
        delete keyname_where;
      }
    }
  };

  void Sar_Dbi::del(int id, std::string classname, SarVector<std::string> keys, coltype ct) {
    std::string tablename = (ct == STRING) ? classname+"_s" : classname+"_i";
    Where * id_where = equals(id);
    std::string sid_where, skeyname_where;
    where_to_string(id_where, sid_where);
    for(unsigned int i=0; i < keys.size(); i++) {
      Where * keyname_where = equals(std::string(keys[i]));
      where_to_string(keyname_where, skeyname_where);
      std::string where = "id " + sid_where + " AND keyname " + skeyname_where;
      remove(tablename, where);
      delete keyname_where;
    }
    delete id_where;
  };
  
  void Sar_Dbi::delete_record(int id, std::string classname) {
    Where * id_where = equals(id);
    std::string tablename = classname + "_s";
    remove(tablename, "id", id_where);
    tablename = classname + "_i";
    remove(tablename, "id", id_where);

    tablename = "relationships";
    Where * classname_where = equals(classname);
    std::string sid_where, sclassname_where;
    where_to_string(id_where, sid_where);
    where_to_string(classname_where, sclassname_where);
    remove(tablename, "class_one " + sclassname_where + " AND class_one_id " + sid_where);
    remove(tablename, "class_two " + sclassname_where + " AND class_two_id " + sid_where);
    delete id_where;
    delete classname_where;
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
    remove(tablename, "class_one " + sclassname_where + " OR class_two " + sclassname_where);

    // delete max id
    tablename = "id_maximums";
    remove(tablename, "classname", classname_where);

    delete classname_where;
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
    std::string sid_where, sclassname_one_where, sclassname_two_where, swhere;
    if(swap) {
      cols << KVT("class_one_id", INTEGER);
      Where * id_where = equals(id);
      Where * classname_one_where = equals(related_classname);
      Where * classname_two_where = equals(classname);
      where_to_string(id_where, sid_where);
      where_to_string(classname_one_where, sclassname_one_where);
      where_to_string(classname_two_where, sclassname_two_where);
      delete id_where;
      delete classname_one_where;
      delete classname_two_where;
      swhere = "class_one " + sclassname_one_where + " AND class_two_id " + sid_where + " AND class_two " + sclassname_two_where;
    } else {
      cols << KVT("class_two_id", INTEGER);
      Where * id_where = equals(id);
      Where * classname_one_where = equals(classname);
      Where * classname_two_where = equals(related_classname);
      where_to_string(id_where, sid_where);
      where_to_string(classname_one_where, sclassname_one_where);
      where_to_string(classname_two_where, sclassname_two_where);
      delete id_where;
      delete classname_one_where;
      delete classname_two_where;
      swhere = "class_one " + sclassname_one_where + " AND class_one_id " + sid_where + " AND class_two " + sclassname_two_where;
    }
    SarVector<Row> rows = select(tablename, cols, swhere);
    for(unsigned int i = 0; i < rows.size(); i++)
      related << rows[i].get_int(0);
  };

};
