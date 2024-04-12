from abc import ABC, abstractmethod
class MemoryManager(ABC):
    def __init__(self, size, processes):
        """
        Initialize the Memory Manager. This is the parent class to VSP, PAG, and SEG which represent the different
        memory management methods.
        :param size: total size of the memory space
        :param processes: list of processes that need to run
        """
        self.size = size
        self.processes = processes
        self.time = 0
        self.inQueue = []
        self.curQueue = []
        self.memorySpace = []
        self.timePrinted = False
        self.numProcesses = len(processes)
        self.totalTurnaround = 0

    def addQueue(self):
        """
        Adds the first process in the list to the queue.
        """
        self.printText(f"Process {self.processes[0].pid} arrives")
        self.inQueue.append(self.processes[0])
        self.printQueue()
        self.processes = self.processes[1:]

    @abstractmethod
    def addToMemory(self, process):
        """
        Abstract method to add a process to memory
        :param process: process to add to memory
        """
        pass

    def checkForFinishing(self):
        """
        Checks the current processes in memory and checks to see if they finish at the current time.
        :return: list of processes ready to be freed from memory
        """
        finishing = []
        for process in self.curQueue:
            if process.finishTime == self.time:
                finishing.append(process)

        return finishing

    def checkForNewProcesses(self):
        """
        Checks the list of processes to see if any get put in queue at the current time.
        :return: list of processes ready to join the queue.
        """
        numNew = 0
        for process in self.processes:
            if process.addTime == self.time:
                numNew += 1

        return numNew

    @abstractmethod
    def checkForSpace(self, process):
        """
        Abstract method that will check to see if the process can be admitted to memory.
        :param process: process to check
        :return: Boolean - true if processes can be admitted, false otherwise
        """
        pass

    def findNextTime(self):
        """
        Finds the time of the next event. The event could either be a process being freed from memory or a process
        joining the input queue.
        :return: int - next time to stop at
        """
        # If there are no more processes to run, find when next process finishes running
        if len(self.processes) == 0:
            return self.findNextToFinish().finishTime
        # If there are no processes running, find when next process is going to join the input queue
        elif len(self.curQueue) == 0:
            return self.processes[0].addTime
        # Compare the time of the next finishing process versus the process wanting to join the input queue.
        else:
            if self.processes[0].addTime < self.findNextToFinish().finishTime:
                return self.processes[0].addTime
            else:
                return self.findNextToFinish().finishTime

    def findNextToFinish(self):
        """
        Find the time of the next finishing process.
        :return: time of the next finishing process
        """
        nextFinish = self.curQueue[0]
        for i in range(1, len(self.curQueue)):
            if self.curQueue[i].finishTime < nextFinish.finishTime:
                nextFinish = self.curQueue[i]

        return nextFinish

    @abstractmethod
    def freeMemory(self, process):
        """
        Abstract method to free a process from memory.
        :param process: process to free
        """
        pass

    def getAverageTurnaround(self):
        """
        Get the average turnaround time of the processes
        :return: average turnaround time
        """
        return self.totalTurnaround / self.numProcesses

    def invoke(self):
        """
        Invoke the memory manager to try and add any processes in queue to memory
        """
        index = 0
        while index < len(self.inQueue):
            if self.checkForSpace(self.inQueue[index]):
                self.addToMemory(self.inQueue[index])
            else:
                index += 1

    def printMemoryMap(self):
        """
        Print the map of the memory to the console.
        :return:
        """
        print(" " * 8 + f"Memory Map: ")
        for i in range(len(self.memorySpace)):
            self.printProcess(self.memorySpace[i])

    @abstractmethod
    def printProcess(self, process):
        """
        Abstract method to print the process to the console
        :param process: process to print
        """
        pass

    def printQueue(self):
        """
        Prints the current input queue to the console.
        """
        print(" " * 8 + "Input Queue:[", end="")
        if self.inQueue:
            for i in range(len(self.inQueue) - 1):
                print(self.inQueue[i].pid, end=" ")
            print(self.inQueue[-1].pid, end="")
        print("]")

    def printText(self, str):
        """
        Print the passed in string to the console. This function is here to accommodate for the fact that some strings
        require a necessary spacing and some are printed directly after the time is printed to the console.
        :param str: string to print
        """
        if self.timePrinted:
            print(" " * 8 + str)
        else:
            print(str)
            self.timePrinted = True

    def start(self):
        """
        Starts the memory manager. It first runs through all the processes and adds them to the input queue while also
        looking for processes that have finished running and need to be freed. After all processes have been added to
        input queue, it waits for all the processes to finish running.
        """
        # Processes still need to run
        while len(self.processes) > 0:
            print("t =", self.time, end=": ")
            self.timePrinted = False
            newProcesses = self.checkForNewProcesses()
            finishingProcesses = self.checkForFinishing()

            # Add processes that get added at the current time to the queue
            for i in range(newProcesses):
                self.addQueue()

            # Free processes that are finished
            for process in finishingProcesses:
                self.freeMemory(process)

            # Invoke the Memory Manager
            self.invoke()

            print("        ")

            # Find the next time in the sequence
            if self.curQueue:
                self.time = self.findNextTime()

        # All processes added to queue and just need to wait for them to finish
        while len(self.curQueue) > 0:
            print("t =", self.time, end=": ")
            self.timePrinted = False

            # find the processes that need to be freed at this time.
            finishingProcesses = self.checkForFinishing()
            for process in finishingProcesses:
                self.freeMemory(process)

            self.invoke()
            print("        ")

            # Find the next time in the sequence
            if self.curQueue:
                self.time = self.findNextTime()