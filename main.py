import cv2
import numpy as np
import functions as f
import time
import serial
import serial.tools.list_ports


blue = [[90, 130], [200, 255], [0, 255]]
green = [[50, 100], [150, 255], [120, 255]]
orange = [[0, 18] , [100, 255], [214, 255]]
colors = [green, orange]
f.serial_init()
camera = cv2.VideoCapture(9)
# img = cv2.imread('0.jpg')
while 1:
    print(1111111)
    center = []
    t = 0
    J = False
    rcv = f.serial_in(1) # 带STM32时的代码
    # rcv = b'\xff' # 摄像头测试时的代码
    if rcv != b'':
        if rcv[0] == 255:
            print('get a reqisition')
            # 识别颜色
            for i in range(20):

                _, frame = camera.read()
                cv2.imwrite('1.jpg', frame)
            ind = f.knowcolor(frame, colors[0])
            color = colors[ind]
            print(ind+1)
            rcv = f.serial_in(1)
            while 1:
                # if f.serial_in(1)==2:
                #     break
                _, frame = camera.read()
                cv2.imwrite('0.jpg', frame)
                # 尝试获取坐标，失败打印未找到
                try:
                    cl = f.get_color(frame, color)
                    rst = f.pre_process(cl)
                    dots = f.findcorner(rst)
                    X, Y, Z = f.get_xyz(dots, 23)
                    # center.append([X, Y, Z])
                    # print(f"方块中心与相机距离：X={X:.2f}cm, Y={Y:.2f}cm, Z={Z:.2f}cm")
                    x, y, z = f.xyz_transaction(float(X), float(Y), float(Z))
                    # x, y, z = f.tran1(float(X), float(Y), float(Z))
                    center.append([x, y, z])
                    print(f"方块中心相对机械臂：X={x:.2f}mm, Y={y:.2f}mm, Z={z:.2f}mm ")
                    #for i in dots:  # 在原图上画多边形的端点
                        #result2 = cv2.circle(frame, i[0], 3, (0, 0, 255), -1)
                except:
                    print('cube not found')
                #cv2.imshow('frame', frame)

                # 在被中断时，返回计时器1.5秒内的中位数
                # key = cv2.waitKey(50)
                # if key != -1:
                #     J = True
                # if J == True and t >= 1500:
                #     break
                # if t > 1600:
                #     t = 0
                #     center = [] 

                # 返回处理好的数据
                cv2.waitKey(2)
                if len(center) > 30:
                    break
            #cv2.destroyAllWindows()

            center = np.array(center, dtype=np.float32)
            x, y, z = np.median(center, axis=0)
            print('串口输出: {}, {}, {}, {}'.format(x, y, z, ind+1))
            if y>224:
                y = 224
            if y<116:
                y = 116
            f.serial_out(x, z, y, ind+1)

            while 1:
                try:
                    if f.serial_in(1)[0] == 254:
                        print('被停止拍照')
                        break
                except:
                    print('STM32 RESETED')
    
            
            # time.sleep(15)
    else:
        print('没收到')

