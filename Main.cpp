#define _CRT_SECURE_NO_WARNINGS	// used so VS will let me use printf peacefully...

#include <vector>
#include <iostream>
#include <random>	// used to generate random values to be searched
#include <functional>	// std::bind
#include <numeric>      // std::iota

#include "SearchAlgorithms.h"

#define USING_CHRONO_TO_MEASURE_DURATION

#ifdef USING_CHRONO_TO_MEASURE_DURATION

#include <chrono>

// measures the execution duration of a function using std::chrono
template<typename TimeT = std::chrono::duration<double>>
struct measure
{
	template<class F, typename RetType, typename ...Args>
	static typename TimeT::rep execution(F func, RetType& index_found, Args&&... args)
	{
		auto start = std::chrono::high_resolution_clock::now();
		
		// call Search()
		index_found = std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
		
		auto duration = std::chrono::duration_cast<TimeT>
			(std::chrono::high_resolution_clock::now() - start);

		return duration.count();
	}
};
#else
// measures the execution duration of a function using omp_get_wtime()
struct measure
{
	template<class F, typename RetType, typename ...Args>
	static double execution(F func, RetType& index_found, Args&&... args)
	{
		const double start = omp_get_wtime();

		// call Search()
		index_found = std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
		
		const double duration = omp_get_wtime() - start;
		
		return duration;
	}
};
#endif

/* wrapper classes for each Search Algorithm so there will exist a unique Test<>()
	instance for each of them so the statics inside Test won't be shared between instances. */
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
template<typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
struct BinarySearchT
{
	static inline const char* prettyName = "Binary Search";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::BinarySearch(arr, val);
	}
};

template<typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
struct LowerBoundT
{
	static inline const char* prettyName = "Lower Bound";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return std::distance(
			arr.begin(),
			std::lower_bound(arr.begin(), arr.end(), val));
	}
};

template<typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
struct ParallelSearchT
{
	static inline const char* prettyName = "Parallel Search";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::ParallelSearch(arr, val);
	}
};

template<typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
struct ShorterParallelSearchT
{
	static inline const char* prettyName = "Shorter Par Search";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::ShorterParallelSearch(arr, val);
	}
};

template<typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
struct BinarySearchInParallelT
{
	static inline const char* prettyName = "Bin Search In Par";

	static VecSizeT Search(const std::vector<VecType>& arr, VecType val)
	{
		return SearchAlgorithms::BinarySearchInParallel(arr, val);
	}
};
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

/* Receives a wrapper class for each algorithm type as template parameter of form:
template<typename VecType, typename VecSizeT> such that the compiler will create an unique
instance of this function so the statics inside Test won't be shared between instances. */
template< template<typename, typename> class SearchT,
	typename VecType, typename VecSizeT = std::vector<VecType>::size_type>
VecSizeT Test(const std::vector<VecType>& arr, const VecType& val)
{
	static double avg_duration = 0.;
	static VecSizeT calls_count = 0;

	VecSizeT index_found;

#ifdef USING_CHRONO_TO_MEASURE_DURATION
	const double duration = measure<>::execution(
		SearchT<VecType, VecSizeT>::Search, index_found, arr, val);
#else
	const double duration = measure::execution(
		SearchT<VecType, VecSizeT>::Search, index_found, arr, val);
#endif

	avg_duration = ((avg_duration * calls_count) + duration) / (calls_count + 1);
	calls_count++;

	printf("%20.20s  |  %12.llu  |  %.9f  |  %.9f\n",
		SearchT<VecType, VecSizeT>::prettyName, index_found, duration, avg_duration);

	return index_found;
}

int main()
{
	// These settings take ~12GB of your RAM (N*sizeof(VecType))
	using VecType = unsigned long;
	using VecSizeT = std::vector<VecType>::size_type;
	constexpr VecSizeT N = 1'000'000'000ul;// std::numeric_limits<unsigned long>::max() - 1; // ~4'000'000'000;
	constexpr VecType START_VALUE = 0;//-static_cast<VecType>(N)/2;

	// Create Sorted vector of form: {START_VALUE, ..., 0, 1, 2, ..., START_VALUE + N-1}
	std::vector<VecType> sortedVec(N);
	std::iota(sortedVec.begin(), sortedVec.end(), START_VALUE);

	// Create generator to generate random numbers between [START_VALUE, START_VALUE + N)
	std::uniform_int_distribution<VecType> distrib(START_VALUE, START_VALUE + static_cast<VecType>(N));
	std::default_random_engine engine(/*seed = */ 5768);
	auto generator = std::bind(distrib, engine);	// or get random value by calling distrib(engine)

	const VecSizeT nrTests = N;
	for (VecSizeT i = 0; i < nrTests; i++)
	{
		const VecType test_value = generator();
		printf("Searching for: %12.lu | found at |    in    (seconds)   avg\n", test_value);

		// binary search
		const VecSizeT b_index = Test<BinarySearchT, VecType>(sortedVec, test_value);

		// std::lower_bound
		const VecSizeT lb_index = Test<LowerBoundT, VecType>(sortedVec, test_value);

		// parallel search
		const VecSizeT p_index = Test<ParallelSearchT, VecType>(sortedVec, test_value);

		// parallel search improved??
		const VecSizeT i_index = Test<ShorterParallelSearchT, VecType>(sortedVec, test_value);

		// binary search in parallel (just curious)
		const VecSizeT bp_index = Test<BinarySearchInParallelT, VecType>(sortedVec, test_value);

		// if one of the algorithms returns a different index then stop
		if (b_index != p_index || p_index != i_index || i_index != bp_index || bp_index != lb_index)
		{
			break;
		}

		std::cout << "\n";
	}

	system("pause");	// Adds a "Press any key.." to your console
	return 0;
}
