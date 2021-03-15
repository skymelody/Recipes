## 简介
本项目是一个http服务器，用于视频的格式转换。支持mp4, mkv, flv等格式间的相互转换。用户在客户端通过http协议将视频上传至服务器，并指定要转换的格式。客户端可通过异步的方式接收转换后的文件。

## 部署
本项目依赖c++ poco库和ffmpeg。在linux下可通过如下命令安装这两个依赖
```
sudo apt install libpoco-dev
sudo apt install libavformat-dev
sudo apt install libavcodec-dev
sudo apt install libavutil-dev
sudo apt install libswresample-dev
sudo apt install libswscale
```

在项目的目录下建立build目录，在build目录下运行
```
cmake ..
make
```

进入build下bin目录，运行main即可

## 配置
在conf/config.json文件中可以配置服务器的参数。支持如下参数的配置
```
上传文件的存放目录
转换后文件的存放目录
http超时时间
最大tcp连接队列
监听端口
线程数
支持的视频格式
```


## 作者
林志锋(i_linzhifeng@cvte.com)

