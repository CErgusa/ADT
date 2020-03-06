import predefines

class Board():
    def __init__(self):
        self.board = [[predefines.EMPTY] * predefines.GRID_SIZE for _ in range(predefines.GRID_SIZE)]

        # create the city
        self.board[2][2] = predefines.BUILDING
        self.board[2][3] = predefines.BUILDING
        self.board[3][2] = predefines.BUILDING
        self.board[3][3] = predefines.BUILDING

        self.board[2][6] = predefines.BUILDING
        self.board[2][7] = predefines.BUILDING
        self.board[3][6] = predefines.BUILDING
        self.board[3][7] = predefines.BUILDING

        self.board[6][2] = predefines.BUILDING
        self.board[6][3] = predefines.BUILDING
        self.board[7][2] = predefines.BUILDING
        self.board[7][3] = predefines.BUILDING

        self.board[6][6] = predefines.BUILDING
        self.board[6][7] = predefines.BUILDING
        self.board[7][6] = predefines.BUILDING
        self.board[7][7] = predefines.BUILDING

        #create the INTERSECTIONs
        self.board[0][3] = predefines.INTERSECTION1
        self.board[0][4] = predefines.INTERSECTION1
        self.board[0][5] = predefines.INTERSECTION1
        self.board[0][6] = predefines.INTERSECTION1
        self.board[1][3] = predefines.INTERSECTION1
        self.board[1][4] = predefines.INTERSECTION1
        self.board[1][5] = predefines.INTERSECTION1
        self.board[1][6] = predefines.INTERSECTION1
        self.board[2][4] = predefines.INTERSECTION1
        self.board[2][5] = predefines.INTERSECTION1

        self.board[3][0] = predefines.INTERSECTION2
        self.board[4][0] = predefines.INTERSECTION2
        self.board[5][0] = predefines.INTERSECTION2
        self.board[6][0] = predefines.INTERSECTION2
        self.board[3][1] = predefines.INTERSECTION2
        self.board[4][1] = predefines.INTERSECTION2
        self.board[5][1] = predefines.INTERSECTION2
        self.board[6][1] = predefines.INTERSECTION2
        self.board[4][2] = predefines.INTERSECTION2
        self.board[5][2] = predefines.INTERSECTION2

        self.board[3][4] = predefines.INTERSECTION3
        self.board[3][5] = predefines.INTERSECTION3
        self.board[4][3] = predefines.INTERSECTION3
        self.board[4][4] = predefines.INTERSECTION3
        self.board[4][5] = predefines.INTERSECTION3
        self.board[4][6] = predefines.INTERSECTION3
        self.board[5][3] = predefines.INTERSECTION3
        self.board[5][4] = predefines.INTERSECTION3
        self.board[5][5] = predefines.INTERSECTION3
        self.board[5][6] = predefines.INTERSECTION3
        self.board[6][4] = predefines.INTERSECTION3
        self.board[6][5] = predefines.INTERSECTION3

        self.board[3][9] = predefines.INTERSECTION4
        self.board[4][9] = predefines.INTERSECTION4
        self.board[5][9] = predefines.INTERSECTION4
        self.board[6][9] = predefines.INTERSECTION4
        self.board[3][8] = predefines.INTERSECTION4
        self.board[4][8] = predefines.INTERSECTION4
        self.board[5][8] = predefines.INTERSECTION4
        self.board[6][8] = predefines.INTERSECTION4
        self.board[4][7] = predefines.INTERSECTION4
        self.board[5][7] = predefines.INTERSECTION4

        self.board[9][3] = predefines.INTERSECTION5
        self.board[9][4] = predefines.INTERSECTION5
        self.board[9][5] = predefines.INTERSECTION5
        self.board[9][6] = predefines.INTERSECTION5
        self.board[8][3] = predefines.INTERSECTION5
        self.board[8][4] = predefines.INTERSECTION5
        self.board[8][5] = predefines.INTERSECTION5
        self.board[8][6] = predefines.INTERSECTION5
        self.board[7][4] = predefines.INTERSECTION5
        self.board[7][5] = predefines.INTERSECTION5

        self.board[0][0] = predefines.OUTER_CORNER1
        self.board[0][predefines.GRID_SIZE - 1] = predefines.OUTER_CORNER2
        self.board[predefines.GRID_SIZE - 1][0] = predefines.OUTER_CORNER3
        self.board[predefines.GRID_SIZE - 1][predefines.GRID_SIZE - 1] = predefines.OUTER_CORNER4

        self.board[1][1] = predefines.INNER_CORNER1
        self.board[1][predefines.GRID_SIZE - 2] = predefines.INNER_CORNER2
        self.board[predefines.GRID_SIZE - 2][1] = predefines.INNER_CORNER3
        self.board[predefines.GRID_SIZE - 2][predefines.GRID_SIZE - 2] = predefines.INNER_CORNER4
