from abc import ABC, abstractmethod
class MemoryManager(ABC):
    def __init__(self, size, processes):
        self.size = size
        self.processes = processes
        self.time = 0
        self.inQueue = []
        self.curQueue = []
        self.memorySpace = []
        self.timePrinted = False
        self.numProcesses = len(processes)
        self.totalTurnaround = 0

    @abstractmethod
    def invoke(self):
        pass

    @abstractmethod
    def freeMemory(self, process):
        pass

    def printText(self, str):
        if self.timePrinted:
            print("       " + str)
        else:
            print(str)
            self.timePrinted = True

    def printMemoryMap(self):
        print(" " * 7 + f"Memory Map: {self.memorySpace[0][0]}-{self.memorySpace[0][1]}: {self.memorySpace[0][2]}")
        for i in range(1, len(self.memorySpace)):
            print(" " * 18 + f"{self.memorySpace[i][0]}-{self.memorySpace[i][1]}: {self.memorySpace[i][2]}")
    def printQueue(self):
        print(" " * 7 + "Input Queue:[", end="")
        if self.inQueue:
            for i in range(len(self.inQueue) - 1):
                print(self.inQueue[i].pid, end=" ")
            print(self.inQueue[-1].pid, end="")
        print("]")

    def addQueue(self):
        self.printText(f"Process {self.processes[0].pid} arrives")
        self.inQueue.append(self.processes[0])
        self.printQueue()
        self.processes = self.processes[1:]

    def checkForNewProcesses(self):
        numNew = 0
        for process in self.processes:
            if process.addTime == self.time:
                numNew += 1

        return numNew

    def checkForFinishing(self):
        finishing = []
        for process in self.curQueue:
            if process.finishTime == self.time:
                finishing.append(process)

        return finishing

    def findNextToFinish(self):
        nextFinish = self.curQueue[0]
        for i in range(1, len(self.curQueue)):
            if self.curQueue[i].finishTime < nextFinish.finishTime:
                nextFinish = self.curQueue[i]

        return nextFinish

    def findNextTime(self):
        if len(self.processes) == 0:
            return self.findNextToFinish().finishTime
        elif len(self.curQueue) == 0:
            return self.processes[0].addTime
        else:
            if self.processes[0].addTime < self.findNextToFinish().finishTime:
                return self.processes[0].addTime
            else:
                return self.findNextToFinish().finishTime

    def start(self):
        # Processes still need to run
        while len(self.processes) > 0:
            print("t =", self.time, end=": ")
            self.timePrinted = False
            newProcesses = self.checkForNewProcesses()
            finishingProcesses = self.checkForFinishing()

            # while there are processes being added or finishing at time t, do work, else MM does what it needs to do
            for i in range(newProcesses):
                self.addQueue()

            for process in finishingProcesses:
                self.freeMemory(process)

            self.invoke()

            print("")

            if self.curQueue:
                self.time = self.findNextTime()

        # Processes still running
        while len(self.curQueue) > 0:
            print("t =", self.time, end=": ")
            self.timePrinted = False
            finishingProcesses = self.checkForFinishing()
            for process in finishingProcesses:
                self.freeMemory(process)
            print("")
            if self.curQueue:
                self.time = self.findNextTime()

    def getAverageTurnaround(self):
        return self.totalTurnaround / self.numProcesses








