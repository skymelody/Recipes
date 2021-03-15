#ifndef BOX_CREATER_INCLUDED
#define BOX_CREATER_INCLUDED
#include "Box.h"
#include <unordered_map>


Box* NewBox(const std::string& type);

class BoxCreater {
public:
    static BoxCreater& Instance();
    Box* CreateBox(const std::string& type);

private:
    BoxCreater();
    ~BoxCreater();

private:
    using CreateFunc = Box* (*)();
    std::unordered_map<std::string, CreateFunc> _createrMap;

};

#endif