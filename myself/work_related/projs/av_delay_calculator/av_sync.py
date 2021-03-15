from short_time_energy_extractor import get_short_time_energy
from matplotlib import pylab
from cv2 import cv2
import sys
import pytesseract
import os



def get_audio_time_stample(mp4_path, audio_frame_size, audio_filter_val, thread_num) :

    res = get_short_time_energy(mp4_path, audio_frame_size, audio_filter_val, thread_num)

    if (res == False) :
        return []

    time_stample = []

    with open("time_stample", "r") as f :
        for line in f :
            time_stample.append(int(line))
    
    return time_stample


def get_video_snapshot_by_audio_time_stample(mp4_path, audio_time_stample, n_diagnosis_frames) :
    video = cv2.VideoCapture(mp4_path)
    fps = video.get(cv2.CAP_PROP_FPS)

    if (video.isOpened() == False) :
        return []


    offset = int(n_diagnosis_frames / 2 if n_diagnosis_frames % 2 == 1 else (n_diagnosis_frames-1) / 2)

    snapshots = []
    for i in range(len(audio_time_stample)) :
        time_val = audio_time_stample[i]
        frame_num = int((time_val/1000)*fps)

        # 读取n帧，中间的帧是我们要的帧，前后帧用于差错检测
        frames = []
        video.set(cv2.CAP_PROP_POS_FRAMES, frame_num-offset)
        for j in range(n_diagnosis_frames) :
            res, frame = video.read()
            if res == False :
                return snapshots
            frames.append(frame)

        snapshots.append([frames, time_val])

        
    return snapshots

# 将视频画面中的时间转换成毫秒
# FIXME 异常帧画面的时间

MS_PER_HOUR = 3600000
MS_PER_MIN = 60000
MS_PER_SEC = 1000


def ms_to_time_str(time) :
    res = ""
    val = int(time / MS_PER_HOUR)
    time -= val * MS_PER_HOUR
    if 0 <= val and val <= 9 :
        res += "0"
    res += str(val)
    res += ":"

    val = int(time / MS_PER_MIN)
    time -= val * MS_PER_MIN
    if 0 <= val and val <= 9 :
        res += "0"
    res += str(val)
    res += ":"

    val = int(time / MS_PER_SEC)
    time -= val * MS_PER_SEC
    if 0 <= val and val <= 9 :
        res += "0"
    res += str(val)
    res += ":"

    val = int(time / 10)
    if 0 <= val and val <= 9 :
        res += "0"
    res += str(val)

    return res






def time_str_to_ms(time_str) :
    time = str.split(time_str, ":")
    res = 0
    try :
        res = int(time[0]) * MS_PER_HOUR + \
            int(time[1]) * MS_PER_MIN +  \
            int(time[2]) * MS_PER_SEC + \
            int(time[3]) * 10
    except Exception :
        return sys.maxsize

    return res

def log(file_path, log_list) :
    with open(file_path, "w") as f :
        for log in log_list :
            f.write(str(log))
            f.write("\n")




# 检测帧画面是否完好，传入视频前后三帧的时间戳，单位ms
# FIXME
def check_video_frame(time_stamples, key_frame, n_diagnosis_frames) :
    if (len(time_stamples) != n_diagnosis_frames) :
        return False, -1 

    key_frame_pre_time = time_stamples[key_frame-1]
    key_frame_time = time_stamples[key_frame]
    key_frame_next_time = time_stamples[key_frame+1]

    time1 = key_frame_time - key_frame_pre_time
    time2 = key_frame_next_time - key_frame_time

    # 如果帧时间间隔在80ms内，则认为正常
    if (time1 >= 0 and time1 <= 80) or (time2 >= 0 and time2 <= 80):
        return True, key_frame_time


    time3 = key_frame_next_time - key_frame_pre_time
    if time3 >= 0 and time3 <= 160 :
        return True, int(time3/2)
    
    return False, key_frame









