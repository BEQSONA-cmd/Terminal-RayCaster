#include "includes.h"

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
