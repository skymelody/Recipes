##  简介
本项目可用于MP4文件与h264流之间的提取和封装。

## 如何运行程序？
将项目clone至本地，在项目目录下建立build目录。进入build目录，运行
```
cmake ..
make
bin/main
```

## 如何以类库方式使用？
```
// 对于mp4文件中h264流的提取
string mp4Path = "/mp4Path";
string h264Path = "/h264Path";

H264Extractor extractor;
bool res = extractor.Extract(mp4Path, h264Path);


// 对于h264流到mp4文件的封装
Mp4Mux mux;
mux.Mux(h264Path, mp4Path);

```


## 作者
林志锋(i_linzhifeng@cvte.com)
