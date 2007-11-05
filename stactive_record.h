#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <exception>

namespace stactiverecord {
  enum coltype { NONE, INTEGER, STRING, RECORD };
  enum scoltype { "NONE", "INTEGER", "STRING", "RECORD" };
};

#include "utils.h"
#include "types.h"
#include "cud_property_register.h"
#include "exception.h"
#include "storage/storage.h"
#include "record.h"


#define false 0
#define true 1

#define VALUE_MAX_SIZE 255
#define VALUE_MAX_SIZE_S "255"

#define DEBUG 1

#define PACKAGE_NAME "Stactive Record"
