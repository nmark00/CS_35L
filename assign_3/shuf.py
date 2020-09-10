#!/usr/bin/python


import random, sys
from optparse import OptionParser

class randline:
    def __init__(self, filename, isI=False):
        if not isI:
            f = open(filename, 'r')
            self.lines = f.readlines()
            f.close()
        else:
            self.lines = filename

    def rchooseline(self): #repeats allowed
        return random.choice(self.lines)
    
    def chooseline(self): #takes out lines that have been printed
        temp = random.choice(self.lines)
        self.lines.remove(temp)
        return temp

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE
    or:  %prog -i LO-HI [OPTION]...

Output randomly selected lines from FILE."""

    parser = OptionParser(version=version_msg,
                          usage=usage_msg)
    # -n option
    parser.add_option("-n", "--head-count",
                      action="store", dest="count", default=None,
                      help="output at most COUNT lines")
    # -r option
    parser.add_option("-r", "--repeat",
                      action="store_true", dest="repeat", default=False,
                      help="output lines can be repeated")
    # -i option
    parser.add_option("-i", "--input-range",
                      action="store", dest="lo_hi", default=None,
                      help="treat each number LO through HI as an input line")

    options, args = parser.parse_args(sys.argv[1:]) #parse the input line
    
    # -n option
    def getCount(numlines,repeat):
        if options.count == None: #do nothing to count if -n is not there
            return numlines #return numlines
        if not repeat and int(options.count)>int(numlines): #count>numlines, and no repeat
            return numlines
        try:
            count = int(options.count) #set count = COUNT
        except:
            parser.error("invalid count: {0}". #COUNT is invalid
                         format(options.count))
        if count < 0: #COUNT gotta be positive
            parser.error("negative count: {0}".
                         format(count))
        return count #return updated COUNT
    # -i option
    def getRange(rangeStr):
        if rangeStr == None:
            return []
        try:
            irange = rangeStr.split('-') #split lo-hi on the dash
            lo = irange[0] #lower bound is first num
            hi = irange[-1] #upper bound is last num
        except:
            parser.error("invalid input-range: {0}". 
                         format(rangeStr))
            #must be exactly 2 arguments
            #both hi and lo must be nums
            #hi must be greater than lo
        if (len(irange) == 2 and
                lo.isdigit() and hi.isdigit()
                and int(hi) >= int(lo)):
            return [int(lo),int(hi)] #return the range as an array
        else:
            parser.error("invalid input-range: {0}".
                         format(rangeStr))


    # ---Check operands----
    if len(args) != 1 and options.lo_hi == None: #there should only be one input file (no -i)
        parser.error("wrong number of operands")
    if len(args) != 0 and options.lo_hi != None: #should not have args when -i is present
        parser.error("wrong number of operands")
    
    # ---SETUP -i lines---
    isI = (options.lo_hi != None) #isI == true when -i is invoked
    if isI:# -i is present
        lines = getRange(options.lo_hi) #lines is [lo, hi]
        lines = [str(x)+'\n' for x in list(range(lines[0],lines[-1]+1))] #transform irange into list of str(int)
        generator = randline(lines,isI) #create generator with numbers as file
    else:# -i is NOT present
        input_file = args[0]
        generator = randline(input_file) #initialize randline class with file
        lines = generator.lines #set total num lines
    
    # ---Perform shuf---
    try:
        numlines = len(lines) #set count to numlines by default
        repeat = options.repeat #set repeat if -r is activated
        count = getCount(numlines,repeat) #update count if -n is activated
        
        if not repeat: #if -r is NOT set
            for index in range(count): #print different line COUNT times
                sys.stdout.write(generator.chooseline())
        else: # -r is active
            if options.count == None: #print infinite repeated lines
                while repeat:
                    sys.stdout.write(generator.rchooseline())
            else:
                for index in range(count): #print repeated lines COUNT times
                    sys.stdout.write(generator.rchooseline())

    except OSError as err:
       parser.error("OS error: {0}".format(err))

if __name__ == "__main__":
    main()
