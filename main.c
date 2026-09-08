#include "includes.h"

char **get_map(void)
{
    char **map = malloc(sizeof(char *) * 10);

    map[0] = "1111111111111111";
    map[1] = "1000000000000001";
    map[2] = "1000001000000001";
    map[3] = "1000000100000001";
    map[4] = "1000000000000001";
    map[5] = "1000000000000001";
    map[6] = "1000000100000001";
    map[7] = "1000000000000001";
    map[8] = "1111111111111111";
    map[9] = NULL;

    return map;
}

void draw_line(t_player *player, char **map, double start_angle, int index, int screen_height)
{
    double cos_angle = cos(start_angle);
    double sin_angle = sin(start_angle);

    double ray_x = player->x;
    double ray_y = player->y;

    while (!touch(ray_x, ray_y, map))
    {
        ray_x += cos_angle * RAY_STEP;
        ray_y += sin_angle * RAY_STEP;

        int screen_x = (int)(ray_x * BLOCK_W) + 4;
        int screen_y = (int)(ray_y * BLOCK_H) + 2;
    }

    float distance = fixed_dist(player->x, player->y, ray_x, ray_y, player->angle);
    float wall_height = (screen_height * 0.8) / distance;
    int center_y = screen_height / 2;

    int start_y = (int)(center_y - wall_height / 2);
    if (start_y < 0)
        start_y = 0;
    int end_y = (int)(center_y + wall_height / 2);
    if (end_y > screen_height)
        end_y = screen_height;

    while (start_y < end_y)
    {
        printf("\033[%d;%dH|", start_y, index + 1);
        start_y++;
    }
}

void draw_map(char **map, t_player *player, int screen_width, int screen_height)
{
    float fraction = PI / 3 / screen_width;
    float start_x = player->angle - PI / 6;
    int i = 0;

    while (i < screen_width)
    {

        draw_line(player, map, start_x, i, screen_height);
        start_x += fraction;
        i++;
    }

    int player_x = (int)(player->x * BLOCK_W) + 4;
    int player_y = (int)(player->y * BLOCK_H) + 2;

    fflush(stdout);
}

void clear_screen(void)
{
    printf("\033[2J\033[H");
}

int main(void)
{
    char **map = get_map();

    t_player player = 
    {
        5.5,
        5.5,
        0.0,
        0.08
    };

    printf("\033[2J");
    printf("\033[?25l");

    t_screen screen;

    enable_raw_mode();

    while (1)
    {
        screen = get_screen_size();
        clear_screen();
        handle_key(&player, map);

        draw_map(map, &player, screen.width, screen.height);

        fflush(stdout);

        usleep(16000);
    }

    disable_raw_mode();

    printf("\033[2J");
    printf("\033[H");
    printf("\033[?25h");

    return 0;
}