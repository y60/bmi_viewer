import glob
import xml.etree.ElementTree as ET
import pandas as pd
import numpy as np
from io import StringIO

class Grid:
    def __init__(self,path,n,m):
        path = path+"/"+self.__generateFileName(n,m)+"-*.xml"
        file = glob.glob( path )
        if len(file) == 0:
            print(path," is not found")
            self.tupleList = np.zeros((750,1125))
            return
        tree = ET.parse(file[0])
        self.root = tree.getroot()    
        df = pd.read_csv(StringIO(self.__findTupleList(self.root).text),header=None)
        arr=np.array(df[1])
        if df[1].size != 750*1125:
            print(file,": Size is not ",750*1125,":",df[1].size)
            #足りなかったら0埋め
            arr=np.concatenate([np.array(df[1]),np.zeros(750*1125-df[1].size)])
        else:
            arr=np.array(df[1])
        self.tupleList  = np.reshape(arr,(750,-1))
    def __findTupleList(self,root):
        for item in root.iter():
            if "tupleList" in item.tag:
                return item
        return None
    def __generateFileName(self,n,m):
        return 'FG-GML-{}-{:02}'.format(n//100,n%100,m)
#         return 'FG-GML-{}-{:02}-{:02}-DEM5A'.format(n//100,n%100,m)

class Mesh:
    def __init__(self,path,n):
        a=[]
        folder = path+'/'+self.__generateFolderName(n)
        self.tupleList=Grid(folder,n,0).tupleList
#         for i in range(10):
#             b=[]
#             for j in range(10):
#                 b.append(Grid(folder,n,(9-i)*10+j).tupleList)
#             a.append(b)
#         self.tupleList=np.block(a)
        
    def __generateFolderName(self,n):
        return 'FG-GML-{}-{:02}-DEM10B'.format(n//100,n%100)
    
class Map:
    def __init__(self,folder,nums):
        h = len(nums)
        w = len(nums[0])
        a=[]
        for i in range(h):
            b=[]
            for j in range(w):
                b.append(Mesh(folder,nums[i][j]).tupleList)
            a.append(b)
        self.tupleList=np.block(a)