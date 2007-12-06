/*
Copyright (C) 2007 Butterfat, LLC (http://butterfat.net)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Created by bmuller <bmuller@butterfat.net>
*/

#include "stactive_record.h"

namespace stactiverecord {
  
  DateTime::DateTime(int day, int month, int year, int hour, int minute, int second) {
    set(day, month, year, hour, minute, second);
  };

  void DateTime::set(int day, int month, int year, int hour, int minute, int second) {
    t.tm_year = (year==0) ? 0 : year-1900;
    t.tm_mon = (month==0) ? 0 : month-1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
  };

  void DateTime::to_string(std::string& s) {
    s = std::string(asctime(&t));
  };

  int DateTime::to_int() {
    time_t tt = mktime(&t);
    return (int) tt;
  };

  bool DateTime::operator==(DateTime& other) {
    return (t.tm_year == other.t.tm_year && t.tm_mon == other.t.tm_mon &&
    	    t.tm_mday == other.t.tm_mday && t.tm_hour == other.t.tm_hour &&
	    t.tm_min == other.t.tm_min && t.tm_sec == other.t.tm_sec);
  };
  bool DateTime::operator!=(DateTime& other) {
    return !(*this == other);
  };
};
