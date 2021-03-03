// std
#include <iostream>
#include <string>

// qt
#include <QWidget>
#include <QApplication>

// ffmpeg
#include <libavcodec/avcodec.h>

// linux header
#include <unistd.h>

// vlc
#include <vlc/vlc.h>

int main(int argc, char** argv) {

  int pipe_fd[2];
  pipe(pipe_fd);

  int pid = fork();
  if (pid > 0) {
    std::string video_path = "/home/skymelody/Pictures/lingmeng.mp4";
    libvlc_instance_t* inst = libvlc_new(0, nullptr);
    libvlc_media_t* media = libvlc_media_new_path(inst, video_path.c_str());
    libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);

    close(pipe_fd[1]);
    char msg[1024];
    read(pipe_fd[0], msg, 1024);
    std::string win_id_str = msg;
    int win_id = std::stoi(win_id_str);

    libvlc_media_player_set_xwindow(player, win_id);
    libvlc_media_player_play(player);

    libvlc_state_t state = libvlc_Ended;
    do {
      state = libvlc_media_player_get_state(player);
    } while (state != libvlc_Error && state != libvlc_Ended);

    libvlc_media_release(media);
    libvlc_media_player_release(player);
    libvlc_release(inst);
  }
  else {
    QApplication app(argc, argv);
    QWidget win;
    win.setWindowTitle("qt window");
    win.resize(1080, 1920);

    std::string win_id_str = std::to_string(win.winId());
    close(pipe_fd[0]);
    write(pipe_fd[1], win_id_str.c_str(), win_id_str.size());

    win.show();
    return app.exec();
  }

  // return ret;
}
