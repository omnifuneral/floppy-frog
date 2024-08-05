#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define MAX_OBSTACLES 100
#define OBSTACLE_WIDTH 1
#define GAP_HEIGHT 10 // Increased gap height to make the game easier
#define FIXED_WIDTH 120 // Fixed width of the window
#define FIXED_HEIGHT 30 // Fixed height of the window

// Structs for the bird and obstacles
typedef struct {
    int x, y;
    int velocity;
} Bird;

typedef struct {
    int x, gap_start;
} Obstacle;

// Global variables
Bird bird;
Obstacle obstacles[MAX_OBSTACLES];
int num_obstacles = 0;
int score = 0;
int tick_count = 0;

// Function prototypes
void setup();
void teardown();
void title_screen();
void wait_for_return();
void game_loop();
void generate_obstacle();
void move_obstacles();
void update_bird();
void handle_input(int ch);
int check_collision();
void update_score();
void game_over();

int main() {
    setup();
    srand(time(NULL)); // Seed the random number generator

    // Display the title screen and wait for Return key press
    title_screen();
    wait_for_return();

    teardown();
    return 0;
}

// Function to set up the ncurses environment
void setup() {
    initscr();    // Initialize the window
    cbreak();     // Disable line buffering
    noecho();     // Don't echo input characters
    curs_set(0);  // Hide the cursor
    timeout(100); // Set a delay for getch (in milliseconds)
    // Set a fixed size for the window
    resizeterm(FIXED_HEIGHT, FIXED_WIDTH);
}

// Function to tear down the ncurses environment
void teardown() {
    endwin(); // End the ncurses window
}

// Function to display the title screen
void title_screen() {
    clear();
    mvprintw(FIXED_HEIGHT / 2 - 5, (FIXED_WIDTH - 11) / 2, "Floppy Frog");
    mvprintw(FIXED_HEIGHT / 2 - 3, (FIXED_WIDTH - 20) / 2, "Press Return to start");

    // ASCII Art Frog
    mvprintw(FIXED_HEIGHT / 2 + 2, (FIXED_WIDTH - 14) / 2, "  @..@");
    mvprintw(FIXED_HEIGHT / 2 + 3, (FIXED_WIDTH - 14) / 2, " (----)");
    mvprintw(FIXED_HEIGHT / 2 + 4, (FIXED_WIDTH - 14) / 2, "( >__< )");
    mvprintw(FIXED_HEIGHT / 2 + 5, (FIXED_WIDTH - 14) / 2, "^^ ~~ ^^");

    refresh();
}

// Function to wait for the Return key press
void wait_for_return() {
    int ch;
    timeout(-1); // Wait indefinitely for a key press
    while ((ch = getch()) != '\n') {
        // Wait for the Return key
    }
    timeout(100); // Restore the timeout value

    // Start the game loop after key press
    game_loop();
}

// Main game loop function
void game_loop() {
    // Initialize the bird's starting position and velocity
    bird = (Bird){FIXED_WIDTH / 4, FIXED_HEIGHT / 2, 0};
    num_obstacles = 0; // Reset obstacles
    score = 0; // Reset score
    tick_count = 0; // Reset tick count

    while (1) {
        // Handle input
        int ch = getch();
        handle_input(ch);

        if (ch == 'q') {
            teardown(); // Properly end ncurses
            exit(0); // Quit the game
        }

        // Generate a new obstacle every 20 ticks
        if (tick_count % 20 == 0) {
            generate_obstacle();
        }

        move_obstacles();    // Move existing obstacles
        update_bird();       // Update the bird's position
        update_score();      // Update the score

        // Clear the screen
        clear();

        // Draw the bird
        mvprintw(bird.y, bird.x, ">");

        // Draw the obstacles
        for (int i = 0; i < num_obstacles; i++) {
            // Draw top part of the obstacle
            for (int y = 0; y < obstacles[i].gap_start; y++) {
                mvprintw(y, obstacles[i].x, "|");
            }
            // Draw bottom part of the obstacle
            for (int y = obstacles[i].gap_start + GAP_HEIGHT; y < FIXED_HEIGHT; y++) {
                mvprintw(y, obstacles[i].x, "|");
            }
        }

        // Display the score
        mvprintw(0, 0, "Score: %d", score);

        // Check for collisions
        if (check_collision()) {
            game_over(); // Handle game over condition
            break; // Exit the game loop on game over
        }

        // Refresh the screen to show the updated state
        refresh();

        // Increment the tick count
        tick_count++;
    }
}

// Function to generate a new obstacle
void generate_obstacle() {
    if (num_obstacles < MAX_OBSTACLES) {
        obstacles[num_obstacles].x = FIXED_WIDTH - 1;
        obstacles[num_obstacles].gap_start = rand() % (FIXED_HEIGHT - GAP_HEIGHT);
        num_obstacles++;
    }
}

// Function to move existing obstacles
void move_obstacles() {
    for (int i = 0; i < num_obstacles; i++) {
        obstacles[i].x--;
        if (obstacles[i].x < 0) {
            // Remove the obstacle if it moves off-screen
            for (int j = i; j < num_obstacles - 1; j++) {
                obstacles[j] = obstacles[j + 1];
            }
            num_obstacles--;
            i--;
        }
    }
}

// Function to update the bird's position
void update_bird() {
    bird.velocity++; // Increase velocity (gravity effect)
    bird.y += bird.velocity; // Move bird

    // Prevent the bird from moving out of bounds
    if (bird.y < 0) bird.y = 0;
    if (bird.y >= FIXED_HEIGHT) bird.y = FIXED_HEIGHT - 1;
}

// Function to handle player input
void handle_input(int ch) {
    if (ch == ' ') {
        bird.velocity = -3; // Flap the bird upwards
    }
}

// Function to check for collisions between the bird and obstacles
int check_collision() {
    for (int i = 0; i < num_obstacles; i++) {
        if (bird.x >= obstacles[i].x && bird.x < obstacles[i].x + OBSTACLE_WIDTH) {
            if (bird.y < obstacles[i].gap_start || bird.y >= obstacles[i].gap_start + GAP_HEIGHT) {
                return 1; // Collision detected
            }
        }
    }
    return 0; // No collision
}

// Function to update the player's score
void update_score() {
    for (int i = 0; i < num_obstacles; i++) {
        if (obstacles[i].x == bird.x) {
            score++; // Increase score if bird passes an obstacle
        }
    }
}

// Function to handle game over condition
void game_over() {
    mvprintw(FIXED_HEIGHT / 2, FIXED_WIDTH / 2 - 5, "Game Over");
    mvprintw(FIXED_HEIGHT / 2 + 1, FIXED_WIDTH / 2 - 10, "Press 'r' to restart");
    mvprintw(FIXED_HEIGHT / 2 + 2, FIXED_WIDTH / 2 - 9, "or 'q' to quit");
    refresh();

    timeout(-1); // Wait indefinitely for a key press

    while (1) {
        int ch = getch();
        if (ch == 'r') {
            // Reset the game
            score = 0;
            bird = (Bird){FIXED_WIDTH / 4, FIXED_HEIGHT / 2, 0};
            num_obstacles = 0;
            tick_count = 0;
            timeout(100); // Restore timeout for the game loop
            game_loop(); // Restart the game loop
            break;
        } else if (ch == 'q') {
            teardown(); // Properly end ncurses
            exit(0); // Quit the game
        }
    }
}