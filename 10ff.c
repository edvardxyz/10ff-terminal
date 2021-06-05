#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#define BOX_HEIGHT 4
#define CTRL_BACKSPACE 0x08
#define ENTER 0x0a
#define SPACE ' '
#define LSIZ 32
#define RSIZ 300

_Bool typing = 1;

int getlastindex(char (*words)[LSIZ], int box_len, int first_index){
    int ch_count = 0;
    while(ch_count < box_len){
        ch_count += strlen(words[first_index]) + 1;
        first_index++;
    }
    return --first_index;
}

int printwords(char (*words)[LSIZ], WINDOW *words_win, int first_index, int typedwidx, int colorpair){
    int col = COLS/2-4;
    int last_index = getlastindex(words, col, first_index);

    wmove(words_win, 1, 2);
    for(;first_index < last_index; first_index++){
        if(first_index == typedwidx){
            wattron(words_win, COLOR_PAIR(colorpair));
            wprintw(words_win,"%s", words[first_index]);
            wattroff(words_win, COLOR_PAIR(colorpair));
            wprintw(words_win," ");
        }else{
            wprintw(words_win,"%s ", words[first_index]);
        }
    }

    int x_pos = getcurx(words_win);
    for(int i = 0; i < col - x_pos; i++){
        waddch(words_win, ' ');
    }

    int return_index = first_index;

    wmove(words_win, 2, 2);
    last_index = getlastindex(words, col, last_index);
    for(;first_index < last_index; first_index++){
        wprintw(words_win,"%s ", words[first_index]);
    }

    x_pos = getcurx(words_win);
    for(int i = 0; i < col - x_pos; i++){
        waddch(words_win, ' ');
    }

    wrefresh(words_win);

    return return_index;
}

_Bool typedcorrect(char (*words)[LSIZ], char typedWord[], int typedwidx, int typedchidx){
    for(int i = 0; i <= typedchidx; i++){
        if(words[typedwidx][i] != typedWord[i]){
            return 0;
        }
    }
    return 1;
}

void sig_handler(int signum){
    typing = 0;
}

int main()
{
    signal(SIGALRM, sig_handler);
    WINDOW *words_win;
    srand(time(0));
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);

    _Bool running = 1;
    char line[RSIZ][LSIZ];
    FILE *fptr = fopen("words.txt", "r");
    int i = 0;
    int total = 0;
    char words[RSIZ][LSIZ];
    char typedWord[LSIZ];
    int ch;
    int starty_win = (LINES - BOX_HEIGHT) / 2;
    int startx_win = (COLS - COLS/2) / 2;
    int starty_type = LINES/2+BOX_HEIGHT/2;
    int startx_type = COLS/2;
    int current_row;
    int current_col;
    words_win = newwin(BOX_HEIGHT, COLS/2, starty_win, startx_win);
    wborder(words_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

    while(fgets(line[i], LSIZ, fptr)){
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    total = i;

    // restart here
    while(running){
        typing = 1;
        for (i = 0; i < RSIZ; ++i){
            strncpy(words[i], line[rand() % (total)], LSIZ);
        }
        int correctwords[RSIZ] = {0};
        int cwidx = 0;
        int typedwidx = 0;
        int typedchidx = 0;
        int prntwidxtmp = 0;
        int prntwidx = printwords(words, words_win, 0, typedwidx, 2);
        _Bool firstflag = 1;
        int totalch = 0;

        move(starty_type, startx_type);

        while(typing){
            refresh();
            switch ((ch = getch())) {
                case KEY_BACKSPACE:
                    getyx(stdscr, current_row, current_col);
                    if(current_col == startx_type)
                        break;
                    mvdelch(current_row, --current_col);
                    typedchidx--;
                    break;
                case ENTER:
                case SPACE:
                    if(typedwidx == RSIZ-30)
                        break;
                    if(getcurx(stdscr) == startx_type)
                        break;
                    move(starty_type, startx_type);
                    clrtoeol();
                    typedWord[typedchidx] = '\0';
                    if(typedcorrect(words, typedWord, typedwidx, typedchidx-1)){
                        correctwords[cwidx++] = typedwidx;
                    }
                    typedwidx++;
                    typedchidx = 0;
                    if(prntwidx == typedwidx){
                        prntwidxtmp = prntwidx;
                        prntwidx = printwords(words, words_win, prntwidx, typedwidx, 2);
                    }
                    break;
                case CTRL_BACKSPACE:
                    move(starty_type, startx_type);
                    clrtoeol();
                    typedchidx = 0;
                    break;
                default:
                    if(typedchidx == 31)
                        continue;
                    if(firstflag){
                        alarm(60);
                        firstflag = 0;
                    }
                    typedWord[typedchidx] = ch;
                    addch(ch);
                    typedchidx++;
            }
            if(!typedcorrect(words, typedWord, typedwidx, typedchidx-1))
                printwords(words, words_win, prntwidxtmp, typedwidx, 1);
            else
                printwords(words, words_win, prntwidxtmp, typedwidx, 2);

        }
        for(i = 0; i < cwidx; i++){
            totalch += strlen(words[correctwords[i]]) + 1;
        }

        mvprintw(starty_type+2,startx_type-10,"Typed %d words correct", cwidx);
        mvprintw(starty_type+3,startx_type-10,"Typed %d words wrong", typedwidx - cwidx);
        mvprintw(starty_type+4,startx_type-10,"WPM: %d ", totalch/5);
        mvprintw(starty_type+5,startx_type-10,"Enter to play again");

        while(ch != ENTER){
            ch = getch();
        }
        mvprintw(starty_type+5,startx_type-10,"                   ");
    }
    endwin();
    return 0;
}

