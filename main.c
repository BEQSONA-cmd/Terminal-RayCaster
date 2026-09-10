#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define PI 3.14159265358979323846

#define BLOCK_W 8
#define BLOCK_H 4
#define RAY_STEP 0.05

#define CHAR '|'

// HELPER FUNCTIONS

typedef struct s_screen
{
    int width;
    int height;
} t_screen;

char **get_map(void)
{
    char **map = malloc(sizeof(char *) * 10);

    map[0] = "1111111111111111";
    map[1] = "1010000000000001";
    map[2] = "1010001000100001";
    map[3] = "1010000000000001";
    map[4] = "1011111111100001";
    map[5] = "1000000000100001";
    map[6] = "1011111101100001";
    map[7] = "1010000000000001";
    map[8] = "1111111111111111";
    map[9] = NULL;

    return map;
}

int get_side(float x, float y, float angle, char **map)
{
    float sx = cos(angle) > 0 ? x - (int)x : (int)x + 1 - x;
    float sy = sin(angle) > 0 ? y - (int)y : (int)y + 1 - y;

    if (sx < sy)
        return (cos(angle) > 0 ? 3 : 2);
    else
        return (sin(angle) > 0 ? 4 : 1);

    return (0);
}

struct termios orig_termios;

void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int touch(float px, float py, char **map)
{
    int x = (int)px;
    int y = (int)py;

    if (x < 0 || y < 0)
        return 1;

    if (map[y] == NULL)
        return 1;

    if (map[y][x] == '\0')
        return 1;

    if (map[y][x] == '1')
        return 1;

    return 0;
}

float distance(float x, float y)
{
    return sqrt(x * x + y * y);
}

float fixed_dist(float x1, float y1, float x2, float y2, float player_angle, float ray_angle)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float ray_dist = sqrtf(dx * dx + dy * dy);

    return ray_dist * cosf(ray_angle - player_angle);
}

t_screen get_screen_size(void)
{
    struct winsize terminal;
    t_screen screen;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

    screen.width = terminal.ws_col;
    screen.height = terminal.ws_row;

    screen.width = screen.height * 3.5;

    return screen;
}

// PLAYER

typedef struct s_player
{
    float x;
    float y;
    float angle;
    float speed;
} t_player;

void move_player(t_player *player, char **map, float dx, float dy)
{
    float new_x = player->x + dx;
    float new_y = player->y + dy;

    if (!touch(new_x, player->y, map))
        player->x = new_x;

    if (!touch(player->x, new_y, map))
        player->y = new_y;
}

void handle_key(t_player *player, char **map, char *keys, KeyCode *keycodes)
{
    float dx = 0, dy = 0;
    float rot = 0;

    if (keys[keycodes[0] >> 3] & (1 << (keycodes[0] & 7)))
    {
        dx = cos(player->angle) * player->speed;
        dy = sin(player->angle) * player->speed;
    }
    if (keys[keycodes[2] >> 3] & (1 << (keycodes[2] & 7)))
    {
        dx = -cos(player->angle) * player->speed;
        dy = -sin(player->angle) * player->speed;
    }
    if (keys[keycodes[1] >> 3] & (1 << (keycodes[1] & 7)))
    {
        dx = cos(player->angle - PI / 2) * player->speed;
        dy = sin(player->angle - PI / 2) * player->speed;
    }
    if (keys[keycodes[3] >> 3] & (1 << (keycodes[3] & 7)))
    {
        dx = cos(player->angle + PI / 2) * player->speed;
        dy = sin(player->angle + PI / 2) * player->speed;
    }

    if (keys[keycodes[5] >> 3] & (1 << (keycodes[5] & 7)))
        rot = -player->speed;
    if (keys[keycodes[6] >> 3] & (1 << (keycodes[6] & 7)))
        rot = player->speed;

    if (keys[keycodes[4] >> 3] & (1 << (keycodes[4] & 7)))
        return;

    if (dx != 0 || dy != 0)
        move_player(player, map, dx, dy);

    if (rot != 0)
    {
        player->angle += rot;
        if (player->angle < 0)
            player->angle += 2 * PI;
        if (player->angle >= 2 * PI)
            player->angle -= 2 * PI;
    }
}

// RENDER

typedef struct s_frame
{
    int width;
    int height;
    unsigned char *pixels;
    char *output;
    int output_size;
} t_frame;

