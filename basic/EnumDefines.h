#include <string>
#include <mutex>
#include <sstream>
#include <vector>

#define DefineEnums(EnumClassName, EnumArgs...)                           \
  enum class EnumClassName {                                              \
    EnumArgs                                                              \
  };                                                                      \
  inline std::string Str##EnumClassName(EnumClassName enumVal) {          \
    static bool inited{ false };                                          \
    static int offset{ 0 };                                               \
    /*
     * to reduce space consume, we can                                    
     * make enumArgsStr pointer                                   
     * after init finished, delete it                                     
     */                                                                   \
    static std::mutex mtx;                                                \
    static std::string enumArgsStr = #EnumArgs;                           \
    static std::vector<std::string> enumStrV;                             \
                                                                          \
    if (!inited) {                                                        \
      std::lock_guard<std::mutex> lock(mtx);                              \
      if (!inited) {                                                      \
        std::istringstream iss(enumArgsStr);                              \
        std::string enumStr;                                              \
        bool first = true;                                                \
                                                                          \
        while (std::getline(iss, enumStr, ',')) {                         \
          /* TODO: trim str */                                            \
          int pos = enumStr.find('=');                                    \
          if (first) {                                                    \
            first = false;                                                \
            if (pos != std::string::npos) {                               \
              offset = std::stoi(enumStr.substr(pos+1));                  \
            }                                                             \
          }                                                               \
          if (pos != std::string::npos) {                                 \
            enumStrV.push_back(enumStr.substr(0, pos));                   \
          }                                                               \
          else {                                                          \
            enumStrV.push_back(enumStr);                                  \
          }                                                               \
        }                                                                 \
      }                                                                   \
      inited = true;                                                      \
    }                                                                     \
                                                                          \
    int pos = static_cast<int>(enumVal) - offset;                         \
    return (pos >= enumStrV.size() || pos < 0) ? "" : enumStrV[pos];      \
  }
