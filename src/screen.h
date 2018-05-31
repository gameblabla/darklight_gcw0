// Screen (.h)

#ifndef SCREEN_H_
#define SCREEN_H_

#define TILES_X      20
#define TILES_Y      12

void draw_screen(void);
void draw_tile(int x, int y, int tile);
void draw_map(void);
void create_map(void);
void add_armies(void);
void draw_status_text(int x, int y);
void draw_turn_text(void);
void draw_winner_text(void);
void draw_title_text(void);
void draw_unit_info(int x, int y);
void draw_credits();

void redraw_cursor_bg();
void draw_cursor();

void show_message( char *fmt, ... );

extern char message[128];

extern int cursor_frame;
extern int cursor_frame_timer;

#endif
