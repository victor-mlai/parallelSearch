#define _CRT_SECURE_NO_WARNINGS

#include <omp.h>
#include <time.h>
#include <vector>
#include <array>
#include <iostream>
#include <random>	// used to generate random values to be searched
#include <functional>	// std::bind
#include <numeric>      // std::iota

#include "SearchAlgorithms.h"

#define USING_CHRONO_TO_MEASURE_DURATION

#ifdef USING_CHRONO_TO_MEASURE_DURATION

#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
	template<typename F, typename RetType, typename ...Args>
	static typename TimeT::rep execution(F&& Search, RetType& index_found, Args&&... args)
	{
		auto start = std::chrono::high_resolution_clock::now();
		
		// call Search()
		index_found = std::forward<decltype(Search)>(Search)(std::forward<Args>(args)...);
		
		auto duration = std::chrono::duration_cast<TimeT>
			(std::chrono::high_resolution_clock::now() - start);

		return duration.count();
	}
};
#else
struct measure
{
	template<typename F, typename RetType, typename ...Args>
	static double execution(F&& Search, RetType& index_found, Args&&... args)
	{
		const double start = omp_get_wtime();

		// call Search()
		index_found = std::forward<decltype(Search)>(Search)(std::forward<Args>(args)...);
		
		const double duration = omp_get_wtime() - start;
		
		return duration;
	}
};
#endif

/* wrapper classes for each Search Algorithm so there will exist a unique Test<>()
	instance for each of them so the statics inside Test won't be shared between instances. */
template<typename VecType, typename VecSizeT/* = unsigned long long*/>
struct BinarySearchT
{
	static inline const char* prettyName = "Binary Search";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::BinarySearch<VecType, VecSizeT>(arr, val);
	}
};

template<typename VecType, typename VecSizeT/* = unsigned long long*/>
struct ParallelSearch1T
{
	static inline const char* prettyName = "Parallel Search1";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::ParallelSearch1<VecType, VecSizeT>(arr, val);
	}
};

template< template<typename, typename> class SearchT,
	typename VecType, typename VecSizeT/* = unsigned long long*/>
VecSizeT Test(const std::vector<VecType>& arr, const VecType& val)
{
	static double avg_duration = 0.;
	static unsigned calls_count = 0;

	VecSizeT index_found;

#ifdef USING_CHRONO_TO_MEASURE_DURATION
	const double duration = measure<std::chrono::duration<double>>::execution(
		SearchT<VecType, VecSizeT>::Search, index_found, arr, val);
#else
	const double duration = measure::execution(
		SearchT<VecType, VecSizeT>::Search, index_found, arr, val);
#endif

	avg_duration = ((avg_duration * calls_count) + duration) / (calls_count + 1);
	calls_count++;

	printf("%.15s found it at:\t%lu\tin: %.9f seconds\tavg : %.9f\n",
		SearchT<VecType, VecSizeT>::prettyName, index_found, duration, avg_duration);

	return index_found;
}

int main(int argc, char *argv[])
{
	using VecType = unsigned long;
	using VecSizeT = unsigned long;
	constexpr VecSizeT N = 2'000'000;

	// Create Sorted vector of form: {0, 1, 2, ..., N-1}
	std::vector<VecType> sortedVec(N);
	std::iota(sortedVec.begin(), sortedVec.end(), 0);	// or the plain code:
	//VecSizeT i = 0;
	//for (auto it = sortedVec.begin(), end = sortedVec.end(); it != end; ++it)
	//{
	//	*it = i++;
	//}

	// Create generator to generate random numbers between [0, N)
	std::uniform_int_distribution<VecType> distrib(0, N);
	std::default_random_engine engine(/*seed = */ 5768);
	auto generator = std::bind(distrib, engine);	// or get random value by calling distrib(engine)

	const VecSizeT nrTests = N;
	for (VecSizeT i = 0; i < nrTests; i++)
	{
		const VecType test_value = generator();
		std::cout << "Searching for: " << test_value << "\n";

		// binary search (just for reference)
		const VecSizeT b_index = Test<BinarySearchT, VecType, VecSizeT>(sortedVec, test_value);
		
		// parallel search
		const VecSizeT p_index = Test<ParallelSearch1T, VecType, VecSizeT>(sortedVec, test_value);

		if (b_index != p_index)
		{
			break;
		}

		std::cout << "\n";
	}

	system("pause");	// Adds a "Press any key.." to your console
	return 0;
}
