from MemoryManager import MemoryManager

class SEG(MemoryManager):
    def __init__(self, size, processes, algorithm):
        super().__init__(size, processes)
        self.algorithm = algorithm

        # space[startIndex, endIndex, occupiedBy, totalSize]
        self.memorySpace = [[0, size - 1, "Hole", size]]

    def getFreeSpaceSizes(self):
        freeSpaces = []
        for space in self.memorySpace:
            if space[2] == "Hole":
                freeSpaces.append(space[1] - space[0] + 1)
        return freeSpaces

    def checkFirstFit(self, process, freeSpaces):
        freeSpaceSizes = freeSpaces
        for segment in process.memSpace:
            index = 0
            fit = False
            while index < len(freeSpaceSizes) and not fit:
                if segment <= freeSpaceSizes[index]:
                    freeSpaceSizes[index] -= segment
                    fit = True
                else:
                    index += 1
            if not fit:
                return False
        return True

    def findSpaceInMemory(self, segment):
        worst = []
        best = []
        first = []
        largestSize = 0
        bestSize = 0
        isFirst = True
        for space in self.memorySpace:
            if space[2] == "Hole" and space[3] >= segment:
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
                if space[3] >= largestSize:
                    worst = space
                    largestSize = space[3]
        return first, best, worst

    def checkBestFit(self, process, freeSpaces):
        freeSpaceSizes = freeSpaces
        for segment in process.memSpace:
            bestIndex = -1
            bestSize = -1

            for i in range(len(freeSpaceSizes)):
                space = freeSpaceSizes[i]
                if space >= segment and (space < bestSize or bestIndex == -1):
                    bestIndex = i
                    bestSize = space
            if bestIndex == -1:
                return False
            freeSpaceSizes[bestIndex] -= segment
        return True

    def checkWorstFit(self, process, freeSpaces):
        freeSpaceSizes = freeSpaces
        for segment in process.memSpace:
            largestIndex = -1
            largestSize = -1

            for i in range(len(freeSpaceSizes)):
                space = freeSpaceSizes[i]
                if space >= segment and (space > largestSize or largestIndex == -1):
                    largestIndex = i
                    largestSize = space
            if largestIndex == -1:
                return False
            freeSpaceSizes[largestIndex] -= segment
        return True
    def checkForSpace(self, process):
        freeSpaceSizes = self.getFreeSpaceSizes()
        if len(freeSpaceSizes) == 0:
            return False

        if self.algorithm == 1:
            return self.checkFirstFit(process, freeSpaceSizes)
        elif self.algorithm == 2:
            return self.checkBestFit(process, freeSpaceSizes)
        else:
            return self.checkWorstFit(process, freeSpaceSizes)

    def addToMemory(self, process):
        segmentNum = 0
        for segment in process.memSpace:
            first, best, worst = self.findSpaceInMemory(segment)
            spaceToFill = first
            if self.algorithm == 2:
                spaceToFill = best
            elif self.algorithm == 3:
                spaceToFill = worst

            index = self.memorySpace.index(spaceToFill)
            start = spaceToFill[0]
            end = spaceToFill[1]
            self.memorySpace.insert(index, [start, start + segment - 1, "Process " + str(process.pid),
                                            segmentNum])
            segmentNum += 1
            # need to check if the start of next space is the end of the one we just created
            if start + segment < self.size and segment != spaceToFill[3]:
                self.memorySpace.insert(index + 1,
                                        [start + segment, end, "Hole", end - start - segment + 1])

            self.memorySpace.remove(spaceToFill)

        self.curQueue.append(process)
        self.inQueue.remove(process)
        process.finishTime = self.time + process.lifeTime
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def findProcessToFree(self, process):
        spaces = []
        for space in self.memorySpace:
            if space[2] != "Hole" and int(space[2][8:]) == process.pid:
                spaces.append(space)
        return spaces

    def freeMemory(self, process):
        spacesToFree = self.findProcessToFree(process)
        for space in spacesToFree:
            index = self.memorySpace.index(space)
            start = space[0]
            end = space[1]
            if index - 1 >= 0 and self.memorySpace[index - 1][2] == "Hole":
                start = self.memorySpace[index - 1][0]
                self.memorySpace.remove(self.memorySpace[index - 1])
                index -= 1
            if index + 1 < len(self.memorySpace) and self.memorySpace[index + 1][2] == "Hole":
                end = self.memorySpace[index + 1][1]
                self.memorySpace.remove(self.memorySpace[index + 1])

            self.memorySpace.insert(index, [start, end, "Hole", end - start + 1 ])
            self.memorySpace.remove(space)

        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime

    def printProcess(self, process):
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}", end="")
        if process[2] == "Hole":
            print("")
        else:
            print(f", Segment {process[3]}")
