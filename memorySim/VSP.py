from MemoryManager import MemoryManager
class VSP(MemoryManager):
    def __init__(self, size, processes, algorithm):
        super().__init__(size, processes)
        self.algorithm = algorithm

        # space[startIndex, endIndex, occupiedBy, totalSize]
        self.memorySpace = [[0, size - 1, "Hole", size]]

    def checkForSpace(self, process):
        for space in self.memorySpace:
            if space[2] == "Hole" and space[3] >= process.totalSpace:
                return True
        return False

    def printProcess(self, process):
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}")

    def findSpaceInMemory(self, process):
        worst = []
        best = []
        first = []
        largestSize = 0
        bestSize = 0
        isFirst = True
        for space in self.memorySpace:
            if space[2] == "Hole" and space[3] >= process.totalSpace:
                if isFirst:
                    first = space
                    best = space
                    worst = space
                    largestSize = space[3]
                    bestSize = space[3]
                    isFirst = False
                if space[3] < bestSize:
                    best = space
                    bestSize = space[3]
                if space[3] > largestSize:
                    worst = space
                    largestSize = space[3]
        return first, best, worst

    def addToMemory(self, process):
        first, best, worst = self.findSpaceInMemory(process)
        spaceToFill = first
        if self.algorithm == 2:
            spaceToFill = best
        elif self.algorithm == 3:
            spaceToFill = worst

        index = self.memorySpace.index(spaceToFill)
        start = spaceToFill[0]
        end = spaceToFill[1]
        self.memorySpace.insert(index, [start, start + process.totalSpace - 1, "Process " + str(process.pid), process.totalSpace])
        # need to check if the start of next space is the end of the one we just created
        if start + process.totalSpace < self.size and process.totalSpace != spaceToFill[3]:
            self.memorySpace.insert(index + 1, [start + process.totalSpace, end, "Hole", end - start - process.totalSpace + 1])

        self.memorySpace.remove(spaceToFill)

        self.curQueue.append(process)
        self.inQueue.remove(process)
        process.finishTime = self.time + process.lifeTime
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def freeMemory(self, process):
        #find process to free
        index = -1
        for i in range(len(self.memorySpace)):
            if self.memorySpace[i][2] != "Hole" and int(self.memorySpace[i][2][8:]) == process.pid:
                index = i

        if index == -1:
            return

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

        self.memorySpace.insert(index, [start, end, "Hole", end - start + 1])
        self.memorySpace.remove(processToRemove)
        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime