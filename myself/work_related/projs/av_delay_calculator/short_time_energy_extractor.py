from ctypes import *

def get_short_time_energy(path, frame_size, filter_val, thread_num) :
    short_time_energy = cdll.LoadLibrary("./libshort_time_energy.so")
    path = bytes(path, "utf8")
    filter_val = bytes(str(filter_val), "utf8")
    res = short_time_energy.CalculateShortTimeEnergy(path, frame_size, filter_val, thread_num)
    return res
