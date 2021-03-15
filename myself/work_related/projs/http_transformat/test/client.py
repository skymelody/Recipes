import requests
import threading
import json
import datetime

url = "http://localhost:9080"


exitFlag = 0
data = {"type" : ".mp4"}

def test_fun(threadName, delay, counter):
    while counter:
        if exitFlag:
            threadName.exit()
        files = {'file' : open("/home/skymelody/test.flv", "rb")}
        r = requests.post(url,  data = data, files=files)
        counter -= 1

class myThread(threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter
    def run(self):
        test_fun(self.name, self.counter, 50)

start_time = datetime.datetime.now()

thread1 = myThread(1, "thread1", 0)
thread2 = myThread(2, "thread2", 0)
thread3 = myThread(3, "thread3", 0)
thread4 = myThread(4, "thread4", 0)


thread1.start()
thread2.start()
thread3.start()
thread4.start()
thread1.join()
thread2.join()
thread3.join()
thread4.join()

end_time = datetime.datetime.now()
ans = (end_time-start_time).seconds

print("----------------_", ans)
print("finished")