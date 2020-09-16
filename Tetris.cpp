#include <iostream>
#include <cmath>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <chrono>
using namespace std;


const int height=4,width=6;
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;//22;
    
//set cursor at start to avoid flickering
void gotoxy ( short x, short y )
{
	COORD coord = {x, y};
	SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}

enum Tetrominoe
{
	NoShape, ZShape, SShape, LineShape,
	TShape, SquareShape, LShape, MirroredLShape
};



struct Shape {
    Tetrominoe piece_shape;
    int coords[4][2];

    Shape() {
        set_shape(NoShape);
    }

    void set_shape(Tetrominoe shape) {

        int coords_table[][4][2] =
		{
            {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
            {{0, -1}, {0, 0}, {-1, 0}, {-1, 1}},
            {{0, -1}, {0, 0}, {1, 0}, {1, 1}},
            {{0, -1}, {0, 0}, {0, 1}, {0, 2}},
            {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{-1, -1}, {0, -1}, {0, 0}, {0, 1}},
            {{1, -1}, {0, -1}, {0, 0}, {0, 1}}
        };

        for(int i=0;i<4;i++)
		{
            coords[i][0]=coords_table[shape][i][0];
            coords[i][1]=coords_table[shape][i][1];
        }

        piece_shape = shape;
    }

    void set_x(int index, int x) {

        coords[index][0] = x;
    }

    void set_y(int index, int y) {

        coords[index][1] = y;
    }

    int x(int index) {

        return coords[index][0];
    }

    int y(int index) {

        return coords[index][1];
    }

    Tetrominoe get_shape() {

        return piece_shape;
    }

    void set_random_shape()
	{
        int x = rand()%7+1;
        Tetrominoe val = static_cast<Tetrominoe>(x);
        set_shape(val);
    }
    void set_random_shape(Tetrominoe y)
	{
		Tetrominoe val;
		do{
        int x = rand()%7+1;
        val = static_cast<Tetrominoe>(x);	
		}
        while(val==y);
        set_shape(val);
    }

    int min_x() {

        int m = coords[0][0];

        for (int i = 0; i < 4; i++) {

            m = min(m, coords[i][0]);
        }

        return m;
    }


    int min_y() {

        int m = coords[0][1];

        for (int i = 0; i < 4; i++) {

            m = min(m, coords[i][1]);
        }

        return m;
    }

    Shape rotate_left() {

        if (piece_shape == SquareShape) {

            return *this;
        }

        Shape result;
        result.piece_shape = piece_shape;

        for (int i = 0; i < 4; i++) {

            result.set_x(i, y(i));
            result.set_y(i,-x(i));
        }

        return result;
    }

    Shape rotate_right() {

        if (piece_shape == SquareShape) {

            return *this;
        }

        Shape result;
        result.piece_shape = piece_shape;

        for (int i=0;i<4;i++)
		{
            result.set_x(i,-y(i));
            result.set_y(i, x(i));
        }

        return result;
    }
};

struct Board{
    int PERIOD_INTERVAL = 300;
    
    bool is_falling_finished = false;
	bool is_paused = false;
	int num_lines_removed = 0;
	int cur_x = 0;
	int cur_y = 0;
	
	Shape cur_piece;
    Tetrominoe *board=new Tetrominoe[BOARD_WIDTH * BOARD_HEIGHT];
	
	Tetrominoe shape_at(int x, int y)
	{
    	return board[(y * BOARD_WIDTH) + x];
	}
	
	void start()
	{

        clear_board();
        new_piece();
    }

    void pause()
	{

        is_paused = !is_paused;
    }
	
	void dropDown()
	{

        int new_y = cur_y;

        while (new_y > 0) {

            if (!try_move(cur_piece, cur_x, new_y - 1)) {

                break;
            }

            new_y--;
        }

        piece_dropped();
    }
    
    char platno[height*BOARD_HEIGHT][width*BOARD_WIDTH+1];
    
