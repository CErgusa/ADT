import predefines
import random
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
    def __init__(self, direction, car_num):
        self.speed = 1
        self.ID = car_num

        if car_num == 0:
            self.point = Point(0, 0)
        else:
            self.point = Point(9, 9)

        self.direction = direction

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


def if_corner(car, previous):
    if -1 >= previous[car.point.x][car.point.y] >= -8:
        return True
    else:
        return False


def change_position(car, previous):
    if previous[car.point.x][car.point.y] == -1:
        car.direction = predefines.DOWN
        return car
    elif previous[car.point.x][car.point.y] == -2:
        car.direction = predefines.LEFT
        return car
    elif previous[car.point.x][car.point.y] == -3:
        car.direction = predefines.RIGHT
        return car
    elif previous[car.point.x][car.point.y] == -4:
        car.direction = predefines.UP
        return car
    elif previous[car.point.x][car.point.y] == -5:
        car.direction = predefines.RIGHT
        return car
    elif previous[car.point.x][car.point.y] == -6:
        car.direction = predefines.DOWN
        return car
    elif previous[car.point.x][car.point.y] == -7:
        car.direction = predefines.LEFT
        return car
    elif previous[car.point.x][car.point.y] == -8:
        car.direction = predefines.UP
        return car

    return car


def turn():
    return False


def move_point(car, board, previous):
    if if_corner(car, previous):
        car = change_position(car, previous)

    if car.direction == predefines.UP:
        # reset board
        board[car.point.x][car.point.y] = previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.x = car.point.x - 1
        # move external grid point
        board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.DOWN:
        # reset board
        board[car.point.x][car.point.y] = previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.x = car.point.x + 1
        # move external grid point
        board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.LEFT:
        # reset board
        board[car.point.x][car.point.y] = previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.y = car.point.y - 1
        # move external grid point
        board[car.point.x][car.point.y] = predefines.CAR
        return car
    elif car.direction == predefines.RIGHT:
        # reset board
        board[car.point.x][car.point.y] = previous[car.point.x][car.point.y]
        # move personal grid point
        car.point.y = car.point.y + 1
        # move external grid point
        board[car.point.x][car.point.y] = predefines.CAR
        return car


def check_if_intersection(car, previous):
    # check if in intersection
    if 0 < previous[car.point.x][car.point.y] <= 5:
        return True
    return False


def check_which_intersection(car, previous):
    if 0 < previous[car.point.x][car.point.y] <= 5:
        return previous[car.point.x][car.point.y]
    return -1


def check_next(car, previous):
    if if_corner(car, previous):
        return 0
    # check if intersection is 1 spot away in current direction
    if car.direction == predefines.UP:
        # check if x + 1 is intersection
        car.point.x = car.point.x - 1
        check = check_if_intersection(car, previous)
        car.point.x = car.point.x + 1
        return check
    elif car.direction == predefines.DOWN:
        # check if x - 1 is intersection
        car.point.x = car.point.x + 1
        check = check_if_intersection(car, previous)
        car.point.x = car.point.x - 1
        return check
    elif car.direction == predefines.RIGHT:
        # check if y + 1 is intersection
        car.point.y = car.point.y - 1
        check = check_if_intersection(car, previous)
        car.point.y = car.point.y + 1
        return check
    elif car.direction == predefines.LEFT:
        # check if y - 1 is intersection
        car.point.y = car.point.y + 1
        check = check_if_intersection(car, previous)
        car.point.y = car.point.y - 1
        return check


def collision_detection(car1,car2, board, previous):
    return False


def pull_next(car, previous):
    if if_corner(car, previous):
        car = change_position(car, previous)
    # check if intersection is 1 spot away in current direction
    if car.direction == predefines.UP:
        # check if x + 1 is intersection
        car.point.x = car.point.x - 1
        check = check_which_intersection(car, previous)
        car.point.x = car.point.x + 1
        return check
    elif car.direction == predefines.DOWN:
        # check if x - 1 is intersection
        car.point.x = car.point.x + 1
        check = check_which_intersection(car, previous)
        car.point.x = car.point.x - 1
        return check
    elif car.direction == predefines.RIGHT:
        # check if y + 1 is intersection
        car.point.y = car.point.y - 1
        check = check_which_intersection(car, previous)
        car.point.y = car.point.y + 1
        return check
    elif car.direction == predefines.LEFT:
        # check if y - 1 is intersection
        car.point.y = car.point.y + 1
        check = check_which_intersection(car, previous)
        car.point.y = car.point.y - 1
        return check


def intersection_mapping(car, car2, board, previous):
    if pull_next(car, previous) == pull_next(car2, previous):
        collision_detection(car, car2, board, previous)
    else:
        return move_point(car, board, previous)
    return car


def car_update(car, car2, board, previous):
    # check if in intersection
    #       if true -> check for collisions
    #       else    -> keep going
    # check if intersection is 1 spot away in current direction
    #       if true -> randomly choose a possible path
    #       else    -> keep going
    if check_next(car, previous):
        return intersection_mapping(car, car2, board, previous)
    else:
        return move_point(car, board, previous)


def main():
    board = predefines.create_graph()
    previous = predefines.create_graph()
    p1 = Point(0, 0)
    p2 = Point(predefines.GRID_SIZE - 1, predefines.GRID_SIZE - 1)

    board[p1.x][p1.y] = predefines.CAR
    board[p2.x][p2.y] = predefines.CAR

    car1 = Car(predefines.LEFT, 0)
    car2 = Car(predefines.RIGHT, 1)

    print_graph(board)

    # predefines.tkinter_init(car1, car2)

    while True:
        car1.print_car()
        car2.print_car()

        car1 = car_update(car1, car2, board, previous)
        car2 = car_update(car2, car1, board, previous)

        print_graph(board)
        car1.print_car()
        car2.print_car()
        # predefines.tkinter_init(car1, car2)


if __name__ == "__main__":
    main()
