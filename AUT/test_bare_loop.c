#include <stdio.h>

int main() {

	// case I  : simple for-loop
	int i=0;
	int j=0;
	for(i=0; i< 10; i++) {
		if (i%2 == 0) {
			printf("simple for loop at i=%d\n", i);
		}
	}


	// case II : simple while-loop
	i=0;
	while(i<10) {
		if (i%2 == 0) {
			printf("simple while loop at i=%d\n", i);
		}
		i++;
	}

	// case III: nested for-loop
	i=0;
	for(i=0; i<10; i++) {
		for(j=0; j<10; j++) {
			if (i%2 == 0 && j%2 == 0) {
				printf("nested for loop at i=%d, j=%d\n", i, j);
			}
		}
	}

	// case IV: nested while-loop
	i=0;
	j=0;
	while(i<10) {
		while(j<10) {
			if (i%2 == 0 && j%2 == 0) {
				printf("nested while loop at i=%d, j=%d\n", i, j);
			}
			j++;
		}
		i++;
	}

	// case V: hybrid for-while-loop
	i=0;
	j=0;
	for(i=0; i<10; i++) {
		while(j<10) {
			if (i%2 == 0 && j%2 == 0) {
				printf("hybrid for-while loop at i=%d, j=%d\n", i, j);
			}
			j++;
		}
	}	

}
