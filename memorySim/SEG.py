from MemoryManager import MemoryManager

class SEG(MemoryManager):
    def __init__(self, size, processes, algorithm):
        """
        Class for the Segmentation Memory Management method. This method allows processes to request memory in variable
        sized chunks.
        :param size: total size of the memory
        :param processes: list of processes to run
        :param algorithm: integer representing which fit algorithm to run (1 - first-fit, 2 - best-fit, 3 - worst-fit)
        """
        super().__init__(size, processes)
        self.algorithm = algorithm

        # space[startIndex, endIndex, occupiedBy, totalSize (free) / segment num (not free)]
        self.memorySpace = [[0, size - 1, "Hole", size]]

    def addToMemory(self, process):
        """
        Add the process to memory. Similar to VSP, however, it needs to run the fit algorithm for each segment that was
        requested.
        :param process: process to add to memory
        """
        segmentNum = 0
        # Run through all segments adding them to memory
        for segment in process.memSpace:
            # Find the space to put the segment
            first, best, worst = self.findSpaceInMemory(segment)
            # Decide on which one to fill based on the fit algorithm
            spaceToFill = first
            if self.algorithm == 2:
                spaceToFill = best
            elif self.algorithm == 3:
                spaceToFill = worst

            # Find index, start, and end of the space that will be filled
            index = self.memorySpace.index(spaceToFill)
            start = spaceToFill[0]
            end = spaceToFill[1]
            # Insert the process into the memory list
            self.memorySpace.insert(index, [start, start + segment - 1, "Process " + str(process.pid),
                                            segmentNum])
            segmentNum += 1
            # need to check if we filled the entire hole or not
            if start + segment < self.size and segment != spaceToFill[3]:
                self.memorySpace.insert(index + 1,
                                        [start + segment, end, "Hole", end - start - segment + 1])

            self.memorySpace.remove(spaceToFill)

        # Update current and in queue
        self.curQueue.append(process)
        self.inQueue.remove(process)
        process.finishTime = self.time + process.lifeTime
        print(" " * 8 + "MM moves Process", process.pid, "to memory")
        self.printQueue()
        self.printMemoryMap()

    def checkBestFit(self, process, freeSpaces):
        """
        Checks to see if the process can be added using the best-fit algorithm
        :param process: process to add to memory
        :param freeSpaces: list of holes
        :return: boolean - true if process can fit
        """
        freeSpaceSizes = freeSpaces
        for segment in process.memSpace:
            bestIndex = -1
            bestSize = -1

            # Loop through holes, looking for the best fit
            for i in range(len(freeSpaceSizes)):
                space = freeSpaceSizes[i]
                if space >= segment and (space < bestSize or bestIndex == -1):
                    bestIndex = i
                    bestSize = space
            if bestIndex == -1:
                return False
            # Mimic inserting by changing the size of the segment
            freeSpaceSizes[bestIndex] -= segment
        return True

    def checkFirstFit(self, process, freeSpaces):
        """
        Check to see if the process can fit in memory using the first-fit algorithm
        :param process: process to add to memory
        :param freeSpaces: list of holes in memory
        :return: boolean - true if process can fit in memory
        """
        freeSpaceSizes = freeSpaces
        # Run through each segment, finding the first spot that it fits in
        for segment in process.memSpace:
            index = 0
            fit = False
            # Loop through all the free spaces until we either reach the end or we find a spot for the segment
            while index < len(freeSpaceSizes) and not fit:
                if segment <= freeSpaceSizes[index]:
                    freeSpaceSizes[index] -= segment
                    fit = True
                else:
                    index += 1
            if not fit:
                return False
        return True

    def checkForSpace(self, process):
        """
        Check for enough space for the process to be admitted to memory.
        :param process: process to add to memory
        :return: boolean - true if process can fit in memory
        """
        freeSpaceSizes = self.getFreeSpaceSizes()
        # If there isn't any free space, exit
        if len(freeSpaceSizes) == 0:
            return False

        if self.algorithm == 1:
            return self.checkFirstFit(process, freeSpaceSizes)
        elif self.algorithm == 2:
            return self.checkBestFit(process, freeSpaceSizes)
        else:
            return self.checkWorstFit(process, freeSpaceSizes)

    def checkWorstFit(self, process, freeSpaces):
        """
        Check to see if process can be admitted using the worst-fit algorithm
        :param process: process to check
        :param freeSpaces: list of free spaces in memory
        :return: boolean - true if process can fit
        """
        freeSpaceSizes = freeSpaces
        # run through each segment that needs to be allocated
        for segment in process.memSpace:
            largestIndex = -1
            largestSize = -1

            # run through each free space, finding the largest space for the process to fit in
            for i in range(len(freeSpaceSizes)):
                space = freeSpaceSizes[i]
                if space >= segment and (space > largestSize or largestIndex == -1):
                    largestIndex = i
                    largestSize = space
            # if there was no space found that could fit the segment, return false
            if largestIndex == -1:
                return False
            # mimic allocating the space to the segment by subtracting from the size
            freeSpaceSizes[largestIndex] -= segment
        return True
    def findProcessToFree(self, process):
        """
        Find all the segments that need to be freed
        :param process: process to be freed from memory
        :return: list of segments that need to be freed
        """
        spaces = []
        for space in self.memorySpace:
            if space[2] != "Hole" and int(space[2][8:]) == process.pid:
                spaces.append(space)
        return spaces

    def findSpaceInMemory(self, segment):
        """
        Finds the spot in memory for the segment to fit in to.
        :param segment: segment to allocate
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
            if space[2] == "Hole" and space[3] >= segment:
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
                if space[3] >= largestSize:
                    worst = space
                    largestSize = space[3]
        return first, best, worst

    def freeMemory(self, process):
        """
        Frees the passed in parameter from memory.
        :param process: process to free
        """
        # Find all the segments that need to be freed
        spacesToFree = self.findProcessToFree(process)
        # Run through each segment, freeing it from memory
        for space in spacesToFree:
            index = self.memorySpace.index(space)
            start = space[0]
            end = space[1]
            # If the space before this one is a hole, combine them
            if index - 1 >= 0 and self.memorySpace[index - 1][2] == "Hole":
                start = self.memorySpace[index - 1][0]
                self.memorySpace.remove(self.memorySpace[index - 1])
                index -= 1
            # If the space after this one is a hole, combine them
            if index + 1 < len(self.memorySpace) and self.memorySpace[index + 1][2] == "Hole":
                end = self.memorySpace[index + 1][1]
                self.memorySpace.remove(self.memorySpace[index + 1])

            # Put Hole in memory
            self.memorySpace.insert(index, [start, end, "Hole", end - start + 1 ])
            # Remove the segment
            self.memorySpace.remove(space)

        # Update the current queue
        self.curQueue.remove(process)
        self.printText(f"Process {process.pid} completes")
        self.printMemoryMap()
        self.totalTurnaround += process.finishTime - process.addTime

    def getFreeSpaceSizes(self):
        """
        Get the sizes of all the holes
        :return: list of all the sizes of the holes
        """
        freeSpaces = []
        for space in self.memorySpace:
            if space[2] == "Hole":
                freeSpaces.append(space[1] - space[0] + 1)
        return freeSpaces

    def printProcess(self, process):
        """
        Print the passed in process to the console.
        :param process: process to print
        """
        print(" " * 16 + f"{process[0]}-{process[1]}: {process[2]}", end="")
        if process[2] == "Hole":
            print("")
        else:
            print(f", Segment {process[3]}")
