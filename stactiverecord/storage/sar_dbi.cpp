#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(string config) {
    vector<string> configparts = explode(config, "://");
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
    if(configparts[0] == "postgres")
      return new PostgresStorage(configparts[1]);
#endif    

    throw Sar_InvalidConfigurationException("DB type of \"" + configparts[0] + "\" not recognized.");
  };

  bool Sar_Dbi::table_is_initialized(string tablename) { 
    return initialized_tables.includes(tablename);
  };

};
