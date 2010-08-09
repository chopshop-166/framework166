__all__ = ['parse_robolog', 'parse_latest']

import csv, os

'''
This python script requires Matplotlib and Numpy to be installed
'''

def parse_robolog(filename):
    # Seperate the robot log into a list of dicts
    global linenum, ln, keyname
    print("Checking " + filename)
    reader = csv.DictReader(open(filename))
    names = reader.fieldnames
    # Initialize all the variables we're going to use
    linelist = []
    listsize = 0
    linenum = 0
    keyname = ""
    ln = []
    for ln in reader:
        linelist.append(ln)
        listsize += 1
    for ln in linelist:
        linenum += 1
        for keyname in names:
            ln[keyname] = float(ln[keyname])
        #Remove extraneous values that we're not using
        del ln['Seconds']
        del ln['Nanoseconds']
    return linelist

def lod2dol(lod):
    '''
    Convert a list of dicts into a dict of lists
    '''
    dol = {}
    for key in lod[0]:
        dol[key] = list()
    for dictset in lod:
        for key in dictset:
            dol[key].append(dictset[key])
    return dol

def graph_robolog(dol, filename):
    try:
        import matplotlib.pyplot as plt
    except:
        print "Error: matplotlib.pyplot failed to load.\nCheck to make sure it is installed."
    times = dol['Elapsed Time']
    del dol['Elapsed Time']
    i=0
    keyrange = 0
    xlen = 1
    ylen = len(dol)
    for key in dol.keys():
        keyrange += 1
    for key in dol.keys():
        i += 1
        plt.subplot(ylen,xlen,i)
        plt.xlabel('Elapsed Time')
        plt.ylabel(key)
        plt.plot(times, dol[key])
    plt.savefig(filename)
    plt.clf()
    
def parse_latest():
    global max_foldername, filename
    base_foldername = raw_input("What is the base log folder name?\n==>")
    output_foldername = raw_input("What is the output folder name?\n==>")
    # The initial timestamp of all 0s
    # All timestamps are guaranteed to be greater than this!
    max_foldername = "00000000.00.00.00"
    for filename in os.listdir(base_foldername):
        # Find the latest files
        if filename > max_foldername:
            max_foldername = filename
    address = base_foldername + "\\" + max_foldername
    pngaddress = output_foldername + "\\" + max_foldername
    try:
        os.mkdir(pngaddress)
    except:
        print("Directory `" + pngaddress + "` already exists.")
        go = raw_input("Redraw graphs? (Y/N)\n==>")
        if go != "Y":
            return
    for filename in os.listdir(address):
        # Check each file in the latest folder
        # Attempt to graph each file
        try:
            lod = parse_robolog(address + "\\" + filename)
        except:
            diagnostics()
            raw_input();
            return
        data = lod2dol(lod)
        graph_robolog(data, pngaddress + "\\" + filename + ".png")
        lod = ()
        data = {}
        
def diagnostics():
    global linenum, ln, name, filename
    # Error in the CSV
    print("\tError: invalid data on line " + str(linenum))
    print("\tIn file: " + str(max_foldername + '\\' + filename))
    print("\tWith key: " + keyname)
    print("\tLine contents:")
    print(ln)

if __name__ == "__main__":
    parse_latest()
