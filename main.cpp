#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define CELL_SIZE 18

// Game states
enum GameState { MENU, PLAYING, PAUSED, GAME_OVER, WIN };
GameState currentState = MENU;

// Pacman structure
struct Pacman {
    float x, y;
    float speed;
    int direction; // 0=right, 1=up, 2=left, 3=down
    int nextDirection;
    float mouthAngle;
    int mouthOpening;
    int lives;
    int score;
} pacman;

// Ghost structure
struct Ghost {
    float x, y;
    float speed;
    int direction;
    int color; // 0=red, 1=pink, 2=cyan, 3=orange
    float stateTimer;
} ghosts[4];

// Game variables
int maze[MAZE_HEIGHT][MAZE_WIDTH];
int totalDots = 0;
int dotsEaten = 0;
float gameTime = 0;
float startTime = 0;
int highScore = 0;

// Simple maze layout (1=wall, 0=path with dot, 2=empty path, 3=ghost house)
void initMaze() {
    int mazeLayout[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,2,1,1,2,1,1,1,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,2,1,1,2,1,1,1,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,3,3,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,3,3,3,3,3,3,1,2,1,1,0,1,1,1,1,1,1},
        {2,2,2,2,2,2,0,2,2,2,1,3,3,3,3,3,3,1,2,2,2,0,2,2,2,2,2,2},
        {1,1,1,1,1,1,0,1,1,2,1,3,3,3,3,3,3,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    totalDots = 0;
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            maze[i][j] = mazeLayout[i][j];
            if (maze[i][j] == 0) totalDots++;
        }
    }
}

void initPacman() {
    pacman.x = 14;
    pacman.y = 23;
    pacman.speed = 0.1f;
    pacman.direction = 0;
    pacman.nextDirection = 0;
    pacman.mouthAngle = 0;
    pacman.mouthOpening = 1;
    pacman.lives = 3;
    pacman.score = 0;
}

void initGhosts() {
    // Red ghost (Blinky)
    ghosts[0].x = 13; ghosts[0].y = 11; ghosts[0].color = 0; ghosts[0].speed = 0.08f;
    // Pink ghost (Pinky)
    ghosts[1].x = 14; ghosts[1].y = 14; ghosts[1].color = 1; ghosts[1].speed = 0.08f;
    // Cyan ghost (Inky)
    ghosts[2].x = 13; ghosts[2].y = 14; ghosts[2].color = 2; ghosts[2].speed = 0.08f;
    // Orange ghost (Clyde)
    ghosts[3].x = 15; ghosts[3].y = 14; ghosts[3].color = 3; ghosts[3].speed = 0.08f;
    
    for (int i = 0; i < 4; i++) {
        ghosts[i].direction = rand() % 4;
        ghosts[i].stateTimer = 0;
    }
}

void resetGame() {
    initMaze();
    initPacman();
    initGhosts();
    dotsEaten = 0;
    gameTime = 0;
    startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

bool isWall(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT)
        return true;
    return maze[y][x] == 1;
}

bool canMove(float x, float y, int dir) {
    float nextX = x, nextY = y;
    float offset = 0.4f;
    
    switch(dir) {
        case 0: nextX += offset; break; // right
        case 1: nextY -= offset; break; // up
        case 2: nextX -= offset; break; // left
        case 3: nextY += offset; break; // down
    }
    
    int gridX = (int)(nextX + 0.5f);
    int gridY = (int)(nextY + 0.5f);
    
    return !isWall(gridX, gridY);
}

void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }
    glEnd();
}
