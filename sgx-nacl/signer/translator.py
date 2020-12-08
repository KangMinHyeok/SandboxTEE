f = open("out.txt",'r')
line = f.readline()
o = open("siglist.txt",'w')

siglist = list()
for i in range(0,len(line),2):
    
    if i%32==30:
        siglist.append('0x'+str(line[i:i+2])+'\n')
    else:
        siglist.append('0x'+str(line[i:i+2]))
# print(siglist)
o.write(', '.join(siglist))

f.close()
o.close()