pool = [
    # slot count name prio
##    [ 0, 1, 'cnc', 5],
##    [ 29, 1, 'pij', 2],
##    [ 0, 1, 'bi', 1],
##    [ 29, 1, 'vrep', 3],
##    [ 28, 6, 'shed', 1]
        [ 0, 3, 'pij', 5],
        [ 0, 5, 'cnc', 3],
        [ 0, 2, 'bi', 11],
    ]

def printpool(pool):
    print 'slot\tcount\tname\tprio\n'+'-'*40
    for i in pool:
        print '%s\t%s\t%s\t%s\t'%(i[0],i[1],i[2],i[3])
    print

def SF(T):
    return T[1]*T[3]

def shedule():
    pool.sort(key=SF)

printpool(pool)
shedule()
printpool(pool)
