# seed the pseudorandom number generator
import random

class Random():
    def __init__(self):
        self.max = 0
        self.min = 0

    def og_randint(self, min, max):
        self.min = min
        self.max = max
        return random.randint(self.min, self.max)

    def skip_randint(self, min, max, num_to_skip):
        self.min = min
        self.max = max
        farts = random.randint(self.min, self.max)

        if farts == num_to_skip:
            return self.skip_randint_aux(num_to_skip)
        else:
            return farts

    def skip_randint_aux(self, skip):
        farts = random.randint(self.min, self.max)

        if farts == skip:
            print("This program is all fucked up, fix it dipshit")
            return self.skip_randint_aux(skip)
        else:
            return farts

    def skip2_randint(self, min, max, num_to_skip, num_to_skip2):
        self.min = min
        self.max = max
        farts = random.randint(self.min, self.max)

        if farts == num_to_skip or farts == num_to_skip2:
            return self.skip_randint_aux(num_to_skip, num_to_skip2)
        else:
            return farts

    def skip2_randint_aux(self, num_to_skip, num_to_skip2):
        farts = random.randint(self.min, self.max)

        if farts == num_to_skip or farts == num_to_skip2:
            print("This program might be wrong, but has high prob. of getting here")
            return self.skip2_randint_aux(num_to_skip, num_to_skip2)
        else:
            return farts