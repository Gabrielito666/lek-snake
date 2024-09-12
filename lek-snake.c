#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK0 "\033[1;31m░░"   //bloque de fondo 0 rojo
#define BLOCK1 "\033[1;31m▒▒"   //bloque de fondo 1 rojo
#define BLOCK2 "\033[1;32m██"
#define BLOCK3 "\x1b[33m██"

#define COLUMNS 20
#define ROWS 10
#define MICROSECONDS_FRAME 300000

char tecla;
char direction;
char temporal_direction;

int game_over = 0;
pthread_t keyboard, screen;

int matrix[ROWS][COLUMNS];

int snake[COLUMNS*ROWS][2];
int snake_length;

void setNewFood();

int getTypeBlock(int x, int y)
{
    return ((x + y) % 2);
}
void configurar_initial_matrix()
{
    for(int x = 0; x < ROWS ;x++)
    {
        for(int y = 0; y < COLUMNS; y++)
        {
            matrix[x][y] = getTypeBlock(x, y);
        }
    }
}
void initSnake()
{
    snake_length = 3;

    snake[0][0] = 4;
    snake[0][1] = 4;
    snake[1][0] = 4;
    snake[1][1] = 3;
    snake[2][0] = 4;
    snake[2][1] = 2;

    matrix[4][4] = 2;
    matrix[4][3] = 2;
    matrix[4][2] = 2;
}

void mooveSnake()
{
    int prev[2];
    int eat;
    eat = 0;

    for(int i = 0; i < snake_length; i++)
    {
        int thisPrev[2];
        thisPrev[0] = snake[i][0];
        thisPrev[1] = snake[i][1];
        
        if(i == 0)
        {
            if(direction == 'w') snake[0][0]--;
            else if(direction == 'a') snake[0][1]--;
            else if(direction == 's') snake[0][0]++;
            else if(direction == 'd') snake[0][1]++;

            int head_collider = matrix[snake[0][0]][snake[0][1]];
            if(head_collider == 3) eat = 1;
            if(
                head_collider == 2 ||
                snake[0][0] < 0 ||
                snake[0][1] < 0 ||
                snake[0][0] >= ROWS ||
                snake[0][1] >= COLUMNS
            ) 
            {
                game_over = 1;
                pthread_cancel(keyboard);
                pthread_cancel(screen);
            }
        }
        else
        {
            snake[i][0] = prev[0];
            snake[i][1] = prev[1];
        }

        prev[0] = thisPrev[0];
        prev[1] = thisPrev[1];
    }

    matrix[snake[0][0]][snake[0][1]] = 2;

    if(eat)
    {
        setNewFood();
        snake[snake_length][0] = prev[0];
        snake[snake_length][1] = prev[1];
        snake_length++;
    }
    else
    {    
        matrix[prev[0]][prev[1]] = getTypeBlock(prev[0], prev[1]);
    }
}

void setNewFood()
{
    int x;
    int y;

    x = y = 2;
    
    while(matrix[x][y] == 2)
    {
        x = (rand() % ((ROWS-1) + 1));
        y = (rand() % ((COLUMNS-1) + 1));
    }
    matrix[x][y] = 3;
}

void configurar_terminal(int modo)
{
    static struct termios oldt, newt;
    if (modo == 1)
    {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void* screenProcess()
{
    while(!game_over)
    {
        printf("\x1b[H\n\n");
        for(int x = 0; x < ROWS;x++)
        {
            printf("\t");
            for(int y = 0; y < COLUMNS; y++)
            {
                if(matrix[x][y] == 0) printf(BLOCK0);
                else if(matrix[x][y] == 1) printf(BLOCK1);
                else if(matrix[x][y] == 2) printf(BLOCK2);
                else if(matrix[x][y] == 3) printf(BLOCK3);
                
            }
            printf("\n");
        }
        usleep(MICROSECONDS_FRAME);
        direction = temporal_direction;
        mooveSnake();
    }
}

void* keyboardProcess()
{
    direction = 'd';
    temporal_direction = 'd';
    while(!game_over)
    {
        tecla = getchar();
        if(
            tecla == 'w' && direction != 's' ||
            tecla == 'a' && direction != 'd' ||
            tecla == 's' && direction != 'w' ||
            tecla == 'd' && direction != 'a'
        ) temporal_direction = tecla;
    }
}

int main() {

    printf("\x1b[2J");
    srand(time(NULL));
    configurar_initial_matrix();
    initSnake();
    setNewFood();
    configurar_terminal(1);

    pthread_create(&keyboard, NULL, keyboardProcess, NULL);
    pthread_create(&screen, NULL, screenProcess, NULL);

    pthread_join(keyboard, NULL);
    pthread_join(screen, NULL);

    printf("%s\n","Game over!");

    configurar_terminal(0);
    return 0;
}