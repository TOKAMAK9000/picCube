#导入所需库文件
import cv2
import random
import numpy as np

#导入图片并显示
img = cv2.imread(r'C:\Users\TOKAMAK\Desktop\new\texture.png')
cv2.imshow("image",img)

#定义常量，生成随机数
black = (0,0,0)
linePositionX=[247,352,250,350,245,356,232,370,220,380,205,395,200,400,195,403,195,403,195,403]
linePositionY=[310,530,155,547,30,550,30,550,172,545]
randSetX=np.random.randint(-7,8,100)
randSetY=np.random.randint(-6,7,200)
rawX=50
rawY=205
count=0

#画横向的直线
for i in range(0,len(linePositionX),2):
    sup=(linePositionX[i+1]-linePositionX[i])//30
    x1_value=linePositionX[i]
    x2_value=linePositionX[i]
    y1_value=rawX
    y2_value=rawX
    for j in range(0,sup):

        x1_value=x2_value
        x2_value=linePositionX[i] + (j + 1) * 30 + randSetX[count]
        count+=1

        y1_value=y2_value
        y2_value=rawX+randSetX[count]
        count+=1

        cv2.line(img,(x1_value,y1_value),(x2_value,y2_value),black,3)

    cv2.line(img,(x2_value,y2_value),(linePositionX[i+1],rawX),black,3)
    rawX += 50


#画纵向直线
count=0
for i in range(0,len(linePositionY),2):
    sup=(linePositionY[i+1]-linePositionY[i])//30
    x1_value=rawY
    x2_value=rawY
    y1_value=linePositionY[i]
    y2_value=linePositionY[i]
    for j in range(0,sup):

        x1_value=x2_value
        x2_value=rawY+randSetY[count]
        count+=1

        y1_value=y2_value
        y2_value=linePositionY[i] + (j + 1) * 30 + randSetY[count]
        count+=1

        cv2.line(img,(x1_value,y1_value),(x2_value,y2_value),black,3)

    cv2.line(img,(x2_value,y2_value),(rawY,linePositionY[i+1]),black,3)
    rawY += 40


cv2.imshow("image",img)
cv2.imwrite(r"C:\Users\TOKAMAK\Desktop\new\image.png",img)
cv2.waitKey(0)
cv2.destroyAllWindows()

