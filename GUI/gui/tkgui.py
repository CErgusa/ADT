import copy
import predefines
from car import Car
from tkinter import *
from rand import Random

# Direction: absolute, looking at grid which way are you driving
# Path: based on direction, which way are you turning

def car_update(car_array):
    for i in range(0, len(car_array)):
        #print('ID:', car_array[i].ID, 'direction:', car_array[i].direction, 'path:', car_array[i].path, 'point:', car_array[i].point.x, ',', car_array[i].point.y)

        # check if in intersection but not entering
        if car_array[i].check_if_intersection(car_array[i].point.x, car_array[i].point.y):
            # continue moving
            car_array[i].intersection_move_point() # TODO: Create function

        # check if entering an intersection
        elif car_array[i].check_next():
            car_array[i].pull_next() # sets the intersectionID (TODO: FoW)
            car_array[i].which_way_to_turn() # sets the path according to intID -^
            car_array[i].turn() # turns based on path-^
            # TODO: check for collisions (IM or nah???)

            # figure out how to move based on intersection array
            # TODO: Use the stack to find the first step info
            car_array[i].entering_intersection_move_point()

        # check if leaving an intersection
        elif car_array[i].check_next() == False and car_array[i].check_if_intersection(car_array[i].point.x, car_array[i].point.y): 
            car_array[i].direction = car_array[i].path; # update the direction (TOTALLY WRONG???)
            # TODO: make a function that changes it properly (looks at both and decides what the dir actually is
            #       should make the path NONE or something arbitrary

            car_array[i].intersectionID = predefines.NONE # update the intersectionID
            car_array[i].move_point()
        else:
            car_array[i].move_point()

    return car_array


def print_graph(grid):
    for row in grid:
        print(row)


