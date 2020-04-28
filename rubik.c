#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rubik.h"

#define DIMENSION 3
#define FACES 6
#define SIZE 3
#define BUFFER 10
#define N 50

#define CHAR_TO_INT(i) (i - '0')

Square *init_square(Colour col)
{
    /* Make a square and set its pixels */

    int i, j, red, green, blue;
    red = green = blue = 0;

    switch (col)
    {
        case 0:
            red = 255;
            break;
        case 1:
            green = 255;
            break;    
        case 2:
            blue = 255;
            break; 
        case 3:
            red = 255;
            green = 255;
            break; 
        case 4:
            red = 255;
            green = 165;
            break; 
        case 5:
            red = 255;
            green = 255;
            blue = 255;
            break; 
    }

    Square *square = (Square*)malloc(sizeof(Square));
    square->pixels = (Pixel**)malloc(SIZE * sizeof(Pixel*));
    for(i = 0; i < SIZE; i++)
        square->pixels[i] = (Pixel*)malloc(SIZE * sizeof(Pixel));

    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++)
        {
            square->pixels[i][j].r = red;
            square->pixels[i][j].g = green;
            square->pixels[i][j].b = blue;
        }
    }
    square->col = col;

    return square;
}

Rubik *init_rubik()
{
    /* Make a rubik with all components */

    int i, j, k;
    Rubik *rubik = (Rubik*)malloc(sizeof(Rubik));
    rubik->faces = (Side**)malloc(FACES * sizeof(Side*));
    for(i = 0; i < FACES; i++)
    {
        rubik->faces[i] = (Side*)malloc(sizeof(Side));
        rubik->faces[i]->squares = (Square**)malloc(DIMENSION * DIMENSION * sizeof(Square*));

        for(j = 0; j < DIMENSION; j++)
        {
            for(k = 0; k < DIMENSION; k++)
                rubik->faces[i]->squares[j*DIMENSION + k] = init_square(i);
        }

    }

    return rubik;

}

char col_to_char(Colour col)
{
    switch (col)
    {
    case 0:
        return 'R';
    case 1:
        return 'G';
    case 2:
        return 'B';
    case 3:
        return 'Y';
    case 4:
        return 'O';
    case 5:
        return 'W';
    }

    return 'W';

}

void print_rubik_text(Rubik *rubik)
{
    /* Print cols in text in net form */

    int i, j;

    for(i = 0; i < DIMENSION; i++)
    {
        printf("      ");
        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[1]->squares[i*DIMENSION + j]->col));
        printf("\n");
    }

    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[4]->squares[i*DIMENSION + j]->col));

        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[0]->squares[i*DIMENSION + j]->col));

        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[2]->squares[i*DIMENSION + j]->col));

        printf("\n");
    }

    for(i = 0; i < DIMENSION; i++)
    {
        printf("      ");
        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[3]->squares[i*DIMENSION + j]->col));
        printf("\n");
    }

    for(i = 0; i < DIMENSION; i++)
    {
        printf("      ");
        for(j = 0; j < DIMENSION; j++)
            printf("%c ", col_to_char(rubik->faces[5]->squares[i*DIMENSION + j]->col));
        printf("\n");
    }

}

void print_rubik(char *file_name, Rubik *rubik)
{
    /* Print just cols */

    FILE *file = fopen(file_name, "w");
    if (!file)
    {
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        exit(1);
    }

    int i, j, k;

    for(i = 0; i < FACES; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            for(k = 0; k < DIMENSION; k++)
            {
                fprintf(file, "%d ", rubik->faces[i]->squares[j * DIMENSION + k]->col);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "--%d--\n", i+1);
    }

    fclose(file);
}

void print_square(FILE *file, Square *square, int row)
{
    /* Print RGB of a square */

    int i;
    for(i = 0; i < SIZE; i++)
        fprintf(file, "%u %u %u ", square->pixels[row][i].r, square->pixels[row][i].g, square->pixels[row][i].b);
}

