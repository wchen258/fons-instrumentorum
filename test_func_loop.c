#include <stdio.h>

int add(int a, int b) {
	for(int i=0; i<10; i++) {
		a++;
	}
	return a + b;
}

int main() {

	// case I  : simple for-loop
	int i=0;
	int j=0;
	for(i=0; i< 10; i++) {
		add(i,j);
	}


	// case II : simple while-loop
	i=0;
	while(i<10) {
		add(i,j);
		i++;
	}

	// case III: nested for-loop
	i=0;
	for(i=0; i<10; i++) {
		for(j=0; j<10; j++) {
			add(i,j);
		}
	}

	// case IV: nested while-loop
	i=0;
	j=0;
	while(i<10) {
		while(j<10) {
			add(i,j);
			j++;
		}
		i++;
	}

	// case V: hybrid for-while-loop
	i=0;
	j=0;
	for(i=0; i<10; i++) {
		while(j<10) {
			add(i,j);
			j++;
		}
	}	

}
