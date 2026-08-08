#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 50
#define HEIGHT 20

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    Point position;
} Asteroid;

typedef struct
{
    Point position;
    int score;
} Ship;

Asteroid new_asteroid()
{
    Asteroid asteroid;
    srand(time(NULL));
    asteroid.position.x = rand() % WIDTH;
    asteroid.position.y = 0;
    return asteroid;
}

void move_asteroid(Asteroid *asteroid)
{
    asteroid->position.y++;
}

Ship new_ship()
{
    Ship ship;
    ship.position.x = WIDTH / 2;
    ship.position.y = HEIGHT - 1;
    ship.score = 0;
    return ship;
}

void move_ship_left(Ship *ship)
{
    ship->position.x--;
    if (ship->position.x < 0)
    {
        ship->position.x = 0;
    }
}

void move_ship_right(Ship *ship)
{
    ship->position.x++;
    if (ship->position.x >= WIDTH)
    {
        ship->position.x = WIDTH - 1;
    }
}

void increase_score(Ship *ship)
{
    ship->score++;
}

bool check_collision(Ship ship, Asteroid asteroid)
{
    if (ship.position.x == asteroid.position.x && ship.position.y == asteroid.position.y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    Asteroid asteroids[100];
    Ship ship = new_ship();
    int num_asteroids = 0;
    bool game_over = false;

    while (!game_over)
    {
        system("clear");

        printf("Score: %d\n", ship.score);
        for (int i = 0; i < HEIGHT; i++)
        {
            for (int j = 0; j < WIDTH; j++)
            {
                bool asteroid_here = false;
                Point current_point = {j, i};
                for (int k = 0; k < num_asteroids; k++)
                {
                    if (asteroids[k].position.x == j && asteroids[k].position.y == i)
                    {
                        asteroid_here = true;
                        break;
                    }
                }
                if (current_point.x == ship.position.x && current_point.y == ship.position.y)
                {
                    printf("O");
                }
                else if (asteroid_here)
                {
                    printf("*");
                }
                else
                {
                    printf(" ");
                }
            }
            printf("\n");
        }

        for (int i = 0; i < num_asteroids; i++)
        {
            move_asteroid(&asteroids[i]);
            if (asteroids[i].position.y == HEIGHT - 1)
            {
		increase_score(&ship);
                for (int j = i; j < num_asteroids - 1; j++)
                {
                    asteroids[j] = asteroids[j + 1];
                }
                num_asteroids--;
                i--;
            }
        }

        if (rand() % 10 == 0)
        {
            asteroids[num_asteroids] = new_asteroid();
            num_asteroids++;
        }

        char input;
        scanf("%c", &input);
        switch (input)
        {
        case 'a':
            move_ship_left(&ship);
            break;
        case 'd':
            move_ship_right(&ship);
            break;
        }

        for (int i = 0; i < num_asteroids; i++)
        {
            if (check_collision(ship, asteroids[i]))
            {
                game_over = true;
                break;
            }
        }
    }

    printf("Game over! Final score: %d\n", ship.score);

    return 0;
}
