#include <ncurses.h>
#include <stdio.h>

void printing();
void moving_and_sleeping();
void colouring();

int pos[2] = {0, 0};
chtype currPos = '#';
int size = 20;
int bombs = 99;
int q;

void save(){
    move(pos[0], pos[1]);
    currPos = mvinch(pos[0], pos[1]);
}

void mark(){
    chtype cc = '!';
    chtype empty = '#';
    chtype back = mvinch(pos[0], pos[1]);
    if (back == cc){
        addch(empty);
    }else if (back == empty){
        addch(cc);
    }
    save();
}

void reset_pos(){
    move(pos[0], pos[1]);
    addch(currPos);
    save();
}

void move_left(){
    reset_pos();
    if (pos[1]>0){
        pos[1]--;
    }
    save();
}

void move_right(){
    reset_pos();
    if (pos[1]<size*2.5-1){
        pos[1]++;
    }
    save();
}

void move_up(){
    reset_pos();
    if (pos[0]>0){
        pos[0]--;
    }
    save();
}

void move_down(){
    reset_pos();
    if (pos[0]<size-1){
        pos[0]++;
    }
    save();
}

int main(){
    initscr();
    addstr("Welcome to Minesweeper\nEnter the size for board\n");
    clear();
    refresh();
        for (int y = 0; y<size; y++)
    {
        for (int x = 0;x<size*2.5;x++)
        {
            addstr("#");
        }
        addstr("\n");
        refresh();
    }
    refresh();
    move(pos[0], pos[1]);
    char str[3];
    while(q != 113 && q != 27)
    {
        q = getch();
        sprintf(str, "%d", q);
        if (q == 10) {
            reset_pos();
        } else if (q == 32) {
            reset_pos();
            mark();
        } else if (q == 119){
            move_up();
        } else if (q == 97){
            move_left();
        } else if (q == 115){
            move_down();
        } else if (q == 100){
            move_right();
        } else{
            reset_pos();
        }
        refresh();
   }
    endwin();
    return 0;
}
