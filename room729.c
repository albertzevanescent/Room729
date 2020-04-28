#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "room729.h"

#define DIMENSION 3
#define BUFFER 10
#define N 50

#define INDEX(i, j) (i * DIMENSION + j)

Block *init_block(FILE *names)
{
    /* Make a Block with 3 3*3 sets of Rooms from file */

    int i, j;
    char buffer [N];
    char *token;
    Block *block = (Block*)malloc(sizeof(Block));
    block->rooms = (Room***)malloc(DIMENSION * sizeof(Room**));

    for(i = 0; i < DIMENSION; i++)
    {
        block->rooms[i] = (Room**)malloc(DIMENSION * DIMENSION * sizeof(Room*));
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            block->rooms[i][j] = (Room*)malloc(sizeof(Room));
            if (i == DIMENSION / 2 && j == DIMENSION * DIMENSION / 2) /* Empty center */
            {
                block->rooms[i][j]->number = -1;
                block->rooms[i][j]->id = -1;
                block->rooms[i][j]->name = malloc(sizeof("-1"));
                strcpy(block->rooms[i][j]->name, "-1");
                block->rooms[i][j]->ladder = 0;
                block->rooms[i][j]->occupied = 0;
                continue;
            }
            fgets(buffer,N, names);
            token = strtok(buffer, ",");
            block->rooms[i][j]->number = atoi(token);
            token = strtok(NULL, ",");
            block->rooms[i][j]->id = atoi(token);
            token = strtok(NULL, ",");
            token[strlen(token) - 1] = '\0'; /* \n at end */
            block->rooms[i][j]->name = malloc(strlen(token)+1);
            strcpy(block->rooms[i][j]->name, token);
            block->rooms[i][j]->ladder = 0;
            block->rooms[i][j]->occupied = 0;
        }
    }
    return block;
}

Block *init_block_blank()
{
    /* Create a center Block with no information */

    int i, j;
    Block *block = (Block*)malloc(sizeof(Block));
    block->rooms = (Room***)malloc(DIMENSION * sizeof(Room**));

    for(i = 0; i < DIMENSION; i++)
    {
        block->rooms[i] = (Room**)malloc(DIMENSION * DIMENSION * sizeof(Room*));
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            block->rooms[i][j] = (Room*)malloc(sizeof(Room));
            block->rooms[i][j]->number = -1;
            block->rooms[i][j]->id = -1;
            block->rooms[i][j]->name = malloc(sizeof("-1"));
            strcpy(block->rooms[i][j]->name, "-1");
            block->rooms[i][j]->ladder = 0;
            block->rooms[i][j]->occupied = 0;
        }
    }
    return block;
}

Room729 *init_room729(char *names)
{
    /* Make a Room729 with 3 3*3 sets of Blocks from file */

    int i, j;
    FILE *fp;
    fp = fopen(names, "r");

    if (!fp) 
    {
        fprintf(stderr, "Unable to open file '%s'\n", names);
        exit(1);
    }

    Room729 *room729 = (Room729*)malloc(sizeof(Room729));
    room729->blocks = (Block***)malloc(DIMENSION * sizeof(Block**));

    for(i = 0; i < DIMENSION; i++)
    {
        room729->blocks[i] = (Block**)malloc(DIMENSION * DIMENSION * sizeof(Block*));
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            if (i == DIMENSION / 2 && j == DIMENSION * DIMENSION / 2) /* Empty center */
                room729->blocks[i][j] = init_block_blank();
            else
                room729->blocks[i][j] = init_block(fp);
        }
    }

    fclose(fp);
    return room729;
}

void free_block(Block *block)
{
    /* Free Block and Room's name */

    int i, j;

    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            free(block->rooms[i][j]->name);
            free(block->rooms[i][j]);
        }
        free(block->rooms[i]);
    }
    free(block->rooms);
    free(block);
}

void free_room729(Room729 *room729)
{
    /* Free Room729 and Blocks */

    int i, j;
    for(i = 0; i < DIMENSION; i++)
    {
        for(j = 0; j < DIMENSION * DIMENSION; j++)
            free_block(room729->blocks[i][j]);
        free(room729->blocks[i]);

    }
    free(room729->blocks);
    free(room729);
}

