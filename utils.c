#include "includes.h"

struct termios orig_termios;

void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    fcntl(
        STDIN_FILENO,
        F_SETFL,
        fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
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

float fixed_dist(float x1, float y1, float x2, float y2, float player_angle)
{
    float delta_x = x2 - x1;
    float delta_y = y2 - y1;
    float angle = atan2(delta_y, delta_x) - player_angle;
    float fix_dist = distance(delta_x, delta_y) * cos(angle);
    return fix_dist;
}

t_screen get_screen_size(void)
{
    struct winsize terminal;
    t_screen screen;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);

    screen.width = terminal.ws_col;
    screen.height = terminal.ws_row;

    screen.width = screen.height * 2.5;

    return screen;
}
