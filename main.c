#include "includes.h"

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
        if(i < start_y)
            frame->pixels[i * frame->width + index] = 238;
        else if (i >= start_y && i <= end_y)
            frame->pixels[i * frame->width + index] = color;
        else if(i > end_y)
            frame->pixels[i * frame->width + index] = 242;
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

int main(void)
{
    char **map = get_map();

    t_player player = {1.5, 1.5, 1.5, 0.08};

    t_screen screen;
    t_frame frame;

    printf("\033[2J");
    printf("\033[H");
    printf("\033[?25l");

    enable_raw_mode();

    Display *display = XOpenDisplay(NULL);

    if (!display)
    {
        fprintf(stderr, "Error: Cannot open X display.\n");
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

    screen = get_screen_size();

    
    while (1)
    {
        screen = get_screen_size();
        frame = create_frame(screen.width, screen.height);
        
        XQueryKeymap(display, keys);

        handle_key(&player, map, keys, keycodes);

        clear_frame(&frame);

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