void print_room729(Room729 *room729, Location *loc)
{
    /* Print out front of Blocks and ladder for Room in focus */

    int i, j, k, l;
    char occupy = ' ';
    for(i = 0; i < DIMENSION * DIMENSION; i += DIMENSION)
    {
        for(j = 0; j < DIMENSION * DIMENSION; j += DIMENSION)
        {
            for(k = 0; k < DIMENSION; k++)
            {
                for(l = 0; l < DIMENSION; l++)
                {
                    if (room729->blocks[0][i+k]->rooms[0][j+l]->occupied == 1) occupy = '*';
                    else occupy = ' ';
                    if (room729->blocks[0][i+k]->rooms[0][j+l]->ladder == 0)
                        printf(" %3d%c", room729->blocks[0][i+k]->rooms[0][j+l]->number, occupy);
                    else
                        printf(" | | ");
                }
            }
            printf("\n");
        }
    }

    for(i = DIMENSION * (DIMENSION - 1); i < DIMENSION * DIMENSION; i++) /* Ground below */
    {
        for(j = DIMENSION * (DIMENSION - 1); j < DIMENSION * DIMENSION; j++)
        {
            if (room729->blocks[0][i]->rooms[0][j]->ladder == 1 ||
            (i == loc->room729_segment && j == loc->block_segment))
                printf(" | | ");
            else printf("_____");
        }
    }
    printf("\n");
}

void room_swap(Block *block, int a, int b, int c, int d)
{
    /* Swap 2 Rooms */

    Room *p = block->rooms[a][b];
    block->rooms[a][b] = block->rooms[c][d];
    block->rooms[c][d] = p;
}

void block_swap(Room729 *room729, int a, int b, int c, int d)
{
    /* Swap 2 Blocks */

    Block *p = room729->blocks[a][b];
    room729->blocks[a][b] = room729->blocks[c][d];
    room729->blocks[c][d] = p;
}

void rotate_face_block(Block *block, int face, int angle)
{
    /* Rotate any of 3 faces (3*3 Rooms) */

    int i, j;

    if(!angle || face < 0 || face >= DIMENSION) return;
    if(angle < 0) return rotate_face_block(block, face, angle + 4); /* -1 == 3 */

    for(i = 0; i < DIMENSION; i++) /* Transpose */
        for(j = i; j < DIMENSION; j++)
            room_swap(block, face, INDEX(i, j), face, INDEX(j, i));

    for(i = 0; i < DIMENSION; i++) /* Flip along vertical */
        for(j = 0; j < DIMENSION / 2; j++)
            room_swap(block, face, INDEX(i, j), face, INDEX(i, DIMENSION - 1 - j));
    rotate_face_block(block, face, angle - 1);
}

void rotate_face_room729(Room729 *room729, int face, int angle)
{
    /* Rotate any of 3 faces (3*3 Blocks) */

    int i, j;

    if(!angle || face < 0 || face >= DIMENSION) return;
    if(angle < 0) return rotate_face_room729(room729, face, angle + 4);

    for(i = 0; i < DIMENSION; i++)
        for(j = i; j < DIMENSION; j++)
            block_swap(room729, face, INDEX(i, j), face, INDEX(j, i));

    for(i = 0; i < DIMENSION; i++)
        for(j = 0; j < DIMENSION / 2; j++)
            block_swap(room729, face, INDEX(i, j), face, INDEX(i, DIMENSION - 1 - j));
    rotate_face_room729(room729, face, angle - 1);
}

void rotate_edge_block_v(Block *block, int edge, int angle)
{
    /* Rotate any vertical edge (3*3 Rooms) */

    if(!angle || edge < 0 || edge >= DIMENSION) return;
    if(angle < 0) return rotate_edge_block_v(block, edge, angle + 4);

    room_swap(block, 2, edge, 2, edge + 2 * DIMENSION); /* Corners */
    room_swap(block, 2, edge, 0, edge + 2 * DIMENSION);
    room_swap(block, 2, edge, 0, edge);

    room_swap(block, 1, edge, 2, edge + DIMENSION); /* Sides */
    room_swap(block, 1, edge, 1, edge + 2 * DIMENSION);
    room_swap(block, 1, edge, 0, edge + DIMENSION);

    rotate_edge_block_v(block, edge, angle - 1);
}

