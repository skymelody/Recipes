#include "Mp4Mux.h"
using namespace std;


int main() {
    Mp4Mux mux;
    mux.Mux("/home/skymelody/t.h264", "/home/skymelody/test.mp4");
    return 0;
}
