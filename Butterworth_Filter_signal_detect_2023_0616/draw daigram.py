import numpy as np
import matplotlib.pyplot as plt


def scale_by_power_of_two(bCoefficients, aCoefficients, exponent):
    bCoefficients = [int(num * (2 ** exponent)) for num in bCoefficients]
    aCoefficients = [int(num * (2 ** exponent)) for num in aCoefficients]
    return bCoefficients, aCoefficients



def display():
    # 示例数据
    array1 = np.array([917, 955, 988,1017,1050,1091,1109,1184,1146,1185,1267,1330,1292,1329,        
 1409,1427,1429,1465,1512,1560,1636,1677,1630,1691,1769,1811,1771,1809,        
 1845,1921,1944,1969,2034,2026,2074,2143,2140,2162,2213,2244])
    #array2 = np.array([915,1318,1333,1416,1441,1091,1109,1184,1146,1185,1267,1330,1292,1329,1409,1427,1429,1465,1512,1560,1636,1677,1630,1691,1769,1811,1771,1809,1845,1921,1944,1969,2034,2026,2074,2143,2545,2611,2636,2244])
    #array3 = np.array([35,26,28,21,15,7,14,18,-2,7,176,653,314,27,2,11,-5,-10,74,21,62,7,1,18,22,4,22,9,17,5,-5,7,19,5,5,-6,15,-2])


    #array1 = np.array([13,  8, 21,  8, 17,  6,  8, 16, 14,  3, 64, 72, 74, 76, 16,115, 75, 74,72, 12, 19,  8, 13, 22, 24, 15, 17, 16, 11, 14, 12,  9,  4,  4,  9,  8, 11, 11])
    #array1= np.array([13,8,21,8,17,6,8,16,14,3,64,266,183,208,16,115,316,292,170,12,19,8,13,22,24,15,17,16,11,14,12,9,4,4,9,8,11,11])
    # array2 = np.array([13,8,21,8,17,6,8,16,14,3,26,26,28,26,16,25,31,29,27,12,19,8,13,22,24,15,17,16,11,14,12,9,4,4,9,8,11,11])
    #array3 = np.array([27,29,31,34,35,37,38,38,39,39,39,38,38,38,37,36,36,35,34,33,33,32,31,30,30,29,28,28,27,26,25,25,24,23,23,22,21,20])
    #array4 = np.array([25,225,463,451,375,53,23,33,20,9,8,25,21,23,27,19,33,18,26,35,15,8,9,26,13,17,32,19,34,22,28,29,28,25,17,21,24,22])

    # 绘制图形
    plt.plot(array1, label='Line 14')
    #plt.plot(array2, label='Line 16')
    #plt.plot(array3, label='Line python')
    #plt.plot(array4,label= 'data')

    # 添加图例
    plt.legend()

    # 显示图形
    plt.show()


def printlen():
    lenarr = [1315-400,1318,1333,1416,1441,1091,1109,1184,1146,1185,1267,1330,1292,1329,1409,1427,1429,1465,1512,1560,1636,1677,1630,1691]
    print (len(lenarr))

def calculateAB():
    bCoefficients = [0.93552671,-1.87105341,0.93552671]
    aCoefficients = [1.0,-1.86689228,0.87521455]

    exponent = 14
    scaled_bCoefficients, scaled_aCoefficients = scale_by_power_of_two(bCoefficients, aCoefficients, exponent)

    print(f"Scaled bCoefficients: {scaled_bCoefficients}")
    print(f"Scaled aCoefficients: {scaled_aCoefficients}")

def calculateBA():
    input_sequence = [1315, 400, 1318, 1333, 1416, 1441, 1091, 1109, 1184, 1146, 1185, 1267, 1330, 1292, 1329, 1409, 1427, 1429, 1465, 1512, 1560, 1636, 1677, 1630, 1691]
    scaled_input_sequence = [int(x / 80) for x in input_sequence]
    print(scaled_input_sequence)


if __name__ == "__main__":
    calculateAB()
    #display()
    #printlen()
    #calculateBA()

