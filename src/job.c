#include "../include/job.h"

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