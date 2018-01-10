#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>

int count = 4;

char* inner[] = {
	"lalka", "lolka",
	"xxxx", "xxyx"
};

float results[2];

pthread_t thrs[4];

pthread_mutex_t mutex;

void abort() {
	exit(1);
}


float persent(char* first, char* second) {
	float res = 0;
	char* a;
	char* b;
	if(strlen(first) < strlen(second)) {
		b = first;
		a = second;
	} else {
		a = first;
		b = second;
	}
	int l = strlen(a);
	for(int i = 0; i < l; i++) {
		if(a[i] != b[i]) {
			res+=1;
		}
	}
	return res / ( (float)l / 100 );
}


void* prod(void* me) {
	int offset = *((int*)me);
	pthread_mutex_lock(&mutex);
	float p = persent(inner[offset], inner[offset+1]);
	results[offset / 2] = p;
	pthread_mutex_unlock(&mutex);
}

int main() {
	int* ids = (int*) calloc(count, sizeof(int));
	for(int i=0; i<count; i++) {
		ids[i] = i;
	}
	pthread_attr_t attrs;
	if(0!=pthread_attr_init(&attrs)) {
		perror("Cannot initialize attributes");
		abort();
	}
	if(0!=pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE)) {
		perror("Error in setting attributes");
		abort();
	}
	pthread_attr_destroy(&attrs);
	int last_id = 0;
	for(int strid = 0; last_id < count; strid+=(4*2)) {
		int ath = 0;
		for(; ath < 4 && last_id < count; ath++) {
			// printf(">> if %d < %d; start %d with %d\n", last_id, count, ath, ids[last_id]);
			if(0!=pthread_create(&thrs[ath], &attrs, prod, &ids[last_id])) {
				perror("Cannot create a thread");
				abort();
			}
			last_id += 2;
		}
		while(ath > 0) {
			--ath;
			// printf("< stop %d\n", ath);
			if(0!=pthread_join(thrs[ath], NULL)) {
				perror("Cannot join a thread");
				abort();
			}
		}
	}
	for(int i = 0; i<count / 2; i++) {
		printf("diff between \"%s\" and \"%s\" is %.2f%%\n", inner[i*2], inner[i*2+1], results[i]);
	}
	free(ids);
	return 0;
}
