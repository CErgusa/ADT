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
         if not self.empty():
            return self.container[len(self.container) - 1]
         else:
            return None

     def size(self):
         return len(self.container)

     def clear(self):
         for i in range(0, self.size()):
             self.pop()
         
