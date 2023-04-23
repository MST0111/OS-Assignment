#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>



// can be made local if required
int N;
 
// Thread Limit
int count_thread = 0;
 

// Possible movements


int moves_X[8] = {2, 1, 2, 1, -2, -1, -2, -1};
int moves_Y[8] = {1, 2, -1, -2, 1, 2, -1, -2};
 
// Check Variable
int isFound = 0;
 
// Helper struct, can utilize any other custom structs if needed 
typedef struct{
	int x;
	int y;
} pair;
 
// Argument Struct
typedef struct{
    int x;
    int y;
    pair Path[2500];
    int count;
    int board[50][50];
} curr_situation;
 
// Priority Struct
typedef struct Access_index_for_possible_moves Access_index_for_possible_moves;

struct Access_index_for_possible_moves {
    int x;
    int y;
    int access;
};
 
// Final answer
curr_situation Result;

bool Check(int Next_X,int Next_Y , int point)
{
	if(Next_X >= 0 && Next_X < N && Next_Y >= 0 && Next_Y < N && point== 0) {			
            return true;
        }
        
        return false;
}
 
 
// Not necessary to use this function as long as same printing pattern is followed 
void Print_final_path(pair path[], int n){
	for(int i=0;i<n;i++){
		printf("%d,%d|",path[i].x,path[i].y);
	}
    printf("\n");
}
// Check the Accessibility of the Point


Access_index_for_possible_moves checkAccess(int x, int y, int board[50][50], int level) {
    int count = 0;
 
    for(int i=0; i<8; i++) 
        if(Check(x + moves_X[i],y + moves_Y[i] , board[x + moves_X[i]][y + moves_Y[i]])) 		// Check the Accessibility of the Point
            count++;
        
 


    Access_index_for_possible_moves pt;
 
    pt.x = x;
    pt.y = y;
    pt.access = count;
 
    if((level+1 != (N*N)) && (count == 0)) {
        pt.access = 9;
    }
 
    return pt;
}
void clean(curr_situation* arg,int start_X,int start_Y)
{
	arg->x = start_X;
        arg->y = start_Y;
 
        arg->count = 0;
        
        
 
        for(int i=0; i<50; i++) {				// function for cleaning
            for(int j=0; j<50; j++) {
                arg->board[i][j] = 0;
            }
        }
 
        for(int i=0; i<2500; i++) {
            arg->Path[i].x = -1;
            arg->Path[i].y = -1;
        }
}
 

void marking(curr_situation* arg,int xy,int yx)
        {
            arg->board[xy][yx] = 1;
            
            arg->Path[arg->count].x = xy;
            arg->Path[arg->count].y = yx;
            arg->count ++;
        }
        

 
// Sort the next given points


void sort(Access_index_for_possible_moves points[8]) {
    for(int i=0; i<7; i++) {
        Access_index_for_possible_moves temp;
 
        for(int j=0; j<8-i-1; j++) {
            if(points[j].access > points[j+1].access) {
                temp = points[j];
                points[j] = points[j+1];
                points[j+1] = temp;
            }
        }
    }
}
 

 
// Find the paths
void* findPath(void* argument) {
    if(argument == NULL) {
        pthread_exit(NULL);
        return;
    }
 
    if(isFound == 1) {
        pthread_exit(NULL);
        return;
    }
 
    curr_situation arg_ptr = (curr_situation) argument;
    curr_situation arg = *arg_ptr; 
 
    marking(&arg,arg.x,arg.y);
 
    if(arg.count == (N*N)) {
        //Result = arg;
 	Print_final_path(arg.Path, N*N);
 	exit(0);
        //isFound = 1;
        //pthread_exit(NULL);
        //return;
    }
 
    int x = arg.x;
    int y = arg.y;
 


    Access_index_for_possible_moves points[8];
 
    int count = 0;
 
    for(int i=0; i<8; i++) {
        int Next_X = x + moves_X[i];
        int Next_Y = y + moves_Y[i];
 
        if(Check(x + moves_X[i],y + moves_Y[i] , arg.board[x + moves_X[i]][y + moves_Y[i]])){			


            points[i] = checkAccess(x + moves_X[i], y + moves_Y[i], arg.board, arg.count);
            count++;
        }
        else {
            points[i].x = Next_X;
            points[i].y = Next_Y;
            points[i].access = 9;
        }
    }
 
    sort(points);
 
    if(isFound == 0) {
        for(int i=0; i<8; i++) {
            if(points[i].access != 9) {
                arg.x = points[i].x;
                arg.y = points[i].y;
 
                count--;
 
                if(count == 0 || count_thread > 100000) 
                    findPath(&arg);
                else {
                    count_thread++;
                    pthread_t tid;
 
                    pthread_create(&tid, NULL, findPath, &arg);
                    pthread_join(tid, NULL);
                }
            }
        }
    }
 
    if(isFound == 0) {
        arg.x = x;
        arg.y = y;
        arg.board[x][y] = 0;
 
        arg.count = arg.count - 1;
        arg.Path[arg.count].x = -1;
        arg.Path[arg.count].y = -1;
    }
 
 count_thread--;
    pthread_exit(NULL);
    return;
}
 
int main(int argc, char *argv[]) {
	
	if (argc != 4) {
		printf("Usage: ./Knight.out grid_size Start_X Start_Y");
		exit(-1);
	}
	
other

	N = atoi(argv[1]);
	int Start_X=atoi(argv[2]);
	int Start_Y=atoi(argv[3]);
 
	/* Do your thing here */
    
    if(N%2 == 1 && (Start_X + Start_Y)%2 == 1) {
        printf("No Possible Tour");
    }
    else {
        curr_situation arg;
 
        arg.x = Start_X;
        arg.y = Start_Y;
 
        clean(&arg, Start_X,Start_Y);	
 
        if(N%2 == 1){
            marking(&arg,Start_X,Start_Y);
 
            pthread_t tid[8];
 
            for(int i=0; i<8; i++) {
                int Next_X = Start_X + moves_X[i];
                int Next_Y = Start_Y + moves_Y[i];
 
                if(isFound == 0) {
                    if(Next_X >= 0 && Next_X < N && Next_Y >= 0 && Next_Y < N) {
                        arg.x = Next_X;
                        arg.y = Next_Y;
 
                        pthread_create(&tid[i], NULL, findPath, &arg);
 
                        // Wait until the thread is done executing
                        pthread_join(tid[i], NULL);
                    }
                }
            }
        }
        else {
            pthread_t tid;
 
            pthread_create(&tid, NULL, findPath, &arg);
 
            // Wait until the thread is done executing
            pthread_join(tid, NULL);
        }
 
        if(isFound == 0) {
            printf("No Possible Tour");
        }
        //else {
            //Print_final_path(Result.path, N*N);
        //}
    }
 
	return 0;
}