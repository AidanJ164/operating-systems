from MemoryManager import MemoryManager
class VSP(MemoryManager):
    def __init__(self, size, processes, algorithm):
        """
        This class holds the Variable-Size Partitioning memory management method. The memory allocated for a process
        must all be sequential and cannot be broken up into pieces.
        :param size: total size of the memory
        :param processes: list of all processes to run
        :param algorithm: integer representing which fit algorithm to run (1 - first-fit, 2 - best-fit, 3 - worst-fit)
        """
        super().__init__(size, processes)
        self.algorithm = algorithm

        # space[startIndex, endIndex, occupiedBy, totalSize]
        self.memorySpace = [[0, size - 1, "Hole", size]]

    def addToMemory(self, process):
        """
        Add process to memory.
        :param process: process to add
        """
        # find the first, best, and worst space for the process to be allocated in
        first, best, worst = self.findSpaceInMemory(process)
        # decide which space to use based on algorithm
        spaceToFill = first
        if self.algorithm == 2:
            spaceToFill = best
        elif self.algorithm == 3:
            spaceToFill = worst

        # get index into the memory list where to allocate process
        index = self.memorySpace.index(spaceToFill)
        start = spaceToFill[0]
        end = spaceToFill[1]
        # allocate memory for process
        self.memorySpace.insert(index, [start, start + process.totalSpace - 1, "Process " + str(process.pid), process.totalSpace])
        # need to check if the process filled the whole hole
        if start + process.totalSpace < self.size and process.totalSpace != spaceToFill[3]:
            self.memorySpace.insert(index + 1, [start + process.totalSpace, end, "Hole", end - start - process.totalSpace + 1])

        self.memorySpace.remove(spaceToFill)

        # update the current and input queue
        self.curQueue.append(process)
        self.inQueue.remove(process)
        process.finishTime = self.time + process.lifeTime
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def checkForSpace(self, process):
        """
        Check to see if the process can fit into memory
        :param process: process to check
        :return: boolean - true if memory can be allocated for process
        """
        for space in self.memorySpace:
            if space[2] == "Hole" and space[3] >= process.totalSpace:
                return True
        return False

    def findSpaceInMemory(self, process):
        """
        Finds the spot in memory for the process to fit in to.
        :param process: process to allocate
        :return: space in memory to fill
        """
        worst = []
        best = []
        first = []
        largestSize = 0
        bestSize = 0
        isFirst = True
        # Run through each space in memory
        for space in self.memorySpace:
            # If space is a hole and is big enough, see if we should use it
            if space[2] == "Hole" and space[3] >= process.totalSpace:
                # Use the first hole that's big enough to initialize
                if isFirst:
                    first = space
                    best = space
                    worst = space
                    largestSize = space[3]
                    bestSize = space[3]
                    isFirst = False
                # If the space is big enough and smaller than the previous best size, replace the best
                if space[3] < bestSize:
                    best = space
                    bestSize = space[3]
                # If the space is bigger than the last largest, replace the largest
                if space[3] > largestSize:
                    worst = space
                    largestSize = space[3]
        return first, best, worst

    def freeMemory(self, process):
        """
        Frees the passed in parameter from memory.
        :param process: process to free
        """
        # find index of process to free
        index = -1
        for i in range(len(self.memorySpace)):
            if self.memorySpace[i][2] != "Hole" and int(self.memorySpace[i][2][8:]) == process.pid:
                index = i

        # if not found, return
        if index == -1:
            return

        # get the actual space in memory, where the process starts, and where the process ends
        processToRemove = self.memorySpace[index]
        start = processToRemove[0]
        end = processToRemove[1]

        # check if space before and after free memory are holes
        if index - 1 >= 0 and self.memorySpace[index - 1][2] == "Hole":
            start = self.memorySpace[index - 1][0]
            self.memorySpace.remove(self.memorySpace[index - 1])
            index -= 1
        if index + 1 < len(self.memorySpace) and self.memorySpace[index + 1][2] == "Hole":
            end = self.memorySpace[index + 1][1]
            self.memorySpace.remove(self.memorySpace[index + 1])

        # Fill space with a hole
        self.memorySpace.insert(index, [start, end, "Hole", end - start + 1])
        # Free the memory the process took up
        self.memorySpace.remove(processToRemove)
        # Update the current queue
        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime

    def printProcess(self, process):
        """
        Print the passed in process to the console.
        :param process: process to print
        :return:
        """
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}")