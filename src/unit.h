// unit (.H)

#include "screen.h"
#include "DarkLight.h"

#define BLANK         16
#define MAX_TYPES     BLANK + 1

#define SOLDIER       0
#define KNIGHT        1
#define ARCHER        2
#define MAGE          3

extern int music_balance;
extern int last_music_change;

typedef struct s_unit
{
    int attack;
    int defence;
    int movement;
    int attack_range;
    int gfx;
};

extern s_unit unit[MAX_TYPES];

typedef struct s_selected
{
    int x;
    int y;
};
extern s_selected selected;

// Make map of units!
extern s_unit unit_map[TILES_X][TILES_Y];

void setup_units(void);
void setup_unit_map(void);
void select_unit(int x, int y);
void unselect_unit(void);
int attack(int a, int b);
int highlight_enemy_units(void);
int count_units_who_can_move(void);
void try_to_move_unit(int x, int y);
void capture_any_castles(void);
void restore_all_units_movement(void);
void check_for_win(void);


