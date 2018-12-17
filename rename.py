import os
import sys

##Simple script to massively rename images to a more conveniente
##Scheme to be used in automatic labelling.

##Usage python rename.py folder/to/images

folder = os.path.abspath(sys.argv[1])
walker = os.walk(folder)
for w in walker:
    objName = os.path.basename(os.path.normpath(w[0]))
    dirPth = w[0]
    imgsPth = w[2]

    for i, f in enumerate(imgsPth):
        objPth = w[0] +"/"+f
        finalPth = w[0] + "/" + objName + str(i) + ".jpg"
        print("Changin {} to {}".format(objPth, finalPth))
        os.rename(objPth,finalPth)

# files = os.listdir(folder)
# for i, file in enumerate(files):
#     objPth = os.path.abspath(folder+file)
#
#     dirPth = os.path.dirname(objPth)
#     finalPth = dirPth + "/" + objName + str(i) + ".jpg"
#     print("Changin {} to {}".format(objPth, finalPth))
#     #os.rename(objPth,finalPth)
