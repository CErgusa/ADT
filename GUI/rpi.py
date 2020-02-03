from tkinter import *
import copy
import predefines
from enum import Enum


class Grid(Enum):
    EMPTY = 0
    INTERSECTION1 = 1
    INTERSECTION2 = 2
    INTERSECTION3 = 3
    INTERSECTION4 = 4
    INTERSECTION5 = 5
    CORNER = 6
    BUILDING = 8
    CAR = 9


def print_graph(grid):
    for row in grid:
        print(row)


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def output(self):
        print("My coordinates are: (" + self.x + ", " + self.y + ")\n")


class Car:
    def __init__(self, direction, path, car_num, previous, board):
        self.speed = 1
        self.ID = car_num

        if car_num == 0:
            self.point = Point(0, 0)
        else:
            self.point = Point(9, 9)

        self.direction = direction
        self.path = path
        self.previous = previous
        self.board = board

    def print_car(self):
        print("ID: ", self.ID)
        print("Speed: ", self.speed)
        print("My location: (", self.point.x, ", ", self.point.y, ")")

        if self.direction == 0:
            print("I am going: up")
        elif self.direction == 1:
            print("I am going: down")
        elif self.direction == 2:
            print("I am going: left")
        elif self.direction == 3:
            print("I am going: right")
        else:
            print("OFFROADING BITCHES!!")


def if_corner(car):
    if -1 >= car.previous[car.point.x][car.point.y] >= -8:
        return True
    else:
        return False


def turn_corner(car):
    # checks which corner, and takes the turn (ie. changes direction) accordingly
    if car.previous[car.point.x][car.point.y] == -1 or car.previous[car.point.x][car.point.y] == -6:
        car.direction = predefines.DOWN
        return car
    elif car.previous[car.point.x][car.point.y] == -2 or car.previous[car.point.x][car.point.y] == -7:
        car.direction = predefines.LEFT
        return car
    elif car.previous[car.point.x][car.point.y] == -3 or car.previous[car.point.x][car.point.y] == -5:
        car.direction = predefines.RIGHT
        return car
    elif car.previous[car.point.x][car.point.y] == -4 or car.previous[car.point.x][car.point.y] == -8:
        car.direction = predefines.UP
        return car

    return car


def move_point(car):
    if if_corner(car):
        car = turn_corner(car)

    if car.direction == predefines.UP:
        # reset board
        car.board[car.point.x][car.point.y] = car.previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.x = car.point.x - 1
        # move external grid point
        car.board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.DOWN:
        # reset board
        car.board[car.point.x][car.point.y] = car.previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.x = car.point.x + 1
        # move external grid point
        car.board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.LEFT:
        # reset board
        car.board[car.point.x][car.point.y] = car.previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.y = car.point.y - 1
        # move external grid point
        car.board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.RIGHT:
        # reset board
        car.board[car.point.x][car.point.y] = car.previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.y = car.point.y + 1
        # move external grid point
        car.board[car.point.x][car.point.y] = predefines.CAR
        return car


def check_if_intersection(car, x, y):
    # check if in intersection
    if 0 < car.previous[x][y] <= 5:
        return True
    return False


def check_which_intersection(car, x, y):
    if 0 < car.previous[x][y] <= 5:
        return car.previous[x][y]
    return -1


def check_next(car):
    if if_corner(car):
        return 0
    # check if intersection is 1 spot away in current direction
    if car.direction == predefines.UP:
        # check if x - 1 is intersection
        return check_if_intersection(car, car.point.x - 1, car.point.y)
    elif car.direction == predefines.DOWN:
        # check if x + 1 is intersection
        return check_if_intersection(car, car.point.x + 1, car.point.y)
    elif car.direction == predefines.RIGHT:
        # check if y - 1 is intersection
        return check_if_intersection(car, car.point.x, car.point.y - 1)
    elif car.direction == predefines.LEFT:
        # check if y + 1 is intersection
        return check_if_intersection(car, car.point.x, car.point.y + 1)


def collision_detection(car1, car2):
    return False


def pull_next(car):
    if if_corner(car):
        car = turn_corner(car)

    # check if intersection is 1 spot away in current direction and output the intersection number
    if car.direction == predefines.UP:
        # check if x - 1 is intersection (and which one)
        return check_which_intersection(car, car.point.x - 1, car.point.y)
    elif car.direction == predefines.DOWN:
        # check if x + 1 is intersection (and which one)
        return check_which_intersection(car, car.point.x + 1, car.point.y)
    elif car.direction == predefines.RIGHT:
        # check if y - 1 is intersection (and which one)
        return check_which_intersection(car, car.point.x, car.point.y - 1)
    elif car.direction == predefines.LEFT:
        # check if y + 1 is intersection (and which one)
        return check_which_intersection(car, car.point.x, car.point.y + 1)


def IM_alg(intersection_array, intersection_id):
    # call the correct intersection manager
    if intersection_id == predefines.INTERSECTION1:
        intersection1_manager(intersection_array)
    elif intersection_id == predefines.INTERSECTION2:
        intersection2_manager(intersection_array)
    elif intersection_id == predefines.INTERSECTION3:
        intersection3_manager(intersection_array)
    elif intersection_id == predefines.INTERSECTION4:
        intersection4_manager(intersection_array)
    elif intersection_id == predefines.INTERSECTION5:
        intersection5_manager(intersection_array)

    return


