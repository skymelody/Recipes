### 简介
shark2file是一个跨平台命令行程序，支持对rtp包以ssrc进行分离。用户可以使用shark2file抓包进行分离，也可以输入pcap文件来分离rtp包。

### 依赖
linux
依赖libpcap库，手动安装
```sh
sudo apt install libpcap0.8-dev
```

### 编译
linux
```
git clone https://gitdojo.gz.cvte.cn/linzhifeng/shark2file.git
cd shark2file
mkdir build
cd build
cmake ..
```
windows 下编译位32位
```
mkdir build
cd build
cmake -A Win32 ..
```


### 使用
```
-in     + input path          :    如果以pcap文件输入，则指明其路径
-out    + output path         :    指明文件分离后的保存路径
-i      + net card            :    指明抓包的网卡
-ws     + tshark path         :    如果使用tshark抓包，则指明tshark的安装路径
-wsout  + tshark output path  :    指明tshark抓包文件的保存位置
-D                            :    列出可抓包的网卡
```
如果同时指明了输入pcap文件和使用tshark，那么程序不会使用tshark抓包，而是使用pacp文件

### 作者
林志锋(i_linzhifeng@cvte.com)
