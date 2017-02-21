// James Corder Guy
// 29 January 2017
// Reversi board and starting pieces


#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>

#include <cmath>
#include <iostream>


using namespace std;


// Gameboard size
#define WIDTH   600
#define HEIGHT  600

// Sidebar size
#define BAR_WIDTH   200

// Players
#define BLACK   -1
#define WHITE   1

// RGB colors
#define BOARD_COLOR         0.149, 0.549, 0.353, 1
#define GRID_COLOR          0, 0, 0, 1
#define DARK_COLOR          0.15, 0.15, 0.15 ,1
#define LIGHT_COLOR         0.85, 0.85, 0.85, 1
#define HIGHLIGHT_COLOR     0.8, 0.3, 0.3, 0.8
#define BAR_COLOR           0.7, 0.7, 0.7, 1

// Other
#define GRID_SIZE       (WIDTH / 8)
#define BOARD_SIZE      8
#define PIECE_RADIUS    32


// Global representation of board
// 0 is empty, white and black are as defined
int BOARD[BOARD_SIZE]
         [BOARD_SIZE] = {{0, 0, 0,     0,     0, 0, 0, 0},
                         {0, 0, 0,     0,     0, 0, 0, 0},
                         {0, 0, 0,     0,     0, 0, 0, 0},
                         {0, 0, 0, WHITE, BLACK, 0, 0, 0},
                         {0, 0, 0, BLACK, WHITE, 0, 0, 0},
                         {0, 0, 0,     0,     0, 0, 0, 0},
                         {0, 0, 0,     0,     0, 0, 0, 0},
                         {0, 0, 0,     0,     0, 0, 0, 0}};

int CURRENT_COLOR = BLACK;

// Draws a piece of the given color in the given grid square (0,0) - (7,7)
void drawPiece(int x, int y, int color) {
    static double pi = 3.14159265358979;

    // Plus one makes it centered
    int center_x = (int) ((x + 0.5) * GRID_SIZE);
    int center_y = (int) ((y + 0.5) * GRID_SIZE) + 1;

    // Set piece color
    if (color == BLACK) {
        glColor4f(DARK_COLOR);
    } else if (color == WHITE) {
        glColor4f(LIGHT_COLOR);
    } else {
        return;
    }

    // Draw a circle using a parametric equation
    glBegin(GL_POLYGON);
    for (double theta = 0; theta < 2 * pi; theta += 0.1) {
        glVertex2d(PIECE_RADIUS * cos(theta) + center_x,
                   PIECE_RADIUS * sin(theta) + center_y);
    }
    glEnd();
}


void drawHighlight(int x, int y) {
    // Don't draw in the right hand side of the screen
    if (x > 7) return;

    // Draw a slightly transparent rectangle
    glEnable(GL_BLEND);
    glColor4f(HIGHLIGHT_COLOR);
    glPolygonMode(GL_FRONT, GL_FILL);

    glRectf(x * GRID_SIZE + 1,
            y * GRID_SIZE + 2,
            (x + 1) * GRID_SIZE - 2,
            (y + 1) * GRID_SIZE - 1);

    glDisable(GL_BLEND);
}


void drawGrid() {
    glColor4f(GRID_COLOR);
    glLineWidth(3);

    // Draw horizontal lines
    glBegin(GL_LINES);
    for (int i = 1; i < BOARD_SIZE; i++) {
        glVertex2i(0, GRID_SIZE * i);
        glVertex2i(WIDTH, GRID_SIZE * i);
    }

    // Draw vertical lines
    for (int i = 1; i < BOARD_SIZE; i++) {
        glVertex2i(GRID_SIZE * i, 0);
        glVertex2i(GRID_SIZE * i, HEIGHT);
    }
    glEnd();

}


void drawGame() {
    // Draw each piece in its place by iterating through
    // the board array
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            drawPiece(i, j, BOARD[i][j]);
        }
    }
}


void drawBoard() {
    glColor4f(BOARD_COLOR);
    glRecti(0, 0, WIDTH, HEIGHT);

    drawGame();
    drawGrid();
}


void handleKey(unsigned char key, int, int) {
    if (key == 'q') {
        exit(0);
    }
}


void drawBar() {
    glColor4f(BAR_COLOR);
    glRecti(WIDTH, 0, WIDTH + BAR_WIDTH, HEIGHT);

    // Put the current player's piece in the corner
    drawPiece(9, 6, CURRENT_COLOR);
}


void handleMotion(int x, int y) {
    y = HEIGHT - y;

    drawBoard();
    drawHighlight(x / GRID_SIZE, y / GRID_SIZE);

    glFlush();
}


void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBoard();
    drawBar();
    glFlush();
}


enum result {
    DIFF = -1,
    EMPT =  0,
    SAME =  1
};


int checkPiece(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) {
        return EMPT;
    }

    return BOARD[x][y] * CURRENT_COLOR;
}


bool checkLine(int x, int y, int dx, int dy) {
    bool isValidMove = false;
    bool isValidLine = false;

    // Travel in the direction (dx, dy) until board edge is reached,
    // as long as there is a continuous line of the opponent's pieces
    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        x += dx;
        y += dy;

        if (checkPiece(x, y) == DIFF) {
            isValidLine = true;
        }

        if (!isValidLine) return false;

        if (isValidLine && checkPiece(x, y) == SAME) {
            isValidMove = true;
            break;
        }

        if (checkPiece(x, y) == EMPT) {
            break;
        }
    } // end while

    return isValidMove;
}


void flipLine(int x, int y, int dx, int dy) {
    // Travel in the direction (dx, dy), flipping pieces until opponent's
    // pieces run out (or until the board edge is reached)
    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        x += dx;
        y += dy;

        if (checkPiece(x, y) == DIFF) {
            BOARD[x][y] = CURRENT_COLOR;
        } else {
            return;
        }
    }
}


bool makeMove(int x, int y) {
    bool isValidMove = false;

    /* Iterate through each of the eight directions as (dx, dy)
     * (-1, 1) | ( 0, 1) | ( 1, 1)
     * --------+---------+--------
     * (-1, 0) |         | ( 1, 0)
     * --------+---------+--------
     * (-1,-1) | ( 0,-1) | ( 1,-1)
     *
     * checking for valid chains of pieces to flip, and flipping them
     */
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;

            if (checkLine(x, y, dx, dy)) {
                isValidMove = true;
                flipLine(x, y, dx, dy);
            }
        }
    }

    return isValidMove;
}


void handleMouse(int button, int state, int mouse_x, int mouse_y) {

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
        return;
    }

    mouse_y = HEIGHT - mouse_y;

    int x = mouse_x / GRID_SIZE;
    int y = mouse_y / GRID_SIZE;

    if (!BOARD[x][y] && makeMove(x, y)) {
        BOARD[x][y] = CURRENT_COLOR;
        CURRENT_COLOR *= -1;
    }

    draw();
}


int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitWindowSize(WIDTH + BAR_WIDTH, HEIGHT);
    glutCreateWindow("Reversi (floating)");

    glutDisplayFunc(draw);
    glutKeyboardFunc(handleKey);
    glutMouseFunc(handleMouse);
    glutPassiveMotionFunc(handleMotion);

    gluOrtho2D(0, WIDTH + BAR_WIDTH, 0, HEIGHT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1, 1, 1, 1);

    glutMainLoop();
}
