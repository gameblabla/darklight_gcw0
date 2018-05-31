// AI (.C)
// -------

//#include <allegro.h>
#include "al2sdl.h"
#include "ai.h"
#include "DarkLight.h"
#include "unit.h"
#include "screen.h"

s_ai ai;

void do_ai_unit_sweep(void)
{
    int x, x2, y, a, times;

    times = 0;
    do
    {


        for (x2 = 0 ; x2 < TILES_X ; x2++)
        {
            if (game.side == 0)
            {
                x = TILES_X - x2 - 1;
            }
            else
            {
                x = x2;
            }

            for (y = 0 ; y < TILES_Y ; y++)
            {
                if (game.winning_side != -1) continue; // We have a winner!

                if (unit_map[x][y].gfx == -1) continue;
                if (unit_map[x][y].movement == -1) continue;
                if (game.side != unit_map[x][y].gfx / 4) continue;

                select_unit(x, y);
                draw_screen();
                rest(300);

                if (key[KEY_ESC]) return;

                for (a = 0 ; a < 8 ; a++)
                {
                    if (game.winning_side != -1) break; // We have a winner!
                    if (selected.x == -1) break;

                    if (unit_map[selected.x][selected.y].movement == 0)
                    {
                        // We've moved, now can we attack?
                        if (find_ai_attack_target() == YES)
                        {
                            rest(200);
                            try_to_move_unit(ai.target_x, ai.target_y);
                            if (selected.x != -1) unit_map[selected.x][selected.y].movement = -1;
                            break;
                        }
                        else
                        {
                            unit_map[selected.x][selected.y].movement = -1;
                            break;
                        }
                    }

                    find_ai_target();
                    if (ai.target_x == -1)
                    {
                        // No targets left? We must have won!
                        unit_map[selected.x][selected.y].movement = -1;
                        game.units_who_can_move--;
                    }
                    ai_move_unit_towards_target();

                    draw_screen();
                    rest(200);
                }


                draw_screen();

                draw_map();
                unselect_unit();
            }
        }

        times++; // Add a safety escape from infinite AI loop..
    }
    while (game.units_who_can_move != 0 && times < 9);
}

void find_ai_target(void)
{
    int x, y, c, dist, score, best_score;

    ai.target_x = -1;
    ai.target_y = -1;
    best_score = 0;

    for (x = 0 ; x < TILES_X ; x++)
    {
        for (y = 0 ; y < TILES_Y ; y++)
        {
            dist = TILES_X - MAX(ABS(selected.x - x), ABS(selected.y - y));
            c = getpixel(map, x, y);

            score = 0;
            if (c == 13) score = 10 + dist;
            if (c == 14 && game.side == 1) score = 15 + dist;
            if (c == 15 && game.side == 0) score = 15 + dist;

            if (unit_map[x][y].gfx != -1 && game.side != unit_map[x][y].gfx / 4)
            {
                score = ((unit_map[x][y].gfx % 4) + 1) * 4 + (dist / 2);
            }
            if (score == 0) continue;

            if (score > best_score)
            {
                best_score = score;
                ai.target_x = x;
                ai.target_y = y;
            }
        }
    }
}

int can_ai_move_here(int x, int y)
{
    int c;

    c = getpixel(map, x, y);

    if ((c == 8 || c == 9 || c == 10) && unit_map[x][y].gfx == -1) return YES;
    if (unit_map[x][y].gfx != -1 && game.side != unit_map[x][y].gfx / 4) return YES;

    return NO;
}

int ai_move_unit_towards_target(void)
{
    int dx, dy, dr, d, a;

    // Setup a 8 direction array (arranged in a circle from top left)
    int dir[8][2] =
    {
        -1, -1,
        0, -1,
        1, -1,
        1, 0,
        1, 1,
        0, 1,
        -1, 1,
        -1, 0
    };

// Reset dx and dy.
    dx = 0;
    dy = 0;

// Find dx and dy values which would move unit close to target.
    if (selected.x < ai.target_x) dx = 1;
    if (selected.y < ai.target_y) dy = 1;

    if (selected.x > ai.target_x) dx = -1;
    if (selected.y > ai.target_y) dy = -1;

    if (dx == 0 && dy == 0)
    {
        game.units_who_can_move--;
        unit_map[selected.x][selected.y].movement = -1;
        return NO; // This shouldn't ever happen!
    }
// Ok, go through our direction array and find the number of the
// position that matches our dx and dy. We need this array so that
// if the direction we want to move in is not available then we can use
// a direction to the left or right (in our circle style direction array).
    for (d = 0 ; d < 8 ; d++)
    {
        if (dx == dir[d][0] && dy == dir[d][1]) dr = d;
    }

    if (can_ai_move_here(selected.x + dx, selected.y + dy) == YES)
    {
        // Move.
        try_to_move_unit(selected.x + dx, selected.y + dy);
        return YES;
    }

// Ok, let's try two alternatives..
    for (a = 0 ; a < 2 ; a++)
    {
        // We can't move in a direct line, so now we pick a direction that is close to the
        // target line one and see if we can move into this one.

        // Do we want to look anti-clockwise, or clockwise in our list?
        if (rand() % 2 == 0)
        {
            // Anti-clockwise.
            // The % 8 keeps the new position within the circle array.
            d = (dr + 7) % 8;
        }
        else
        {
            // Clockwise.
            // The % 8 keeps the new position within the circle array.
            d = (dr + 9) % 8;
        }

        // Find our new dx and dy values from the direction array!
        dx = dir[d][0];
        dy = dir[d][1];

        if (can_ai_move_here(selected.x + dx, selected.y + dy) == YES)
        {
            // Move.
            try_to_move_unit(selected.x + dx, selected.y + dy);
            return YES;
        }
    }

// Let's give up!
    unit_map[selected.x][selected.y].movement = -1;
    game.units_who_can_move--;
    unselect_unit();
}

int find_ai_attack_target(void)
{
    int x1, y1, x2, y2, x, y, count, size, score, best_score;

    size = unit_map[selected.x][selected.y].attack_range;
    ai.target_x = -1;
    ai.target_y = -1;

    x1 = selected.x - size;
    x2 = selected.x + size;
    y1 = selected.y - size;
    y2 = selected.y + size;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= TILES_X) x2 = TILES_X - 1;
    if (y2 >= TILES_Y) y2 = TILES_Y - 1;

    count = 0;
    best_score = 0;

    for (y = y1 ; y <= y2 ; y++)
    {
        for (x = x1 ; x <= x2 ; x++)
        {
            if (unit_map[x][y].gfx == -1) continue;
            if (game.side == unit_map[x][y].gfx / 4) continue;
            score = (unit_map[x][y].gfx % 4) + 1;

            if (score > best_score)
            {
                ai.target_x = x;
                ai.target_y = y;
                best_score = score;
            }
            if (score == best_score && rand() % 2 == 0)
            {
                ai.target_x = x;
                ai.target_y = y;
            }
            count++;
        }
    }

    if (count > 0) return YES;
    return NO;
}
