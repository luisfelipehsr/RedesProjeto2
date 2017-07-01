#include <time.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

long get_time () {
	struct timespec time;
	long time_sec;
	long time_ms;

	if (clock_gettime(CLOCK_REALTIME, &time))
		return 0;
	time_sec = time.tv_sec;
	time_ms = (long) (time.tv_nsec / 1.0e6);
	time_ms += (long) time_sec * (1000);

	return time_ms;
}

int main() {
	long time, initial_time;
	int i;
	
	i = 0;
	time = get_time();
	if (!time) {
		printf("ERROR: can't retrive time\n");
	}

	printf("Current time %ld ms\n", time);

	initial_time = time;

	i = 0;
	while(i < 5) {
		time = get_time();
		if (time - initial_time > 500*i) {
			printf("Got a 0.5s interval! %ld\n", time);
			i++;
		}
	}

	return 0;
}
	
