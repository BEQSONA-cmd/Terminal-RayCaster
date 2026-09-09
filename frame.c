#include "includes.h"

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
                    pos += sprintf(
                        frame->output + pos,
                        "\033[49m");
                else
                    pos += sprintf(
                        frame->output + pos,
                        "\033[48;5;%dm",
                        color);

                current_color = color;
            }

            frame->output[pos++] = ' ';
        }

        if (y != frame->height - 1)
            frame->output[pos++] = '\n';
    }

    write(STDOUT_FILENO, frame->output, pos);
}

void clear_frame(t_frame *frame)
{
    memset(frame->pixels, 0, frame->width * frame->height);
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
