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
int width;
int height;
int board[100][100];

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

// Fix for Tab clearnig part of screen
void clear_tab(){
    int i = 0;
    chtype clearChar = ' ';
    while(mvinch(pos[0], pos[1]+i) == clearChar){
        move(pos[0], pos[1]+i);
        addch('#');
        i++;
    }
    move(pos[0], pos[1]);
    refresh();
}

bool check_aval(int aval[2]){
    if (aval[0]<0) return false;
    if (aval[1]<0) return false;
    if (aval[0]>height-1)return false;
    if (aval[1]>width-1)return false;
    return true;
}

int check_around(int pos[2], int brd[height][width]){
    if (board[pos[0]][pos[1]] != -2) return board[pos[0]][pos[1]];
    if (brd[pos[0]][pos[1]] == -1) return -1;
    int count = 0;
    for (int row = pos[0]-1; row<pos[0]+2; row++){
        for (int col = pos[1]-1; col<pos[1]+2; col++){
            int ww[2] = {row, col};
            if (check_aval(ww)){
                if (brd[row][col] == -1) count++;
            }
        }
    }
    return count;
}

void reveal(int pos[2]){
    int ll[height][width];
     
}

void check(){
    // Initializing empty array, doesn't matter
    int ll[height][width];
    int count = check_around(pos, ll);
    char str[2];
    sprintf(str, "%d", count);
    move(pos[0], pos[1]);
    if (count == -1){
        addstr("w");
    }else if (count == 0){
        reveal(pos);
    }
    else{
        addstr(str);
    }
    save();
}

void clear_board(int brd[height][width]){
    for (int y = 0; y<height; y++){
        for(int x = 0; x<width; x++){
            int wPos[2] = {y, x};
            board[y][x] = check_around(wPos, brd); 
        }
    } 
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

void start_screen(){
    addstr(" __  __ ___ _   _ _____ ______        _______ _____ ____  _____ ____  \n");
    addstr("|  \\/  |_ _| \\ | | ____/ ___\\ \\      / / ____| ____|  _ \\| ____|  _ \\  _/\\_  \n");
    addstr("| |\\/| || ||  \\| |  _| \\___ \\\\ \\ /\\ / /|  _| |  _| | |_) |  _| | |_) | \\!!/\n");
    addstr("| |  | || || |\\  | |___ ___) |\\ V  V / | |___| |___|  __/| |___|  _ <  /__\\\n");
    addstr("|_|  |_|___|_| \\_|_____|____/  \\_/\\_/  |_____|_____|_|   |_____|_| \\_\\\n");
    addstr("Press any key to start...");
    refresh();
    cbreak();
    getch();
    clear();
}

void init_board(){
    for (int y = 0; y<100; y++){
        for (int x = 0; x<100; x++){
            board[y][x] = -2;
        }
    }
}

int main(){
    width = size*2.5;
    height = size;
    int board[height][width];
    init_board();
    initscr();
    start_screen();
    refresh();
        for (int y = 0; y<height; y++)
    {
        for (int x = 0;x<width;x++)
        {
            addstr("#");
            board[y][x] = -2;
        }
        addstr("\n");
        refresh();
    }
    board[0][0] = -1;
    clear_board(board);
    refresh();
    move(pos[0], pos[1]);
    char str[3];
    while(q != 113 && q != 27)
    {
        q = getch();
        sprintf(str, "%d", q);
        if (q == 10) {
            check();
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
        } else if (q == 9){
            clear_tab();
        } else{
            reset_pos();
        }
        refresh();
   }
    endwin();
    return 0;
}
