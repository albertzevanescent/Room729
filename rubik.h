#ifndef RUBIK_H
#define RUBIK_H

typedef struct pixel {
    unsigned char r, g, b;
} Pixel;

typedef enum {
    red = 0,
    green,
    blue,
    yellow,
    orange,
    white
} Colour;

typedef struct square {
    Pixel **pixels;
    Colour col;
} Square;

typedef struct side {
    Square **squares;
} Side;

typedef struct rubik {
    Side **faces;
} Rubik;

typedef struct action {
    int type, a, b;
} Action;

Square *init_square(Colour col);
Rubik *init_rubik();
char col_to_char(Colour col);
void print_rubik_text(Rubik *rubik);
void print_rubik(char *file_name, Rubik *rubik);
void print_square(FILE *file, Square *square, int row);
void print_square_row(FILE *file, Square **squares, int row);
void print_space(FILE *file, int h, int w, Colour bg);
void print_rubik_img(char *file_name, Rubik *rubik);
void free_square(Square *square);
void free_rubik(Rubik *rubik);
void flip(Side *face);
void side_swap(Side **sides, int a, int b);
void change_perspective(Rubik *rubik, int dir);
void rotate_face(Side *face, int angle);
void row_swap(Side *a, Side *b, int row);
void col_swap(Side *a, Side *b, int col);
void rotate_edge(Side *a, Side *b, Side *c, Side *d, int row, int col, int angle);
void rotate_front_edge(Rubik *rubik, int edge, int angle);
void rotate_front(Rubik *rubik, int angle);
void randomize(Rubik *rubik, Action *moves, int n);
void solve(Rubik *rubik, Action *moves, int n);

#endif
