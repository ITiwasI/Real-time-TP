#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

long cpu_load_1 = 50;
pthread_barrier_t barrier;

void function_1() {
	for (int i = 0; i < cpu_load_1 * 10000; i++)
	{
		for (int j=0; j < 1000; j++) ; 
      		/* do nothing , keep counting */
    	}
}	

struct periodic_task {
	// just for convenience, we keep track of the
	// sequence number of the current job
	unsigned long current_job_id;

	// desired separation of consecutive activations
	struct timespec period;

	// time at which the task became fist operational
	struct timespec first_activation;

	// time at which the current instance was (supposed
	// to be) activated
	struct timespec current_activation;

	// flag to let applications terminate themselves
	int terminated;
};

void sleep_until_next_activation(struct periodic_task *tsk) {
	int err;
	// sleep until tsk->current_activation
	do {
	  err = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
				&(tsk->current_activation), NULL);
	} while (err != 0 && errno == EINTR);
	assert (err == 0);	  
}

void timespec_add(struct timespec *a, struct timespec *b) {
	a->tv_sec += b->tv_sec;
	a->tv_nsec += b->tv_nsec;
	if (a->tv_nsec >= 1000000000UL) {
		a->tv_sec++;
		a->tv_nsec %= 1000000000UL;
	}
}

void timespec_subtract(struct timespec *a, struct timespec *b) {
	a->tv_nsec -=  b->tv_nsec;
  	a->tv_sec  -=  b->tv_sec;

  	if (a->tv_nsec < 0) {
    	/* Handle carry */
    		a->tv_nsec += 1000000000UL;
    		a->tv_sec -= 1;
  	}
}

void * task_1(void *) {
        //Wait for all thread ready
	pthread_barrier_wait(&barrier);
  
	int err;
	struct periodic_task tsk;
	struct timespec time_now;

	// to match the real-time theory, the job count starts at “1”
	tsk.current_job_id = 1;

	// run until application logic tells us to shut down
	tsk.terminated = 0;

	// set the desired period
	tsk.period.tv_sec =  1;
	tsk.period.tv_nsec = 0;

	// record time of first job in &tsk.first_activation
	err=clock_gettime(CLOCK_MONOTONIC, &(tsk.first_activation));
	assert(err==0);
	//
	tsk.current_activation = tsk.first_activation;

	// display message
	printf("Hello World\n");
	long delay_ms;
	
	// execute a sequence of jobs until app shuts down (if ever)
	while (!tsk.terminated) {
		// wait until release of next job
		sleep_until_next_activation(&tsk);
		// call the actual application logic
		function_1();
		// advance the job count in preparation of the next job
		tsk.current_job_id++;
		// display task response time
		err=clock_gettime(CLOCK_MONOTONIC, &time_now);
		assert(err==0);
		timespec_subtract(&time_now,&(tsk.current_activation));
		delay_ms = time_now.tv_nsec/1000000;
		printf("1_Response time : %ld ms\n",delay_ms);
		//printf("Délai d'activation: %lu secondes et %ld nanosecondes\n", time_now.tv_sec, time_now.tv_nsec);
		//
		// compute the next activation time
		timespec_add(&(tsk.current_activation), &(tsk.period));
		//
				   
	}
	sleep_until_next_activation(&tsk);
}


void * task_2(void *) {

  	//Wait for all thread ready
	pthread_barrier_wait(&barrier);
	int err;
	struct periodic_task tsk;
	struct timespec time_now;

	// to match the real-time theory, the job count starts at “1”
	tsk.current_job_id = 1;

	// run until application logic tells us to shut down
	tsk.terminated = 0;

	// set the desired period
	tsk.period.tv_sec =  2;
	tsk.period.tv_nsec = 0;

	// record time of first job in &tsk.first_activation
	err=clock_gettime(CLOCK_MONOTONIC, &(tsk.first_activation));
	assert(err==0);
	//
	tsk.current_activation = tsk.first_activation;

	// display message
	printf("Hello World\n");
	long delay_ms;
	
	// execute a sequence of jobs until app shuts down (if ever)
	while (!tsk.terminated) {
		// wait until release of next job
		sleep_until_next_activation(&tsk);
		// call the actual application logic
		function_1();
		// advance the job count in preparation of the next job
		tsk.current_job_id++;
		// display task response time
		err=clock_gettime(CLOCK_MONOTONIC, &time_now);
		assert(err==0);
		timespec_subtract(&time_now,&(tsk.current_activation));
		delay_ms = time_now.tv_nsec/1000000;
		printf("2_Response time : %ld ms\n",delay_ms);
		//printf("Délai d'activation: %lu secondes et %ld nanosecondes\n",time_now.tv_sec, time_now.tv_nsec);
		//
		// compute the next activation time
		timespec_add(&(tsk.current_activation), &(tsk.period));
		//
				   
	}
	sleep_until_next_activation(&tsk);
}

int main(int argc, char *argv[]) {
  int num_thread = 2;
  pthread_t tid[num_thread];
  pthread_attr_t attr1; 
  pthread_attr_t attr2;
  
  // task_1(NULL);
  //task_2(NULL);

  
  pthread_barrier_init(&barrier, NULL, num_thread+1);//barrier for task and main threads
  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);
  pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&attr1, SCHED_RR);
  pthread_attr_setschedpolicy(&attr2, SCHED_RR);
  
  if(pthread_create(&tid[0], &attr1, task_1, NULL)!=0){
    printf("error in pthread_create task 1");
    exit(1);
  }
  if(pthread_create(&tid[1], &attr2, task_2, NULL)!=0){
    printf("error in pthread_create task 2");
    exit(1);
  }

  pthread_setschedprio(tid[0], 2);
  pthread_setschedprio(tid[1], 1);
  
  //Thread of main start at the same time than the task's thread
  pthread_barrier_wait(&barrier);
    
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  return 0;
}
