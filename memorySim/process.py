class Process:
    def __init__(self, pid, addTime, lifeTime, memSpace):
        """
        Data structure to hold information about each process.
        :param pid: process id
        :param addTime: time to add process to queue
        :param lifeTime: time it takes for process to finish running
        :param memSpace: the memory segments the process is requesting
        """
        self.pid = pid
        self.addTime = addTime
        self.lifeTime = lifeTime
        self.memSpace = memSpace
        self.finishTime = -1
        self.totalSpace = sum(memSpace)

    def __lt__(self, other):
        """
        less than override to sort processes by addition time
        :param other: other process to compare to
        :return: boolean - true if self is less than the other
        """
        return self.addTime < other.addTime

    def setFinishTime(self, startTime):
        """
        Set the finish time for the process.
        :param startTime: Time the process was added to memory
        """
        self.finishTime = startTime + self.lifeTime