void rotate_edge_room729_v(Room729 *room729, int edge, int angle)
{
    /* Rotate any vertical edge (3*3 Blocks) */

    int i, j;

    if(!angle || edge < 0 || edge >= DIMENSION) return;
    if(angle < 0) return rotate_edge_room729_v(room729, edge, angle + 4);

    block_swap(room729, 2, edge, 2, edge + 2 * DIMENSION); /* Corners */
    block_swap(room729, 2, edge, 0, edge + 2 * DIMENSION);
    block_swap(room729, 2, edge, 0, edge);

    block_swap(room729, 1, edge, 2, edge + DIMENSION); /* Sides */
    block_swap(room729, 1, edge, 1, edge + 2 * DIMENSION);
    block_swap(room729, 1, edge, 0, edge + DIMENSION);

    for(i = 0; i < DIMENSION; i++) /* Change blocks themselves */
        for(j = 0; j < DIMENSION; j++)
            change_perspective_block(room729->blocks[i][edge + j * DIMENSION], 2);

    rotate_edge_room729_v(room729, edge, angle - 1);
}

void rotate_edge_block_h(Block *block, int edge, int angle)
{
    /* Rotate any vertical edge (3*3 Rooms) */

    if(!angle || edge < 0 || edge >= DIMENSION) return;
    if(angle < 0) return rotate_edge_block_h(block, edge, angle + 4);

    room_swap(block, 2, edge * DIMENSION, 0, edge * DIMENSION); /* Corners */
    room_swap(block, 2, edge * DIMENSION, 0, 2 + edge * DIMENSION);
    room_swap(block, 2, edge * DIMENSION, 2, 2 + edge * DIMENSION);

    room_swap(block, 2, 1 + edge * DIMENSION, 1, edge * DIMENSION); /* Sides */
    room_swap(block, 2, 1 + edge * DIMENSION, 0, 1 + edge * DIMENSION);
    room_swap(block, 2, 1 + edge * DIMENSION, 1, 2 + edge * DIMENSION);

    rotate_edge_block_h(block, edge, angle - 1);
}

void rotate_edge_room729_h(Room729 *room729, int edge, int angle)
{
    /* Rotate any vertical edge (3*3 Blocks) */

    int i, j;

    if(!angle || edge < 0 || edge >= DIMENSION) return;
    if(angle < 0) return rotate_edge_room729_h(room729, edge, angle + 4);

    block_swap(room729, 2, edge * DIMENSION, 0, edge * DIMENSION); /* Corners */
    block_swap(room729, 2, edge * DIMENSION, 0, 2 + edge * DIMENSION);
    block_swap(room729, 2, edge * DIMENSION, 2, 2 + edge * DIMENSION);

    block_swap(room729, 2, 1 + edge * DIMENSION, 1, edge * DIMENSION); /* Sides */
    block_swap(room729, 2, 1 + edge * DIMENSION, 0, 1 + edge * DIMENSION);
    block_swap(room729, 2, 1 + edge * DIMENSION, 1, 2 + edge * DIMENSION);

    for(i = 0; i < DIMENSION; i++) /* Change blocks themselves */
    {
        for(j = 0; j < DIMENSION; j++)
            change_perspective_block(room729->blocks[i][edge * DIMENSION + j], 3);
    }

    rotate_edge_room729_h(room729, edge, angle - 1);
}

void change_perspective_block(Block *block, int dir)
{
    /* Rotate edges */

    if(dir == 0)
    {
        rotate_edge_block_v(block, 0, -1);
        rotate_edge_block_v(block, 1, -1);
        rotate_edge_block_v(block, 2, -1);
    }
    else if(dir == 1)
    {
        rotate_edge_block_h(block, 0, -1);
        rotate_edge_block_h(block, 1, -1);
        rotate_edge_block_h(block, 2, -1);
    }
    else if(dir == 2)
    {
        rotate_edge_block_v(block, 0, 1);
        rotate_edge_block_v(block, 1, 1);
        rotate_edge_block_v(block, 2, 1);
    }
    else
    {
        rotate_edge_block_h(block, 0, 1);
        rotate_edge_block_h(block, 1, 1);
        rotate_edge_block_h(block, 2, 1);
    }
}

