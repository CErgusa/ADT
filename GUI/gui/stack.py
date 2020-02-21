class Stack:
     def __init__(self):
         self.container = []

     def empty(self):
         return self.container == []

     def push(self, item):
         self.container.append(item)

     def pop(self):
         return self.container.pop()

     def top(self):
         return self.container[len(self.container) - 1]

     def size(self):
         return len(self.container)