class GUI(Frame):
    # basically inherits Car and Point class
    # this is the class for the tkinter GUI live update
    #
    # __init__ is being passed both cars, the current and previous board, and the window root
    # sets up the structure
    def __init__(self, car_array, board, previous, master=None):
        Frame.__init__(self, master)

        self.car_array = car_array

        # python sucks so I have to deep copy these
        # saving previous cars for refresh
        self.prev_car_array = copy.deepcopy(car_array)

        self.board = board        # current city layout with cars
        self.previous = previous  # previous game board for refreshing
        self.window = master      # master => master = Tk()
        self.update_tk()          # initialize the tkinter screen

    # this is really just the update function for the cars
    def tkinter_init(self):
        # debug car info before update
        #self.car1.print_car()
        #self.car2.print_car()

        # keep track of the previous spot to refresh the screen properly
        self.prev_car_array = copy.deepcopy(self.car_array)

        # update each car position
        self.car_array = car_update(self.car_array)

        #print_graph(board)  # output board to terminal

        # debug car info after update
        #self.car1.print_car()
        #self.car2.print_car()

        self.only_update_car()  # update the tkinter board
        self.after(1000, self.tkinter_init)  # refresh tkinter board output

    # this is the initialization function for the GUI, sets up the screen
    def update_tk(self):
        # the true init function, it creates the original board
        # Label ( tkinter window name, what text you want in the block, block color ). grid ( x value, y value )
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=1)
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=2)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=1)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=2)
        Label(self.window, text='              ', bg='khaki').grid(row=2, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=2, column=1)

        Label(self.window, text='              ', bg='khaki').grid(row=7, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=7, column=1)
        Label(self.window, text='              ', bg='khaki').grid(row=7, column=2)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=1)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=2)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=0)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=1)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=2)

        Label(self.window, text='              ', bg='khaki').grid(row=7, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=7, column=9)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=7)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=8, column=9)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=7)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=9, column=9)

        Label(self.window, text='              ', bg='khaki').grid(row=2, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=2, column=9)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=7)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=1, column=9)
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=7)
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=8)
        Label(self.window, text='              ', bg='khaki').grid(row=0, column=9)

        Label(self.window, text='              ', bg='indian red').grid(row=2, column=2)
        Label(self.window, text='              ', bg='indian red').grid(row=2, column=3)
        Label(self.window, text='              ', bg='indian red').grid(row=3, column=2)
        Label(self.window, text='              ', bg='indian red').grid(row=3, column=3)

        Label(self.window, text='              ', bg='indian red').grid(row=2, column=6)
        Label(self.window, text='              ', bg='indian red').grid(row=2, column=7)
        Label(self.window, text='              ', bg='indian red').grid(row=3, column=6)
        Label(self.window, text='              ', bg='indian red').grid(row=3, column=7)

        Label(self.window, text='              ', bg='indian red').grid(row=6, column=2)
        Label(self.window, text='              ', bg='indian red').grid(row=6, column=3)
        Label(self.window, text='              ', bg='indian red').grid(row=7, column=2)
        Label(self.window, text='              ', bg='indian red').grid(row=7, column=3)

        Label(self.window, text='              ', bg='indian red').grid(row=6, column=6)
        Label(self.window, text='              ', bg='indian red').grid(row=6, column=7)
        Label(self.window, text='              ', bg='indian red').grid(row=7, column=6)
        Label(self.window, text='              ', bg='indian red').grid(row=7, column=7)

        Label(self.window, text='              ', bg='dodger blue').grid(row=0, column=3)
        Label(self.window, text='              ', bg='dodger blue').grid(row=0, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=0, column=5)
        Label(self.window, text='              ', bg='dodger blue').grid(row=0, column=6)
        Label(self.window, text='              ', bg='dodger blue').grid(row=1, column=3)
        Label(self.window, text='              ', bg='dodger blue').grid(row=1, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=1, column=5)
        Label(self.window, text='              ', bg='dodger blue').grid(row=1, column=6)
        Label(self.window, text='              ', bg='dodger blue').grid(row=2, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=2, column=5)

        Label(self.window, text='              ', bg='dodger blue').grid(row=3, column=0)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=0)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=0)
        Label(self.window, text='              ', bg='dodger blue').grid(row=6, column=0)
        Label(self.window, text='              ', bg='dodger blue').grid(row=3, column=1)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=1)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=1)
        Label(self.window, text='              ', bg='dodger blue').grid(row=6, column=1)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=2)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=2)

        Label(self.window, text='              ', bg='deep sky blue').grid(row=3, column=4)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=3, column=5)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=4, column=3)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=4, column=4)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=4, column=5)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=4, column=6)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=5, column=3)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=5, column=4)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=5, column=5)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=5, column=6)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=6, column=5)
        Label(self.window, text='              ', bg='deep sky blue').grid(row=6, column=4)

        Label(self.window, text='              ', bg='dodger blue').grid(row=3, column=9)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=9)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=9)
        Label(self.window, text='              ', bg='dodger blue').grid(row=6, column=9)
        Label(self.window, text='              ', bg='dodger blue').grid(row=3, column=8)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=8)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=8)
        Label(self.window, text='              ', bg='dodger blue').grid(row=6, column=8)
        Label(self.window, text='              ', bg='dodger blue').grid(row=4, column=7)
        Label(self.window, text='              ', bg='dodger blue').grid(row=5, column=7)

        Label(self.window, text='              ', bg='dodger blue').grid(row=9, column=3)
        Label(self.window, text='              ', bg='dodger blue').grid(row=9, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=9, column=5)
        Label(self.window, text='              ', bg='dodger blue').grid(row=9, column=6)
        Label(self.window, text='              ', bg='dodger blue').grid(row=8, column=3)
        Label(self.window, text='              ', bg='dodger blue').grid(row=8, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=8, column=5)
        Label(self.window, text='              ', bg='dodger blue').grid(row=8, column=6)
        Label(self.window, text='              ', bg='dodger blue').grid(row=7, column=4)
        Label(self.window, text='              ', bg='dodger blue').grid(row=7, column=5)

        #for i in range(0, len(self.car_array)):
        #    Label(self.window, text=i, bg='forest green').grid(row=self.car_array[i].point.x, column=self.car_array[i].point.y)

        # this is what it is supposed to do, but trying it with the for loop
        Label(self.window, text='      1      ', bg='forest green').grid(row=self.car_array[0].point.x, column=self.car_array[0].point.y)
        Label(self.window, text='      2      ', bg='forest green').grid(row=self.car_array[1].point.x, column=self.car_array[1].point.y)

    # this updates the cars on the screen
    def only_update_car(self):
        # move the car
        #for i in range(0, len(self.car_array)):
        #    Label(self.window, text=i, bg='forest green').grid(row=self.car_array[i].point.x, column=self.car_array[i].point.y)


        Label(self.window, text='      1      ', bg='forest green').grid(row=self.car_array[0].point.x, column=self.car_array[0].point.y)
        Label(self.window, text='      2      ', bg='forest green').grid(row=self.car_array[1].point.x, column=self.car_array[1].point.y)

        # replace the previous location with the proper color
        # check what type of spot the car is occupying and replace it with the right color
        for i in range(0, len(self.prev_car_array)):
            if self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.EMPTY:
                Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION1:
                Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION2:
                Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION3:
                Label(self.window, text='              ', bg='deep sky blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION4:
                Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION5:
                Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER1:
                Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER2:
                Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER3:
                Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER4:
                Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)


#        if self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.EMPTY:
#            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION1:
#            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION2:
#            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION3:
#            Label(self.window, text='              ', bg='deep sky blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION4:
#            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION5:
#            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER1:
#            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER2:
#            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#        elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER3:
#            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
#       elif self.previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER4:
#            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
