import glob
import xml.etree.ElementTree as ET
import pandas as pd
import numpy as np
from io import StringIO
from multiprocessing import Pool
import multiprocessing as multi

class Grid:
    def __init__(self,path,n,m):
        path = path+"/"+self.__generateFileName(n,m)+"-*.xml"
        file = glob.glob( path )
        if len(file) == 0:
            print(path," is not found")
            self.tupleList = np.zeros((750,1125))
            self.topLeftLatLong=[0,0]
            return
        tree = ET.parse(file[0])
        self.root = tree.getroot()    
        self.topLeftLatLong=self.__findLatLong(self.root)
        df = pd.read_csv(StringIO(self.__findTupleList(self.root).text),header=None)
        arr=df[1]
        if arr.size != 750*1125:
            print(file,": Size is not ",750*1125,":",arr.size)
            #足りなかったら0埋め
            arr=np.concatenate([arr,np.zeros(750*1125-arr.size)])
       
        self.tupleList = np.reshape(arr,(750,-1))
    def __findTupleList(self,root):
        return self.__findByTag(root,'tupleList')
    def __findLatLong(self,root):
        temp=self.__findByTag(root,'Envelope')
        lowerCorner=self.__findByTag(temp,"lowerCorner").text
        upperCorner=self.__findByTag(temp,"upperCorner").text
        return [ float(lowerCorner.split(' ')[1]),float(upperCorner.split(' ')[0])]
    def __findByTag(self,root,tag):
        for item in root.iter():
            if tag in item.tag:
                return item
        return None
    def __generateFileName(self,n,m):
        return 'FG-GML-{}-{:02}'.format(n//100,n%100,m)
#         return 'FG-GML-{}-{:02}-{:02}-DEM5A'.format(n//100,n%100,m)

class Mesh:
    def __init__(self,path,n):
        a=[]
        folder = path+'/'+self.__generateFolderName(n)
        grid=Grid(folder,n,0)
        self.tupleList=grid.tupleList
        self.topLeftLatLong=grid.topLeftLatLong
#         for i in range(10):
#             b=[]
#             for j in range(10):
#                 b.append(Grid(folder,n,(9-i)*10+j).tupleList)
#             a.append(b)
#         self.tupleList=np.block(a)
       
    def __generateFolderName(self,n):
        return 'FG-GML-{}-{:02}-DEM10B'.format(n//100,n%100)
def process(arg):
    return Mesh(arg[0],arg[1])   
class Map:
    def __init__(self,folder,nums):
        h = len(nums)
        w = len(nums[0])
        a=[]
        p = Pool(multi.cpu_count())  
        for i in range(h):
            b=p.map(process, list(map(lambda x: [folder,x] ,nums[i])))
            a.append(list(map(lambda x: x.tupleList ,b)))
            if i ==0:
                self.topLeftLatLong=b[0].topLeftLatLong 
        self.tupleList=np.block(a)