void print_square_row(FILE *file, Square **squares, int row)
{
    /* Print row by row of a row in a Side */

    int i, j;
    /*printf("BBB%d\n",(row+16)->col);
    printf("CCC%d\n",(row+0)->pixels[0][0].r);*/
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < DIMENSION; j++) /* The top of 3 squares at a time */
        {
            print_square(file, squares[row * DIMENSION + j], i);
        }
        fprintf(file, "\n");
    }

}

void print_space(FILE *file, int h, int w, Colour bg)
{
    /* Print space between Sides */

    int i, j;
    unsigned char col = 255;
    if (bg != white)
        col = 0;
    for(i = 0; i < h; i++)
    {
        for(j = 0; j < w; j++)
            fprintf(file, "%u %u %u ", col, col, col);
        fprintf(file, "\n");
    }
}

void print_rubik_img(char *file_name, Rubik *rubik)
{
    /* Print pixels by order of Sides */

    FILE *file = fopen(file_name, "w");
    if (!file)
    {
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        exit(1);
    }

    fprintf(file, "P3\n");
    fprintf(file, "%u %u\n", SIZE * DIMENSION, SIZE * DIMENSION * FACES + 5);
    fprintf(file, "255\n");

    int i, j;
    for(i = 0; i < FACES; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            print_square_row(file, rubik->faces[i]->squares, j);
        }
        print_space(file, 1, SIZE * DIMENSION, 0);

    }

    fclose(file);
}

/*void print_rubik_img_2(char *file_name, Rubik *rubik)
{

    Print pixels in the form of a net

    FILE *file = fopen(file_name, "w");
    if (!file)
    {
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        exit(1);
    }

    fprintf(file, "P3\n");
    fprintf(file, "%u %u\n", SIZE * DIMENSION * DIMENSION + (DIMENSION - 1) * DIMENSION, 
    SIZE * DIMENSION * (FACES - 2) + DIMENSION * DIMENSION);
    fprintf(file, "255\n");

    int i, j;

    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            print_space(file, 1, SIZE, 0);
            print_square_row(file, rubik->faces[i]->squares[j]);
            print_space(file, DIMENSION, SIZE * DIMENSION);
        }

    }

    fclose(file);
}*/

void free_square(Square *square)
{
    int i;
    for(i = 0; i < SIZE; i++)
        free(square->pixels[i]);
    free(square->pixels);
    free(square);
}


void free_rubik(Rubik *rubik)
{
    int i, j, k;
    for(i = 0; i < FACES; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            for(k = 0; k < DIMENSION; k++)
                free_square(rubik->faces[i]->squares[j * DIMENSION + k]);
        }
        free(rubik->faces[i]->squares);
        free(rubik->faces[i]);

    }
    free(rubik->faces);
    free(rubik);

}

void flip(Side *face)
{
    /* Flip side vertically and horizontally */

    int i, j;
    Square *p;

    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION / 2; j++)
        {
            p = face->squares[i*DIMENSION + j];
            face->squares[i*DIMENSION + j] = face->squares[i * DIMENSION + DIMENSION-1-j];
            face->squares[i * DIMENSION + DIMENSION-1-j] = p;
        }

    }

    for(i = 0; i < DIMENSION / 2; i++)
    {
        for(j = 0; j < DIMENSION; j++)
        {
            p = face->squares[i*DIMENSION + j];
            face->squares[i*DIMENSION + j] = face->squares[(DIMENSION-1-i)*DIMENSION + j];
            face->squares[(DIMENSION-1-i)*DIMENSION + j] = p;
        }
    }
}

void side_swap(Side **sides, int a, int b)
{
    Side *p = sides[a];
    sides[a] = sides[b];
    sides[b] = p;        
}

