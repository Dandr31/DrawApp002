import numpy.matlib 
import numpy as np
import math
#  transform =[[m11,m12,m13],[m21,m22,m23],[m31,m32,m33]]
#
#  C++
#   x' = m11*x + m21*y + dx
#   y' = m22*y + m12*x + dy
#   if (is not affine) {
#      w' = m13*x + m23*y + m33
#      x' /= w'
#      y' /= w'
#  }
'''
 The m31 (dx) and m32 (dy) elements specify horizontal and vertical translation. 
 The m11 and m22 elements specify horizontal and vertical scaling. 
 The m21 and m12 elements specify horizontal and vertical shearing.
 And finally, the m13 and m23 elements specify horizontal and vertical projection, 
 with m33 as an additional projection factor.
'''
#Note that the m13, m23, and m33 elements are set to 0, 0, and 1 respectively
#QTransform(type=TxNone, 11=1 12=0 13=0 21=0 22=1 23=0 31=0 32=0 33=1)
def initialMatrix():
    t = np.array([
                 [1,0,0],
                 [0,1,0],
                 [0,0,1]
                 ])
    return t
def rotationMatrix( angle ):
    pi = math.pi
    a    = pi/180 * angle;
    sina = math.sin(a);
    cosa = math.cos(a);
    #cosa, sina             , -sina, cosa,              0, 0
    #qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy
    # 45 
    # 11=0.707388 12=0.706825 13=0 21=-0.706825 22=0.707388 23=0 31=0 32=0 33=1
    t = np.array([
        [cosa,-sina,0],
        [sina,cosa,0],
        [0,0,1]])
    return t
def translationMatrix(dx , dy):
    t = np.array([
        [1,0,dx],
        [0,1,dy],
        [0,0,1]])
    return t;
def scalingMatrix(scaleX,scaleY):
    t = np.array([
        [scaleX,0,0],
        [0,scaleY,0],
        [0,0,1]])
    return t;
def matmul(a,b):
    return np.array(np.matmul(a,b).tolist())
def getMatrix(scaleX,scaleY,dx,dy,angle,w,h):
    sMatrix = scalingMatrix(scaleX,scaleY)
    tMatrix = translationMatrix(dx,dy)
    rMatrix = rotationMatrix(angle)

    o1Matrix =translationMatrix(w/2,h/2)
    o2Matrix =translationMatrix(-w/2,-h/2)
    m1 = matmul(initialMatrix(),tMatrix)
    m2 = matmul(m1,o1Matrix)
    m3 = matmul(m2,rMatrix)
    m4 = matmul(m3,sMatrix)
    m5 = matmul(m4,o2Matrix)
    return m5
    
def pointMatrix(x,y):
    return np.array([x,y,1])

def transformMap(x,y,transformList):
    p = pointMatrix(x,y)
    t = np.array(transformList)
    newP = np.matmul(t,p)
    return newP.tolist()
def getNdarry(transform):
    return np.array(transform)
#print transformMap(100,100,getMatrix(0.5,0.5,0.0,0.0,80,100,100).tolist())