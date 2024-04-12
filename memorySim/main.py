from process import Process
from VSP import VSP
from PAG import PAG
from SEG import SEG

def main():
    size = int(input("Memory size: "))
    policy = int(input("Memory management policy (1 - VSP, 2 - PAG, 3 - SEG): "))

    # get page size or algorithm to use, depending on what kind of memory manager to use
    pageSize = 0
    algorithm = 0
    if policy == 2:
        pageSize = int(input("Page/frame size: "))
    else:
        algorithm = int(input("Fit algorithm (1 - first-fit, 2 - best-fit, 3 - worst-fit): "))

    # get and read file
    fileName = input("Workload file: ")
    processes = readfile(fileName)

    # create the memory manager
    if policy == 1:
        mm = VSP(size, processes, algorithm)
    elif policy == 2:
        mm = PAG(size, processes, pageSize)
    else:
        mm = SEG(size, processes, algorithm)

    # start the memory manager
    mm.start()

    averageTurnaroundTime = "{:.2f}".format(mm.getAverageTurnaround())
    if averageTurnaroundTime[-1] == "0":
        averageTurnaroundTime = averageTurnaroundTime[:-1]
    print("Average Turnaround Time: " + averageTurnaroundTime)


def cleanRead(infile):
    """
    This function makes sure that the line being read in is not just an empty line.

    :param infile: file to read from
    :return: string - next line in file that contains content
    """
    line = infile.readline()
    # while line is empty, get the next line
    while(line == "\n"):
        line = infile.readline()

    return line

def readfile(fileName):
    """
    Takes in a name of a workload file that contains the process information to manage.
    It will read in each process and store the necessary information about each process in a Process
    class and append it to a list of all the processes.

    :param fileName: workload file
    :return: list of processes
    """
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