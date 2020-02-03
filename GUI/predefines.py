# seed the pseudorandom number generator
import random


def random_number():
    return random.randint(0, 3)


GRID_SIZE = 10
INTERSECTION_SIZE = 3

NONE = -1
UP = 0
DOWN = 1
LEFT = 2
RIGHT = 3

EMPTY = 0
INTERSECTION1 = 1
INTERSECTION2 = 2
INTERSECTION3 = 3
INTERSECTION4 = 4
INTERSECTION5 = 5

OUTER_CORNER1 = -1
OUTER_CORNER2 = -2
OUTER_CORNER3 = -3
OUTER_CORNER4 = -4

INNER_CORNER1 = -5
INNER_CORNER2 = -6
INNER_CORNER3 = -7
INNER_CORNER4 = -8

BUILDING = 8
CAR = 9


class Path:
    # using up down left and right predefines
    def __int__(self, path):
        self.direction = path


def create_graph():
    board = [[EMPTY] * GRID_SIZE for _ in range(GRID_SIZE)]

    # create the city
    board[2][2] = BUILDING
    board[2][3] = BUILDING
    board[3][2] = BUILDING
    board[3][3] = BUILDING

    board[2][6] = BUILDING
    board[2][7] = BUILDING
    board[3][6] = BUILDING
    board[3][7] = BUILDING

    board[6][2] = BUILDING
    board[6][3] = BUILDING
    board[7][2] = BUILDING
    board[7][3] = BUILDING

    board[6][6] = BUILDING
    board[6][7] = BUILDING
    board[7][6] = BUILDING
    board[7][7] = BUILDING

    #create the INTERSECTIONs
    board[0][3] = INTERSECTION1
    board[0][4] = INTERSECTION1
    board[0][5] = INTERSECTION1
    board[0][6] = INTERSECTION1
    board[1][3] = INTERSECTION1
    board[1][4] = INTERSECTION1
    board[1][5] = INTERSECTION1
    board[1][6] = INTERSECTION1
    board[2][4] = INTERSECTION1
    board[2][5] = INTERSECTION1

    board[3][0] = INTERSECTION2
    board[4][0] = INTERSECTION2
    board[5][0] = INTERSECTION2
    board[6][0] = INTERSECTION2
    board[3][1] = INTERSECTION2
    board[4][1] = INTERSECTION2
    board[5][1] = INTERSECTION2
    board[6][1] = INTERSECTION2
    board[4][2] = INTERSECTION2
    board[5][2] = INTERSECTION2

    board[3][4] = INTERSECTION3
    board[3][5] = INTERSECTION3
    board[4][3] = INTERSECTION3
    board[4][4] = INTERSECTION3
    board[4][5] = INTERSECTION3
    board[4][6] = INTERSECTION3
    board[5][3] = INTERSECTION3
    board[5][4] = INTERSECTION3
    board[5][5] = INTERSECTION3
    board[5][6] = INTERSECTION3
    board[6][4] = INTERSECTION3
    board[6][5] = INTERSECTION3

    board[3][9] = INTERSECTION4
    board[4][9] = INTERSECTION4
    board[5][9] = INTERSECTION4
    board[6][9] = INTERSECTION4
    board[3][8] = INTERSECTION4
    board[4][8] = INTERSECTION4
    board[5][8] = INTERSECTION4
    board[6][8] = INTERSECTION4
    board[4][7] = INTERSECTION4
    board[5][7] = INTERSECTION4

    board[9][3] = INTERSECTION5
    board[9][4] = INTERSECTION5
    board[9][5] = INTERSECTION5
    board[9][6] = INTERSECTION5
    board[8][3] = INTERSECTION5
    board[8][4] = INTERSECTION5
    board[8][5] = INTERSECTION5
    board[8][6] = INTERSECTION5
    board[7][4] = INTERSECTION5
    board[7][5] = INTERSECTION5

    board[0][0] = OUTER_CORNER1
    board[0][GRID_SIZE - 1] = OUTER_CORNER2
    board[GRID_SIZE - 1][0] = OUTER_CORNER3
    board[GRID_SIZE - 1][GRID_SIZE - 1] = OUTER_CORNER4

    board[1][1] = INNER_CORNER1
    board[1][GRID_SIZE - 2] = INNER_CORNER2
    board[GRID_SIZE - 2][1] = INNER_CORNER3
    board[GRID_SIZE - 2][GRID_SIZE - 2] = INNER_CORNER4

    return board


