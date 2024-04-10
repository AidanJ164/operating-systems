from MemoryManager import MemoryManager
import math

class PAG(MemoryManager):
    def __init__(self, size, processes, pageSize):
        super().__init__(size, processes)
        self.pageSize = pageSize
        self.freePages = int(size / pageSize)

        # space[start index, end index, occupied by, page num]
        self.memorySpace = [[0, size - 1, "Free Frame(s)", 0]]

    def checkForSpace(self, process):
        numPages = math.ceil(process.totalSpace / self.pageSize)
        if self.freePages >= numPages:
            return True
        return False

    def printProcess(self, process):
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}", end="")
        if process[2] == "Free Frame(s)":
            print("")
        else:
            print(f", Page {process[3]}")

    def findFramesToFill(self, numPages):
        pagesNeeded = numPages
        freeFrames = []
        index = 0
        while pagesNeeded > 0:
            if self.memorySpace[index][2] == "Free Frame(s)":
                pagesInSpace = (self.memorySpace[index][1] - self.memorySpace[index][0] + 1) / self.pageSize
                if pagesInSpace >= pagesNeeded:
                    freeFrames.append(self.memorySpace[index])
                    pagesNeeded = 0
                else:
                    pagesNeeded -= pagesInSpace
                    freeFrames.append(self.memorySpace[index])
                    index += 1
            else:
                index += 1
        return freeFrames


    def addToMemory(self, process):
        numPages = math.ceil(process.totalSpace / self.pageSize)
        fillFrames = self.findFramesToFill(numPages)
        index = 0
        pageNum = 1
        while numPages > 0:
            holeToFill = fillFrames[index]
            pagesInSpace = math.floor((holeToFill[1] - holeToFill[0] + 1) / self.pageSize)
            start = holeToFill[0]
            end = holeToFill[1]
            if pagesInSpace >= numPages:
                for i in range(numPages):
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace.insert(holeIndex, [start + (i * self.pageSize), start + ((i + 1) * self.pageSize) - 1, "Process " + str(process.pid), pageNum])
                    pageNum += 1
                    end = start + ((i + 1) * self.pageSize) - 1
                if end < holeToFill[1]:
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace[holeIndex][0] = end + 1
                else:
                    self.memorySpace.remove(holeToFill)
                numPages = 0
            else:
                index += 1
                for i in range(pagesInSpace):
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace.insert(holeIndex, [start + (i * self.pageSize), start + ((i + 1) * self.pageSize) - 1, "Process " + str(process.pid), pageNum ])
                    pageNum += 1
                    numPages -= 1
                self.memorySpace.remove(holeToFill)
        self.freePages -= pageNum - 1
        self.curQueue.append(process)
        self.inQueue.remove(process)
        process.finishTime = self.time + process.lifeTime
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def invoke(self):
        index = 0
        while index < len(self.inQueue):
            if self.checkForSpace(self.inQueue[index]):
                self.addToMemory(self.inQueue[index])
            else:
                index += 1

    def findProcessToFree(self, process):
        spaces = []
        for space in self.memorySpace:
            if space[2] != "Free Frame(s)" and int(space[2][8:]) == process.pid:
                spaces.append(space)
        return spaces

    def freeMemory(self, process):
        spacesToFree = self.findProcessToFree(process)
        for space in spacesToFree:
            index = self.memorySpace.index(space)
            start = space[0]
            end = space[1]
            if index - 1 >= 0 and self.memorySpace[index - 1][2] == "Free Frame(s)":
                start = self.memorySpace[index - 1][0]
                self.memorySpace.remove(self.memorySpace[index - 1])
                index -= 1
            if index + 1 < len(self.memorySpace) and self.memorySpace[index + 1][2] == "Free Frame(s)":
                end = self.memorySpace[index + 1][1]
                self.memorySpace.remove(self.memorySpace[index + 1])

            self.memorySpace.insert(index, [start, end, "Free Frame(s)", 0])
            self.memorySpace.remove(space)

        self.freePages += len(spacesToFree)
        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime

