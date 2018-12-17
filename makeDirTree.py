import os
import sys

#Simple sript to create recommended  folder strcuture for automatic labeler
#usage
if len(sys.argv)<3:
    print("wrong Usage")
    print("python3 makeDirTree.py path/to/obj.names /path/to/images")
    exit()

nameFile = sys.argv[1]
folder = sys.argv[2]

with open(nameFile) as names:
    content = names.readlines()

content = [x.strip() for x in content]

for name in content:
    relPth = folder + name
    if not os.path.exists(relPth):
        os.makedirs(relPth)
