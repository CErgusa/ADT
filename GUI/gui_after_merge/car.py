from point import Point
from rand import Random
from stack import Stack
from intersection import Intersection
import predefines


class Car(Point):
    def __init__(self, direction, path, car_num, previous, board):
        self.speed = 1
        self.ID = car_num

        if car_num == 0:
            self.point = Point(0, 0)
        else:
            self.point = Point(9, 9)

        self.intersectionID = 0
        self.int_step_counter = 0
        self.int_array = [] #TODO: fix this in turn. from return to self.int_array
        self.direction_stack = Stack()
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
    
    def turn_corner(self):
        # checks which corner, and takes the turn (ie. changes direction) accordingly
        if self.previous[self.point.x][self.point.y] == predefines.OUTER_CORNER1:
            self.direction = predefines.DOWN
        elif self.previous[self.point.x][self.point.y] == predefines.INNER_CORNER1:
            self.direction = predefines.RIGHT
        elif self.previous[self.point.x][self.point.y] == predefines.OUTER_CORNER2:
            self.direction = predefines.LEFT
        elif self.previous[self.point.x][self.point.y] == predefines.INNER_CORNER2:
            self.direction = predefines.DOWN
        elif self.previous[self.point.x][self.point.y] == predefines.OUTER_CORNER3:
            self.direction = predefines.RIGHT
        elif self.previous[self.point.x][self.point.y] == predefines.INNER_CORNER3:
            self.direction = predefines.UP
        elif self.previous[self.point.x][self.point.y] == predefines.OUTER_CORNER4:
            self.direction = predefines.UP
        elif self.previous[self.point.x][self.point.y] == predefines.INNER_CORNER4:
            self.direction = predefines.LEFT

        return self

    def move_point_down(self):
        # reset board
        self.board[self.point.x][self.point.y] = self.previous[self.point.x][self.point.y]
        # move personal grid point
        self.point.x = self.point.x + 1
        # move external grid point
        self.board[self.point.x][self.point.y] = predefines.CAR
        return self

    def move_point_up(self):
        # reset board
        self.board[self.point.x][self.point.y] = self.previous[self.point.x][self.point.y]
        # move personal grid point
        self.point.x = self.point.x - 1
        # move external grid point
        self.board[self.point.x][self.point.y] = predefines.CAR
        return self

    def move_point_right(self):
        # reset board
        self.board[self.point.x][self.point.y] = self.previous[self.point.x][self.point.y]
        # move personal grid point
        self.point.y = self.point.y + 1
        # move external grid point
        self.board[self.point.x][self.point.y] = predefines.CAR
        return self

    def move_point_left(self):
        # reset board
        self.board[self.point.x][self.point.y] = self.previous[self.point.x][self.point.y]
        # move personal grid point
        self.point.y = self.point.y - 1
        # move external grid point
        self.board[self.point.x][self.point.y] = predefines.CAR
        return self

    def intersection_move_point(self):
        if self.direction_stack.empty() == False:
            farts = self.direction_stack.top()
            self.direction = farts.direction
            self.int_step_counter = farts.step
            self.direction_stack.pop()

            self.direction_move_point()
            return self
        return self

    def direction_move_point(self):
        if self.if_corner():
            self.turn_corner()

        if self.direction == predefines.UP:
            return self.move_point_up()
        elif self.direction == predefines.DOWN:
            return self.move_point_down()
        elif self.direction == predefines.LEFT:
            return self.move_point_left()
        elif self.direction == predefines.RIGHT:
            return self.move_point_right()
        return self


    def if_corner(self):
        if -1 >= self.previous[self.point.x][self.point.y] >= -8:
            return True
        else:
            return False


    def check_if_intersection(self, x, y):
        # check if in intersection
        if 0 < self.previous[x][y] <= 5:
            return True
        return False

    def check_if_intersection_edgy(self, x, y):
        # check if in intersection
        farts = False
        if self.previous[x][y] == 8 and self.path == predefines.DOWN:
            farts = True

        if 0 < self.previous[x][y] <= 5:
            return True, farts
        return False, farts

    def check_which_intersection(self, x, y):
        if 0 < self.previous[x][y] <= 5:
            return self.previous[x][y]
        return -1

    
    def check_if_in_and_out(self, check_next, if_intersection):
        if if_intersection:
            which_out = self.check_which_intersection(self.point.x, self.point.y)

            if check_next:
                which_in = self.check_next_which()

                if which_out != which_in:
                    return True
            else:
                return False
        else:
            return False


    def check_next_which(self):
        # check which intersection is 1 spot away in current direction
        if self.direction == predefines.UP:
            # check if x - 1 is intersection
            return self.check_which_intersection(self.point.x - 1, self.point.y)
        elif self.direction == predefines.DOWN:
            # check if x + 1 is intersection
            return self.check_which_intersection(self.point.x + 1, self.point.y)
        elif self.direction == predefines.RIGHT:
            # check if y - 1 is intersection
            return self.check_which_intersection(self.point.x, self.point.y + 1)
        elif self.direction == predefines.LEFT:
            # check if y + 1 is intersection
            return self.check_which_intersection(self.point.x, self.point.y - 1)


    def if_building(self, x, y):
        if self.previous[x][y] == 8:
            return True
        else:
            return False


    def uturn_fuck_up(self, if_intersection):
        # make sure you are in intersection
        if not if_intersection:
            return predefines.NONE

        # make sure it only works during UTURNs
        if self.path != predefines.DOWN:
            return predefines.NONE

        edge, which = self.edge()
        if edge:
            return predefines.EDGE

        ass_rape = self.direction_stack.top()
        next_move = ass_rape.direction

        if next_move == predefines.UP:
            # check if x - 1 is intersection
            intersection = self.check_if_intersection(self.point.x - 1, self.point.y)
        elif next_move == predefines.DOWN:
            # check if x + 1 is intersection
            intersection = self.check_if_intersection(self.point.x + 1, self.point.y)
        elif next_move == predefines.RIGHT:
            # check if y - 1 is intersection
            intersection = self.check_if_intersection(self.point.x, self.point.y + 1)
        elif next_move == predefines.LEFT:
            # check if y + 1 is intersection
            intersection = self.check_if_intersection(self.point.x, self.point.y - 1)
        else:
            return
        
        if self.int_step_counter > 0 and if_intersection and intersection == True:
            return predefines.EDGY
        

    def edge(self):
        if self.point.x == 0:
            # only possible if dir = UP
            return True, predefines.NORTH
        elif self.point.y == 9:
            # only possible if dir = LEFT
            return True, predefines.EAST
        elif self.point.x == 9:
            # only possible if dir = DOWN
            return True, predefines.SOUTH
        elif self.point.y == 0:
            # only possible if dir = RIGHT
            return True, predefines.WEST
        return False, predefines.NONE


    def if_edge_perp(self, which):
        if self.direction == predefines.UP:
            if which == predefines.NORTH:
                return True
            else:
                return False

        elif self.direction == predefines.DOWN:
            if which == predefines.SOUTH:
                return True
            else:
                return False

        elif self.direction == predefines.LEFT:
            if which == predefines.WEST:
                return True
            else:
                return False

        elif self.direction == predefines.RIGHT:
            if which == predefines.EAST:
                return True
            else:
                return False
        return predefines.idk
    
    
    def check_next_if(self):
        if self.if_corner():
            return False, False, False

        edge, which = self.edge()
        if_intersection_current = self.check_if_intersection(self.point.x, self.point.y)
        
        if edge == True and if_intersection_current:
            if self.if_edge_perp(which):
                return predefines.NONE, False, if_intersection_current
             
        # check if intersection is 1 spot away in current direction
        if self.direction == predefines.UP:
            # check if x - 1 is intersection
            if_intersection_next, edge_case =  self.check_if_intersection_edgy(self.point.x - 1, self.point.y)
        elif self.direction == predefines.DOWN:
            # check if x + 1 is intersection
            if_intersection_next, edge_case =  self.check_if_intersection_edgy(self.point.x + 1, self.point.y)
        elif self.direction == predefines.RIGHT:
            # check if y - 1 is intersection
            if_intersection_next, edge_case =  self.check_if_intersection_edgy(self.point.x, self.point.y + 1)
        elif self.direction == predefines.LEFT:
            # check if y + 1 is intersection
            if_intersection_next, edge_case = self.check_if_intersection_edgy(self.point.x, self.point.y - 1)
        
        if edge_case:
            check_next = self.direction_stack.top()
            # check if intersection is 1 spot away in current direction
            if check_next.direction == predefines.UP:
                # check if x - 1 is intersection
                if_intersection_next, edge_case2 =  self.check_if_intersection_edgy(self.point.x - 1, self.point.y)
            elif check_next.direction == predefines.DOWN:
                # check if x + 1 is intersection
                if_intersection_next, edge_case2 =  self.check_if_intersection_edgy(self.point.x + 1, self.point.y)
            elif check_next.direction == predefines.RIGHT:
                # check if y - 1 is intersection
                if_intersection_next, edge_case2 =  self.check_if_intersection_edgy(self.point.x, self.point.y + 1)
            elif check_next.direction == predefines.LEFT:
                # check if y + 1 is intersection
                if_intersection_next, edge_case2 = self.check_if_intersection_edgy(self.point.x, self.point.y - 1)

        return if_intersection_next, edge_case, if_intersection_current


    def pull_next(self):
        if self.if_corner():
            self = self.turn_corner()

        # check if intersection is 1 spot away in current direction and output the intersection number
        if self.direction == predefines.UP:
            # check if x - 1 is intersection (and which one)
            self.intersectionID = self.check_which_intersection(self.point.x - 1, self.point.y)
        elif self.direction == predefines.DOWN:
            # check if x + 1 is intersection (and which one)
            self.intersectionID = self.check_which_intersection(self.point.x + 1, self.point.y)
        elif self.direction == predefines.RIGHT:
            # check if y - 1 is intersection (and which one)
            self.intersectionID = self.check_which_intersection(self.point.x, self.point.y + 1)
        elif self.direction == predefines.LEFT:
            # check if y + 1 is intersection (and which one)
            self.intersectionID = self.check_which_intersection(self.point.x, self.point.y - 1)
    
    def which_way_to_turn(self):
        # create a random number generator
        rand = Random()
        if self.intersectionID == predefines.INTERSECTION1:
            # can't be coming down
            if self.direction == predefines.UP:
                # can go down, left, or right
                self.path = rand.og_randint(1, 3)
            elif self.direction == predefines.LEFT:
                # down, left or up
                self.path = rand.og_randint(0, 2)
            elif self.direction == predefines.RIGHT:
                # up, down, or right
                self.path = rand.skip_randint(0, 3, 2)
            else:
                return
        elif self.intersectionID == predefines.INTERSECTION2:
            # can't be coming right
            if self.direction == predefines.UP:
                # can go up, down, or right
                self.path = rand.skip_randint(0, 3, 2)
            elif self.direction == predefines.DOWN:
                # up, down, left
                self.path = rand.og_randint(0, 2)
            elif self.direction == predefines.LEFT:
                # down, left, right
                self.path = rand.og_randint(1, 3)
            else:
                return
        elif self.intersectionID == predefines.INTERSECTION3:
            # can go anywhere
            self.path = rand.og_randint(0, 3) # up, down, left, right

        elif self.intersectionID == predefines.INTERSECTION4:
            # can't be coming left
            if self.direction == predefines.UP:
                # can go up, down, or left
                self.path = rand.og_randint(0, 2)
            elif self.direction == predefines.DOWN:
                # up, down, right
                self.path = rand.skip_randint(0, 3, 2)
            elif self.direction == predefines.RIGHT:
                # down, left, right
                self.path = rand.og_randint(1, 3)
            else:
                return
        elif self.intersectionID == predefines.INTERSECTION5:
            # can't be coming up
            if self.direction == predefines.RIGHT:
                # can go up, down, or right
                self.path = rand.og_randint(0, 2)
            elif self.direction == predefines.DOWN:
                # down, left, right (can't go up)
                self.path = rand.og_randint(1, 3)
            elif self.direction == predefines.LEFT:
                # up, down, left
                self.path = rand.skip_randint(0, 3, 2)
            else:
                return
        else:
            return

    def turn(self):    
        #print('ID:', self.ID, 'direction:', self.direction, 'path:', self.path, 'point:', self.point.x, ',', self.point.y)
        if self.path == None:
            self.which_way_to_turn()

        if not self.direction_stack.empty():
            self.direction_stack.clear()

        # GOING UP (cant go up through 5)
        if self.direction == predefines.UP and self.path == predefines.UP:
            # go through intersection (cant be int. 1 or 5)
            # check which intersection and occupy the correct spots
            self.direction_stack.push(Intersection(predefines.UP, 5))
            self.direction_stack.push(Intersection(predefines.UP, 4))
            self.direction_stack.push(Intersection(predefines.UP, 3))
            self.direction_stack.push(Intersection(predefines.UP, 2))
            self.direction_stack.push(Intersection(predefines.UP, 1))

            if self.intersectionID == predefines.INTERSECTION2:
                self.int_array = [0, 4, -1], [0, 3, 0], [0, 2, 0], [0, 1, -1]
                # TODO: dir_stack implementation
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 4, -1], [0, 0, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 4], [0, 0, 3], [0, 0, 2], [-1, 0, 1]
            else:
                return

        elif self.direction == predefines.UP and self.path == predefines.DOWN:
            # take a u-turn
            self.direction_stack.push(Intersection(predefines.DOWN, 4))
            self.direction_stack.push(Intersection(predefines.DOWN, 3))
            self.direction_stack.push(Intersection(predefines.LEFT, 2))
            self.direction_stack.push(Intersection(predefines.UP, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 0, 0, 0], [0, 0, 0, 0], [-1, 2, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION2:
                self.int_array = [0, 0, -1], [0, 0, 0], [0, 0, 0], [2, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 0, 0], [-1, 2, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 0], [0, 0, 0], [0, 0, 0], [-1, 2, 1]
            else:
                return

        elif self.direction == predefines.UP and self.path == predefines.LEFT:
            # take a left turn (cant turn left at 2)
            self.direction_stack.push(Intersection(predefines.LEFT, 6))
            self.direction_stack.push(Intersection(predefines.LEFT, 5))
            self.direction_stack.push(Intersection(predefines.LEFT, 4))
            self.direction_stack.push(Intersection(predefines.UP, 3))
            self.direction_stack.push(Intersection(predefines.UP, 2))
            self.direction_stack.push(Intersection(predefines.UP, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [5, 4, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [5, 4, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 0], [5, 4, 3], [0, 0, 2], [-1, 0, 1]
            else:
                return

        elif self.direction == predefines.UP and self.path == predefines.RIGHT:
            # take a right turn (cant turn right at 4)
            self.direction_stack.push(Intersection(predefines.RIGHT, 4))
            self.direction_stack.push(Intersection(predefines.RIGHT, 3))
            self.direction_stack.push(Intersection(predefines.UP, 2))
            self.direction_stack.push(Intersection(predefines.UP, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 0, 0, 0], [0, 0, 2, 3], [-1, 0, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION2:
                self.int_array = [0, 0, -1], [0, 0, 0], [0, 2, 3], [0, 1, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 2, 3], [-1, 0, 1, -1]
            else:
                return


        # GOING DOWN (cant go down from 1)
        elif self.direction == predefines.DOWN and self.path == predefines.UP:
            # go through self.intersectionID(cant go down through 5)
            self.direction_stack.push(Intersection(predefines.DOWN, 5))
            self.direction_stack.push(Intersection(predefines.DOWN, 4))
            self.direction_stack.push(Intersection(predefines.DOWN, 3))
            self.direction_stack.push(Intersection(predefines.DOWN, 2))
            self.direction_stack.push(Intersection(predefines.DOWN, 1))

            if self.intersectionID == predefines.INTERSECTION2:
                self.int_array = [1, 0, -1], [2, 0, 0], [3, 0, 0], [4, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION3:
                self.int_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 0, 0], [-1, 4, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION4:
                self.int_array = [-1, 1, 0], [0, 2, 0], [0, 3, 0], [-1, 4, 0]
            else:
                return

        elif self.direction == predefines.DOWN and self.path == predefines.DOWN:
            # take a u-turn
            self.direction_stack.push(Intersection(predefines.UP, 4))
            self.direction_stack.push(Intersection(predefines.UP, 3))
            self.direction_stack.push(Intersection(predefines.RIGHT, 2))
            self.direction_stack.push(Intersection(predefines.DOWN, 1))

            if self.intersectionID == predefines.INTERSECTION2:
                self.int_array = [1, 2, -1], [0, 0, 0], [0, 0, 0], [0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION3:
                self.int_array = [-1, 1, 2, -1], [0, 0, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION4:
                self.int_array = [-1, 1, 2], [0, 0, 0], [0, 0, 0], [-1, 0, 0]
            elif self.intersectionID == predefines.INTERSECTION5:
                self.int_array = [-1, 1, 2, -1], [0, 0, 0, 0], [0, 0, 0, 0]
            else:
                return

        elif self.direction == predefines.DOWN and self.path == predefines.RIGHT:
            # take a right turn (cant turn right at 2)
            self.direction_stack.push(Intersection(predefines.LEFT, 4))
            self.direction_stack.push(Intersection(predefines.LEFT, 3))
            self.direction_stack.push(Intersection(predefines.DOWN, 2))
            self.direction_stack.push(Intersection(predefines.DOWN, 1))

            if self.intersectionID == predefines.INTERSECTION3:
                self.int_array = [-1, 1, 0, -1], [3, 2, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION4:
                self.int_array = [-1, 1, 0], [3, 2, 0], [0, 0, 0], [-1, 0, 0]
            elif self.intersectionID == predefines.INTERSECTION5:
                self.int_array = [-1, 1, 0, -1], [3, 2, 0, 0], [0, 0, 0, 0]
            else:
                return

        elif self.direction == predefines.DOWN and self.path == predefines.LEFT:
            # take a left turn (cant turn left at 4)
            self.direction_stack.push(Intersection(predefines.RIGHT, 6))
            self.direction_stack.push(Intersection(predefines.RIGHT, 5))
            self.direction_stack.push(Intersection(predefines.RIGHT, 4))
            self.direction_stack.push(Intersection(predefines.DOWN, 3))
            self.direction_stack.push(Intersection(predefines.DOWN, 2))
            self.direction_stack.push(Intersection(predefines.DOWN, 1))

            if self.intersectionID== predefines.INTERSECTION2:
                self.int_array = [1, 0, -1], [2, 0, 0], [3, 4, 5], [0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 4, 5], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION5:
                self.int_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 4, 5]
            else:
                return       


        # GOING LEFT (cant go left from 4)
        elif self.direction == predefines.LEFT and self.path == predefines.UP:
            # go through self.intersectionID(cant go through 2)
            self.direction_stack.push(Intersection(predefines.LEFT, 5))
            self.direction_stack.push(Intersection(predefines.LEFT, 4))
            self.direction_stack.push(Intersection(predefines.LEFT, 3))
            self.direction_stack.push(Intersection(predefines.LEFT, 2))
            self.direction_stack.push(Intersection(predefines.LEFT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [4, 3, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [4, 3, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION5:
                self.int_array = [-1, 0, 0, -1], [4, 3, 2, 1], [0, 0, 0, 0]
            else:
                return
            
        elif self.direction == predefines.LEFT and self.path == predefines.DOWN:
            # take a u-turn
            self.direction_stack.push(Intersection(predefines.RIGHT, 4))
            self.direction_stack.push(Intersection(predefines.RIGHT, 3))
            self.direction_stack.push(Intersection(predefines.DOWN, 2))
            self.direction_stack.push(Intersection(predefines.LEFT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 0, 0, 1], [0, 0, 0, 2], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION2:
                self.int_array = [0, 0, -1], [0, 0, 1], [0, 0, 2], [0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 1], [0, 0, 0, 2], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION5:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 1], [0, 0, 0, 2]
            else:
                return
            
        elif self.direction == predefines.LEFT and self.path == predefines.LEFT:
            # take a left turn (cant go down from 5)
            self.direction_stack.push(Intersection(predefines.DOWN, 6))
            self.direction_stack.push(Intersection(predefines.DOWN, 5))
            self.direction_stack.push(Intersection(predefines.DOWN, 4))
            self.direction_stack.push(Intersection(predefines.LEFT, 3))
            self.direction_stack.push(Intersection(predefines.LEFT, 2))
            self.direction_stack.push(Intersection(predefines.LEFT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 3, 2, 1], [0, 4, 0, 0], [-1, 5, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION2:
                self.int_array = [0, 0, -1], [3, 2, 1], [4, 0, 0], [5, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 3, 2, 1], [0, 4, 0, 0], [-1, 5, 0, -1]
            else:
                return

        elif self.direction == predefines.LEFT and self.path == predefines.RIGHT:
            # take a right turn (cant go up from 1)
            self.direction_stack.push(Intersection(predefines.UP, 4))
            self.direction_stack.push(Intersection(predefines.UP, 3))
            self.direction_stack.push(Intersection(predefines.LEFT, 2))
            self.direction_stack.push(Intersection(predefines.LEFT, 1))

            if self.intersectionID == predefines.INTERSECTION2:
                self.int_array = [0, 3, -1], [0, 2, 1], [0, 0, 0], [0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION3:
                self.int_array = [-1, 0, 3, -1], [0, 0, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION5:
                self.int_array = [-1, 0, 3, -1], [0, 0, 2, 1], [0, 0, 0, 0]
            else:
                return


        # GOING RIGHT (cant go right from 2)
        elif self.direction == predefines.RIGHT and self.path == predefines.UP:
            # go through intersection(cant go through 4)
            # TODO: Fix leaving intersection to work with this in car_update
            self.direction_stack.push(Intersection(predefines.RIGHT, 5)) # 5 is used for the exiting direction and path
            self.direction_stack.push(Intersection(predefines.RIGHT, 4))
            self.direction_stack.push(Intersection(predefines.RIGHT, 3))
            self.direction_stack.push(Intersection(predefines.RIGHT, 2))
            self.direction_stack.push(Intersection(predefines.RIGHT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 0, 0, 0], [1, 2, 3, 4], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 3, 4], [-1, 0, 0, -1]
            elif self.intersectionID == predefines.INTERSECTION5:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 3, 4]
            else:
                return
            
        elif self.direction == predefines.RIGHT and self.path == predefines.DOWN:
            # take a u-turn
            self.direction_stack.push(Intersection(predefines.LEFT, 4))
            self.direction_stack.push(Intersection(predefines.LEFT, 3))
            self.direction_stack.push(Intersection(predefines.UP, 2))
            self.direction_stack.push(Intersection(predefines.RIGHT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [2, 0, 0, 0], [1, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [2, 0, 0, 0], [1, 0, 0, 0], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 0], [2, 0, 0], [1, 0, 0], [-1, 0, 0]               
            elif self.intersectionID== predefines.INTERSECTION5:
                self.int_array = [-1, 0, 0, -1], [2, 0, 0, 0], [1, 0, 0, 0]
            else:
                return
            
        elif self.direction == predefines.RIGHT and self.path == predefines.LEFT:
            # decide which way to turn in the intersection
            # take a left turn (cant go up from 1)
            self.direction_stack.push(Intersection(predefines.UP, 6))
            self.direction_stack.push(Intersection(predefines.UP, 5))
            self.direction_stack.push(Intersection(predefines.UP, 4))
            self.direction_stack.push(Intersection(predefines.RIGHT, 3))
            self.direction_stack.push(Intersection(predefines.RIGHT, 2))
            self.direction_stack.push(Intersection(predefines.RIGHT, 1))

            if self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 5, -1], [0, 0, 4, 0], [1, 2, 3, 0], [-1, 0, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 5], [0, 0, 4], [1, 2, 3], [-1, 0, 0]
            elif self.intersectionID== predefines.INTERSECTION5:
                self.int_array = [-1, 0, 5, -1], [0, 0, 4, 0], [1, 2, 3, 0]
            else:
                return

        elif self.direction == predefines.RIGHT and self.path == predefines.RIGHT:
            # take a right turn (cant go down from 5)
            self.direction_stack.push(Intersection(predefines.DOWN, 4))
            self.direction_stack.push(Intersection(predefines.DOWN, 3))
            self.direction_stack.push(Intersection(predefines.RIGHT, 2))
            self.direction_stack.push(Intersection(predefines.RIGHT, 1))

            if self.intersectionID== predefines.INTERSECTION1:
                self.int_array = [0, 0, 0, 0], [1, 2, 0, 0], [-1, 3, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION3:
                self.int_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 0, 0], [-1, 3, 0, -1]
            elif self.intersectionID== predefines.INTERSECTION4:
                self.int_array = [-1, 0, 0], [0, 0, 0], [1, 2, 0], [-1, 3, 0]
            else:
                return
        else:
            return
