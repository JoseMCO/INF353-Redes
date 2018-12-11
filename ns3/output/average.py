from os import listdir
from os.path import isfile, join
import sys
import csv

row_count = 7;

def readcsv(filename):  
  ifile = open(filename, "rU")
  reader = csv.reader(ifile, delimiter=",", quoting=csv.QUOTE_NONNUMERIC)
  rownum = 0  
  a = []

  for row in reader:
    a.append (row)
    rownum += 1
  
  ifile.close()
  return a

def avgcsv(files):
  n = len(files)
  rows = None
  data = None
  for f in files:
    newf = readcsv(f)
    if data == None:
      data = newf
      rows = len(data)
    else:
      if len(newf) < rows:
        rows = len(newf)
      for i in range(rows-1):
        for r in range(row_count-1):
          data[i+1][r+1]+=newf[i+1][r+1]
  data = data[:rows]
  for i in range(rows-1):
    for r in range(row_count-1):
      data[i+1][r+1]/=(n*1.0)
  return data

def writecsv(data, filename):
  with open(filename, "w") as output:
    writer = csv.writer(output, lineterminator='\n', delimiter=",")
    writer.writerows(data)

path = "."
if len(sys.argv) > 1:
  path = sys.argv[1]
  if not path.endswith("/"):
    path+="/" 

files = [f for f in listdir(path) if isfile(join(path, f))]
files = [f for f in files if f.endswith(".csv")]

leach = [(path+f) for f in files if f.startswith("leach_")]
teen = [(path+f) for f in files if f.startswith("teen_")]
mod = [(path+f) for f in files if f.startswith("mod_")]
adv = [(path+f) for f in files if f.startswith("adv_")]
tam = [(path+f) for f in files if f.startswith("tam_")]

writecsv(avgcsv(leach), path+"leach.csv")
writecsv(avgcsv(teen), path+"teen.csv")
writecsv(avgcsv(mod), path+"mod.csv")
writecsv(avgcsv(adv), path+"adv.csv")
writecsv(avgcsv(tam), path+"tam.csv")