def intersection1_manager(intersection_array):
    # highlight the path as green with step numbers

    # if numbers are the same with another car, mark as red
    return


def intersection2_manager(intersection_array):
    # highlight the path as green with step numbers
    # if numbers are the same with another car, mark as red
    return


def intersection3_manager(intersection_array):
    # highlight the path as green with step numbers
    # if numbers are the same with another car, mark as red
    return


def intersection4_manager(intersection_array):
    # highlight the path as green with step numbers
    # if numbers are the same with another car, mark as red
    return


def intersection5_manager(intersection_array):
    # highlight the path as green with step numbers
    # if numbers are the same with another car, mark as red
    return


def car_update(car, car2):
    # check if entering an intersection
    if check_next(car):
        # decide which way to turn in the intersection
        car.path = predefines.random_number()
        car1_intersection = pull_next(car)

        # send turn data to Intersection Manager
        # BELOW THIS POINT IS TOTAL BULLSHIT
        car1_intersection_array = predefines.turn(car.direction, car.path, car1_intersection)
        IM_alg(car1_intersection_array, car1_intersection)

    move_point(car)
    return car


class GUI(Frame):
    # basically inherits Car and Point class
    # this is the class for the tkinter GUI live update
    #
    # __init__ is being passed both cars, the board current and previous, and the window root
    # sets up the structure
    def __init__(self, car1, car2, board, previous, master=None):
        Frame.__init__(self, master)  # idk
        self.car1 = car1
        self.car2 = car2

        # python sucks so I have to deep copy these
        # saving previous cars for refresh
        self.prev_car1 = copy.deepcopy(car1)
        self.prev_car2 = copy.deepcopy(car2)

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
        self.prev_car1 = copy.deepcopy(self.car1)
        self.prev_car2 = copy.deepcopy(self.car2)

        # update each car position
        self.car1 = car_update(car1, car2)
        self.car2 = car_update(car2, car1)

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

        Label(self.window, text='      1      ', bg='forest green').grid(row=self.car1.point.x, column=self.car1.point.y)
        Label(self.window, text='      2      ', bg='forest green').grid(row=self.car2.point.x, column=self.car2.point.y)

    # this updates the cars on the screen
    def only_update_car(self):
        # move the car
        Label(self.window, text='      1      ', bg='forest green').grid(row=self.car1.point.x, column=self.car1.point.y)
        Label(self.window, text='      2      ', bg='forest green').grid(row=self.car2.point.x, column=self.car2.point.y)

        # replace the previous location with the proper color
        # check what type of spot the car is occupying and replace it with the right color
        if previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.EMPTY:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.INTERSECTION1:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.INTERSECTION2:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.INTERSECTION3:
            Label(self.window, text='              ', bg='deep sky blue').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.INTERSECTION4:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.INTERSECTION5:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.OUTER_CORNER1:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.OUTER_CORNER2:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.OUTER_CORNER3:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)
        elif previous[self.prev_car1.point.x][self.prev_car1.point.y] == predefines.OUTER_CORNER4:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car1.point.x, column=self.prev_car1.point.y)

        if previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.EMPTY:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION1:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION2:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION3:
            Label(self.window, text='              ', bg='deep sky blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION4:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.INTERSECTION5:
            Label(self.window, text='              ', bg='dodger blue').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER1:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER2:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER3:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)
        elif previous[self.prev_car2.point.x][self.prev_car2.point.y] == predefines.OUTER_CORNER4:
            Label(self.window, text='              ', bg='khaki').grid(row=self.prev_car2.point.x, column=self.prev_car2.point.y)


board = predefines.create_graph()     # sets up the city
previous = predefines.create_graph()  # having two is needed for refreshability

# create the points for the Cars
# Point ( x value, y value )
p1 = Point(0, 0)
p2 = Point(predefines.GRID_SIZE - 1, predefines.GRID_SIZE - 1)

# put the cars on the board
board[p1.x][p1.y] = predefines.CAR
board[p2.x][p2.y] = predefines.CAR

# create the car objects with the points from before
# Car ( direction, which car they are )
car1 = Car(predefines.LEFT, predefines.NONE, 0, previous, board)
car2 = Car(predefines.RIGHT, predefines.NONE, 1, previous, board)

#print_graph(board)

# set up the gui/tkinter
root = Tk()
gui = GUI(car1, car2, board, previous, root)        # create the GUI object
root.title("Instrumentation of Self-Driving Tank")  # give the screen a label
root.geometry('605x215')                            # give the screen a proper size : geometry( ' X x Y ' )
#cookImage = PhotoImage(file=r"/home/rgusa/Documents/digipen/junior/ECE360/code/working/farts.png")

root.after(1000, gui.tkinter_init)  # refresh function for tkinter : after ( ms period for refresh, function to call )
root.mainloop()  # idk, some function needed for tkinter