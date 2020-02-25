import predefines
from tkgui import *
from tkinter import *
from point import Point
from board import Board


current = Board()   # sets up the city
previous = Board()  # having two is needed for refreshability

# create the points for the Cars
# Point ( x value, y value )
p1 = Point(0, 0)
p2 = Point(predefines.GRID_SIZE - 1, predefines.GRID_SIZE - 1)

# put the cars on the board
current.board[p1.x][p1.y] = predefines.CAR
current.board[p2.x][p2.y] = predefines.CAR

# create the car objects with the points from before
# Car ( direction, path, ID )
car1 = Car(predefines.DOWN, predefines.DOWN, 0, previous.board, current.board)
car2 = Car(predefines.UP, predefines.UP, 1, previous.board, current.board)

# create an array of cars for easy addition of cars
car_array = [car1, car2]

#gui.print_graph(board)

# set up the gui/tkinter
root = Tk()
gui = GUI(car_array, current.board, previous.board, root)         # create the GUI object
root.title("Instrumentation of Self-Driving Tank")  # give the screen a label
root.geometry('605x215')                            # give the screen a proper size : geometry( ' X x Y ' )
#cookImage = PhotoImage(file=r"/home/rgusa/Documents/digipen/junior/ECE360/code/working/farts.png")

root.after(1000, gui.tkinter_init)  # refresh function for tkinter : after ( ms period for refresh, function to call )
root.mainloop()  # idk, some function needed for tkinter


# DONE List:
# - when exiting the intersection, updates the car to correct info
#
# - fix bug where inner corner sends you back the other way
#
# - make a function that changes dir correctly when leaving an intersection
#
# - Use the stack to find the first step info
#
# - fix flying car bug
#
# - fix check_if_intersection to allow for touching building
#
# - edge collision detection


# TODO List:
# - turn intersectionID into an array for fog of war
#   this way it isn't a shitty update to a variable that literally does nothing
#
# - collision detection
# 
# - edge detection for check_next
# 
# - fix mystery (None) path bug
#
# - corners not working?