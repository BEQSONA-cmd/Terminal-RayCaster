#include "includes.h"

void move_player(t_player *player, char **map, double dx, double dy)
{
    double new_x = player->x + dx;
    double new_y = player->y + dy;

    if (!touch(new_x, player->y, map))
        player->x = new_x;

    if (!touch(player->x, new_y, map))
        player->y = new_y;
}

void handle_key(t_player *player, char **map)
{
    char key;

    if (read(STDIN_FILENO, &key, 1) <= 0)
        return;

    if (key == 'w')
        move_player(player, map, cos(player->angle) * player->speed, sin(player->angle) * player->speed);

    if (key == 's')
        move_player(player, map, -cos(player->angle) * player->speed, -sin(player->angle) * player->speed);

    if (key == 'a')
        move_player(player, map, cos(player->angle - PI / 2) * player->speed, sin(player->angle - PI / 2) * player->speed);

    if (key == 'd')
        move_player(player, map, cos(player->angle + PI / 2) * player->speed, sin(player->angle + PI / 2) * player->speed);

    if (key == '\033')
    {
        char sequence[2];

        if (read(STDIN_FILENO, &sequence[0], 1) > 0 &&
            read(STDIN_FILENO, &sequence[1], 1) > 0)
        {
            if (sequence[0] == '[' && sequence[1] == 'D')
                player->angle -= player->speed;

            if (sequence[0] == '[' && sequence[1] == 'C')
                player->angle += player->speed;
        }
    }

    if (player->angle < 0)
        player->angle += 2 * PI;

    if (player->angle >= 2 * PI)
        player->angle -= 2 * PI;
}
