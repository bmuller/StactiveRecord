#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(string config) {
    vector<string> configparts = explode(config, "://");
    if(configparts.size() != 2)
      throw Sar_InvalidConfigurationException("Invalid database configuration string: " + config);

    if(configparts[0] == "mysql")
      return new MySQLStorage(configparts[1]);
    if(configparts[0] == "sqlite")
      return new SQLiteStorage(configparts[1]);
    
    throw Sar_InvalidConfigurationException("DB type of \"" + configparts[0] + "\" not recognized.");
  };

  bool Sar_Dbi::table_is_initialized(string tablename) { 
    return initialized_tables.includes(tablename);
  };

};
