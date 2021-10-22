#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int total_balance = 1000000;
int total_borrow = 0;
int total_pay = 0;
int buffer[10] = {0,0,0,0,0,0,0,0,0,0};
int in = 0;
int out = 0;

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

static void* f1(void* p) {
	for(int i = 0; i < 10; i++) {
		puts("f1 wait for f2");
		enter_region();
		printf("f1 start its critical section\n");
		buffer[in] = rand() % 10000;
		total_balance -= buffer[in];
		total_borrow += buffer[in];
		printf("%d>>>  nextProduced: %d, total_balance : %d\n",i+1, buffer[in], total_balance);
 		in = (in + 1) % 10;
		printf("f1 end its critical section\n");
		leave_region();
	}
	return NULL;
}

static void* f2(void* p) {
	for(int i = 0; i < 10; i++) {
		puts("f2 wait for f1");
		enter_region();
		printf("f2 start its critical section\n");
		total_balance += buffer[out];
		total_pay += buffer[out];
                printf("%d<<< nextConsumed: %d, total_balance : %d\n", i+1,buffer[out], total_balance);
                out = (out + 1) % 10;
		printf("f2 end its critical section\n");
		leave_region();
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
	printf("total_balance: %d, total_borrow : %d, total_pay: %d\n", total_balance, total_borrow, total_pay);
	printf("total_borrow - total_pay = %d\n", total_borrow - total_pay);

	return 0;
}

