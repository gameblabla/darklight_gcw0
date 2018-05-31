// Unit (.C)

//#include <allegro.h>
#include "DarkLight.h"
#include "unit.h"
#include "al2sdl.h"

s_unit unit[MAX_TYPES];
s_unit unit_map[TILES_X][TILES_Y];
s_selected selected;



void setup_units(void)
{
    int u;

// Setup attributes for dark units first, then just copy to light.
    unit[SOLDIER].attack = 1;
    unit[SOLDIER].defence = 2;
    unit[SOLDIER].movement = 2;
    unit[SOLDIER].attack_range = 1;
    unit[SOLDIER].gfx = 0;

    unit[KNIGHT].attack = 2;
    unit[KNIGHT].defence = 2;
    unit[KNIGHT].movement = 1;
    unit[KNIGHT].attack_range = 1;
    unit[KNIGHT].gfx = 1;

    unit[ARCHER].attack = 2;
    unit[ARCHER].defence = 1;
    unit[ARCHER].movement = 1;
    unit[ARCHER].attack_range = 3;
    unit[ARCHER].gfx = 2;

    unit[MAGE].attack = 3;
    unit[MAGE].defence = 2;
    unit[MAGE].movement = 1;
    unit[MAGE].attack_range = 3;
    unit[MAGE].gfx = 3;

// And set up 'blank' unit to represent empty tile.
    unit[BLANK].attack = 0;
    unit[BLANK].defence = 0;
    unit[BLANK].movement = -2;
    unit[BLANK].attack_range = 0;
    unit[BLANK].gfx = -1;

    for (u = 0 ; u < 4 ; u++)
    {
        unit[u + 4] = unit[u];
        unit[u + 4].gfx = unit[u].gfx + 4;
    }
}

void setup_unit_map(void)
{
    int x, y;

    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            unit_map[x][y] = unit[BLANK];
        }
    }
}

void select_unit(int x, int y)
{
// Any unit here?
    if (unit_map[x][y].gfx == -1) return;

// Are we trying to select an enemy?
    if (game.side != unit_map[x][y].gfx / 4)
    {
        show_message("That is an enemy soldier!");
        return;
    }


    selected.x = x;
    selected.y = y;

    highlight_enemy_units();

    rectfill(small_buffer, x * 16, (y * 16)+14, (x * 16) + 15, (y * 16) + 14 + 15, al2sdl_color(2));
    draw_tile(x * 16, (y * 16)+14, unit_map[x][y].gfx);
    draw_cursor();

}

void unselect_unit(void)
{
// Draw map to remove any highlighting. Then set as no unit selected.
    //selected.x = -1;
    draw_map();
    selected.x = -1;
}

void try_to_move_unit(int x, int y)
{
    int dist, c;

    dist = MAX(ABS(selected.x - x), ABS(selected.y - y));
    if (dist == 0) return;

// Cannot move a unit without movement points
/*
    if ( unit_map[selected.x][selected.y].movement <= 0 )
    {
        unselect_unit();
        return;
    }
*/

// We can't move onto a friendly unit..
    if (unit_map[x][y].gfx != -1 && unit_map[x][y].gfx / 4 == game.side) return;

    if (unit_map[x][y].gfx != -1 && dist <= unit_map[selected.x][selected.y].attack_range)
    {
        // We must be trying to attack an enemy..

        draw_tile(x * 16, (y * 16)+16, 11);
        draw_screen();

        // Play sound effect
        if ( dist == 1 ) // melee
        {
            //Mix_PlayChannel(-1, sfx_hit, 0);
            PlaySound( sfx_hit );
        }
        else
        {
            if ( unit_map[selected.x][selected.y].gfx == 2 || unit_map[selected.x][selected.y].gfx == 6 )
            {
                //Mix_PlayChannel(-1, sfx_arrow, 0);
                PlaySound( sfx_arrow );
            }

            if ( unit_map[selected.x][selected.y].gfx == 3 || unit_map[selected.x][selected.y].gfx == 7 )
            {
                //Mix_PlayChannel(-1, sfx_fireball, 0);
                PlaySound( sfx_fireball );
            }
        }


        rest(700);



        if (attack(unit_map[selected.x][selected.y].attack,
                   unit_map[x][y].defence) == YES)
        {
            // Kill enemy unit!
            show_message("The enemy was killed!");

            // Move our unit over enemy if we are next to enemy.
            if (dist == 1)
            {
                unit_map[x][y] = unit_map[selected.x][selected.y];
                unit_map[selected.x][selected.y] = unit[BLANK];
                selected.x = x;
                selected.y = y;
                capture_any_castles();
            }
            else
            {
                // Ranged attack, just kill enemy unit.
                unit_map[x][y] = unit[BLANK];
            }
        }
        unit_map[selected.x][selected.y].movement = -1;
        game.units_who_can_move--;
        unselect_unit();
        //draw_map();
        check_for_win();

        return;
    }

    if (selected.x != -1 && unit_map[selected.x][selected.y].movement <= 0)
    {
        unit_map[selected.x][selected.y].movement = -1;
        game.units_who_can_move--;
        unselect_unit();
        return;
    }

// We must be trying to move onto terrain.
// Stop us moving more than 1 square at a time..
    if (dist > 1) return;

    c = getpixel(map, x, y);
    if (c == 8 || c == 9)
    {
        unit_map[x][y] = unit_map[selected.x][selected.y];
        unit_map[selected.x][selected.y] = unit[BLANK];

        PlaySound(sfx_step); // Mix_PlayChannel( -1, sfx_step, 0 );
        show_message(""); // Remove old messages


        draw_map(); // Update display and remove any enemy highlighting..
        unit_map[x][y].movement--;
        selected.x = x;
        selected.y = y;

        capture_any_castles();
        check_for_win();

        if (highlight_enemy_units() == NO && unit_map[x][y].movement <= 0)
        {
            // Run out of movement points and no enemies in range.
            unit_map[x][y].movement = -1;
            game.units_who_can_move--;

            unselect_unit();
            //draw_map();
        }
        else
        {
            select_unit(x, y);
            if( player[game.side].cpu == NO && unit_map[x][y].movement <= 0 && highlight_enemy_units() != NO )
            {
                show_message("Attack enemy, or press (B)");
                highlight_enemy_units();
            }
        }
    }
    if (c == 10)
    {
        draw_tile(x * 16, (y * 16)+14, 11);
        PlaySound(sfx_step); //Mix_PlayChannel( -1, sfx_step, 0 );
        draw_screen();
        rest(700);

        // Can we defeat a mighty tree..?
        if (attack(unit_map[selected.x][selected.y].attack * 2, 1) == YES)
        {
            // Kill tree!
            putpixel(map, x, y, 8); // Change tree to plain tile.
            unit_map[x][y] = unit_map[selected.x][selected.y];
            unit_map[selected.x][selected.y] = unit[BLANK];

            selected.x = x;
            selected.y = y;

            capture_any_castles();
            check_for_win();

            unit_map[selected.x][selected.y].movement = -1;
            game.units_who_can_move--;
            draw_map();
            unselect_unit();

            return;
        }
        else
        {
            // Attack failed.
            unit_map[selected.x][selected.y].movement = -1;
            game.units_who_can_move--;
            draw_map();
            unselect_unit();

            return;
        }
    }
}

