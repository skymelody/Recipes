#include <string>
#include <vector>

namespace basic {
  namespace stringutils {

    void trim_left(std::string& str);
    void trim_right(std::string& str);
    void trim(std::string& str);

    bool contains(const std::string& str, const std::string target);
    bool contains(const std::string& str, char ch);

    std::vector<std::string> split(const std::string& str, char ch);
  }
}
