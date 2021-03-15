//
// Created by skymelody on 2019/12/18.
//

#include "DinfBox.h"
#include <cstring>

DinfBox::DinfBox() {
    size = 8;
    Byte tp[4]{'d', 'i', 'n', 'f'};
    memmove(&type, tp, 4);
}
