#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOX_HEIGHT 4
#define CTRL_BACKSPACE 0x08
#define ENTER 0x0a
#define SPACE ' '
#define LSIZ 32
#define RSIZ 256

void drawbox(int,int,int);
int print_ran(int,int);

void printwords(char (*words)[LSIZ],int first_index, int last_index, int row, int col){
    //mvprintw(0, 0, "%d ", last_index);
    move(row, col);
    for(int i = first_index; i < last_index; i++){
        printw("%s ", words[i]);
    }
}

int getlastindex(char (*words)[LSIZ], int box_len, int first_index){

    int ch_count = 0;

    while(ch_count < box_len){
        ch_count += strlen(words[first_index]) + 1;
        first_index++;
    }
    return first_index-1;
}

int main()
{
    srand(time(0));
    initscr();
    if(has_colors() == FALSE){
        endwin();
        printf("Your terminal does not support color\n");
        return 1;
    }

    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    char line[RSIZ][LSIZ];
    FILE *fptr = NULL;
    int i = 0;
    int total = 0;
    char words[256][LSIZ];

    fptr = fopen("words.txt", "r");
    while(fgets(line[i], LSIZ, fptr)){
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    total = i;
    for (i = 0; i < 256; ++i){
        strncpy(words[i], line[rand() % (total)], LSIZ);
    }
    char c;
    //for (i = 0; i < 256; ++i) {
    //    printf("%s\n\n", words[i]);
    //}

    //printf("%s", words[255]);
    //printf("\n%lu", strlen(words[255]));
    //printf("\n%lu", sizeof(words[255]));

    char typed[LSIZ];
    int max_row, max_col, ch;
    int typed_index = 0;
    getmaxyx(stdscr,max_row,max_col);
    int mid_row = max_row/2;
    int mid_col = max_col/2;
    int start_col_box = mid_col/2;
    int end_col_box = max_col-start_col_box;
    int current_row = mid_row+BOX_HEIGHT+2;
    int current_col = mid_col;
    int start_col_input = current_col;
    int start_col_output = start_col_box + 1;
    int start_row_output = mid_row + 1;

    drawbox(mid_row, start_col_box, end_col_box);

    int last_index = getlastindex(words, mid_col-2, 0);
    printwords(words, 0, last_index, start_row_output,start_col_output);

    printwords(words,last_index, getlastindex(words, mid_col-2, last_index), start_row_output+1,start_col_output);

    move(current_row,current_col);
    refresh();

    //mvprintw(mid_row, (max_col-strlen(words))/2,"%s",words);
    while(1){
        switch ((ch = getch())) {
            case KEY_BACKSPACE:
                getyx(stdscr, current_row, current_col);
                if(current_col == mid_col)
                    break;
                mvdelch(current_row, --current_col);
                typed_index--;
                break;
            case ENTER:
            case SPACE:
                move(current_row,start_col_input);
                clrtoeol();
                break;
            case CTRL_BACKSPACE:
                move(current_row,start_col_input);
                clrtoeol();
                break;
            default:
                typed[typed_index] = ch;
                addch(ch);
                typed_index++;
        }
        refresh();
        if(typed_index == 255)
            break;
        //if time 60 seconds break
    }

    endwin();
    return 0;
}



void drawbox(int row, int start_col, int end_col){

    mvaddch(row, start_col, ACS_ULCORNER);

    for(int i = 1; i < end_col-start_col; i++){
        mvaddch(row, start_col+i, ACS_HLINE);
        mvaddch(row+BOX_HEIGHT, start_col+i, ACS_HLINE);
    }

    mvaddch(row, end_col, ACS_URCORNER);

    for(int i = 1; i < BOX_HEIGHT; i++){
        mvaddch(row+i, start_col, ACS_VLINE);
        mvaddch(row+i, end_col, ACS_VLINE);
    }

    mvaddch(row+BOX_HEIGHT, start_col, ACS_LLCORNER);
    mvaddch(row+BOX_HEIGHT, end_col, ACS_LRCORNER);
}
