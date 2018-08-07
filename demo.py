from ctypes import *
from threading import Thread

class My_Thread(Thread):
    def __init__(self,arg):
        Thread.__init__(self)
        self.cur = arg[0]
        self.name = arg[1]
        self.bin_name = arg[2]
        self.h = c_int(0)
        self.ph = pointer(self.h)
        self.str_name = create_string_buffer(bytes(self.name,encoding = 'utf8'),32)

    def open(self):
        status = self.cur.dvpOpenByName(self.str_name,1, self.ph)
        print(status)

    # 设置增益
    def setGain(self,num):
        c_num = c_float(num)
        p_num = pointer(c_num) 
        return self.cur.dvpSetAnalogGain(self.h,p_num)

    def setPrint(self):
        pass

    def takePhoto(self):
        self.frame = dvpFrame()
        print(sizeof(dvpFrame))
        self.p = c_void_p()
        status = self.cur.dvpGetFrame(self.h, 
            pointer(self.frame),
            pointer(self.p),
            1000)
        print(status)
        pass

    def begin(self):
        status = self.cur.dvpSetQuickRoiSel(self.h,0)
        print(status)
        status = self.cur.dvpStart(self.h)
        print(status)
    
    def save(self, name = 'one_pic.jpg'):
        hzd = cdll.LoadLibrary('./libhzd.so')
        c_name = create_string_buffer(bytes(name,encoding = 'utf8'),32)
        status = hzd.dvpSavePicture(
            pointer(self.frame),
            self.p,
            c_name,
            100)
        print(status)

    def stop(self):
        status = self.cur.dvpStop(self.h)
        print(status)
        pass

    def run(self):
        self.open()
        status = self.setGain(8.0)
        print(status)
        self.begin()
        self.takePhoto()
        self.save()
        self.stop()
        pass
        

class dvpRegion(Structure):
    _fields_=[
        ('X',c_int,32),
        ('Y',c_int,32),
        ('W',c_int,32),
        ('H',c_int,32),
    ]

# class dvpCameraInfo(Structure):
#     _fields_ = [
#         ('Vendor',c_char_p),
#         ('Manufacturer',c_char_p),
#         ('Model',c_char_p),
#         ('Family',c_char_p),
#         ('LinkName',c_char_p),
#         ('SensorInfo',c_char_p),
#         ('HardwareVersion',c_char_p),
#         ('FirmwareVersion',c_char_p),
#         ('KernelVersion',c_char_p),
#         ('DscamVersion',c_char_p),
#         ('FriendlyName',c_char_p),
#         ('PortInfo',c_char_p),
#         ('SerialNumber',c_char_p),
#         ('CameraInfo',c_char*128),
#         ('UserID',c_char*128),
#         ('reserved',c_char*128),
#     ]

class dvpFrame(Structure):
    _fields_ = [
        ('format', c_uint),
        ('bits',c_uint),
        ('uBytes',c_uint),
        ('iWidth',c_int),
        ('iHeight',c_int),
        ('uFrameID',c_ulonglong),
        ('uTimestamp',c_ulonglong),
        ('fExposure',c_double),
        ('fAGain',c_float),
        ('position',c_uint),
        ('bFlipHorizontalState',c_bool),
        ('bFlipVerticalState',c_bool),
        ('bRotateState',c_bool),
        ('bRotateOpposite',c_bool),
        ('reserved',c_uint*30),
    ]

class dvpCameraInfo(Structure):
    _fields_ = [
        ('Vendor',c_char*64),
        ('Manufacturer',c_char*64),
        ('Model',c_char*64),
        ('Family',c_char*64),
        ('LinkName',c_char*64),
        ('SensorInfo',c_char*64),
        ('HardwareVersion',c_char*64),
        ('FirmwareVersion',c_char*64),
        ('KernelVersion',c_char*64),
        ('DscamVersion',c_char*64),
        ('FriendlyName',c_char*64),
        ('PortInfo',c_char*64),
        ('SerialNumber',c_char*64),
        ('CameraInfo',c_char*128),
        ('UserID',c_char*128),
        ('reserved',c_char*128),
    ]

# 把c_char数组转化成文字
def my_to_string(char_array):
    foo_ptr_1 = cast( char_array, c_char_p )
    # print(foo_ptr_1.value)
    # print(type(foo_ptr_1.value))
    s = str(foo_ptr_1.value, encoding = "utf8")
    return s

def thread_saving():
    pass

def main():
    # 导入那个了
    cur = cdll.LoadLibrary('./libdvp.so')
    i=c_int(12)
    count = pointer(i)
    cur.dvpRefresh(count)
    # print(i)
    cameraInfo = dvpCameraInfo()
    # print(sizeof(cameraInfo))
    pcameraInfo = pointer(cameraInfo)
    cur.dvpEnum(0, pcameraInfo)
    raw_friendly_name = cameraInfo.FriendlyName
    name = my_to_string(raw_friendly_name)
    # print(cameraInfo.FriendlyName)
    p_raw_friendly_name = c_char_p(raw_friendly_name)
    void_p_raw_friendly_name = c_void_p()
    void_p_raw_friendly_name = byref(p_raw_friendly_name)
    # friendly_name = my_to_string(raw_friendly_name)
    print(raw_friendly_name)
    the_thread = My_Thread([cur,
        name,
        void_p_raw_friendly_name])
    the_thread.start()
    the_thread.join()


if __name__ == '__main__':
    main()