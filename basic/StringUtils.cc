#include "StringUtils.h"
#include <sstream>

static inline bool is_space(char ch) {
  return 
    ch == ' '   || ch == '\t'  || ch == '\n'  ||
    ch == '\v'  || ch == '\f'  || ch == '\r';
}

namespace basic {
  namespace stringutils {

    void trim_left(std::string& str) {
      int i = 0;
      while (i < str.size() && is_space(str[i])) ++i;
      str = str.substr(i);
    }

    void trim_right(std::string& str) {
      int i = str.size()-1;
      while (i >= 0 && is_space(str[i])) --i;
      str = str.substr(0, i+1);
    }

    void trim(std::string& str) {
      trim_left(str);
      trim_right(str);
    }

    bool contains(const std::string& str, const std::string target) {
      if (target.size() == 1) return contains(str, target[0]);
      return str.find(target) != std::string::npos;
    }

    bool contains(const std::string& str, char ch) {
      return str.find(ch) != std::string::npos;
    }

    std::vector<std::string> split(const std::string& str, char ch) {
      std::vector<std::string> res;
      std::istringstream iss(str);

      std::string word;
      while (std::getline(iss, word, ch)) {
        res.push_back(word);
      }
      return res;
    }
  }
}
