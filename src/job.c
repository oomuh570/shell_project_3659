/*

File Name: job.c
Author(s): Slater Dennington, Ochihai Omuha, Salman Tajammal, Dan Meulendyk
Creation Date: Feb. 16th, 2026
Last Modified Date: Feb 16th, 2026
Purpose: Contains the function to initiate job structures

*/

#include "../include/job.h"

/*

FUNCTION: job_init
PURPOSE: Initiates the job structure
INPUT: job - job to be made
OUTPUT: none

*/

void job_init(Job *job) {
	unsigned int i;
	
	job->num_stages = 0;
	job->background = 0;
	job->infile_path = 0;
	job->outfile_path = 0;
	
	for (i = 0; i < MAX_PIPELINE_LEN; i++) {
		command_init(&job->pipeline[i]);
	}
}