void change_perspective_room729(Room729 *room729, int dir)
{
    /* Rotate edges */

    if(dir == 0)
    {
        rotate_edge_room729_v(room729, 0, -1);
        rotate_edge_room729_v(room729, 1, -1);
        rotate_edge_room729_v(room729, 2, -1);
    }
    else if(dir == 1)
    {
        rotate_edge_room729_h(room729, 0, -1);
        rotate_edge_room729_h(room729, 1, -1);
        rotate_edge_room729_h(room729, 2, -1);
    }
    else if(dir == 2)
    {
        rotate_edge_room729_v(room729, 0, 1);
        rotate_edge_room729_v(room729, 1, 1);
        rotate_edge_room729_v(room729, 2, 1);
    }
    else
    {
        rotate_edge_room729_h(room729, 0, 1);
        rotate_edge_room729_h(room729, 1, 1);
        rotate_edge_room729_h(room729, 2, 1);
    }
}

void randomize(Room729 *room729, int n)
{
    /* Randomize Room729 and each Block n times */

    int i, j, k, type, a, b;

    for(i = 0; i < n; i++) /* Rotate Room729 faces randomly */
    {
        type = rand() % 2;
        a = rand() % 3;
        b = rand() % 3 + 1;

        if (type == 0) rotate_edge_room729_v(room729, a, b);
        else rotate_edge_room729_h(room729, a, b);
    }

    for(i = 0; i < DIMENSION; i++) /* Rotate Block faces randomly */
    {
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            for(k = 0; k < n; k++)
            {
                type = rand() % 2;
                a = rand() % 3;
                b = rand() % 3 + 1;

                if (type == 0) rotate_edge_block_v(room729->blocks[i][j], a, b);
                else rotate_edge_block_h(room729->blocks[i][j], a, b);
            }
        }
    }
}

void set_loc(Location *loc)
{
    /* Set location */

    loc->room729_face = -1;
    loc->room729_segment = -1;
    loc->block_face = -1;
    loc->block_segment = -1;
}

int find_room(Block *block, int num, Location *loc)
{
    /* Find a given Room and store it's Block location */

    int i, j;

    for(i = 0; i < DIMENSION; i++)
        for(j = 0; j < DIMENSION * DIMENSION; j++)
            if (block->rooms[i][j]->number == num)
            {
                loc->block_face = i;
                loc->block_segment = j;
                return 1;
            }
    return 0;
}

int focus_room(Block *block, int num, Location *loc)
{
    /* Put a Room at the bottom-most of the front face */

    if (!find_room(block, num, loc))
        return 0;

    if (loc->block_face == 2) /* Move to front face and mirror segment */
    {
        change_perspective_block(block, 1);
        change_perspective_block(block, 1);
        loc->block_face = 0;
        loc->block_segment = loc->block_segment - 2 * (loc->block_segment % 3) + 2;
    }

    else if (loc->block_face == 1) /* Move to front face and change segment */
    {
        switch (loc->block_segment)
        {
        case 0:
        case 1:
        case 2:
            change_perspective_block(block, 0);
            loc->block_face = 0;
            loc->block_segment = loc->block_segment + 3;
            break;
        case 3:
            change_perspective_block(block, 3);
            loc->block_face = 0;
            loc->block_segment = loc->block_segment + 1;
            break;      
        case 4:
            return 0;
        case 5:
            change_perspective_block(block, 1);
            loc->block_face = 0;
            loc->block_segment = loc->block_segment - 1;
            break;   
        case 6:
        case 7:
        case 8:
            change_perspective_block(block, 2);
            loc->block_face = 0;
            loc->block_segment = loc->block_segment - 3;
            break;            
        }
    }
    /* Modify room order to bottom most */
    /*switch (loc->block_segment)
    {
    case 0:
    case 3:
        rotate_face_block(block, 0, -1);
        break;
    case 1:
        rotate_face_block(block, 0, -2);
        break;
    case 2:
        rotate_face_block(block, 0, 2);
        break;
    case 5:
    case 8:
        rotate_face_block(block, 0, 1);
        break;
    }
    switch (loc->block_segment)  New location 
    {
    case 0:
    case 2:
    case 8:
        loc->block_segment = 6;
        break;
    case 1:
    case 3:
    case 5:
        loc->block_segment = 7;
        break;
    }   */
    return 1;

}

