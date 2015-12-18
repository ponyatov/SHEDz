pool = [
    [ 0, 0, 'cnc', 5],
    [ 29, 1, 'pij', 2],
    [ 0, 0, 'bi', 0],
    [ 29, 1, 'vrep', 3],
    [ 28, 6, 'shed', 0]
    ]

def printpool():
    print 'slot\tcount\tname\tprio\n'+'-'*40
    for i in pool:
        print '%s\t%s\t%s\t%s\t'%(i[0],i[1],i[2],i[3])
    print

def shedule(): pass

printpool()
shedule()
printpool()
