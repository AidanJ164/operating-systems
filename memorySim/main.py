from process import Process
from VSP import VSP
from PAG import PAG
from SEG import SEG

def main():
    size = int(input("Memory size: "))
    policy = int(input("Memory management policy (1 - VSP, 2 - PAG, 3 - SEG): "))
    pageSize = 0
    algorithm = 0
    if policy == 2:
        pageSize = int(input("Page/frame size: "))
    else:
        algorithm = int(input("Fit algorithm (1 - first-fit, 2 - best-fit, 3 - worst-fit): "))

    fileName = input("Workload file: ")
    processes = readfile(fileName)

    if policy == 1:
        mm = VSP(size, processes, algorithm)
    elif policy == 2:
        mm = PAG(size, processes, pageSize)
    else:
        mm = SEG(size, processes, algorithm)

    mm.start()
    print("Average Turnaround Time: " + "{:.2f}".format(mm.getAverageTurnaround()))


def cleanRead(infile):
    line = infile.readline()
    while(line == "\n"):
        line = infile.readline()

    return line

def readfile(fileName):
    processes = []
    infile = open(fileName)

    # get number of processes
    numProcesses = int(cleanRead(infile))
    for i in range(numProcesses):
        # get pid of process
        pid = int(cleanRead(infile))

        # get start time and lifetime of process
        temp = cleanRead(infile)
        temp = temp.split()
        startTime = int(temp[0])
        lifeTime = int(temp[1])

        # get the size of the memory spaces needed for process
        blocks = cleanRead(infile)
        blocks = blocks.split()
        blocks = blocks[1:]
        blocks = [eval(i) for i in blocks]

        processes.append(Process(pid, startTime, lifeTime, blocks))

    return processes

if __name__ == '__main__':
    main()