intersection1_array = [0, 0, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
intersection2_array = [0, 0, -1], [0, 0, 0], [0, 0, 0], [0, 0, -1]
intersection3_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
intersection4_array = [-1, 0, 0], [0, 0, 0], [0, 0, 0], [-1, 0, 0]
intersection5_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 0, 0]


def turn(current_direction, path, intersection):
    # GOING UP (cant go up through 5)
    if current_direction == UP and path.direction == UP:
        # go through intersection (cant be int. 1)
        # check which intersection and occupy the correct spots
        if intersection == INTERSECTION2:
            intersection_array = [0, 4, -1], [0, 3, 0], [0, 2, 0], [0, 1, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 4, -1], [0, 0, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 4], [0, 0, 3], [0, 0, 2], [-1, 0, 1]

    elif current_direction == UP and path.direction == DOWN:
        # take a u-turn
        if intersection == INTERSECTION1:
            intersection_array = [0, 0, 0, 0], [0, 0, 0, 0], [-1, 2, 1, -1]
        elif intersection == INTERSECTION2:
            intersection_array = [0, 0, -1], [0, 0, 0], [0, 0, 0], [2, 1, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 0, 0], [-1, 2, 1, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 0], [0, 0, 0], [0, 0, 0], [-1, 2, 1]

    elif current_direction == UP and path.direction == LEFT:
        # take a left turn (cant turn left at 2)
        if intersection == INTERSECTION1:
            intersection_array = [5, 4, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [5, 4, 3, 0], [0, 0, 2, 0], [-1, 0, 1, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 0], [5, 4, 3], [0, 0, 2], [-1, 0, 1]
    elif current_direction == UP and path.direction == RIGHT:
        # take a right turn (cant turn right at 4)
        if intersection == INTERSECTION1:
            intersection_array = [0, 0, 0, 0], [0, 0, 2, 3], [-1, 0, 1, -1]
        elif intersection == INTERSECTION2:
            intersection_array = [0, 0, -1], [0, 0, 0], [0, 2, 3], [0, 1, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 0], [0, 0, 2, 3], [-1, 0, 1, -1]

    # GOING DOWN (cant go down from 1)
    elif current_direction == DOWN and path.direction == UP:
        # take a u-turn
        if intersection == INTERSECTION2:
            intersection_array = [1, 2, -1], [0, 0, 0], [0, 0, 0], [0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 1, 2, -1], [0, 0, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 1, 2], [0, 0, 0], [0, 0, 0], [-1, 0, 0]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 1, 2, -1], [0, 0, 0, 0], [0, 0, 0, 0]
    elif current_direction == DOWN and path.direction == DOWN:
        # go through intersection (cant go down through 5)
        if intersection == INTERSECTION2:
            intersection_array = [1, 0, -1], [2, 0, 0], [3, 0, 0], [4, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 0, 0], [-1, 4, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 1, 0], [0, 2, 0], [0, 3, 0], [-1, 4, 0]
    elif current_direction == DOWN and path.direction == RIGHT:
        # take a right turn (cant turn right at 2)
        if intersection == INTERSECTION3:
            intersection_array = [-1, 1, 0, -1], [3, 2, 0, 0], [0, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 1, 0], [3, 2, 0], [0, 0, 0], [-1, 0, 0]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 1, 0, -1], [3, 2, 0, 0], [0, 0, 0, 0]
    elif current_direction == DOWN and path.direction == LEFT:
        # take a left turn (cant turn left at 4)
        if intersection == INTERSECTION2:
            intersection_array = [1, 0, -1], [2, 0, 0], [3, 4, 5], [0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 4, 5], [-1, 0, 0, -1]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 1, 0, -1], [0, 2, 0, 0], [0, 3, 4, 5]

    # GOING LEFT (cant go left from 4)
    elif current_direction == LEFT and path.direction == UP:
        # take a right turn (cant go up from 1)
        if intersection == INTERSECTION2:
            intersection_array = [0, 3, -1], [0, 2, 1], [0, 0, 0], [0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 3, -1], [0, 0, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 0, 3, -1], [0, 0, 2, 1], [0, 0, 0, 0]
    elif current_direction == LEFT and path.direction == DOWN:
        # take a left turn (cant go down from 5)
        if intersection == INTERSECTION1:
            intersection_array = [0, 3, 2, 1], [0, 4, 0, 0], [-1, 5, 0, -1]
        elif intersection == INTERSECTION2:
            intersection_array = [0, 0, -1], [3, 2, 1], [4, 0, 0], [5, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 3, 2, 1], [0, 4, 0, 0], [-1, 5, 0, -1]
    elif current_direction == LEFT and path.direction == LEFT:
        # go through intersection (cant go through 2)
        if intersection == INTERSECTION1:
            intersection_array = [4, 3, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [4, 3, 2, 1], [0, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION5:
            intersection_array = [-1, 0, 0, -1], [4, 3, 2, 1], [0, 0, 0, 0]
    elif current_direction == LEFT and path.direction == RIGHT:
        # take a u-turn
        if intersection == INTERSECTION1:
            intersection_array = [0, 0, 0, 1], [0, 0, 0, 2], [-1, 0, 0, -1]
        elif intersection == INTERSECTION2:
            intersection_array = [0, 0, -1], [0, 0, 1], [0, 0, 2], [0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 1], [0, 0, 0, 2], [-1, 0, 0, -1]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 1], [0, 0, 0, 2]

    # GOING RIGHT (cant go right from 2)
    elif current_direction == RIGHT and path.direction == UP:
        # take a left turn (cant go up from 1)
        if intersection == INTERSECTION3:
            intersection_array = [-1, 0, 5, -1], [0, 0, 4, 0], [1, 2, 3, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 5], [0, 0, 4], [1, 2, 3], [-1, 0, 0]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 0, 5, -1], [0, 0, 4, 0], [1, 2, 3, 0]
    elif current_direction == RIGHT and path.direction == DOWN:
        # take a right turn (cant go down from 5)
        if intersection == INTERSECTION1:
            intersection_array = [0, 0, 0, 0], [1, 2, 0, 0], [-1, 3, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 0, 0], [-1, 3, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 0], [0, 0, 0], [1, 2, 0], [-1, 3, 0]
    elif current_direction == RIGHT and path.direction == LEFT:
        # take a u-turn
        if intersection == INTERSECTION1:
            intersection_array = [2, 0, 0, 0], [1, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [2, 0, 0, 0], [1, 0, 0, 0], [-1, 0, 0, -1]
        elif intersection == INTERSECTION4:
            intersection_array = [-1, 0, 0], [2, 0, 0], [1, 0, 0], [-1, 0, 0]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 0, 0, -1], [2, 0, 0, 0], [1, 0, 0, 0]
    elif current_direction == RIGHT and path.direction == RIGHT:
        # go through intersection (cant go through 4)
        if intersection == INTERSECTION1:
            intersection_array = [0, 0, 0, 0], [1, 2, 3, 4], [-1, 0, 0, -1]
        elif intersection == INTERSECTION3:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 3, 4], [-1, 0, 0, -1]
        elif intersection5_array == INTERSECTION5:
            intersection_array = [-1, 0, 0, -1], [0, 0, 0, 0], [1, 2, 3, 4]

    return intersection_array

