#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

#define N 200000

/*
val = 8
                     P1               P2         
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14

			       P1 P2 
directions = right  ?  ?  left

after the first while:
					 P1               P2
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14

					 P1   P2
directions = right right left left
                     ^     ^
   			 these 2 are different

=> l = 6, r = 10;

		     P1    P2 
arr = ... 6  7  8  9  10...

=> P1 will find the value at index 4
*/
int parallelSearch(int *arr, int val) {
	int l = 0;	// left most index
	int r = N - 1; // right most index

	enum direction { left = -1, right = 1 };
	int p = omp_get_max_threads();	// number of threads
	// each process will set left or right depending where the value is
	std::vector<direction> directions(p + 2);


	directions[0] = right;
	directions[p+1] = left;

    printf("max threads = %d\n", p);

	std::vector<int> j(p);	// position for each process

	int index = -1;	// returned index
	int temp_index = -1;

	#pragma omp parallel shared(l, r, p, directions, j, index, temp_index)
	{
		int proc_id = omp_get_thread_num();	// process ID
        printf("p_id = %d\n", proc_id);

		while (l <= r && index == -1) {
			int segm_size = (r - l + 1) / (p + 1) + 1;
			j[proc_id] = l + (proc_id + 1) * segm_size - 1;

			if (j[proc_id] > r) {	// the formula for j[p] is not perfect
				directions[proc_id + 1] = left;
			}
			else {
				// here is the same as binary search
				if (arr[j[proc_id]] == val) {
					temp_index = j[proc_id];
				}
				else if (arr[j[proc_id]] < val) {
					directions[proc_id + 1] = right;
				}
				else {
					directions[proc_id + 1] = left;
				}
			}

			// waiting for every thread to finish modifying
			#pragma omp barrier

			// only 1 thread needs to modify this
			#pragma omp single
			{
				index = temp_index;
			}

			// searching for 2 different positions in directions
			if (directions[proc_id] != directions[proc_id + 1]) {
				r = j[proc_id] - 1;
				if (proc_id != 0)
					l = j[proc_id - 1] + 1;
			}

			// only 1 thread needs to modify this
			#pragma omp single
			{
				if (directions[p] != directions[p + 1]) {
					l = j[p - 1] + 1;
				}
			}
		}
	}

	return index;
}

int binarySearch(int *arr, int val) {
	int l = 0, r = N - 1;
	int m;
	while (l < r) {
		m = l + ((r - l) >> 1 /* /2 */); 
		if (arr[m] <= val) {
			l = m;
		}
		else {
			r = m - 1;
		}
	}

    if (arr[l] == val)
        return l;

	return -1;
}

int main(int argc, char *argv[])
{
    int arr[N];
	for (int i = 0; i < N; i++) {
		arr[i] = i;
	}

	int b_index, p_index;
	int test_value;

	srand((unsigned int)time(NULL));

	//for (int i = 0; i < N; i++) {
		//int rand_value = rand() % N;

		test_value = 8*N/13;
		printf("searching for: %d\n", test_value);

		// binary search (just for reference)
		{
			double start = omp_get_wtime();
			int index = binarySearch(&arr[0], test_value);
			double end = omp_get_wtime();
			printf("binarySearch time taken: %.9lf\n", end - start);
			printf("found at: %d\n", index);

			b_index = index;
		}

		// parallel search
		{
			omp_set_num_threads(2);

			double start = omp_get_wtime();
			int index = parallelSearch(&arr[0], test_value);
			double end = omp_get_wtime();
			printf("parallelSearch time taken: %.9lf\n", end - start);
			printf("found at: %d\n", index);
		
			p_index = index;
		}

		//if (b_index != p_index) {
		//	break;
		//}

	//	system("cls");
	//}

	system("pause");
	return 0;
}