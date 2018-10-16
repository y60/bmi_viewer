import xml.etree.ElementTree as ET
import pandas as pd
import numpy as np
import sys
import os

ns="{http://www.topografix.com/GPX/1/1}"
args = sys.argv
tree = ET.parse(args[1])
trkseg = tree.getroot().find(ns+"trk").find(ns+"trkseg")
trkpts = trkseg.findall(ns+"trkpt")
with open(os.path.splitext(os.path.basename(args[1]))[0]+".csv",mode='w') as f:
    for trkpt in trkpts:
        f.write(trkpt.get('lat')+","+trkpt.get('lon')+","+trkpt.find(ns+"ele").text+"\n")
