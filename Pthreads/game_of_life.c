/*
    WRITE YOUR NAME HERE:



    Simulation of Conway's Game of Life using OpenMP
    Based on the explanations at:
        https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
    Online versions:
        http://pmav.eu/stuff/javascript-game-of-life-v3.1.1/
        https://bitstorm.org/gameoflife/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm_image.h"
// ADD YOUR EXTRA LIBRARIES HERE
#include <pthread.h>

#define STRING_SIZE 50
#define NUM_THREADS 5

// struct for args to threads
typedef struct arg_struct{
    int height;
    int iterations;
    int * t_id;
}arg_t;

// GLOBAL VARIABLES
pgm_t * pointer;
pgm_t * nextptr;
pgm_t * temp;

///// Function declarations /////
void usage(const char * program);
void lifeSimulation(pgm_t * pointer, pgm_t * nextptr, int start, int finish);
void preparePGMImage(const void * board, pgm_t * board_image);
void saveAsPGM(const void * board, pgm_t * board_image, int iteration);
// ADD YOUR FUNCTION DECLARATIONS HERE
void readFile(char * start_file, pgm_t * board_image);
void * threadStart(void * arg);
int adjacent_to (pgm_t * pointer, int i, int j) ;

int main(int argc, char * argv[])
{
    char * start_file = "Boards/sample_4.txt";
    int iterations = 5;
    int thread_status;
    long i;
    arg_t args;
    int * t_ids;

    pthread_t tid[NUM_THREADS];

    t_ids = malloc(NUM_THREADS * sizeof(int));
	if(t_ids==NULL)
	{
		printf("Problem in memory allocation of ids array!\n");
		printf("Program exited.\n");
		return -1;
	}

    printf("\n=== GAME OF LIFE ===\n");
    printf("{By: Aaron Zajac}\n");

    // Parsing the arguments
    if (argc == 2)
    {
        iterations = atoi(argv[1]);
    }
    else if (argc == 3)
    {
        iterations = atoi(argv[1]);
        start_file = argv[2];
    }
    else if (argc != 1)
    {
        usage(argv[0]);
        return 1;
    }

    // Run the simulation with the iterations specified
    printf("Running the simulation with file '%s' using %d iterations\n", start_file, iterations);
    allocateImage(pointer);
    allocateImage(nextptr);
    readFile(start_file, pointer);
    copyPGM(pointer, nextptr);
    printf("\n%d - %d\n", pointer->image.width, pointer->image.height);

    args.height = pointer->image.height;
    args.iterations = iterations;
    args.t_id = t_ids;

    // ------------- CREATE THREADS------------- //
    for(i=0; i<NUM_THREADS; i++)
    {
        thread_status = pthread_create(&tid[i], NULL, &threadStart, (void *)&args);
        // Check for errors
        if (thread_status)
        {
            fprintf(stderr, "ERROR: pthread_create %d\n", thread_status);
            exit(EXIT_FAILURE);
        }

        printf("Created thread %ld with ID %lu\n", i, tid[i]);
    }
    // Finish the threads
    for(i=0; i<NUM_THREADS; i++)
    {
        //pthread_join(tid, &result);
        //printf("Thread %d finished with return value: %d\n", i, *(int *)result);
        pthread_join(tid[i], NULL);
        printf("Thread %ld finished\n", i);
    }

    printf("All threads have finished\n");

    // Call this function to wait for the other threads to finish
    pthread_exit(NULL);

    //free memory
    freeImage(pointer);
    freeImage(nextptr);

    return 0;
}

// Print usage information for the program
void usage(const char * program)
{
    printf("Usage:\n");
    printf("%s [iterations] [board file]\n", program);
}

// Main loop for the simulation
void lifeSimulation(pgm_t * pointer, pgm_t * nextptr, int start, int finish)
{   
    int i, j, alive;

	// Exclude board region and apply for each cell the game's rules
	for (i=1; i<pointer->image.width-1; i++) for (j=start; j<finish; j++) 
	{
		alive = adjacent_to (pointer, i, j);
		if (alive == 2) nextptr->image.pixels[i][j].value = pointer->image.pixels[i][j].value;
		if (alive == 3) nextptr->image.pixels[i][j].value = 1;
		if (alive < 2) nextptr->image.pixels[i][j].value = 0;
		if (alive > 3) nextptr->image.pixels[i][j].value = 0;
	}
}

// Get the memory necessary to store an image
void preparePGMImage(const void * board, pgm_t * board_image)
{

}

// Save the contents of the board as a PGM image
void saveAsPGM(const void * board, pgm_t * board_image, int iteration)
{

}

// Read the txt
void readFile(char * filename, pgm_t * board_image){
    FILE* file_ptr = NULL;

    file_ptr = fopen(filename, "r");
    if (!file_ptr) {
        printf("Unable to open the file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fscanf(file_ptr, "%d", &board_image->image.height);
    fscanf(file_ptr, "%d", &board_image->image.width);
    for(int j = 0; j<board_image->image.height; j++){
        for(int k = 0; k<board_image->image.width; k++){
            fscanf(file_ptr, "%hhu", &(board_image->image.pixels[j][k].value));
        }
    }
}

void * threadStart(void * arg){
    arg_t *args = arg;
    int * thread_id = args->t_id;
    int height = args->height;
    int iter = args->iterations;

    int bound = height / NUM_THREADS;
    int start = *thread_id * bound;
	int finish = start + bound;

    int i;

	// exclude extern cells
	if(*thread_id==0) start++;
	if(*thread_id==NUM_THREADS-1) finish=height-1;

    for(i = 0; i<iter; i++){
        //each thread will play with each bound
        lifeSimulation(pointer, nextptr, start, finish);
        //the thread with the id = 0 is the one to swap the pointers
        if(i == 0)
        {
            printf("Done!\n");
            //swap pointers
            temp = pointer;
            pointer = nextptr;
            nextptr = temp;
        }
    //pthread_exit(NULL);**************************************************************
    }
    pthread_exit(NULL);
    return NULL;
}

// To know the neighbors from each cell
int adjacent_to (pgm_t * pointer, int i, int j) 
{
	int row, col, count;

	count = 0;

	// Examine all the neighbors
	for (row=-1; row<=1; row++) 
		for (col=-1; col<=1; col++)
		{
			// exclude current cell from count
			if (row || col)
				if (pointer->image.pixels[i+row][j+col].value) count++;		
			// we don't need to keep counting if the number is >3 (no change in behaviour)		
			if(count>3)
			{//break nested loops
				break;
				break;
			}
		}		
	return count;
}