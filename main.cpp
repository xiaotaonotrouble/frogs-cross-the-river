#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#define ROW 10
#define COLUMN 50 

int flag = 1; //0:quit; 1:playing; 2:win; 3:go into river; 4:collide with bank
pthread_mutex_t mutex;
int thread_ids[9] = {0,1,2,3,4,5,6,7,8};
struct Node{
	int x , y; 
	Node( int _x , int _y ) : x( _x ) , y( _y ) {}; 
	Node(){} ; 
} frog ; 


char map[ROW+10][COLUMN] ; 

// Determine a keyboard is hit or not. If yes, return 1. If not, return 0. 
int kbhit(void){
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

void rendering() {
	printf("\033[H\033[2J"); // clear everything and set the cursor to up left
	for (int i = 0; i <= ROW; i++){
		for (int j = 0; j < COLUMN; j++){
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	
}


void *logs_move( void *t ){
	int row = *((int*)t) + 1;
	int length = rand()%6 + 6;
	// initialize the log from the left side
	for (int i = 0; i < length; i++)
		map[row][i] = '=';
	
	for (int j = length; j < COLUMN; j++)
		map[row][j] = ' ';
	/*  Move the logs  */
	char temp[COLUMN];
	int step = (row % 2 == 0)?1 : (COLUMN-1);
	while(flag == 1){
		
		pthread_mutex_lock(&mutex);

		for (int i=0; i<COLUMN; i++){
			temp[(i+step)%COLUMN] = map[row][i];
		}

		for (int i = 0; i<COLUMN; i++){
			map[row][i] = temp[i];
		}

		// update frog's position if frog is on this log
		if (frog.y == row){
			frog.x = (frog.x + step) % COLUMN;
		}

		if (frog.x == 0 || frog.x == COLUMN-1)
			flag = 4;
	
	// update screen
	rendering();

	/*  Check keyboard hits, to change frog's position or quit the game. */
	if(kbhit()){
		char hit = getchar();

		if (hit == 'q' || hit == 'Q') {
			flag = 0;
		}

		if (hit == 'w' || hit == 'W') {
			if (map[frog.y-1][frog.x] == '|'){
				flag = 2;
				map[frog.y-1][frog.x] = '0';
				map[frog.y][frog.x] = '=';
				frog.y--;
				rendering();
			}
			else if (map[frog.y-1][frog.x] == '=') {
				map[frog.y-1][frog.x] = '0';
				if (frog.y == 10){
					map[frog.y][frog.x] ='|';
				}
				else 
					map[frog.y][frog.x] ='=';
				frog.y--;
				rendering();
			}
			else
				flag = 3;
		}

		if (hit == 'a' || hit == 'A') {
			if(frog.x == 0){flag = 4;}
			else if (map[frog.y][frog.x-1] == '|')
			{
				map[frog.y][frog.x-1] = '0';
				map[frog.y][frog.x] = '|';
				frog.x--;
			}
			
			else if (map[frog.y][frog.x-1] == '=')
			{
				map[frog.y][frog.x-1] = '0';
				map[frog.y][frog.x] = '=';
				frog.x--;
			}
			 else
			 	flag = 3;
		}

		if (hit == 's' || hit == 'S') {
			if(frog.y == 10){flag = 4;}

			else if (map[frog.y+1][frog.x] == '|')
			{
				map[frog.y+1][frog.x] = '0';
				map[frog.y][frog.x] = '=';
				frog.y++;
			}
			
			else if (map[frog.y+1][frog.x] == '=')
			{
				map[frog.y+1][frog.x] = '0';
				map[frog.y][frog.x] = '=';
				frog.y++;
			}
			
			 else
			 	flag = 3; // jumo into the river	
		}

		if (hit == 'd' || hit == 'D') {
			if(frog.x == COLUMN-2){flag = 4;}
			else if (map[frog.y][frog.x+1] == '|')
			{
				map[frog.y][frog.x+1] = '0';
				map[frog.y][frog.x] = '|';
				frog.x++;
			}
			
			else if (map[frog.y][frog.x+1] == '=')
			{
				map[frog.y][frog.x+1] = '0';
				map[frog.y][frog.x] = '=';
				frog.x++;
			}
			 else
			 	flag = 3;
		}
	}

	pthread_mutex_unlock(&mutex);
	usleep(100000);
	if (row == 1)
		usleep(50000);
	else if (row == 1)
		usleep(30000);
	else if (row == 2)
		usleep(20000);
	else if (row == 3)
		usleep(60000);
	else if (row == 4)
		usleep(90000);
	else if (row == 5)
		usleep(80000);
	else if (row == 6)
		usleep(100000);
	else if (row == 7)
		usleep(20000);
	else if (row == 8)
		usleep(50000);

	}


	/*  Check game's status  */
	// implemented in main function

	/*  Print the map on the screen  */
	// update the screen after logs move
	pthread_exit(NULL);
}

int main( int argc, char *argv[] ){

	// Initialize the river map and frog's starting position
	memset( map , 0, sizeof( map ) ) ;
	int i , j ; 
	for( i = 1; i < ROW; ++i ){	
		for( j = 0; j < COLUMN - 1; ++j )	
			map[i][j] = ' ' ;  
	}	

	for( j = 0; j < COLUMN - 1; ++j )	
		map[ROW][j] = map[0][j] = '|' ;

	for( j = 0; j < COLUMN - 1; ++j )	
		map[0][j] = map[0][j] = '|' ;

	frog = Node( (COLUMN-1) / 2, ROW ) ; 
	map[frog.y][frog.x] = '0' ; 

	//Print the map into screen
	for( i = 0; i <= ROW; ++i)	
		puts( map[i] );


	/*  Create pthreads for wood move and frog control.  */
	pthread_t threads[ROW-1];
	pthread_attr_t attr;
	int rc;

	pthread_mutex_init(&mutex, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	for (int i = 0; i < ROW-1; i++){
		rc = pthread_create(&threads[i], &attr, logs_move, (void*)&thread_ids[i]);
		if(rc){
			printf("ERROR: return code from pthread_create() is %d", rc);
			exit(1);
		}
	}

	for(int i = 0; i<ROW-1; i++){
		// wait for all threads exit
		rc = pthread_join(threads[i], NULL);
		if(rc){
			printf("ERROR: return code from pthread_join is %d", rc);
			exit(1);
		}
	}

	
	
	
	
	/*  Display the output for user: win, lose or quit.  */
	switch (flag)
	{
	case 0:
		printf("\nYOU EXIT THE GAME\n");
		break;
	
	case 2:
		printf("\nYOU WIN THE GAME\n");
		break;

	case 3:
		printf("\nYOU JUMP INTO THE RIVER\n");
		break;

	case 4:
		printf("\nYOU COLIIDE WITH THE BOUNDARY\n");
		break;
	}
	
	// free the resource
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);

	pthread_exit(NULL);
	
	return 0;

}