# 返回视频帧画面中的时间戳 ms
def get_video_time_in_snapshot(snapshots, n_diagnosis_frames) :
    video_time_stamples = []
    err_logs = []
    err_imgs = []

    key_frame_pos = int(n_diagnosis_frames / 2 if n_diagnosis_frames%2 == 1 else (n_diagnosis_frames-1) / 2)
    for i in range(len(snapshots)) :
        time_stamples = []

        for j in range(n_diagnosis_frames) :
            frame = snapshots[i][0][j]
            video_height = int(frame.shape[0])
            video_width = int(frame.shape[1])
            frame = frame[int(video_height/2):video_height-100, 0:video_width]
            # time_stamples.append(time_str_to_ms(pytesseract.image_to_string(snapshots[i][0][j])))
            time_stamples.append(time_str_to_ms(pytesseract.image_to_string(frame)))



        # 三帧都无法识别的情况
        if time_stamples[key_frame_pos-1] == sys.maxsize and  \
           time_stamples[key_frame_pos]   == sys.maxsize and  \
           time_stamples[key_frame_pos+1] == sys.maxsize :
           err_time = ms_to_time_str(snapshots[i][1])
           err_logs.append("在时间戳: " + err_time + " 无法识别视频画面")
           err_imgs.append([err_time, snapshots[i][0][key_frame_pos]])
           video_time_stamples.append(sys.maxsize)
           continue


        # 两帧或一帧或零帧无法识别的情况
        check_res, checked_time_stamples = check_video_frame(time_stamples, key_frame_pos, n_diagnosis_frames)
        if check_res == False :
            err_time = ms_to_time_str(snapshots[i][1])
            err_logs.append("在时间戳: " + err_time + " 无法识别视频画面")
            err_imgs.append([err_time, snapshots[i][0][key_frame_pos]])
        else :
            video_time_stamples.append(checked_time_stamples)



    if len(err_logs) != 0 :
        print("出现错误, 请查看日志(err_logs)")
        log("./err_logs", err_logs)
        
    if len(err_imgs) != 0 :
        if os.path.exists("./err_imgs") == False :
            os.mkdir("./err_imgs")

        for i in range(len(err_imgs)) :
            file_path = "./err_imgs/" + err_imgs[i][0] + ".jpg"

            cv2.imwrite(file_path, err_imgs[i][1])



    return video_time_stamples


# 返回一个延时列表
def get_delays(video_time_stamples, time_interval) :
    delays = []
    for i in range(len(video_time_stamples)) :
        if video_time_stamples[i] == sys.maxsize :
            delays.append(video_time_stamples[i])
            continue

        delay = (video_time_stamples[i] % time_interval)
        if delay < (time_interval / 2):
            delays.append(delay)
        else :
            delays.append(delay-time_interval)
    return delays



def gen_delay_logs(time_stample, delays) :
    size = min(len(time_stample), len(delays))
    delay_logs = []
    for i in range(size) :
        delay_logs.append("在视频 " + ms_to_time_str(time_stample[i]) + " 处, 延迟为 : " + str(delays[i]) + "ms.")
    return delay_logs



def gen_delay_pic(time_stample, delays, time_interval) :
    size = min(len(time_stample), len(delays))

    pylab.figure(figsize=(20, 20))
    pylab.xlabel("time_stample(ms)")
    pylab.ylabel("delay(ms)")
    colors = []
    for i in range(size) :
        if delays[i] == sys.maxsize :
            delays[i] = 100
            colors.append("r")
        else :
            colors.append("deepskyblue")
    pylab.bar(range(len(time_stample)), delays, color=colors)
    pylab.xticks(range(len(time_stample)), time_stample, rotation=90)

    for x, y in zip(range(len(time_stample)), delays) :
        pylab.text(x, y, "%d" %y)
    save_path = input("输入延时图片保存路径: ")
    pylab.savefig(save_path)


def main():
    arg_len = len(sys.argv)
    if arg_len < 2 :
        print("没有输入文件")
        exit(0)

    mp4_path = sys.argv[1]
    if os.path.exists(mp4_path) == False :
        print("视频文件不存在")
        exit(0)
    audio_frame_size = 20 if arg_len < 3 else sys.argv[2]
    audio_filter_val = 100000000000 if arg_len < 4 else sys.argv[3]
    thread_num  = 4 if arg_len < 5 else sys.argv[4]
    audio_time_stamples = get_audio_time_stample(mp4_path, int(audio_frame_size), audio_filter_val, int(thread_num))
    if (len(audio_time_stamples) == 0):
        print("音频时间戳不存在, 请查看当前目录下是否存在time_stample文件或time_stample文件中是否有内容")
        return


    n_diagnosis_frames = 3 if arg_len < 6 else 3 if int(sys.argv[5]) < 3 else int(sys.argv[5])
    video_snapshots = get_video_snapshot_by_audio_time_stample(mp4_path, audio_time_stamples, n_diagnosis_frames)
    if (len(video_snapshots) == 0):
        print("无法根据音频时间戳截取视频帧画面")
        return


    video_time_stamples = get_video_time_in_snapshot(video_snapshots, n_diagnosis_frames)

    audio_time_interval = 5000 if arg_len < 7 else int(sys.argv[6])
    delays = get_delays(video_time_stamples, audio_time_interval)

    delay_logs = gen_delay_logs(audio_time_stamples, delays)
    log("./delays", delay_logs)


    gen_delay_pic(audio_time_stamples, delays, audio_time_interval)







if __name__ == "__main__" :
    main()
