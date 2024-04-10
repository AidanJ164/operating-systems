from MemoryManager import MemoryManager

class SEG(MemoryManager):
    def __init__(self, size, processes, algorithm):
        super().__init__(size, processes)
        self.algorithm = algorithm

        # space[startIndex, endIndex, occupiedBy, totalSize]
        self.memorySpace = [[0, size - 1, "Hole", size]]