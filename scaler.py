import sys
import pandas as pd
import os
args = sys.argv
arr = pd.read_csv(args[1],header=None).values
w=int(args[2])
arr=arr[1::w,1::w]/w
pd.DataFrame(arr).to_csv(os.path.splitext(args[1])[0]+"_"+args[2]+".csv",header=None,index=None)