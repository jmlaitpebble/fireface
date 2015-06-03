/**
 * Fire Simulator
 *
 **/

#include <pebble.h>
#include "fire.h"
#include "mytypes.h"
#include "rand.h"
#include "alphabet.h"
#include "backtable.h"

#define FIREWIDTH 72
#define FIREHEIGHT 56

#define UPTIME 60000

u8 glb_firedata[FIREWIDTH*FIREHEIGHT];

#define MODE_HOURMIN    0
#define MODE_SECDAY     1
#define MODE_WDAY       2
#define MODE_YEAR       3
#define NUM_MODE        4

u8      glb_dofire = 0;
u8      glb_mode = 0;
u32     glb_mssincelastdraw = 0;

#define FIREVAL(x, y) glb_firedata[(x) + ((y) * FIREWIDTH)]

inline u8
gfx_getchartile(u8 c)
{
    return gfx_backtable[c];
};

void
fire_update()
{
    // Let's see how good the compiler is :>
    for (int y = 0; y < FIREHEIGHT-2; y++)
    {
        int             dx;
        dx = 0;
        int             rval = RAND();
        if (rval & 256) dx--;
        if (rval & 2048) dx++;
        for (int x = 0; x < FIREWIDTH; x++)
        {
            int total = 0;
            total = FIREVAL(x-1+dx, y+1) + FIREVAL(x+1+dx, y+1);
            total += 2 * (FIREVAL(x, y+2));

            total /= 4;
            total -= 10;
            if (total < 0)
                total = 0;
            FIREVAL(x, y) = total;
        }
    }
}
void
fire_putchar(int x, int y, u8 c)
{
    // Clip.
    if (x < 0 || x > FIREWIDTH - 8)
        return;
    if (y < 0 || y > FIREHEIGHT - 8)
        return;

    int         offset = gfx_getchartile(c);
    offset *= 8;
    u8          data;
    int         drawoffset = x + y * FIREWIDTH;

    for (int sy = 0; sy < 8; sy++)
    {
        data = alphabet_Tiles[offset+sy];
        for (int sx = 0; sx < 8; sx++)
        {
            if (data & 128)
                glb_firedata[drawoffset + sx] = 255;
            data <<= 1;
        }
        drawoffset += FIREWIDTH;
    }
}

void
fire_putchar2x(int x, int y, u8 c)
{
    // Clip.
    if (x < 0 || x > FIREWIDTH - 16)
        return;
    if (y < 0 || y > FIREHEIGHT - 16)
        return;

    int         offset = gfx_getchartile(c);
    offset *= 8;
    u8          data;
    int         drawoffset = x + y * FIREWIDTH;

    for (int sy = 0; sy < 16; sy++)
    {
        data = alphabet_Tiles[offset+(sy>>1)];
        for (int sx = 0; sx < 8; sx++)
        {
            if (data & 128)
            {
                glb_firedata[drawoffset + sx*2] = 255;
                glb_firedata[drawoffset + sx*2+1] = 255;
            }
            data <<= 1;
        }
        drawoffset += FIREWIDTH;
    }
}

void
fire_putstring(int x, int y, const char *str)
{
    while (*str)
    {
        fire_putchar(x, y, *str);
        x += 8;
        str++;
    }
}


void
fire_putstring2x(int x, int y, const char *str)
{
    while (*str)
    {
        fire_putchar2x(x, y, *str);
        x += 16;
        str++;
    }
}
void
fire_source()
{
#define NUMSOURCE               10
    static u8           srcpos[NUMSOURCE] = { 10, 20, 30, 40, 25,
                                              10, 20, 30, 40, 25
    };

    int         dir = RAND();
    dir >>= 9;
    for (int p = 0; p < NUMSOURCE; p++)
    {
        if (dir & 1)
            srcpos[p]++;
        if (dir & 2)
            srcpos[p]--;

        if (srcpos[p] < 2)
            srcpos[p] = FIREWIDTH-3;
        else if (srcpos[p] >= FIREWIDTH-3)
            srcpos[p] = 2;

        dir >>= 2;
    }

    for (int y = FIREHEIGHT-2; y < FIREHEIGHT; y++)
    {
        for (int x = 0; x < FIREWIDTH; x++)
        {
            FIREVAL(x, y) = 0;
        }
    }
    for (int p = 0; p < NUMSOURCE; p++)
    {
        for (int y = FIREHEIGHT-2; y < FIREHEIGHT; y++)
        {
            for (int x = -2; x <= 2; x++)
            {
                int             fval = (RAND() >> 8) & 255;
                if (x < 0)
                    fval >>= -x;
                else
                    fval >>= x;
                fval += FIREVAL(x+srcpos[p], y);
                if (fval > 255) fval = 255;
                FIREVAL(x+srcpos[p], y) = fval;
            }
        }
    }

#if 0
    for (int y = FIREHEIGHT-2; y < FIREHEIGHT; y++)
    {
        for (int x = 0; x < FIREWIDTH; x++)
        {
            FIREVAL(x, y) = (RAND() >> 8) & 255;
        }
    }
#endif
}


color24_t
fire_colour(u8 val)
{
    color24_t   c;

#if 1
    c.red = val < 128 ? (val * 2) : 255;
    c.green = 0;
    c.blue = 0;
    if (val > 64)
        c.green = val < 196 ? ((val-64)*2) : 255;
    if (val > 128)
        c.blue = (val -128)*2;
#else
#if 0
    c.red = val < 128 ? (val*2) : 255;
    c.green = val;
    c.blue = val >= 128 ? (val * 2) : 0;
#else
    if (val < 128)
    {
        c.red = val * 2;
        c.green = 0;
        c.blue = 0;
    }
    else if (val < 196)
    {
        c.red = 255;
        c.green = (val - 128) * 4;
        c.blue = 0;
    }
    else
    {
        c.red = 255;
        c.green = 255;
        c.blue = val - 196 * 4;
    }
#endif
#endif
    c.alpha = 0;

    return c;
}

void
fire_draw(Layer *layer, GContext *ctx)
{
	for (int y = 0; y < FIREHEIGHT; y++)
    {
        int             offset = y * FIREWIDTH;
        int             srcx = 0;
        color24_t       cd;
		for (int x = 0; x < FIREWIDTH; x++ )
        {
            cd = fire_colour(glb_firedata[offset+srcx]);
			GColor gcd = GColorFromRGB(cd.red, cd.green, cd.blue);
			graphics_context_set_stroke_color(ctx, gcd);
			graphics_draw_pixel(ctx, GPoint(x*2, y*3));
			graphics_draw_pixel(ctx, GPoint(x*2+1, y*3));
			graphics_draw_pixel(ctx, GPoint(x*2, y*3+1));
			graphics_draw_pixel(ctx, GPoint(x*2+1, y*3+1));
			graphics_draw_pixel(ctx, GPoint(x*2, y*3+2));
			graphics_draw_pixel(ctx, GPoint(x*2+1, y*3+2));
            srcx += 1;
        }
    }
}

void
fire_tick(Layer *layer, GContext *ctx)
{
    fire_source();
    fire_update();
    fire_draw(layer, ctx);
}

