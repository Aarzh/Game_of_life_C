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
#include <omp.h>

#define STRING_SIZE 50

// GLOBAL VARIABLES //
pgm_t * pointer;
pgm_t * nextptr;
pgm_t * temp;
int number_threads = 4;

///// Function declarations /////
void usage(const char * program);
void lifeSimulation(int iterations, char * start_file);
void preparePGMImage(const void * board, pgm_t * board_image);
void saveAsPGM(const void * board, pgm_t * board_image, int iteration);
// ADD YOUR FUNCTION DECLARATIONS HERE
void readFile(char * start_file, pgm_t * board_image);
int adjacent_to (pgm_t * pointer, int i, int j);
void game (pgm_t * pointer, pgm_t * nextptr, int start, int finish);

int main(int argc, char * argv[])
{
    char * start_file = "Boards/sample_4.txt";
    int iterations = 5;
	unsigned int start;
    unsigned int finish;
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
    else if (argc == 4)
    {
        iterations = atoi(argv[1]);
        start_file = argv[2];
        number_threads = atoi(argv[3]);
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
    lifeSimulation(iterations, start, finish);

    return 0;
}

// Print usage information for the program
void usage(const char * program)
{
    printf("Usage:\n");
    printf("%s [iterations] [board file] [number of threads]\n", program);
}

// Main loop for the simulation
void lifeSimulation(int iterations, int start, int finish);
{
    int tid;
    int i;
    #pragma omp parallel shared(pointer, nextptr, temp, number_threads, iterations) private(tid, i, start, finish) num_threads(number_threads)
    {
        //Get thread number ID
        tid = omp_get_thread_num();
        //Specify the bounds
		start = tid * (pointer->image.height / number_threads);
		finish = start + (pointer->image.height / number_threads);
		
		// Exclude extern cells
		if(tid==0) start++;
		if(tid==number_threads-1) finish=pointer->image.height-1;

        // Play the game
        for(i = 0; i<iter; i++){
            //each thread will play with each bound
            game(pointer, nextptr, start, finish);
            #pragma omp barrier
            //the thread with the id = 0 is the one to swap the pointers
            if(i == 0)
            {
                printf("Done!\n");
                //swap pointers
                temp = pointer;
                pointer = nextptr;
                nextptr = temp;
            }
            #pragma omp barrier
        }
    }
    return 0;
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

void game (pgm_t * pointer, pgm_t * nextptr, int start, int finish)
{
	int i, j, alive;

	// Exclude board region and apply for each cell the game's rules
	for (i=1; i<pointer->image.width-1; i++) for (j=start; j<finish; j++) 
	{
		alive = adjacent_to (pointer, i, j);
		if (alive == 2) nextptr[i][j] = pointer[i][j];
		if (alive == 3) nextptr[i][j] = 1;
		if (alive < 2) nextptr[i][j] = 0;
		if (alive > 3) nextptr[i][j] = 0;
	}
}