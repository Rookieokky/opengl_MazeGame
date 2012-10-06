#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 400

/*
 * Must be a odd number. 
 * The vertex and edge line all stored in maze.
 * So if a  maze is 4*4, then maze size must be 4*2 + 1;
 */
#define MAZE_SIZE 41
#define MAZE_SIZE_SMALL 41
#define MAZE_SIZE_MEDIUM 51
#define MAZE_SIZE_LARGE 61
#define LEVEL_SMALL 0
#define LEVEL_MEDIUM 1
#define LEVEL_LARGE 2

#define MAZE_UNIT_SIZE 10
#define MAZE_OPEN_COUNT 300

#define LEVEL_BUTTON 0
#define RESET_BUTTON 1

typedef struct {
    int fromX;
    int fromY;
    int toX;
    int toY;

    int id;
} button;

void display();
void reshape(int w, int h);
void composeMaze(int maze[MAZE_SIZE][MAZE_SIZE]);
void drawMaze(int maze[MAZE_SIZE][MAZE_SIZE]);
void drawStatus();
void drawButtons();
void changeLevel();
void restart();
void win();

int canMoveObjHori(int x);
int canMoveObjVert(int y);

