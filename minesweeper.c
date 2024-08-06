#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void printing();
void moving_and_sleeping();
void colouring();

#define LAND 10
#define OFF 11
#define MINE 12

int pos[2] = {0, 0};
chtype currPos = '#';
chtype buff[5] = {'#', '#', '#', '#', '#'};
int size = 20;
int bombs = 20;
int q;
int width;
int height;
int board[100][100];
int found = 0;
bool revealed = false;

void save(){
    currPos = mvinch(pos[0], pos[1]);
    for (int i = 1; i<6; i++){
        buff[i-1] = mvinch(pos[0], pos[1]+i);
    }
    move(pos[0], pos[1]);
}

void reset_pos(){
    move(pos[0], pos[1]);
    addch(currPos);
    save();
}

// Fix for Tab cleaning part of screen
void clear_tab(){
    for (int i = 0; i<6; i++){
        move(pos[0], pos[1]+i);
        if(i == 0) addch(currPos);
        if(i != 0) addch(buff[i-1]);
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

void reveal_all(){
    revealed = true;
    for(int y = 0; y<height; y++){
        for (int x = 0; x<width; x++){
            move(y, x);
            if (board[y][x] == 0){
                if (has_colors()){
                    attron(COLOR_PAIR(OFF));
                    addstr(" ");
                    attroff(COLOR_PAIR(OFF));
                }else{
                    addstr(" ");
                }
            }else if(board[y][x] == -1){
                if (has_colors()){
                    attron(COLOR_PAIR(MINE));
                    addstr(" ");
                    attroff(COLOR_PAIR(MINE));
                }else{
                    addstr("!");
                }
            }else{
                if (has_colors()){
                    attron(COLOR_PAIR(LAND));
                    addstr(" ");
                    attroff(COLOR_PAIR(LAND));
                }else{
                    addstr("#");
                }
            }
        }
    }
    save();
}
void won(){
    move(height, 0);
    addstr("__   _____  _   _  __        _____  _   _ _ \n");
    addstr("\\ \\ / / _ \\| | | | \\ \\      / / _ \\| \\ | | |\n");
    addstr(" \\ V / | | | | | |  \\ \\ /\\ / / | | |  \\| | |\n");
    addstr("  | || |_| | |_| |   \\ V  V /| |_| | |\\  |_|\n");
    addstr("  |_| \\___/ \\___/     \\_/\\_/  \\___/|_| \\_(_)\n");
    addstr("Press q or escape to quit...");
    refresh();
}
void mark(){
    chtype cc = '!';
    chtype empty = '#';
    chtype back = mvinch(pos[0], pos[1]);
    if (back == cc){
        if (board[pos[0]][pos[1]] == -1) found--;
        addch(empty);
    }else if (back == empty){
        if (board[pos[0]][pos[1]] == -1) found++;
        addch(cc);
    }
    if (found == bombs){
        reveal_all();
        won();
    }
    save();
}

void init_colors(){
    if (has_colors() == false){
        addstr("Playing colorblind mode (no colors)\n");
    }else{
        start_color();
        for (int i = 1; i<9; i++){
            if(i<3){init_pair(i, COLOR_BLACK, COLOR_GREEN); continue;}
            if(i<5){init_pair(i, COLOR_BLACK, COLOR_YELLOW); continue;}
            if(i<9)init_pair(i, COLOR_WHITE, COLOR_RED);
        } 
        init_pair(LAND, COLOR_BLACK, COLOR_GREEN);
        init_pair(OFF, COLOR_BLACK, COLOR_BLUE);
        init_pair(MINE, COLOR_BLACK, COLOR_RED);
    }
}

void put_num(int num){
    char str[2];
    sprintf(str, "%d", num);
    if (has_colors()) attron(COLOR_PAIR(num));
    addstr(str);
    if (has_colors()) attroff(COLOR_PAIR(num));
}


bool zero_fix(){
    chtype empty =  '#';
    chtype zero = ' ';
    bool added_zeroes = false;
    for(int y = 0; y<height; y++){
        for(int x = 0; x<width; x++){
            if (mvinch(y, x) == zero){
                int conf_changes[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
                for(int i = 0; i<4; i++){
                    int new_pos[2] = {y+conf_changes[i][0], x+conf_changes[i][1]};
                    if(mvinch(new_pos[0], new_pos[1]) != empty) continue;
                    added_zeroes = true;
                    if (check_aval(new_pos)){
                        move(new_pos[0], new_pos[1]);
                        int to_wr = board[new_pos[0]][new_pos[1]];
                        if (to_wr == 0){
                            addstr(" ");
                        }else{
                            put_num(to_wr);
                        }
                    }
                }
            }
        }
    }
    return added_zeroes;
}

void lost(){
    reveal_all();
    move(height, 0);
    addstr("__   _____  _   _   _     ___  ____ _____      __\n");
    addstr("\\ \\ / / _ \\| | | | | |   / _ \\/ ___|_   _|  _ / /\n");
    addstr(" \\ V / | | | | | | | |  | | | \\___ \\ | |   (_) | \n");
    addstr("  | || |_| | |_| | | |__| |_| |___) || |    _| | \n");
    addstr("  |_| \\___/ \\___/  |_____\\___/|____/ |_|   (_) | \n");
    addstr("                                              \\_\\\n");
    addstr("Press q or escape to quit...");
    refresh();
}

void check(){
    if (revealed != true){
        int count = board[pos[0]][pos[1]];
        char str[2];
        sprintf(str, "%d", count);
        move(pos[0], pos[1]);
        if (count == -1){
            lost();
        }else if (count == 0){
            addstr(" ");
            while(zero_fix()) continue;
        }
        else{
            put_num(count);
        }
        save();
    }
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
    init_colors();
    addstr("=====================================================\n");
    addstr("Instructions:\n");
    addstr("Use the W, A, S, D or the arrow keys to move.\n");
    addstr("To mark a bomb press the Space key.\n");
    addstr("To reveal a square use the Enter key.\n");
    addstr("Use Q or press Esc twice to quit.\n");
    addstr("=====================================================\n");
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

int main(int argc, char *argv[]){
    if (argc == 1) {
        printf("Enter the size of map\nDefault=(%i):", size);
        scanf("%i", &size);
        printf("Enter the amount of mines\nDefault=(%i/%i):", bombs, (int)(size*size*2.5));
        scanf("%i", &bombs);
    }
    if (argc > 4){
        printf("Too many arguments given\n");
        return 0;
    }
    if (argc == 2 && strcmp(argv[1], "h") == 0){
        printf("Help Menu\n");
        printf("\n");
        printf("Syntax\n");
        printf("\n");
        printf("./minesweeper <size>                    //Defaults bomb count 10%% of size\n");
        printf("./minesweeper <size> <bomb>             //Defaults width to 250%% of height\n");
        printf("./minesweeper <width> <height> <bomb>   //Doesn't assume anything\n");
        printf("\n");
        return 0;
    }
    if (argc == 2 || argc == 3) size = atoi(argv[1]);
    srand(time(0));
    width = size*2.5;
    height = size;
    if (argc != 1) bombs = (int)(width*height) / 10;
    if (argc == 3) bombs = atoi(argv[2]);
    if (argc == 4) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        bombs = atoi(argv[3]);
    }
    int board[height][width];
    init_board();
    initscr();
    start_screen();
    refresh();
    for (int y = 0; y<height; y++){
        for (int x = 0;x<width;x++)
        {
            addstr("#");
            board[y][x] = -2;
        }
        addstr("\n");
        refresh();
    }
    for (int i = 0; i<bombs; i++){
        int posY = rand()%height;
        int posX = rand()%width;
        while (board[posY][posX] == -1){
            posY = rand()%height;
            posX = rand()%width;
        }
        board[rand()%height][rand()%width] = -1;
    }
    clear_board(board);
    refresh();
    move(pos[0], pos[1]);
    char str[3];
    
    int cmds[3] = {0, 0, 0};
    while(q != 113)
    {
        q = getch();
        cmds[2] = cmds[1];
        cmds[1] = cmds[0];
        cmds[0] = q;
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
        }else{
            clear_tab();
            reset_pos();
        }
        if (cmds[1] == 0x5B && cmds[2] == 0x1B){
            if (cmds[0] == 0x42){
                move_down();
            }else if(cmds[0] == 0x44){
                move_left();
            }else if(cmds[0] == 0x43){
                move_right();
            }else if(cmds[0] == 0x41){
                move_up();
            }
        }
        if(cmds[1] == 0x1B && cmds[0] != 0x5B){
            break;
        }
        refresh();
   }
    endwin();
    return 0;
}