int find_block(Room729 *room729, int num, Location *loc)
{
    /* Find a given Room in a Block and store it's Room729 location */

    int i, j, upper, lower, spacing;
    spacing = DIMENSION * DIMENSION * DIMENSION - 1;

    for(lower = 0; lower + spacing <= num; lower += spacing); /* Bounds of num to find Block */
    for(upper = spacing * spacing - 1; upper - spacing >= num; upper -= spacing);

    for(i = 0; i < DIMENSION; i++)
        for(j = 0; j < DIMENSION * DIMENSION; j++)
        {
            num = room729->blocks[i][j]->rooms[0][0]->number;
            if (num >= lower && num <= upper)
            {
                loc->room729_face = i;
                loc->room729_segment = j;
                return 1;
            }
        }
    return 0;
}

int focus_block(Room729 *room729, int num, Location *loc)
{
    /* Put a Block at the bottom-most of the front face */

    move_ladder(room729, loc, 0); /* Remove ladder */
    if (!find_block(room729, num, loc))
        return 0;

    if(loc->room729_face == 2) /* Move to front face and mirror segment */
    {
        change_perspective_room729(room729, 1);
        change_perspective_room729(room729, 1);
        loc->room729_face = 0;
        loc->room729_segment = loc->room729_segment - 
        2 * (loc->room729_segment % 3) + 2; /* Spacial change */
    }
    else if(loc->room729_face == 1)/* Move to front face and change segment */
    {
        switch (loc->room729_segment)
        {
        case 0:
        case 1:
        case 2:
            change_perspective_room729(room729, 0);
            loc->room729_face = 0;
            loc->room729_segment = loc->room729_segment + 3;
            break;
        case 3:
            change_perspective_room729(room729, 3);
            loc->room729_face = 0;
            loc->room729_segment = loc->room729_segment + 1;
            break;      
        case 4:
            return 0;
        case 5:
            change_perspective_room729(room729, 1);
            loc->room729_face = 0;
            loc->room729_segment = loc->room729_segment - 1;
            break;   
        case 6:
        case 7:
        case 8:
            change_perspective_room729(room729, 2);
            loc->room729_face = 0;
            loc->room729_segment = loc->room729_segment - 3;
            break;            
        }
    }

    /* Modify room order to bottom most */

    switch (loc->room729_segment)
    {
    case 0:
    case 3:
        rotate_face_room729(room729, 0, -1);
        break;
    case 1:
        rotate_face_room729(room729, 0, -2);
        break;
    case 2:
    case 5:
        rotate_face_room729(room729, 0, 1);
        break;
    /*case 5:
    case 8:
        rotate_face_room729(room729, 0, 1);
        break;*/
    }
    switch (loc->room729_segment) /* New location */
    {
    case 0:
        loc->room729_segment = 6;
        break;
    case 1:
    case 3:
    case 5:
        loc->room729_segment = 7;
        break;
    case 2:
        loc->room729_segment = 8;
        break;

    }   
    /*printf("BBB  %d %d\n", loc->room729_face, loc->room729_segment);*/

    focus_room(room729->blocks[loc->room729_face][loc->room729_segment], num, loc);
    move_ladder(room729, loc, 1);
    return 1;

}

void move_ladder(Room729 *room729, Location *loc, int move)
{
    /* Set ladder of Rooms below loc */

    int i, j, k;

    if(loc->room729_face == -1) return; /* No ladder */

    for(i = loc->room729_segment; i < DIMENSION * DIMENSION; i += DIMENSION)
    {
        if (i == loc->room729_segment) k = loc->block_segment + DIMENSION;
        /* Room in same Block */
        else k = loc->block_segment % DIMENSION; /* Different Block */
        for(j = k; j < DIMENSION * DIMENSION; j += DIMENSION)
            room729->blocks[loc->room729_face][i]->rooms[loc->block_face][j]->
            ladder = move;
    }

}

