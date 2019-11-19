#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <omp.h>
#include <time.h>
#include <iostream>
#include <chrono>

constexpr unsigned long long N = 1'299'000'000;

/*
val = 7
                     P1               P2         
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
c = right            ?                ?           left

after the first while:
					 P1               P2
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
c = right            right            left        left
                     ^                ^
   					these 2 are different
thrdPos[P1] = 6
thrdPos[P2] = 11
=> l = 6, r = 10;

	     P1       P2 
arr = 6  7  8  9  10 11

=> P1 will find the value at index 7
*/
template<typename VecType, typename VecSizeT = unsigned long long>
VecSizeT parallelSearch(const std::vector<VecType>& arr, VecType val)
{
	VecSizeT l = 0;	// left most index
	VecSizeT r = N - 1; // right most index

	enum state { left = -1, right = 1 };
	VecSizeT threadCount = omp_get_max_threads();

	// each thread will tell if "val" is to their "left" or to their "right"
	std::vector<state> dirToVal(threadCount + 2); // + 2 <=> the 2 borders

	dirToVal[0] = right;	// left border 
	dirToVal[threadCount+1] = left;

	//int g = (int)ceil(log2(N + 1) / log2(p + 1));	// nr pasi <=> complexity O(g)

	std::vector<VecSizeT> thrdPos(threadCount);	// position for each thread

	VecSizeT index = -1;
	VecSizeT temp_index = -1;

	#pragma omp parallel shared(l, r, threadCount, index, temp_index)
	{
		VecSizeT thrdID = omp_get_thread_num();
		//printf("Number of threads: %d\n", omp_get_max_threads());

		while (l <= r && index == -1)
		{
			VecSizeT segm_size = (r - l + 1) / (threadCount + 1) + 1;
			thrdPos[thrdID] = l + (thrdID + 1) * segm_size - 1;

			if (thrdPos[thrdID] > r)
			{
				dirToVal[thrdID + 1] = left;
			}
			else
			{
				if (arr[thrdPos[thrdID]] == val)
				{
					temp_index = thrdPos[thrdID];
				}
				else if (arr[thrdPos[thrdID]] < val)
				{
					dirToVal[thrdID + 1] = right;
				}
				else
				{
					dirToVal[thrdID + 1] = left;
				}
			}

			#pragma omp barrier

			#pragma omp single
			{
				index = temp_index;
			}

			if (dirToVal[thrdID] != dirToVal[thrdID + 1])
			{
				r = thrdPos[thrdID] - 1;
				if (thrdID != 0)
				{
					l = thrdPos[thrdID - 1] + 1;
				}
			}

			#pragma omp single
			{
				if (dirToVal[threadCount] != dirToVal[threadCount + 1])
				{
					l = thrdPos[threadCount - 1] + 1;
				}
			}
		}
	}

	return index;
}

template<typename VecType, typename VecSizeT = unsigned long long>
VecSizeT binarySearch(const std::vector<VecType>& arr, VecType val)
{
	VecSizeT l = 0, r = N - 1;
	VecSizeT m;
	while (l <= r)
	{
		m = l + (r - l) / 2;
		if (arr[m] <= val)
		{
			l = m + 1;
		}
		else
		{
			r = m - 1;
		}
	}

	return r;
}

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
	template<typename F, typename RetType, typename ...Args>
	static typename TimeT::rep execution(F&& func, RetType& retVal, Args&&... args)
	{
		auto start = std::chrono::steady_clock::now();
		retVal = std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
		auto duration = std::chrono::duration_cast<TimeT>
			(std::chrono::steady_clock::now() - start);
		return duration.count();
	}
};

int main(int argc, char *argv[])
{
	using VecType = unsigned long long;
	using VecSizeT = unsigned long long;
	std::vector<VecType> arr(N);
	for (VecSizeT i = 0; i < N; i++)
	{
		arr[i] = i;
	}

	VecSizeT b_index, p_index;
	VecType test_value;

	srand(42);

	for (VecSizeT i = 0; i < N; i++)
	{
		VecType rand_value = rand() % RAND_MAX;

		test_value = rand_value;
		printf("searching for: %lld\n", test_value);

		// binary search (just for reference)
		{
			auto duration = measure<std::chrono::nanoseconds>::execution(
				binarySearch<VecType, VecSizeT>, b_index, arr, test_value);
			printf("binarySearch time taken: %lld ns\n", duration);
			printf("found at pos: %lld\n", b_index);
		}

		// parallel search
		{
			//omp_set_num_threads(2);
			auto duration = measure<std::chrono::nanoseconds>::execution(
				parallelSearch<VecType, VecSizeT>, p_index, arr, test_value);
			printf("parallelSearch time taken: %lld ns\n", duration);
			printf("found at pos: %lld\n", p_index);
		}

		if (b_index != p_index)
		{
			break;
		}

		printf("\n");

	//	system("cls");
	}

	system("pause");
	return 0;
}
