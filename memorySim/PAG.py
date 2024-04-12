from MemoryManager import MemoryManager
import math

class PAG(MemoryManager):
    def __init__(self, size, processes, pageSize):
        """
        Initializes the Paging Method of the Memory Manager. This method uses pages to allocate memory for a process.
        :param size: total size of the memory
        :param processes: processes needed to run
        :param pageSize: size of each page
        """
        super().__init__(size, processes)
        self.pageSize = pageSize
        self.freePages = int(size / pageSize)

        # space[start index, end index, occupied by, page num]
        self.memorySpace = [[0, size - 1, "Free Frame(s)", 0]]

    def addToMemory(self, process):
        """
        Adds a process to memory. Doesn't need to check if there are spots since it should be called beforehand. Divides
        the total amount of space the process needs by the page size to find the total amount of pages the process needs
        to allocate. It then runs through each frame, checks to see if it is empty, and then allocates that spot for the
        process.
        :param process: process to allocate
        """
        numPages = math.ceil(process.totalSpace / self.pageSize)

        # Free frames that can be filled
        fillFrames = self.findFramesToFill(numPages)
        index = 0
        pageNum = 1
        # While there are still pages that need to be allocated, allocate them
        while numPages > 0:
            holeToFill = fillFrames[index]
            pagesInSpace = math.floor((holeToFill[1] - holeToFill[0] + 1) / self.pageSize)
            start = holeToFill[0]
            end = holeToFill[1]
            # If the free frame has enough space for all the pages, allocate all the pages in that hole.
            if pagesInSpace >= numPages:
                for i in range(numPages):
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace.insert(holeIndex, [start + (i * self.pageSize), start + ((i + 1) * self.pageSize) - 1, "Process " + str(process.pid), pageNum])
                    pageNum += 1
                    end = start + ((i + 1) * self.pageSize) - 1
                # If the whole hole was not filled, just adjust the original start of the hole to the appropriate spot
                if end < holeToFill[1]:
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace[holeIndex][0] = end + 1
                # If the whole hole was filled, remove it from the list
                else:
                    self.memorySpace.remove(holeToFill)
                numPages = 0
            # The hole is only big enough for some pages, fill the hole and then go to the next one
            else:
                index += 1
                for i in range(pagesInSpace):
                    holeIndex = self.memorySpace.index(holeToFill)
                    self.memorySpace.insert(holeIndex, [start + (i * self.pageSize), start + ((i + 1) * self.pageSize) - 1, "Process " + str(process.pid), pageNum ])
                    pageNum += 1
                    numPages -= 1
                self.memorySpace.remove(holeToFill)

        # Decrease the amount of freePages for easy lookup when checking for space
        self.freePages -= pageNum - 1
        # Add process to the current queue and remove it from the input queue
        self.curQueue.append(process)
        self.inQueue.remove(process)
        # Set the time the process will finish
        process.finishTime = self.time + process.lifeTime
        # Print output to console
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def checkForSpace(self, process):
        """
        Check for enough space for the process
        :param process: process to add to memory
        :return: boolean - true if there is enough space in memory for the process
        """
        numPages = math.ceil(process.totalSpace / self.pageSize)
        if self.freePages >= numPages:
            return True
        return False

    def findFramesToFill(self, numPages):
        """
        Finds the free frames in the memory that the process can fill.
        :param numPages: Amount of pages to be added to memory
        :return: list of free frames
        """
        pagesNeeded = numPages
        freeFrames = []
        index = 0
        while pagesNeeded > 0:
            if self.memorySpace[index][2] == "Free Frame(s)":
                pagesInSpace = (self.memorySpace[index][1] - self.memorySpace[index][0] + 1) / self.pageSize
                # If this frame has enough space, no need to check for any more
                if pagesInSpace >= pagesNeeded:
                    freeFrames.append(self.memorySpace[index])
                    pagesNeeded = 0
                # Else, we need to go to the next free frame and find space
                else:
                    pagesNeeded -= pagesInSpace
                    freeFrames.append(self.memorySpace[index])
                    index += 1
            else:
                index += 1
        return freeFrames


    def findProcessToFree(self, process):
        """
        Finds the pages of the process that need to be freed
        :param process: process to free from memory
        :return: list of indices of the pages that will be freed
        """
        spaces = []
        for space in self.memorySpace:
            # If it's not a free frame and it's the correct process, free it
            if space[2] != "Free Frame(s)" and int(space[2][8:]) == process.pid:
                spaces.append(space)
        return spaces


    def freeMemory(self, process):
        """
        Frees a process from the memory space. Looks to find all of its pages and then removes them one by one, filling
        them with free frames.
        :param process: process to free from memory
        """
        # Find pages that need to be freed
        spacesToFree = self.findProcessToFree(process)
        # Run through each space and remove it from the memory map
        for space in spacesToFree:
            index = self.memorySpace.index(space)
            start = space[0]
            end = space[1]
            # If the space before this one is free, combine them
            if index - 1 >= 0 and self.memorySpace[index - 1][2] == "Free Frame(s)":
                start = self.memorySpace[index - 1][0]
                self.memorySpace.remove(self.memorySpace[index - 1])
                index -= 1
            # If the space after this one is free, combine them
            if index + 1 < len(self.memorySpace) and self.memorySpace[index + 1][2] == "Free Frame(s)":
                end = self.memorySpace[index + 1][1]
                self.memorySpace.remove(self.memorySpace[index + 1])

            self.memorySpace.insert(index, [start, end, "Free Frame(s)", 0])
            self.memorySpace.remove(space)

        # Update amount of free spaces
        self.freePages += len(spacesToFree)
        # Remove the process from the current queue
        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime
    def printProcess(self, process):
        """
        Print the processes information to the console.
        :param process: process to print
        """
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}", end="")
        if process[2] == "Free Frame(s)":
            print("")
        else:
            print(f", Page {process[3]}")