int attack(int a, int b)
{
    a = a * ((rand() % 10) + 1);
    b = b * ((rand() % 10) + 1);

    if (a > b) return YES;

    return NO;
}

int highlight_enemy_units(void)
{
    int x1, y1, x2, y2, x, y, count, size;
    if( selected.x == -1 ) return 0;

    size = unit_map[selected.x][selected.y].attack_range;

    x1 = selected.x - size;
    x2 = selected.x + size;
    y1 = selected.y - size;
    y2 = selected.y + size;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= TILES_X) x2 = TILES_X - 1;
    if (y2 >= TILES_Y) y2 = TILES_Y - 1;

    count = 0;

    for (y = y1 ; y <= y2 ; y++)
    {
        for (x = x1 ; x <= x2 ; x++)
        {
            if (unit_map[x][y].gfx == -1) continue;
            if (game.side == unit_map[x][y].gfx / 4) continue;

            count++;
            rectfill(small_buffer, x * 16, (y * 16)+14, (x * 16) + 15, (y * 16)+14 + 15, al2sdl_color(1) );
            draw_tile(x * 16, (y * 16)+14, unit_map[x][y].gfx);
        }
    }

    return count;
}

int count_units_who_can_move(void)
{
    int x, y, count;

    count = 0;
    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            if (unit_map[x][y].gfx == -1) continue;
            if (unit_map[x][y].gfx / 4 == game.side) count++;
        }
    }

    return count;
}

void capture_any_castles(void)
{
    int x1, y1, x2, y2, x, y, nx, ny, c, r, size, change;

    size = 1;
    change = NO;

    x1 = selected.x - size;
    x2 = selected.x + size;
    y1 = selected.y - size;
    y2 = selected.y + size;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= TILES_X) x2 = TILES_X - 1;
    if (y2 >= TILES_Y) y2 = TILES_Y - 1;

    draw_map();
    int spawned = false;
    int castle = false;

    for (y = y1 ; y <= y2 ; y++)
    {
        for (x = x1 ; x <= x2 ; x++)
        {
            c = getpixel(map, x, y);

            if (c == 13 || (game.side == 0 && c == 15) || (game.side == 1 && c == 14))
            {
                nx = x + (rand() % 3) - 1;
                ny = y + (rand() % 3) - 1;
                if (change == NO) change = YES;

                // Change castle gfx.
                putpixel(map, x, y, 14 + game.side);
                show_message("Castle captured!");
                castle = true;

                c = getpixel(map, nx, ny);
                if ((c == 8 || c == 9) && unit_map[nx][ny].gfx == -1)
                {
                    // Add new friendly unit.
                    r = rand() % 4 + (game.side * 4);
                    draw_tile(nx * 16, (ny * 16)+14, 16);
                    unit_map[nx][ny] = unit[r];
                    game.units_who_can_move++;
                    change = 2;
                    spawned = true;
                }
            }

        }
    }

    if ( spawned )
    {
        show_message("Reinforcements have arrived!");
        PlaySound(sfx_spawn); //Mix_PlayChannel( -1, sfx_spawn, 0 );
    }

    if ( castle )
        PlaySound(sfx_castle); //Mix_PlayChannel( -1, sfx_castle, 0 );


    if (change > NO)
    {
        select_unit(selected.x, selected.y);
        draw_screen();
        rest(300 * change);
        draw_map();
    }
}

void restore_all_units_movement(void)
{
    int x, y;

    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            // Restore movement points!
            if (unit_map[x][y].gfx != -1) unit_map[x][y].movement = unit[unit_map[x][y].gfx].movement;
        }
    }
}

void check_for_win(void)
{
    int x, y, units;

    units = 0;

    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            if (unit_map[x][y].gfx == -1) continue;
            if (game.side != (unit_map[x][y].gfx / 4)) units++;
        }
    }

    if (units == 0) game.winning_side = game.side;
}



