class Intersection():
    def __init__(self, direction, step):
        self.direction = direction
        self.step = step
        self.speed = 1 # TODO: implement speed

    def getDirection(self):
        return self.direction
    
    def getStep(self):
        return self.step
