#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <omp.h>
#include <time.h>

#define N 1500

/*
val = 8
                     P1               P2         
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
c = 1                ?                ?           -1

after the first while:
					 P1               P2
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
c = 1                1                -1          -1
                     ^                ^
   					these 2 are different

=> l = 6, r = 10;

		     P1    P2 
arr = ... 6  7  8  9  10...

=> P1 will find the value at index 4
*/
int parallelSearch(std::vector<int> arr, int val) {
	int l = 0;	// left most index
	int r = N - 1; // right most index

	enum state { left = -1, right = 1 };
	int p = omp_get_max_threads();
	std::vector<state> c(p + 2);

	c[0] = right;
	c[p+1] = left;

	int g = (int)ceil(log2(N + 1) / log2(p + 1));	// nr pasi <=> complexity O(g)

	std::vector<int> j(p);	// position for each process

	int index = -1;
	int temp_index = -1;

	#pragma omp parallel shared(l, r, p, c, g, j, index, temp_index)
	{
		int proc_id = omp_get_thread_num();

		while (l <= r && index == -1) {
			int segm_size = (r - l + 1) / (p + 1) + 1;
			j[proc_id] = l + (proc_id + 1) * segm_size - 1;

			if (j[proc_id] > r) {
				c[proc_id + 1] = left;
			}
			else {
				if (arr[j[proc_id]] == val) {
					temp_index = j[proc_id];
				}
				else if (arr[j[proc_id]] < val) {
					c[proc_id + 1] = right;
				}
				else {
					c[proc_id + 1] = left;
				}
			}

			#pragma omp barrier

			#pragma omp single
			{
				index = temp_index;
			}

			if (c[proc_id] != c[proc_id + 1]) {
				r = j[proc_id] - 1;
				if (proc_id != 0)
					l = j[proc_id - 1] + 1;
			}

			#pragma omp single
			{
				if (c[p] != c[p + 1]) {
					l = j[p - 1] + 1;
				}
			}
		}
	}

	return index;
}

int binarySearch(std::vector<int> arr, int val) {
	int l = 0, r = N - 1;
	int m;
	while (l <= r) {
		m = l + (r - l) / 2;
		if (arr[m] == val) {
			return m;
		}
		else if (arr[m] < val) {
			l = m + 1;
		}
		else {
			r = m - 1;
		}
	}

	return -1;
}

int main(int argc, char *argv[])
{
	std::vector<int> arr(N);
	for (int i = 0; i < N; i++) {
		arr[i] = i;
	}

	int b_index, p_index;
	int test_value;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < N; i++) {
		//int rand_value = rand() % N;

		test_value = i;
		printf("searching for: %d\n", test_value);

		// binary search (just for reference)
		{
			double start = omp_get_wtime();
			int index = binarySearch(arr, test_value);
			double end = omp_get_wtime();
			printf("binarySearch time taken: %.9lf\n", end - start);
			printf("found at: %d\n", index);

			b_index = index;
		}

		// parallel search
		{
			omp_set_num_threads(2);

			double start = omp_get_wtime();
			int index = parallelSearch(arr, test_value);
			double end = omp_get_wtime();
			printf("parallelSearch time taken: %.9lf\n", end - start);
			printf("found at: %d\n", index);
		
			p_index = index;
		}

		if (b_index != p_index) {
			break;
		}

	//	system("cls");
	}

	system("pause");
	return 0;
}