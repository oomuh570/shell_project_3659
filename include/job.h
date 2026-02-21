#ifndef JOB_H
#define JOB_H

#include "command.h"

#define MAX_PIPELINE_LEN 16

typedef struct {
	Command pipeline[MAX_PIPELINE_LEN];
    unsigned int num_stages;
    char *outfile_path;
    char *infile_path;
    int background;
	
} Job;

void job_init(Job *job);

#endif
