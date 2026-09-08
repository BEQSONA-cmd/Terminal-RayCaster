#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>

#define PI 3.14159265358979323846

#define BLOCK_W 8
#define BLOCK_H 4

#define RAY_STEP 0.05

extern struct termios orig_termios;

typedef struct s_screen
{
    int width;
    int height;
} t_screen;

typedef struct s_player
{
    double x;
    double y;
    double angle;
    float speed;
} t_player;

t_screen get_screen_size(void);
int touch(double px, double py, char **map);
float fixed_dist(float x1, float y1, float x2, float y2, float player_angle);
void enable_raw_mode(void);
void disable_raw_mode(void);
void handle_key(t_player *player, char **map);