void render_frame(t_frame *frame)
{
    int pos = 0;
    int x;
    int y;
    unsigned char color;
    int current_color = -1;

    pos += sprintf(frame->output + pos, "\033[H");

    for (y = 0; y < frame->height; y++)
    {
        for (x = 0; x < frame->width; x++)
        {
            color = frame->pixels[y * frame->width + x];

            if (color != current_color)
            {
                if (color == 0)
                    pos += sprintf(frame->output + pos, "\033[39m");
                else
                    pos += sprintf(frame->output + pos, "\033[38;5;%dm", color);

                current_color = color;
            }

            frame->output[pos++] = CHAR;
        }

        if (y != frame->height - 1)
            frame->output[pos++] = '\n';
    }

    write(STDOUT_FILENO, frame->output, pos);
}

t_frame create_frame(int width, int height)
{
    t_frame frame;

    frame.width = width;
    frame.height = height;

    frame.pixels = malloc(width * height);
    frame.output = malloc(width * height * 30);

    frame.output_size = 0;

    return frame;
}

void draw_line(t_player *player, char **map, t_frame *frame, int index, float ray_angle)
{
    float cos_angle = cos(ray_angle);
    float sin_angle = sin(ray_angle);

    float ray_x = player->x;
    float ray_y = player->y;

    while (!touch(ray_x, ray_y, map))
    {
        ray_x += cos_angle * RAY_STEP;
        ray_y += sin_angle * RAY_STEP;
    }

    int side = get_side(ray_x, ray_y, ray_angle, map);

    float distance = fixed_dist(player->x, player->y, ray_x, ray_y, player->angle, ray_angle);

    float wall_height = (frame->height * 0.8f) / distance;
    int center_y = frame->height / 2;
    int start_y = center_y - wall_height / 2;
    int end_y = center_y + wall_height / 2;

    if (start_y < 0)
        start_y = 0;

    if (end_y >= frame->height)
        end_y = frame->height - 1;

    unsigned char color;

    if (side == 1)
        color = 226;
    else if (side == 2)
        color = 21;
    else if (side == 3)
        color = 196;
    else
        color = 208;

    int i = 0;
    while (i < frame->height)
    {
        if (i < start_y)
            frame->pixels[i * frame->width + index] = 232;
        else if (i >= start_y && i <= end_y)
            frame->pixels[i * frame->width + index] = color;
        else if (i > end_y)
            frame->pixels[i * frame->width + index] = 231;
        i++;
    }
}

void draw(char **map, t_player *player, t_frame *frame)
{
    float fov = PI / 2;
    float ray_angle;
    int i = 0;

    ray_angle = player->angle - fov / 2;

    while (i < frame->width)
    {
        draw_line(player, map, frame, i, ray_angle);
        ray_angle += fov / frame->width;
        i++;
    }
}

// MAIN

int main(void)
{
    char **map = get_map();

    t_player player = {1.5, 1.5, 1.5, 0.08};

    t_screen old_screen = {0, 0};
    t_screen new_screen = {0, 0};
    t_frame frame;

    printf("\033[2J\033[H\033[?25l");

    enable_raw_mode();

    Display *display = XOpenDisplay(NULL);

    if (!display)
    {
        disable_raw_mode();
        return 1;
    }

    KeyCode *keycodes = malloc(sizeof(KeyCode) * 7);

    keycodes[0] = XKeysymToKeycode(display, XK_w);
    keycodes[1] = XKeysymToKeycode(display, XK_a);
    keycodes[2] = XKeysymToKeycode(display, XK_s);
    keycodes[3] = XKeysymToKeycode(display, XK_d);
    keycodes[4] = XKeysymToKeycode(display, XK_q);
    keycodes[5] = XKeysymToKeycode(display, XK_Left);
    keycodes[6] = XKeysymToKeycode(display, XK_Right);

    char keys[32];

    while (1)
    {
        new_screen = get_screen_size();
        if (new_screen.width != old_screen.width || new_screen.height != old_screen.height)
        {
            printf("\033[2J\033[H");
            fflush(stdout);
            old_screen = new_screen;
        }

        frame = create_frame(new_screen.width, new_screen.height);

        XQueryKeymap(display, keys);

        handle_key(&player, map, keys, keycodes);
        draw(map, &player, &frame);

        render_frame(&frame);

        usleep(16000);
    }

    XCloseDisplay(display);
    disable_raw_mode();

    printf("\033[2J");
    printf("\033[H");
    printf("\033[?25h");

    return 0;
}
