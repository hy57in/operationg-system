#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int total_balance = 1000000;
int total_borrow = 0;
int total_pay = 0;
int _d = 0;

void enter_region() {
	/* asm("	.data\n"
	"lock:	.byte 0 \n"
	"	.text\n"
	"_enter_region:\n" 
	"	movb $1, %al\n"
	"	xchgb lock, %al\n"
	"	cmp $0, %al\n"
	"	jne _enter_region\n"
	); */
}

void leave_region() {
	// asm("	movb $0, lock");
}

void borrow() {
	int _borrow = rand() % 10000;
	total_balance -= _borrow;
	total_borrow += _borrow;
	_d = _borrow;
	printf(">>>>>borrow: %d, total_balance: %d\n", _d, total_balance);
}

void pay() {
	int _pay = _d;
	total_balance += _pay;
	total_pay += _pay;
	printf("<<<<<payback: %d, total_balance: %d\n", _pay, total_balance);
}

void critical_region(char *p) {
	int d = rand() % 10000;
	if(p == "f1") {
		borrow();
	}
	if(p == "f2") {
		pay();
	}
	printf("%s sleep %d microsecond in critical section\n", p, d);
	usleep(d);
}

void noncritical_region(char *p) {
	int d = rand() % 10000;
	printf("%s sleep %d microsecond in NON-critical section\n", p, d);
	usleep(d);
}

static void* f1(void* p) {
	for(int i = 0; i < 10; i++) {
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
	for(int i = 0; i < 10; i++) {
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
	printf("total_balance: %d, total_borrow : %d, total_pay: %d\n", total_balance, total_borrow, total_pay);
	printf("total_borrow - total_pay = %d\n", total_borrow - total_pay);

	return 0;
}

