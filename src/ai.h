// AI (.H)
// -------

typedef struct s_ai
{
    int target_x;
    int target_y;
};

extern s_ai ai;

void do_ai_unit_sweep(void);
void find_ai_target(void);
int can_ai_move_here(int x, int y);
int ai_move_unit_towards_target(void);
int find_ai_attack_target(void);