    void draw_square(int y,int x,Tetrominoe shape)
	{
		
		char squares[8][height][width+1]=
		{
			{
			"      ",
			"      ",
			"      ",
			"      "
			},
			{
			"*O*O*O",
			"O*O*O*",
			"*O*O*O",
			"O*O*O*"
			},
			{
			"OOOOOO",
			"OOOOOO",
			"OOOOOO",
			"OOOOOO"
			},
			{
			"/====\\",
			"||  ||",
			"||  ||",
			"\\====/"
			},
			{
			"MWMWMW",
			"MWMWMW",
			"MWMWMW",
			"MWMWMW"
			},
			{
			"O^^^^O",
			"<    >",
			"<    >",
			"OvvvvO"
			},
			{
			"@//@//",
			"//@//@",
			"/@//@/",
			"@//@//"
			},
			{
			"oooooo",
			"oo..oo",
			"oo..oo",
			"oooooo"
			}
		};
		
		int index=(int)shape;
		
		for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			platno[height*y+i][width*x+j]=squares[index][i][j];
		}}
	}
    
    void draw()
    {
    	for(int i=0;i<BOARD_HEIGHT;i++)
    	{
    		for(int j=0;j<BOARD_WIDTH;j++)
    		{
    			Tetrominoe shape=shape_at(j,BOARD_HEIGHT-1-i);
    			draw_square(i,j,shape);
			}
			for(int j=0;j<height;j++)	platno[height*i+j][width*BOARD_WIDTH]='\n';
		}
		platno[height*BOARD_HEIGHT-1][width*BOARD_WIDTH]='\0';
		
		if (cur_piece.get_shape() != NoShape) {

            for (int i = 0; i < 4; i++) {

                int x = cur_x + cur_piece.x(i);
                int y = cur_y - cur_piece.y(i);
                draw_square(BOARD_HEIGHT-1-y,x,cur_piece.get_shape());
            }
        }
        char platno2[height*(BOARD_HEIGHT+2)][width*(BOARD_WIDTH+2)+1];
        //code:
        
        for(int i=0;i<height*(BOARD_HEIGHT+2);i++)
        	platno2[i][width*(BOARD_WIDTH+2)]='\n';
        platno2[height*(BOARD_HEIGHT+2)-1][width*(BOARD_WIDTH+2)]='\0';
        
        for(int i=0;i<height*BOARD_HEIGHT;i++){
        for(int j=0;j<width*BOARD_WIDTH ;j++){
        	platno2[i+height][j+width]=platno[i][j];
		}}
		
		for(int j=0;j<width*(BOARD_WIDTH+2);j++)
		{
			for(int i=0;i<height;i++)
			{
				if((i+j)%2)
					platno2[i][j]='o';
				else
					platno2[i][j]='.';
				if(((BOARD_HEIGHT+1)*height+i+j)%2)
					platno2[(BOARD_HEIGHT+1)*height+i][j]='o';
				else
					platno2[(BOARD_HEIGHT+1)*height+i][j]='.';
			}
		}
		
		for(int i=height;i<height*(BOARD_HEIGHT+1);i++)
		{
			for(int j=0;j<width;j++)
			{
				if((i+j)%2)
					platno2[i][j]='o';
				else
					platno2[i][j]='.';
				if(((BOARD_WIDTH+1)*width+j+i)%2)
					platno2[i][(BOARD_WIDTH+1)*width+j]='o';
				else
					platno2[i][(BOARD_WIDTH+1)*width+j]='.';
			}
		}
        
        if(is_paused)
        {
        	char paused[9][45]=
        	{
    		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    		"@  _____       _    _  _____ ______ _____  @",
			"@ |  __ \\ /\\  | |  | |/ ____|  ____|  __ \\ @",
			"@ | |__) /  \\ | |  | | (___ | |__  | |  | |@",
			"@ |  ___/ /\\ \\| |  | |\\___ \\|  __| | |  | |@",
			"@ | |  / ____ \\ |__| |____) | |____| |__| |@",
			"@ |_| /_/    \\_\\____/|_____/|______|_____/ @",
			"@                                          @",
    		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
			};
			char game_over[9][69]=
			{
			"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
			"@    _____          __  __ ______    ______      ________ _____    @",
			"@   / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\   @",
			"@  | |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |  @",
			"@  | | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  /   @",
			"@  | |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\   @",
			"@   \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\  @",
			"@                                                                  @",
			"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
			};
			if(cur_piece.get_shape()!=NoShape)
			{
				for(int i=0;i<9;i++){
				for(int j=0;j<44;j++){
					platno2[i+8*height][j+14]=paused[i][j];
				}}
			}
			else
			{
				for(int i=0;i<9;i++){
				for(int j=0;j<68;j++){
					platno2[i+8*height][j+2]=game_over[i][j];
				}}
			}
		}
        
        puts(platno2[0]);
        //delete:
        for(int i=0;i<(height+2)*BOARD_HEIGHT;i++)
        	delete[] platno2[i];
        delete[] platno2;
	}
	
	
    
    void one_line_down() {

        if (!try_move(cur_piece, cur_x, cur_y - 1)) {

            piece_dropped();
        }
    }
    
    void clear_board() {

        for (int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++) {

            board[i] = NoShape;
        }
    }
    
    void piece_dropped() {

        for (int i = 0; i < 4; i++) {

            int x = cur_x + cur_piece.x(i);
            int y = cur_y - cur_piece.y(i);
            board[(y * BOARD_WIDTH) + x] = cur_piece.get_shape();
        }

        remove_full_lines();

        if (!is_falling_finished) {

            new_piece();
        }
    }
    
    void new_piece() {

        cur_piece.set_random_shape(cur_piece.get_shape());
        cur_x = BOARD_WIDTH / 2 + 1;
        cur_y = BOARD_HEIGHT - 1 + cur_piece.min_y();

        if (!try_move(cur_piece, cur_x, cur_y)) {
        	//game is over
        	
            cur_piece.set_shape(NoShape);
            pause();
            
        }
    }
    
    boolean try_move(Shape new_piece, int new_x, int new_y) {

        for (int i = 0; i < 4; i++) {

            int x = new_x + new_piece.x(i);
            int y = new_y - new_piece.y(i);

            if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {

                return false;
            }

            if (shape_at(x, y) != NoShape) {

                return false;
            }
        }

        cur_piece = new_piece;
        cur_x = new_x;
        cur_y = new_y;

        return true;
    }
    
    void remove_full_lines() {

        int num_full_lines = 0;

        for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {

            boolean line_is_full = true;

            for (int j = 0; j < BOARD_WIDTH; j++) {

                if (shape_at(j, i) == NoShape) {

                    line_is_full = false;
                    break;
                }
            }

            if (line_is_full) {

                num_full_lines++;

                for (int k = i; k < BOARD_HEIGHT - 1; k++) {
                    for (int j = 0; j < BOARD_WIDTH; j++) {
                        board[(k * BOARD_WIDTH) + j] = shape_at(j, k + 1);
                    }
                }
            }
        }

        if (num_full_lines > 0) {

            num_lines_removed += num_full_lines;
            is_falling_finished = true;
            cur_piece.set_shape(NoShape);
        }
        
    }
    
    void update() {
    	if(num_lines_removed==10)
    	{
    		PERIOD_INTERVAL=200;
		}
    	if(num_lines_removed==15)
    	{
    		PERIOD_INTERVAL=150;
		}
    	if(num_lines_removed==10)
    	{
    		PERIOD_INTERVAL=100;
		}

        if (is_paused) {
            return;
        }

        if (is_falling_finished) {

            is_falling_finished = false;
            new_piece();
        } else {

            one_line_down();
        }
    }
    
    void key_pressed(char e) {
	    if (cur_piece.get_shape() == NoShape) {
	
	        return;
	    }
	    
	    switch (e) {
	    	case 112:				//p
	    		pause();
	    	break;
	    	case 75:			//left
	    		try_move(cur_piece, cur_x - 1, cur_y);
	    	break;
	    	case 77:			//right
	    		try_move(cur_piece, cur_x + 1, cur_y);
	    	break;
	    	case 80:			//down
	    		try_move(cur_piece.rotate_right(), cur_x, cur_y);
	    	break;
	    	case 72:			//up
	    		try_move(cur_piece.rotate_left(), cur_x, cur_y);
	    	break;
	    	case 32:			//space
	    		dropDown();
	    	break;
	    	case 100:				//d
	    		one_line_down();
	    	break;
	    	default:
	    		return;
	    	break;
	    }
	}
};

int main()
{
	
	srand (time(NULL));
	
	int time_variable;
	
	clock_t start=clock(),finish;
	char c;
	//int check;
	Board board;
	board.start();
	while(1)
	{
		gotoxy(0,0);
		
		finish=clock();
		time_variable=(int)(1000*(finish-start))/CLOCKS_PER_SEC;
		if(_kbhit())
		{
			c=getch();
			board.key_pressed(c);
		}
		board.draw();
		
		if(time_variable>board.PERIOD_INTERVAL)
		{
			start=clock();
			board.update();
			//board.one_line_down();
		}
	}
	return 0;
}

