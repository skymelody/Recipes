#include <string>
#include <vector>
#include <iostream>
#include "../StringUtils.h"

using namespace std;
using namespace basic::stringutils;

void output_strs(
    const std::vector<std::string>& strs, 
    const std::string& before_output = "",
    const std::string& after_output = "") {

  if (!before_output.empty()) std::cout << before_output << std::endl;
  for (const auto& str : strs) {
    std::cout << str << "  " << str.size() << std::endl;
  }
  if (!after_output.empty()) std::cout << after_output << std::endl;
}

void test_trim() {
  std::vector<std::string> strs {
    "   b"          ,   
    "c    "         ,
    "    d     "    , 
    ""              ,
    "fff"           ,
  };
  output_strs(strs, "original val:", "\n");

  {
    auto tmp = strs;
    for (auto&& str : tmp) trim_left(str);
    output_strs(tmp, "after trim left:", "\n");
  }

  {
    auto tmp = strs;
    for (auto&& str : tmp) trim_right(str);
    output_strs(tmp, "after trim right:", "\n");
  }

  {
    auto tmp = strs;
    for (auto&& str : tmp) trim(str);
    output_strs(tmp, "after trim:", "\n");
  }
}

int main() {
  test_trim();
}
