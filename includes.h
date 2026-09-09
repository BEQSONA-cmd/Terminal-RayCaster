#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

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
    float x;
    float y;
    float angle;
    float speed;
} t_player;

typedef struct s_frame
{
    int width;
    int height;
    unsigned char *pixels;
    char *output;
    int output_size;
} t_frame;

t_screen get_screen_size(void);
int touch(float px, float py, char **map);
float fixed_dist(float x1, float y1, float x2, float y2, float player_angle, float ray_angle);
void enable_raw_mode(void);
void disable_raw_mode(void);
void handle_key(t_player *player, char **map, char *keys, KeyCode *keycodes);
void render_frame(t_frame *frame);
void clear_frame(t_frame *frame);
t_frame create_frame(int width, int height);