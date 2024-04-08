class Process:
    def __init__(self, pid, addTime, lifeTime, memSpace):
        self.pid = pid
        self.addTime = addTime
        self.lifeTime = lifeTime
        self.memSpace = memSpace
        self.finishTime = -1
        self.totalSpace = sum(memSpace)

    def __lt__(self, other):
        return self.addTime < other.addTime

    def setFinishTime(self, startTime):
        self.finishTime = startTime + self.lifeTime