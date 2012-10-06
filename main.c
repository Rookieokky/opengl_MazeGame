#include "main.h"

int maze[MAZE_SIZE_LARGE][MAZE_SIZE_LARGE];

int mazeSize = MAZE_SIZE_LARGE;
int mazeOpenCount = mazeSize/4*30;
int entranceX = mazeSize - 2;
int entranceY = 1;

int level = 0;

//The left and top cornners of maze.
int leftEdge = 20;
int topEdge = 20;

int objCenterX;
int objCenterY;
int winCount = -1;

int blockPixals[WINDOW_WIDTH][WINDOW_HEIGHT];

int dirs[][2] = {{2, 0}, {0, 2}};

button buttons[3];
int buttonCount = 0;


//Status panel.
//To display some informations. 
int statusY = WINDOW_HEIGHT - 50;


int isLineBlock(int r, int c) {
    return (r%2 == 1 && c%2 == 0) || 
                (r%2 == 0 && c%2 == 1);
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

void drawObj() {
    int e = MAZE_UNIT_SIZE/3;
    glBegin(GL_QUADS);
    glVertex2i(objCenterX - e, objCenterY - e);
    glVertex2i(objCenterX - e, objCenterY + e);
    glVertex2i(objCenterX + e, objCenterY + e);
    glVertex2i(objCenterX + e, objCenterY - e);
    glEnd();
}

void makePathToTarget(int maze[MAZE_SIZE_LARGE][MAZE_SIZE_LARGE]) {
    int startR = 1;
    int startC = 1;
    int r = startR;
    int c = startC;
    int count = mazeSize - 3;
    while (count != 0) {
        int dirIndex = rand()%2;
        int ar = dirs[dirIndex][0];
        int ac = dirs[dirIndex][1];
        if (r + ar < mazeSize - 1 &&
                c + ac < mazeSize - 1) {
            //Remove block.
            maze[r + ar/2][c + ac/2] = 0;

            r += ar;
            c += ac;

            count --;
        }
    }

    //Remove entrance and exit blocks.
    maze[1][0] = 0;//entrance.
    maze[mazeSize - 2][mazeSize - 1] = 0;//exit.
}

void randomlyRemoveBlock(int maze[MAZE_SIZE_LARGE][MAZE_SIZE_LARGE]) {
    int i;
    for (i = 0; i < mazeOpenCount; ) {
        int ranR = rand()%(mazeSize - 1);
        int ranC = rand()%(mazeSize - 1);
        if (maze[ranR][ranC] && isLineBlock(ranR, ranC) && ranR != 0 && ranC != 0) {
            maze[ranR][ranC] = 0;
            i += 1;
        }
    }
}

void composeMaze(int maze[MAZE_SIZE_LARGE][MAZE_SIZE_LARGE]) {
    int i,j;
    for (i = 0; i < MAZE_SIZE_LARGE; i ++) {
        for (j = 0; j < MAZE_SIZE_LARGE; j ++) {
            maze[i][j] = 1;
        }
    }

    randomlyRemoveBlock(maze);
    makePathToTarget(maze);
}

//Add blocks data at the same time drawing lines.
//We assume that the line for drawing is either horzontal or vertical.
void drawMazeLine(int x1, int y1, int x2, int y2) {
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();

    //Add drawed line to block pixals.
    //
    //Because the line is either horizontal or vertical, 
    //so when x1 != x2, y is the same. And vise versa.
    int i;
    if (x1 != x2) {
        int maxv = max(x1, x2);
        int minv = min(x1, x2);
        for (i = minv; i < maxv; i ++) {
            blockPixals[i][y1] = 1;
        }
    } else {
        int maxv = max(y1, y2);
        int minv = min(y1, y2);
        for (i = minv; i < maxv; i ++) {
            blockPixals[x1][i] = 1;
        }
    }
}

int isPathBlock(int r, int c) {
    return r%2 == 1 && c%2 == 1; 
}

void drawMaze(int maze[MAZE_SIZE_LARGE][MAZE_SIZE_LARGE]) {
    int x, y, r, c;
    x = leftEdge; //offset x.
    y = topEdge; //offset y.

    //Draw horizontal lines.
    for (r = 0; r < mazeSize; r += 2) {
        for (c = 1; c < mazeSize; c += 2) {
            if (maze[r][c]) {
                drawMazeLine(x, y, x + MAZE_UNIT_SIZE, y);
            }

            x += MAZE_UNIT_SIZE;
        }

        //Reset x to left edge. And move y to next line.
        x = leftEdge;
        y += MAZE_UNIT_SIZE;
    }

    //Draw vertical lines.
    x = leftEdge;
    y = topEdge;
    for (c = 0; c < mazeSize; c += 2) {
        for (r = 1; r < mazeSize; r += 2) {
            if (maze[r][c]) {
                drawMazeLine(x, y, x, y + MAZE_UNIT_SIZE);
            }

            y += MAZE_UNIT_SIZE;
        }

        //Reset y to top edge move x one unit size forward.
        y = topEdge;
        x += MAZE_UNIT_SIZE;
    }
}

void display() {
    glColor3d(0, 0, 0);
    glClearColor(255, 255, 255, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawMaze(maze);
    drawObj();

    drawStatus();
    drawButtons();

    glFlush();
}

void drawButton(button btn) {
    glBegin(GL_LINE_STRIP);
    glVertex2i(btn.fromX, btn.fromY);
    glVertex2i(btn.fromX, btn.toY);
    glVertex2i(btn.toX, btn.toY);
    glVertex2i(btn.toX, btn.fromY);
    glVertex2i(btn.fromX, btn.fromY);
    glEnd();

    glRasterPos2f(btn.fromX + 3, btn.fromY + 15);
    char* str;
    int i;
    if (btn.id == LEVEL_BUTTON) {
        char tmp[] = {"level"};
        str = tmp;
    } else {
        char tmp[] = {"reset"};
        str = tmp;
    }
    int len = strlen(str);
    for (i = 0; i < len; i ++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, str[i]);
    }
}

void drawButtons() {
    int x = WINDOW_WIDTH - 80;
    int y = 50;

    button btn;
    btn.id = LEVEL_BUTTON;
    btn.fromX = x;
    btn.fromY = y;
    btn.toX = x + 40;
    btn.toY = y + 20;
    drawButton(btn);
    buttons[0] = btn;
    buttonCount = 1;

    y += 50;
    btn.id = RESET_BUTTON;
    btn.fromX = x;
    btn.fromY = y;
    btn.toX = x + 40;
    btn.toY = y + 20;
    drawButton(btn);
    buttons[1] = btn;
    buttonCount = 2;
}

void showMessage1() {
    int y = statusY + 20; 
    int x = leftEdge;
    glRasterPos2f(x, y);
    int i;
    char str[] = {"Diaoyudao belongs to china,"};
    int len = strlen(str);
    for (i = 0; i < len; i ++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

void showMessage2() {
    int y = statusY + 44; 
    int x = 100;
    glRasterPos2f(x, y);
    int i;
    char str[] = {"aoisola belongs to the world."};
    int len = strlen(str);
    for (i = 0; i < len; i ++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

void drawStatus() {
    glBegin(GL_LINES);
    glVertex2i(0, statusY);
    glVertex2i(WINDOW_WIDTH, statusY);
    glEnd();

    fprintf(stderr, "%d\n", winCount);
    if (winCount >= 1) {
        showMessage1();
    } 

    if (winCount >= 2) {
        showMessage2();
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //set the coordinate system, with the origin in the top left
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
}
int canMoveObj(int x, int y) {
    if (x != 0) {
        return canMoveObjHori(x);
    } else {
        return canMoveObjVert(y);
    }
}

int canMoveObjHori(int x) {
    int from = min(x + objCenterX, objCenterX);
    int to = max(x + objCenterX, objCenterX);
    int i;
    //Judge horizontal moving.
    for (i = from; i < to; i ++) {
        if (blockPixals[i][objCenterY]) {
            return 0;
        }
    }
    return 1;
}

int canMoveObjVert(int y) {
    int from = min(y + objCenterY, objCenterY);
    int to = max(y + objCenterY, objCenterY);
    int i;
    //Judge vertical moving.
    for (i = from; i < to; i ++) {
        if (blockPixals[objCenterX][i]) {
            return 0;
        }
    }

    return 1;
}

int isMoveToExit(int x, int y) {
    return (x + objCenterX) > leftEdge + mazeSize/2*MAZE_UNIT_SIZE;
}

void win() {
    fprintf(stderr, "%s", "win\n");
    if (winCount == -1) {
        winCount = 1;
    } else {
        winCount ++;
    }
    restart();
}

void moveObj(int x, int y) {
    if (canMoveObj(x, y)) {
        if (isMoveToExit(x, y)) {
            win();
            return;
        }

        objCenterX += x;
        objCenterY += y;
        glutPostRedisplay();
    }
}

void keyboard(int key, int x, int y) {
    int moveSize = MAZE_UNIT_SIZE;
    switch (key) {
        case GLUT_KEY_RIGHT:
            moveObj(moveSize, 0);
            break;
        case GLUT_KEY_LEFT:
            moveObj(-moveSize, 0);
            break;
        case GLUT_KEY_UP:
            moveObj(0, -moveSize);
            break;
        case GLUT_KEY_DOWN:
            moveObj(0, moveSize);
            break;
    }
}

void clearBlockPixals() {
    int r, c;
    for (r = 0; r < WINDOW_WIDTH; r ++) {
        for (c = 0; c < WINDOW_HEIGHT; c ++) {
            blockPixals[r][c] = 0;
        }
    }
}

void initData() {
    objCenterX = leftEdge + MAZE_UNIT_SIZE/2;
    objCenterY = topEdge + MAZE_UNIT_SIZE/2;
    clearBlockPixals();
    composeMaze(maze);
}

void updateMazeOpenCount() {
    mazeOpenCount = mazeSize/4*30;
}

void setupLevel() {
    switch (level) {
        case LEVEL_SMALL:
            mazeSize = MAZE_SIZE_SMALL;
            break;
        case LEVEL_MEDIUM:
            mazeSize = MAZE_SIZE_MEDIUM;
            break;
        case LEVEL_LARGE:
            mazeSize = MAZE_SIZE_LARGE;
            break;
    }
    updateMazeOpenCount();
}

void restart() {
    setupLevel();
    initData();
    glutPostRedisplay();
}

void testKeyboard(unsigned char key, int x, int y) {
    if (key == 'r') {
        restart();
    }
}

int getButton(int x, int y) {
    int i;
    for (i = 0; i < buttonCount; i ++) {
        button btn = buttons[i];
        if (x <= btn.toX && x >= btn.fromX && y <= btn.toY && y >= btn.fromY) {
            return btn.id;
        }
    }

    return -1;
}


void changeLevel() {
    level ++;
    level %= 3;
    restart();
}

void mouse(int btn, int status, int x, int y) {
    if (btn == GLUT_DOWN && status == GLUT_LEFT_BUTTON) {
        int clickedBtn = getButton(x, y);
        if (clickedBtn == LEVEL_BUTTON) {
            //fprintf(stderr, "%s", "level\n");
            changeLevel();
        } else if (clickedBtn == RESET_BUTTON) {
            //fprintf(stderr, "%s", "restart\n");
            restart();
        }
    }
}

int main(int argc, char *argv) {
    initData();
    
    glutInit(&argc, &argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Simple maze");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc(testKeyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
}