void change_perspective(Rubik *rubik, int dir)
{
    /* Swap sides and rotate unswapped ones */

    flip(rubik->faces[5]); /* Stored upside down */
    if(dir == 0)
    {
        side_swap(rubik->faces, 0, 3);
        side_swap(rubik->faces, 0, 5);
        side_swap(rubik->faces, 0, 1);
        rotate_face(rubik->faces[4], 1);
        rotate_face(rubik->faces[2], -1);
    }
    else if(dir == 1)
    {
        side_swap(rubik->faces, 0, 4);
        side_swap(rubik->faces, 0, 5);
        side_swap(rubik->faces, 0, 2);
        rotate_face(rubik->faces[1], 1);
        rotate_face(rubik->faces[3], -1);
    }
    else if(dir == 2)
    {
        side_swap(rubik->faces, 0, 1);
        side_swap(rubik->faces, 0, 5);
        side_swap(rubik->faces, 0, 3);
        rotate_face(rubik->faces[4], -1);
        rotate_face(rubik->faces[2], 1);
    }
    else
    {
        side_swap(rubik->faces, 0, 2);
        side_swap(rubik->faces, 0, 5);
        side_swap(rubik->faces, 0, 4);
        rotate_face(rubik->faces[1], -1);
        rotate_face(rubik->faces[3], 1);
    }
    flip(rubik->faces[5]);
}

void rotate_face(Side *face, int angle)
{
    /* Rotate face around */

    int i, j;
    Square *p;

    if(!angle) return;
    if(angle < 0) return rotate_face(face, angle + 4);

    for(i = 0; i < DIMENSION; i++)
    {
        for(j = i; j < DIMENSION; j++)
        {
            p = face->squares[i*DIMENSION + j];
            face->squares[i*DIMENSION + j] = face->squares[j*DIMENSION + i];
            face->squares[j*DIMENSION + i] = p;
        }
    }


    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION / 2; j++)
        {
            p = face->squares[i*DIMENSION + j];
            face->squares[i*DIMENSION + j] = face->squares[i * DIMENSION + DIMENSION-1-j];
            face->squares[i * DIMENSION + DIMENSION-1-j] = p;
        }

    }
    rotate_face(face, angle - 1);

}

void row_swap(Side *a, Side *b, int row)
{
    int i;
    Square *p;

    for(i = 0; i < DIMENSION; i++)
    {
        p = a->squares[row*DIMENSION + i];
        a->squares[row*DIMENSION + i] = b->squares[row*DIMENSION + i];
        b->squares[row*DIMENSION + i] = p;        
    }

}

void col_swap(Side *a, Side *b, int col)
{
    int i;
    Square *p;

    for(i = 0; i < DIMENSION; i++)
    {
        p = a->squares[i*DIMENSION + col];
        a->squares[i*DIMENSION + col] = b->squares[i*DIMENSION + col];
        b->squares[i*DIMENSION + col] = p;        
    }

}

void rotate_edge(Side *a, Side *b, Side *c, Side *d, int row, int col, int angle)
{
    /* Swap edges of 4 Sides */

    if(row > -1 && col > -1) exit(1);
    if(!angle) return;
    if(angle < 0) return rotate_edge(a, b, c, d, row, col, angle + 4);

    if(row != -1)
    {
        row_swap(a, b, row); /* rotate edges also */
        row_swap(a, c, row);
        row_swap(a, d, row);
    }
    else
    {
        col_swap(a, b, col);
        col_swap(a, c, col);
        col_swap(a, d, col);
    }

    rotate_edge(a, b, c, d, row, col, angle -1);
}

void rotate_front_edge(Rubik *rubik, int edge, int angle)
{
    if(!angle) return;
    if(angle < 0) return rotate_front_edge(rubik, edge, angle + 4);
    

    flip(rubik->faces[5]);  /* Physical correction */

    switch (edge)
    {
    case 0:
        rotate_face(rubik->faces[1], angle);
        rotate_edge(rubik->faces[0], rubik->faces[2], rubik->faces[5], rubik->faces[4], 0, -1, angle);
        break;
    
    case 1:
        rotate_face(rubik->faces[2], angle);
        rotate_edge(rubik->faces[0], rubik->faces[1], rubik->faces[5], rubik->faces[3], -1, 2, angle);
        break;

    case 2:
        rotate_face(rubik->faces[3], angle);
        rotate_edge(rubik->faces[0], rubik->faces[2], rubik->faces[5], rubik->faces[4], 2, -1, angle);
        break;
    
    case 3:
        rotate_face(rubik->faces[4], angle);
        rotate_edge(rubik->faces[0], rubik->faces[1], rubik->faces[5], rubik->faces[3], -1, 0, angle);
        break;
    }

    flip(rubik->faces[5]);
}

