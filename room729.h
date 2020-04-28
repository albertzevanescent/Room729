#ifndef ROOM729_H
#define ROOM729_H

typedef struct room {
    int number, id, ladder, occupied;
    char *name;
} Room;

typedef struct block {
    Room ***rooms;
} Block;

typedef struct room729 {
    Block ***blocks;
} Room729;

typedef struct location {
    int room729_face, room729_segment, block_face, block_segment;
} Location;

Block *init_block(FILE *names);
Block *init_block_blank();
Room729 *init_room729(char *names);
void free_block(Block *block);
void free_room729(Room729 *room729);
void print_room729(Room729 *room729, Location *loc);
void room_swap(Block *block, int a, int b, int c, int d);
void block_swap(Room729 *room729, int a, int b, int c, int d);
void rotate_face_block(Block *block, int face, int angle);
void rotate_face_room729(Room729 *room729, int face, int angle);
void rotate_edge_block_v(Block *block, int edge, int angle);
void rotate_edge_room729_v(Room729 *room729, int edge, int angle);
void rotate_edge_block_h(Block *block, int edge, int angle);
void rotate_edge_room729_h(Room729 *room729, int edge, int angle);
void change_perspective_block(Block *block, int dir);
void change_perspective_room729(Room729 *room729, int dir);
void randomize(Room729 *room729, int n);
void set_loc(Location *loc);
int find_room(Block *block, int num, Location *loc);
int find_block(Room729 *room729, int num, Location *loc);
int focus_room(Block *block, int num, Location *loc);
int focus_block(Room729 *room729, int num, Location *loc);
void move_ladder(Room729 *room729, Location *loc, int move);
int enter_leave(Room729 *room729, Location *loc, int intent);
void message(Room729 *room729, Location *loc, int type);

#endif
