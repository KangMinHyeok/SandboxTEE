import sys
import random


def genRandom(featNum):

    myLen = len(str(featNum))+1

    myRange = random.randrange(1, myLen)
    result = str(random.randrange(1, 10))
    for i in range(myRange-1):
        result = result + str(random.randrange(0, 10))

    if int(result) < featNum:
        result = result
        return result
    else:
        return genRandom(featNum)

def genTestSet(featNum, recordNum, output):
    f = open(output, 'w')

    for i in range(recordNum):
        exist = set([])
        profile = str(random.randrange(1, 301)) + ' '
        profileLen = random.randrange(10, 46)
        for j in range(profileLen-1):
            item = genRandom(featNum)
            while item in exist:
                item = genRandom(featNum)
            exist.add(item)
            profile = profile + item  + ' '
        item = genRandom(featNum)
        profile = profile + item + '\n'
        f.write(profile)
    f.close()


if __name__ == "__main__":
    featNum = input('Enter # of Features: ')
    recordNum = input('Enter # of Records: ')
    featNum = int(featNum)
    recordNum = int(recordNum)

    testOutput = './data/test_input.txt'
    genTestSet(featNum, recordNum, testOutput)

    print("Test data generated ---> data/test_input.txt}")

