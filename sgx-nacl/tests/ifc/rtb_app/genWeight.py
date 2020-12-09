import sys
import random

def genWeight(featNum, inputDir):

    fi = open("../make-ipinyou-data/"+inputDir+"/train.yzx.txt.lr.weight", 'r')
    fo = open("./data/weight_"+inputDir+".txt", 'w')

    while True:
        line = fi.readline()
        if not line: break

        if int(line.split()[0]) > featNum:
            break
        fo.write(line)


    fi.close()
    fo.close()


if __name__ == "__main__":
    featNum = input('Enter # of Feature: ')
    featNum = int(featNum)

    dirs = ["1458", "3358", "3386", "3427", "3476"]

    for inputDir in dirs:
        genWeight(featNum, inputDir);

    print("Test weight generated ---> test_wieght.txt")