void rotate_front(Rubik *rubik, int angle)
{
    change_perspective(rubik, 0);
    rotate_front_edge(rubik, 2, angle);
    change_perspective(rubik, 2);
}

int check_response(char *s, int *p)
{
    if (s[0] == 'q')
        return 1;
    if (s[0] == 'p')
    {
        if (s[2] >= '0' && s[2] <= '3')
        {
            p[0] = CHAR_TO_INT(s[2]);
            return 2;
        }

    }
    if (s[0] == 'r')
    {
        if (s[2] >= '0' && s[2] <= '3')
        {
            p[0] = CHAR_TO_INT(s[2]);
            if (s[4] >= '0' && s[4] <= '3')
            {
                p[1] = CHAR_TO_INT(s[4]);
                return 3;
            }
            else if (s[4] == '-' && s[5] >= '0' && s[5] <= '3')
            {
                p[1] = -1 * CHAR_TO_INT(s[5]);
                return 3;
            }

        }
    }
    if (s[0] == 'o')
        return 4;

    if (s[0] == 'w')
        return 5;

    if (s[0] == 'e')
        return 6;

    return 0;

}

int power(int a, int b)
{
    int i, p = 1;

    for(i = 0; i < b; i++)
        p = p * a;
    return p;
}

void randomize(Rubik *rubik, Action *moves, int n)
{
    /* Randomize Rubik and store the reverse of every move */

    int i, type, a, b;
    srand(time(0));

    for(i = 0; i < n; i++)
    {
        type = rand() % 2;
        a = rand() % 3;
        b = (rand() % 2 + 1) * power(-1, rand() % 2 + 1);

        if (type == 0)
        {
            change_perspective(rubik, a);
            moves[i].type = 0;
            moves[i].a = (a + 2) % 4;
            moves[i].b = 0;
        }
        else
        {
            rotate_front_edge(rubik, a, b);
            moves[i].type = 1;
            moves[i].a = a;
            moves[i].b = -1 * b;
        }
    }
}

void solve(Rubik *rubik, Action *moves, int n)
{
    /* Perform moves backwards */

    int i;

    for(i = n - 1; i >= 0; i--)
    {
        if (moves[i].type == 0)
            change_perspective(rubik, moves[i].a);
        else
            rotate_front_edge(rubik, moves[i].a, moves[i].b);

    }
}

int main()
{
    Rubik *rubik = init_rubik();

    int run = 1, counter = N/4;
    char s[BUFFER];
    int p[2];
    Action moves[N];

    randomize(rubik, moves, counter);

    printf("\nEdge: T->R->B->L 0->3\nAngle: -3->3 Rotations U/R\n"
    "Direction: U->R->D->L 0->3\nQuit: q\nSolve: w\nRandomize: e\n"
    "Rotate edge: r edge angle\nPerspective: p direction\nOptions: o\n");

    while (run)
    {
        if (counter == N)
        {
            printf("Too many moves time to quit\n");
            exit(1);
        }

        printf("\n");
        print_rubik_text(rubik);

        printf("Enter an option: ");
        fgets(s, BUFFER, stdin);

        switch (check_response(s, p))
        {
            case 0:
                printf("Incorrect input\n");
                break;
            case 1:
                run = 0;
                break;
            case 2:
                moves[counter].type = 0;
                moves[counter].a = (p[0] + 2) % 4;
                moves[counter].b = 0;
                counter++;
                change_perspective(rubik, p[0]);
                break;
            case 3:
                moves[counter].type = 1;
                moves[counter].a = p[0];
                moves[counter].b = -1 * p[1];
                counter++;
                rotate_front_edge(rubik, p[0], p[1]);
                break;
            case 4:
                printf("\nEdge: T->R->B->L 0->3\nAngle: -3->3 Rotations U/R\n"
                "Direction: U->R->D->L 0->3\nQuit: q\nSolve: w\nRandomize: e\n"
                "Rotate edge: r edge angle\nPerspective: p direction\nOptions: o\n");
                break;
            case 5:
                solve(rubik, moves, counter);
                counter = 0;
                break;
            case 6:
                solve(rubik, moves, counter);
                counter = N / 4;
                randomize(rubik, moves, counter);
                break;
        }

    }

    return 0;
}