#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  void check_classname(string classname) {
    char chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    bool found;
    for(string::size_type i=0; i<classname.size(); i++) {
      found = false;
      for(int z=0; z<26; z++) 
	if(classname[i] == chars[z] || classname[i] == '_')
	  found = true;
      if(!found) throw Sar_InvalidClassnameException("\"" + classname + "\" is an invalid classname.");
    }
  };

  void coltype_to_name(coltype ct, string& name) {
    string scoltype[] = { "NONE", "INTEGER", "STRING", "RECORD" };
    name = scoltype[ct];
  };

  void int_to_string(int i, string& s) {
    char c_int[100];
    sprintf(c_int, "%ld", i);
    s = string(c_int);
  };

  void debug(string s) {
#ifdef DEBUG
    string time_s = "";
    time_t rawtime = time(NULL);
    tm *tm_t = localtime(&rawtime);
    char rv[40];
    if(strftime(rv, sizeof(rv)-1, "%a %b %d %H:%M:%S %Y", tm_t))
      time_s = "[" + string(rv) + "] ";
    s = time_s + "[" + string(PACKAGE_NAME) + "] " + s + "\n";
    // escape %'s
    string cleaned_s = "";
    vector<string> parts = explode(s, "%");
    for(unsigned int i=0; i<parts.size()-1; i++)
      cleaned_s += parts[i] + "%%";
    cleaned_s += parts[parts.size()-1];
    // stderr is redirected by apache to apache's error log
    fprintf(stderr, cleaned_s.c_str());
    fflush(stderr);
#endif
  };

  vector<string> explode(string s, string e) {
    vector<string> ret;
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
  }

};
