//
// Created by skymelody on 2019/12/13.
//

#include "Sample.h"

Sample::Sample() : isIdr(false), decodeOrder(0), displayOrder(0), nalCnt(0) {}

void Sample::Clear() {
    data.clear();
    isIdr = false;
    decodeOrder = 0;
    displayOrder = 0;
    nalCnt = 0;
}
