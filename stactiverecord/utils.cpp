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
  
  /** Determind if classname is valid - throws exception if not */
  void check_classname(std::string classname) {
    char chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    bool found;
    for(std::string::size_type i=0; i<classname.size(); i++) {
      found = false;
      for(int z=0; z<52; z++) 
	if(classname[i] == chars[z] || classname[i] == '_')
	  found = true;
      if(!found) throw Sar_InvalidClassnameException("\"" + classname + "\" is an invalid classname.");
    }
  };

  /** convert column type to string */
  void coltype_to_name(coltype ct, std::string& name) {
    std::string scoltype[] = { "NONE", "INTEGER", "STRING", "RECORD" };
    name = scoltype[ct];
  };

  /** convert integer to string */
  void int_to_string(int i, std::string& s) {
    char c_int[100];
    sprintf(c_int, "%ld", i);
    s = std::string(c_int);
  };

  /** Better be sure s is really a string - used for postgres int column
      retrieval
  **/
  int string_to_int(std::string s) {
    int i;
    sscanf(s.c_str(), "%d", &i);
    return i;
  }

  /** print debugging information if DEBUG is defined */
  void debug(std::string s) {
#ifdef DEBUG
    std::string time_s = "";
    time_t rawtime = time(NULL);
    tm *tm_t = localtime(&rawtime);
    char rv[40];
    if(strftime(rv, sizeof(rv)-1, "%a %b %d %H:%M:%S %Y", tm_t))
      time_s = "[" + std::string(rv) + "] ";
    s = time_s + "[" + std::string(PACKAGE_NAME) + "] " + s + "\n";
    // escape %'s
    std::string cleaned_s = "";
    std::vector<std::string> parts = explode(s, "%");
    for(unsigned int i=0; i<parts.size()-1; i++)
      cleaned_s += parts[i] + "%%";
    cleaned_s += parts[parts.size()-1];
    fprintf(stderr, cleaned_s.c_str());
    fflush(stderr);
#endif
  };

  /** Split a string s into parts by value e */
  std::vector<std::string> explode(std::string s, std::string e) {
    std::vector<std::string> ret;
    int iPos = s.find(e, 0);
    int iPit = e.length();
    while(iPos>-1) {
      if(iPos!=0)
        ret.push_back(s.substr(0,iPos));
      s.erase(0,iPos+iPit);
      iPos = s.find(e, 0);
    }
    if(s!="")
      ret.push_back(s);
    return ret;
  };

  void join(std::vector<std::string> v, std::string joiner, std::string& result) {
    result = "";
    for (unsigned int i=0; i<v.size(); i++) 
      result += (i==(v.size()-1)) ? v[i] : v[i] + joiner;
  }; 

};
