__all__ = ['parse_robolog', 'parse_latest']

import csv, os

'''
This python script requires Matplotlib and Numpy to be installed.
It is designed for Python 2.6 or 2.7
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
		try:
		    del ln['Seconds']
		except: pass
		try:
            del ln['Nanoseconds']
		except: pass
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

def graph_robolog(dol, filename, path):
    try:
        import matplotlib.pyplot as plt
    except:
        print "Error: matplotlib.pyplot failed to load.\nCheck to make sure it is installed."
    times = dol['Elapsed Time']
    del dol['Elapsed Time']
    i=0
    xlen = 1
    ylen = len(dol)
    for key in dol.keys():
        plt.xlabel('Elapsed Time')
        plt.ylabel(key)
        plt.plot(times, dol[key])
        plt.savefig(filename + "__" + key + ".png")
        plt.clf()
    for key in dol.keys():
        i += 1
        plt.subplot(ylen,xlen,i)
        plt.xlabel('Elapsed Time')
        plt.ylabel(key)
        plt.plot(times, dol[key])
    plt.savefig(filename + ".png")
    plt.clf()
    

def parse_latest():
    global max_foldername, filename
    base_foldername = raw_input("What is the base log folder name?\n==>")
    # The initial timestamp of all 0s
    # All timestamps are guaranteed to be greater than this!
    max_foldername = "00000000.00.00.00"
    for filename in os.listdir(base_foldername):
        # Find the latest files
        if not filename=="output":
            if filename > max_foldername:
                max_foldername = filename
    address = os.path.join(base_foldername, max_foldername)
    pngaddress = os.path.join(base_foldername, max_foldername, 'output')

    x = os.path.join(base_foldername, max_foldername, 'output')
    
    if os.path.isdir(os.path.join(base_foldername, max_foldername, 'output'))==False:
        os.mkdir(os.path.join(base_foldername, max_foldername, 'output'))
    else:
        print("Directory `" + pngaddress + "` already exists.")
        go = raw_input("Redraw graphs? (Y/N)\n==>")
        if go == "N" or go == "n":
            return
    for filename in os.listdir(address):
        # Check each file in the latest folder
        # Attempt to graph each file
        r = os.path.join(address, filename)
        z = os.path.splitext(filename)
        if z[1] == ".csv":    
            try:
                lod = parse_robolog(r)
            except:
                diagnostics(pngaddress)
                raw_input();
                return
            data = lod2dol(lod)
            f = os.path.join(pngaddress, filename)
            graph_robolog(data, f, x)
            lod = ()
            data = {}
    print "\n\nGraphing complete. Outputted graphs to " + pngaddress
        
def diagnostics(pngaddress):
    global linenum, ln, name, filename, log
    # Error in the CSV
    log = open(os.path.join(pngaddress, 'error.log'), 'w')
    log.write("Error: invalid data on line " + str(linenum))
    log.write("\nIn file: " + str(max_foldername + '\\' + filename))
    log.write("\nWith key: " + keyname)
    log.close()
    # log.write("\tLine contents:")
    # log.write(ln)
    print("\tError: invalid data on line " + str(linenum))
    print("\tIn file: " + str(max_foldername + '\\' + filename))
    print("\tWith key: " + keyname)
    # print("\tLine contents:")
    # print(ln)
    print("\n\n\tError was outputted to file \"error.log\" \n\tat " + pngaddress + "\\error.log")

if __name__ == "__main__":
    parse_latest()
