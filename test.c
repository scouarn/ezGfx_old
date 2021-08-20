#include <stdlib.h>
#include <stdio.h>

const char* test = " 1.000 \n";

int main() {

	float g = 0.0f;
	float *f = &g;

	int read = sscanf(test, "%f \n", f);

	if (read == 0)
		printf("nothing read !\n");
	else
		printf("%f\n", *f);


	return 0;
}