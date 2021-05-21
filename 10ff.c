#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CTRL_BACKSPACE 0x08
#define ENTER 0x0a
#define SPACE ' '
#define LSIZ 32
#define RSIZ 256

void printwords(char (*words)[LSIZ],int first_index, int last_index, int row, int col){
    move(row, col);
    for(int i = first_index; i < last_index; i++){
        printw("%s ", words[i]);
    }
}

int main()
{
    srand(time(0));
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    char line[RSIZ][LSIZ];
    FILE *fptr = NULL;
    int i = 0;
    int total = 0;
    char words[RSIZ][LSIZ];

    fptr = fopen("words.txt", "r");
    while(fgets(line[i], LSIZ, fptr)){
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    total = i;
    for (i = 0; i < RSIZ; ++i){
        strncpy(words[i], line[rand() % (total)], LSIZ);
    }
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
    int current_row = mid_row+2;
    int current_col = mid_col;
    int start_col_input = current_col;
    int start_col_output = mid_col;
    int start_row_output = mid_row;

   //printwords(words, 0, , start_row_output,start_col_output);

    //printwords(words,last_index, getlastindex(words, mid_col-2, last_index), start_row_output+1,start_col_output);
   printwords(words, 0, 10, start_row_output, start_col_output);
   printwords(words, 10, 20, start_row_output+1, start_col_output);

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
