import copy
import predefines
from car import Car
from tkinter import *
from rand import Random
from point import Point
from board import Board
# Direction: absolute, looking at grid which way are you driving
# Path: based on direction, which way are you turning

def car_update(car_array):
    for i in range(0, len(car_array)):
        print('ID:', car_array[i].ID, 'direction:', predefines.dir_array[car_array[i].direction], 'path:', predefines.dir_array[car_array[i].path], 'intID:', car_array[i].intersectionID, 'stack_length:', car_array[i].direction_stack.size())
        check_next, edge_case, if_intersection = car_array[i].check_next_if()
        fuck_up = car_array[i].uturn_fuck_up(if_intersection)

        if check_next == predefines.NONE:
            # believe in itself
            car_array[i].intersection_move_point()
            if_intersection = car_array[i].check_if_intersection(car_array[i].point.x, car_array[i].point.y)
            if if_intersection != True:
                intersection = car_array[i].direction_stack.top() # update the direction
                car_array[i].direction = intersection.direction # update the direction
                car_array[i].direction_stack.pop();
                car_array[i].int_step_counter = 0

                car_array[i].intersectionID = predefines.NONE # update the intersectionID
            
            continue
        
        god_damn_it = car_array[i].check_if_in_and_out(check_next, if_intersection)

        # check if entering an intersection
        if (check_next and car_array[i].int_step_counter == 0 and car_array[i].direction_stack.empty()) or god_damn_it or (god_damn_it and edge_case) or (fuck_up == predefines.EDGY and god_damn_it):
            if god_damn_it and edge_case:
                intersection = car_array[i].direction_stack.top() # update the direction
                car_array[i].direction = intersection.direction # update the direction
                car_array[i].direction_stack.pop();
                car_array[i].int_step_counter = 0

            car_array[i].pull_next() # sets the intersectionID (TODO: FoW)

            car_array[i].which_way_to_turn() # sets the path according to intID -^

            car_array[i].turn() # turns based on path-^
            # TODO: check for collisions (IM or nah???)

            # figure out how to move based on direction stack

            car_array[i].intersection_move_point()

        # check if in intersection but not entering
        elif (check_next and if_intersection) or check_next == predefines.NONE:
            # continue moving
            car_array[i].intersection_move_point()

        # check if leaving an intersection
        elif (check_next == False and if_intersection) or (not god_damn_it and edge_case): 
            intersection = car_array[i].direction_stack.top() # update the direction
            car_array[i].check_if_in_and_out(check_next, if_intersection)
            car_array[i].direction = intersection.direction # update the direction
            car_array[i].direction_stack.pop();

            if not car_array[i].direction_stack.empty():
                car_array[i].direction_stack.clear()

            car_array[i].int_step_counter = 0

            car_array[i].intersectionID = predefines.NONE # update the intersectionID
            car_array[i].direction_move_point()
        else:
            car_array[i].direction_move_point()

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
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=1)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=2)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=1)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=2)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=2, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=2, column=1)

        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=7, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=7, column=1)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=7, column=2)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=1)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=2)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=0)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=1)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=2)

        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=7, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=7, column=9)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=7)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=8, column=9)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=7)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=9, column=9)

        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=2, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=2, column=9)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=7)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=1, column=9)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=7)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=8)
        Label(self.window, text='\n             0             \n', bg='khaki').grid(row=0, column=9)

        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=2, column=2)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=2, column=3)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=3, column=2)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=3, column=3)

        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=2, column=6)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=2, column=7)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=3, column=6)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=3, column=7)

        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=6, column=2)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=6, column=3)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=7, column=2)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=7, column=3)

        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=6, column=6)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=6, column=7)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=7, column=6)
        Label(self.window, text='\n             8             \n', bg='indian red').grid(row=7, column=7)

        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=0, column=3)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=0, column=4)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=0, column=5)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=0, column=6)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=1, column=3)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=1, column=4)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=1, column=5)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=1, column=6)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=2, column=4)
        Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=2, column=5)

        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=3, column=0)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=4, column=0)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=5, column=0)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=6, column=0)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=3, column=1)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=4, column=1)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=5, column=1)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=6, column=1)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=4, column=2)
        Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=5, column=2)

        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=3, column=4)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=3, column=5)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=4, column=3)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=4, column=4)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=4, column=5)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=4, column=6)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=5, column=3)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=5, column=4)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=5, column=5)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=5, column=6)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=6, column=5)
        Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=6, column=4)

        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=3, column=9)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=4, column=9)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=5, column=9)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=6, column=9)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=3, column=8)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=4, column=8)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=5, column=8)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=6, column=8)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=4, column=7)
        Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=5, column=7)

        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=9, column=3)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=9, column=4)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=9, column=5)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=9, column=6)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=8, column=3)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=8, column=4)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=8, column=5)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=8, column=6)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=7, column=4)
        Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=7, column=5)

        #for i in range(0, len(self.car_array)):
        #    Label(self.window, text=i, bg='forest green').grid(row=self.car_array[i].point.x, column=self.car_array[i].point.y)

        # this is what it is supposed to do, but trying it with the for loop
        Label(self.window, text='\n             1             \n', bg='forest green').grid(row=self.car_array[0].point.x, column=self.car_array[0].point.y)
        Label(self.window, text='\n             2             \n', bg='forest green').grid(row=self.car_array[1].point.x, column=self.car_array[1].point.y)

    # this updates the cars on the screen
    def only_update_car(self):
        # move the car
        #for i in range(0, len(self.car_array)):
        #    Label(self.window, text=i, bg='forest green').grid(row=self.car_array[i].point.x, column=self.car_array[i].point.y)


        Label(self.window, text='\n             1             \n', bg='forest green').grid(row=self.car_array[0].point.x, column=self.car_array[0].point.y)
        Label(self.window, text='\n             2             \n', bg='forest green').grid(row=self.car_array[1].point.x, column=self.car_array[1].point.y)

        # replace the previous location with the proper color
        # check what type of spot the car is occupying and replace it with the right color
        for i in range(0, len(self.prev_car_array)):
            if self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.EMPTY:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION1:
                Label(self.window, text='\n             1             \n', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION2:
                Label(self.window, text='\n             2             \n', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION3:
                Label(self.window, text='\n             3             \n', bg='deep sky blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION4:
                Label(self.window, text='\n             4             \n', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INTERSECTION5:
                Label(self.window, text='\n             5             \n', bg='dodger blue').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER1:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER2:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER3:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.OUTER_CORNER4:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INNER_CORNER1:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INNER_CORNER2:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INNER_CORNER3:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)
            elif self.previous[self.prev_car_array[i].point.x][self.prev_car_array[i].point.y] == predefines.INNER_CORNER4:
                Label(self.window, text='\n             0             \n', bg='khaki').grid(row=self.prev_car_array[i].point.x, column=self.prev_car_array[i].point.y)

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

def gui_init():
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

    root.geometry('905x515')                            # give the screen a proper size : geometry( ' X x Y ' )
    #cookImage = PhotoImage(file=r"/home/rgusa/Documents/digipen/junior/ECE360/code/working/farts.png")
    return root, gui