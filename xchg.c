#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void enter_region() {
	asm("	.data\n"
	"lock:	.byte 0 \n"
	"	.text\n"
	"_enter_region:\n" 
	"	movb $1, %al\n"
	"	xchgb lock, %al\n"
	"	cmp $0, %al\n"
	"	jne _enter_region\n"
	);
}

void leave_region() {
	asm("	movb $0, lock");
}

void critical_region(char *p) {
	int d = rand() % 1000000;
	printf("%s sleep %d microsecond in critical section\n", p, d);
	usleep(d);
}

void noncritical_region(char *p) {
	int d = rand() % 1000000;
	printf("%s sleep %d microsecond in NON-critical section\n", p, d);
	usleep(d);
}

static void* f1(void* p) {
	for(int i = 0; i < 5; i++) {
		puts("f1 wait for f2");
		enter_region();
		printf("f1 start its critical section\n");
		critical_region(p);
		printf("f1 end its critical section\n");
		leave_region();
		noncritical_region(p);
	}
	return NULL;
}

static void* f2(void* p) {
	for(int i = 0; i < 5; i++) {
		puts("f2 wait for f1");
		enter_region();
		printf("f2 start its critical section\n");
		critical_region(p);
		printf("f2 end its critical section\n");
		leave_region();
		noncritical_region(p);
	}
	return NULL;
}

int main() {
	int rc;

	pthread_t t1, t2;

	rc = pthread_create(&t1, NULL, f1, "f1");
	if(rc != 0) {
		fprintf(stderr, "pthread f1 failed\n");
		return EXIT_FAILURE;
	}

	rc = pthread_create(&t2, NULL, f2, "f2");
	if(rc != 0) {
		fprintf(stderr, "pthread f2 failed\n");
		return EXIT_FAILURE;
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	puts("All threads finished.\n");

	return 0;
}































