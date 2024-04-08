from MemoryManager import MemoryManager

class PAG(MemoryManager):
    def __init__(self, size, processes, pageSize):
        super().__init__(size, processes)
        self.pageSize = pageSize

        # space[start index, end index, occupied by, page num]
        self.memorySpace = [[0, size - 1, "Free frame(s)", 0]]