int enter_leave(Room729 *room729, Location *loc, int intent)
{
    /* Enter or leave a Room */

    if(loc->room729_face == -1) return 2; /* Room is not in focus */

    if(room729->blocks[loc->room729_face][loc->room729_segment]->
    rooms[loc->block_face][loc->block_segment]->occupied == intent) /* Full or empty */
    return 0;

    room729->blocks[loc->room729_face][loc->room729_segment]->
    rooms[loc->block_face][loc->block_segment]->occupied = intent;
    return 1;
}

void message(Room729 *room729, Location *loc, int type)
{
    /* Greet with name */

    if (type == 0) printf("Hello ");
    else printf("Goodbye ");

    printf("Agent %d, %s\n", 
    room729->blocks[loc->room729_face][loc->room729_segment]->
    rooms[loc->block_face][loc->block_segment]->id,
    room729->blocks[loc->room729_face][loc->room729_segment]->
    rooms[loc->block_face][loc->block_segment]->name);
}

void make_id(int *id, int n, int m)
{
    /* Generate list of n ints from m */

    int i, j, t;

    for(i = 0; i < n; i++)
        id[i] = m + i;

    for(i = 0; i < n; i++)
    {
        j = rand() % n;
        t = id[i];
        id[i] = id[j];
        id[j] = t;
    }
}

void makelist()
{
    /* Create file of agent room #, id, and name */

    int i, j, first, last, count, len;
    FILE *output = fopen("names.csv", "w");
    char charset [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *s = (char*)malloc(30 * sizeof(char));
    int *id = (int*)malloc(26 * 26 * sizeof(int));
    make_id(id, 26 * 26, 400);

    for(i = 0; i < 26*26; i++) /* Name from random letters */
    {
        first = rand() % 6 + 3;
        last = rand() % 7 + 5;
        len = 0;

        for(j = 0; j < first; j++)
        {
            count = rand() % 26;
            s[len] = charset[count];
            len++;
        }
        s[len] = ' ';
        len++;

        for(j = 0; j < last; j++)
        {
            count = rand() % 26;
            s[len] = charset[count];
            len++;
        }
        s[len] = '\0';

        fprintf(output, "%d,%d,%s\n", i,id[i],s);
    }

    free(s);
    free(id);
    fclose(output);

}

int main()
{
    srand(time(0));
    Location *loc = (Location*)malloc(sizeof(Location));
    set_loc(loc);
    Room729 *room729 = init_room729("names.csv");
    char s[BUFFER];
    int ans;

    print_room729(room729, loc);
    printf("Welcome bureaucrat.\nFocus room: 0-675\nEnter: +\nExit: -\nQuit: q\nOptions: *\n");

    for(;;)
    {
        scanf("%s", s);
        printf("\n");
        if (s[0] == 'q') break;
        else if (s[0] == '*') printf("Focus room: 0-675\nEnter: +\nExit: -\nQuit: q\nOptions: *\n");
        else if (s[0] == '+') 
        {
            ans = enter_leave(room729, loc, 1);
            if (ans == 2) printf("Room not in focus\n");
            else if (ans == 0) printf("Room already occupied\n");
            else message(room729, loc, 0);
        }
        else if (s[0] == '-') 
        {
            ans = enter_leave(room729, loc, 0);
            if (ans == 2) printf("Room not in focus\n");
            else if (ans == 0) printf("Room empty\n");
            else message(room729, loc, 1);

        }
        else if (s[0] >= '0' && s[0] <= '9')
        {
            ans = focus_block(room729, atoi(s), loc);
            if (ans == 0) printf("Room not found\n");
            else printf("Room %d in focus\n", atoi(s));
        }
        else
            printf("Command not found\n");
        
        print_room729(room729, loc);
    }

    printf("Farewell...\n");   

    free_room729(room729);
    free(loc);
    return 0;
}