#include "BoxCreater.h"
#include "BoxDefines.h"
#include "Box.h"
#include "FtypBox.h"
#include "MdatBox.h"
#include "MoovBox.h"
#include "MdiaBox.h"
#include "HdlrBox.h"
#include "MdhdBox.h"
#include "MvhdBox.h"
#include "MinfBox.h"
#include "StblBox.h"
#include "StssBox.h"
#include "SttsBox.h"
#include "StscBox.h"
#include "TrakBox.h"
#include "StcoBox.h"
#include "StszBox.h"
#include "CttsBox.h"
#include "StsdBox.h"
#include "UrlBox.h"
#include "AvcCBox.h"
#include "Avc1Box.h"
#include "VmhdBox.h"
#include "TkhdBox.h"
#include "DinfBox.h"
#include "UnknowBox.h"
#include "DrefBox.h"

using namespace std;

static Box* CreateDrefBox() {
    return new DrefBox;
}

static Box* CreateUrlBox() {
    return new UrlBox;
}

static Box* CreateDinfBox() {
    return new DinfBox;
}

static Box* CreateVmhdBox() {
    return new VmhdBox;
}

static Box* CreateCttsBox() {
    return new CttsBox;
}

static Box* CreateMdhdBox() {
    return new MdhdBox;
}

static Box* CreateMvhdBox() {
    return new MvhdBox;
}

static Box* CreateHdlrBox() {
    return new HdlrBox;
}

static Box* CreateMdatBox() {
    return new MdatBox;
}

static Box* CreateMdiaBox() {
    return new MdiaBox;
}

static Box* CreateMinfBox() {
    return new MinfBox;
}

static Box* CreateMoovBox() {
    return new MoovBox;
}

static Box* CreateStblBox() {
    return new StblBox;
}

static Box* CreateStscBox() {
    return new StscBox;
}

static Box* CreateStsdBox() {
    return new StsdBox;
}

static Box* CreateTrakBox() {
    return new TrakBox;
}

static Box* CreateStszBox() {
    return new StszBox;
}

static Box* CreateStcoBox() {
    return new StcoBox;
};

static Box* CreateAvc1Box() {
    return new Avc1Box;
};

static Box* CreateAvcCBox() {
    return new AvcCBox;
}

static Box* CreateSttsBox() {
    return new SttsBox;
}

static Box* CreateStssBox() {
    return new StssBox;
}

static Box* CreateTkhdBox() {
    return new TkhdBox;
}

static Box* CreateFtypBox() {
    return new FtypBox;
}

static Box* CreateUnknowBox() {
    return new UnknowBox;
}

BoxCreater::BoxCreater() {
    _createrMap[BOX_FTYP] = CreateFtypBox;
    _createrMap[BOX_MOOV] = CreateMoovBox;
    _createrMap[BOX_TRAK] = CreateTrakBox;
    _createrMap[BOX_MDIA] = CreateMdiaBox;
    _createrMap[BOX_HDLR] = CreateHdlrBox;
    _createrMap[BOX_MINF] = CreateMinfBox;
    _createrMap[BOX_MDAT] = CreateMdatBox;
    _createrMap[BOX_STBL] = CreateStblBox;
    _createrMap[BOX_STSC] = CreateStscBox;
    _createrMap[BOX_STSZ] = CreateStszBox;
    _createrMap[BOX_STCO] = CreateStcoBox;
    _createrMap[BOX_MDHD] = CreateMdhdBox;
    _createrMap[BOX_CTTS] = CreateCttsBox;
    _createrMap[BOX_STSD] = CreateStsdBox;
    _createrMap[BOX_AVC1] = CreateAvc1Box;
    _createrMap[BOX_AVCC] = CreateAvcCBox;
    _createrMap[BOX_TKHD] = CreateTkhdBox;
    _createrMap[BOX_DREF] = CreateDrefBox;
    _createrMap[BOX_STTS] = CreateSttsBox;
    _createrMap[BOX_STSS] = CreateStssBox;
    _createrMap[BOX_URL] = CreateUrlBox;
    _createrMap[BOX_MVHD] = CreateMvhdBox;
    _createrMap[BOX_VMHD] = CreateVmhdBox;
    _createrMap[BOX_DINF] = CreateDinfBox;
    _createrMap[BOX_UNKNOW] = CreateUnknowBox;
}
BoxCreater::~BoxCreater() {}


Box* BoxCreater::CreateBox(const string& type) {
    if (_createrMap.count(type)) {
        return _createrMap[type]();
    }
    return _createrMap[BOX_UNKNOW]();
}


BoxCreater& BoxCreater::Instance() {
    static BoxCreater creater;
    return creater;
}



Box* NewBox(const string& type) {
    auto& creater = BoxCreater::Instance();
    return creater.CreateBox(type);
}