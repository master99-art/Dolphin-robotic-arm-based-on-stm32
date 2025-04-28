import cv2
import numpy as np
import math
import serial
import serial.tools.list_ports
import struct


def get_color(BGRimg, hsv_range):
    hsv = cv2.cvtColor(BGRimg, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)
    hmin, hmax = hsv_range[0]
    smin, smax = hsv_range[1]
    vmin, vmax = hsv_range[2]
    huemask =  cv2.inRange(h, hmin, hmax)
    satmask = cv2.inRange(s, smin, smax)
    valmask = cv2.inRange(v, vmin, vmax)
    mask = huemask & satmask & valmask
    rst = cv2.bitwise_and(hsv, hsv, mask=mask)
    rst = cv2.cvtColor(rst, cv2.COLOR_HSV2BGR)
    return rst

def pre_process(img):
    # 使用开运算对图像去噪：
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))
    img2 = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel)

    # 阈值处理
    img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
    # retval, srt= cv2.threshold(img2,25,255,cv2.THRESH_BINARY)
    retval, srt = cv2.threshold(img2, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    # srt = cv2.adaptiveThreshold(img2, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,cv2.THRESH_BINARY_INV, 5, 3)
    return srt

def knowcolor(img, defaultcolor):
    # 使用defaultcolor提取试试，若有则为此颜色，无则更换
    greenimg = get_color(img, defaultcolor)
    rst1 = pre_process(greenimg)
    # orangeimg = get_color(img, defaultcolor)
    # rst2 = pre_process(orangeimg)
    contours1, _ = cv2.findContours(rst1, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    color = 1
    for i in contours1:
        area = cv2.contourArea(i)
        if area > 100:
            color = 0
            break
    return color

def findcorner(img):
    # 寻找边缘
    contours, a = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contour = []
    for i in contours:
        area = cv2.contourArea(i)
        if area > 60:
            contour.append(i)
    # 下面是画轮廓的代码
    # image = cv2.drawContours(img, contours, -1, (0, 255, 0), 1)
    # cv2.imshow('img', image)
    # cv2.waitKey(0)

    # 对正方体进行逼近多边形
    curves = []
    for con in contour:
        approxCurve = cv2.approxPolyDP(con, 5, True)
        # result = cv2.drawContours(img, [approxCurve], -1, (0,0,255), -1)
        curves.append(approxCurve)
    # 选取在图像最上方角点属于的四边形
    n = 0
    min = curves[0][0][0][1]
    for i in range(len(curves)):
        if min > curves[i][0][0][1]:
            min = curves[i][0][0][1]
            n = i
    dots = curves[n]
    return dots

def get_xyz(dots, a):
    # 求解pnp问题
    pts_2d = np.array(dots, dtype=np.float32)
    K = np.array([[934.93785113, 0., 312.57651013],
                  [0., 937.69843264, 199.89742491],
                  [0., 0., 1.]], dtype=np.float32)
    dist_coeffs = np.array([-3.36421951e-01, -6.95713498e-01, -1.63077197e-03, 1.29975279e-03, 4.62877807e+00],
                           dtype=np.float32)
    pts_3d = np.array([
        [a / 2, a / 2, a / 2],
        [-a / 2, a / 2, a / 2],
        [-a / 2, -a / 2, a / 2],
        [a / 2, -a / 2, a / 2]
    ], dtype=np.float32)
    success, rvec, tvec = cv2.solvePnP(pts_3d, pts_2d, K, dist_coeffs)
    Xc, Yc, Zc = tvec.flatten()
    return Xc, Yc, Zc

def xyz_transaction(x, y, z): # 将相机坐标系的坐标转换到机械臂坐标系的坐标
    camara_location = (0, 182, 257)  # 单位：mm
    theta = math.radians(12) # 两个坐标系z轴的夹角
    # camara_location = (0, 190, 240) #单位：mm
    # theta = math.radians(13) # 两个坐标系z轴的夹角
    m, n, v = camara_location
    sin = math.sin(theta)
    cos = math.cos(theta)
    X = x + m
    Y = n - y*cos + z*sin
    Z = v + y*sin - z*cos
    return X, Y, Z

def tran1(x, y, z):
    camara_location = (0, 182, 260)  # 单位：mm
    angle = 20

    m, n, v = camara_location
    while 1:
        theta = math.radians(angle)  # 两个坐标系z轴的夹角
        sin = math.sin(theta)
        cos = math.cos(theta)
        Z = v + y * sin - z * cos
        a = 1
        if Z - 10 > 0.5:
            angle -= a
        elif Z - 10 < -0.5:
            angle += a
        else:
            X = x + m
            Y = n - y*cos + z*sin
            break
        a /= 2
    return X, Y, Z

def serial_init():
    global ser
    ser = serial.Serial('/dev/ttyS1', 9600, timeout=1)
    if ser.isOpen():
        print(ser.name, ' Open Successed!')
    else:
        print('Open Failed!')

def serial_out(x, y, z, color):
    bytesdata = struct.pack('<4f', float(x), float(y), float(z), float(color))
    combined = b'\xFF' + bytesdata + b'\xFe'
    write_len = ser.write(combined)
    print('hex值为：', combined)
    print('字节串长度为：', write_len)

# def serial_in(length):
#     try:
#         recieved_data = ser.read(length)
#         # print(recieved_data)
#         if recieved_data[0] == 255 :

#             # print('get a requisition')
#             return 1
#         elif recieved_data[0] == 254:
#             print("被中断了！！！！！！！！！！！！！！！！！！")
#             return 2
#         else:
#             print(recieved_data)
#             return False
#     except:
#         print('not recieved any data')
#         return False

def serial_in(length):
    data = ser.read(length)
    return data

def searchposition(camera, color):
    center = []
    while 1:
        _, frame = camera.read()
        cv2.imwrite('0.jpg', frame)
        # 尝试获取坐标，失败打印未找到
        try:
            cl = get_color(frame, color)
            rst = pre_process(cl)
            dots = findcorner(rst)
            X, Y, Z = get_xyz(dots, 23)
            # center.append([X, Y, Z])
            # print(f"方块中心与相机距离：X={X:.2f}cm, Y={Y:.2f}cm, Z={Z:.2f}cm")
            x, y, z = xyz_transaction(float(X), float(Y), float(Z))
            # x, y, z = tran1(float(X), float(Y), float(Z))
            center.append([x, y, z])
            print(f"方块中心相对机械臂：X={x:.2f}mm, Y={y:.2f}mm, Z={z:.2f}mm ")
            #for i in dots:  # 在原图上画多边形的端点
                #result2 = cv2.circle(frame, i[0], 3, (0, 0, 255), -1)
        except:
            print('cube not found')
        #cv2.imshow('frame', frame)

        # 返回处理好的数据
        # cv2.waitKey(2)
        if len(center) > 30:
            break
    center = np.array(center, dtype=np.float32)
    x, y, z = np.median(center, axis=0)
    return x, y, z
