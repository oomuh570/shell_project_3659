#include <stdio.h>
#include <string.h>
#include "../include/shell.h"
#include "../include/command.h"
#include "../include/job.h"

static void print_summary(int passed, int failed) {
    printf("\n====================\n");
    printf("JOB TEST SUMMARY\n");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    printf("====================\n");
}

int main(void) {
	Job job;
    int passed = 0;
    int failed = 0;

    printf("Running Job & Pipeline tests\n");

	// Test initial job
	printf("Test job_init\n");
	    job_init(&job);

    // Check num_stages
    printf(" -Check num_stages == 0 ... ");
    if (job.num_stages == 0) {
        printf("pass\n");
        passed++;
    } else {
        printf("fail (num_stages=%d)\n", job.num_stages);
        failed++;
    }

    // Check redirection paths are NULL
    printf(" -Check infile_path == NULL ... ");
    if (job.infile_path == NULL) {
        printf("pass\n");
        passed++;
    } else {
        printf("fail (in path is not NULL)\n");
        failed++;
    }

    printf(" -Check outfile_path == NULL ... ");
    if (job.outfile_path == NULL) {
        printf("pass\n");
        passed++;
    } else {
        printf("fail (out path is not NULL)\n");
        failed++;
    }

    // Check background flag
    printf(" -Check background == 0 ... ");
    if (job.background == 0) {
        printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }


    // Test Single Command (ls -al /library)
	printf("Test Single Command\n");
	Job eg1 = {
		{ { { "/usr/bin/ls", "-al", "/library", NULL }, 3, 0 } },
		1, NULL, NULL, 0
    };
	run_job(&eg1);
	if(eg1.num_stages == 1) {
	printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }

    // Test Two Stage Pipe (ls -al library | wc -l) 
	printf("Test Two Stage Pipe\n");
    Job eg2 = {
        { { { "/usr/bin/ls", "-al", "/library", NULL }, 3, 0 },
          { { "/usr/bin/wc", "-l", NULL }, 2, 0 } },
        2, NULL, NULL, 1
    };
	run_job(&eg2);
	if(eg2.num_stages == 2) {
	printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }

    // Test Input & Output Redirection (sort < input.txt > output.txt)
	printf("Test Input & Output Redirection\n");
    Job eg3 = {
        { { { "/usr/bin/sort", NULL }, 1, 0 } },
        1, "tests/input.txt", "output.txt", 0
    };
	run_job(&eg3);
	if(eg3.num_stages == 1) {
	printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }

	// Test Background
	printf("Test Background\n");
	Job eg4 = {
		{ { { "/usr/bin/sleep", "5", NULL }, 2, 0 } },
		1, NULL, NULL, 1
	};
	run_job(&eg4);
	if(eg4.num_stages == 1) {
	printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }

	// Test Too many Commands
	printf("Test Too many Commands\n");
	Job eg5 = {	
		{ { { "/bin/ls", NULL }, 1, 0 },
          { { "/bin/grep", "lib", NULL }, 2, 0 },
          { { "/bin/wc", "-l", NULL }, 2, 0 } },
		  3, NULL, NULL, 0
	};
	run_job(&eg5);
	if(eg5.num_stages == 0) {
	printf("pass\n");
        passed++;
    } else {
        printf("fail (background=%d)\n", job.background);
        failed++;
    }
	
    print_summary(passed, failed);

    return 0;
}