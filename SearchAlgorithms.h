#pragma once
#include <vector>

namespace SearchAlgorithms
{
	
constexpr unsigned int THREAD_COUNT = 4;
using Shbool = short;	// my reference-able bool

/*
val = 7
                     P1               P2         
arr = 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
c = right            ?                ?           left

			       P1 P2 
directions = right  ?  ?  left

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
template<typename VecType, typename VecSizeT/* = unsigned long long*/>
VecSizeT ParallelSearch(const std::vector<VecType>& arr, const VecType& val)
{
	VecSizeT l = 0;	// left most index
	VecSizeT r = arr.size() - 1; // right most index

	//const VecSizeT threadCount = omp_get_max_threads();
	constexpr unsigned int lastThrdID = THREAD_COUNT - 1;

	// each thread will tell if "val" is to their "right"
	std::array<Shbool, THREAD_COUNT + 1> isValToTheRight; // + 1 <=> the right border
	isValToTheRight.back() = false;	// last one will say no every time

	VecSizeT segm_size = (r - l + 1) / (THREAD_COUNT + 1);
	
	#pragma omp parallel default(none) shared(l, r, arr, segm_size, val, isValToTheRight)
	{
		const unsigned thrdID = omp_get_thread_num();

		Shbool& currThrdSaysItsRight = isValToTheRight[thrdID];
  const Shbool& nextThrdSaysItsRight = isValToTheRight[thrdID + 1];

		while (l < r)
		{
			const VecSizeT currThrdPos = l + segm_size * thrdID + 1;
			currThrdSaysItsRight = arr[currThrdPos] <= val;

			// waiting for every thread to finish checking if "val" is on their right
			#pragma omp barrier

			if (currThrdSaysItsRight != nextThrdSaysItsRight)
			{
				l = currThrdPos;
				if (thrdID != lastThrdID)
					r = currThrdPos + segm_size - 1;

				segm_size = (r - l + 1) / (THREAD_COUNT + 1);
			}

			#pragma omp barrier
		}
	}

	// now l == r

	// if "val" should've been here but it isn't
	if (arr[l] != val)
		return -1;

	return l;
}

template<typename VecType, typename VecSizeT/* = unsigned long long*/>
VecSizeT BinarySearchInParallel(const std::vector<VecType>& arr, const VecType& val)
{

}

template<typename VecType, typename VecSizeT/* = unsigned long long*/>
VecSizeT ParallelSearch1(const std::vector<VecType>& arr, const VecType& val)
{
	VecSizeT l = 0;	// left most index
	VecSizeT r = arr.size() - 1; // right most index

	//VecSizeT threadCount = omp_get_max_threads();

	// each thread will tell if "val" is to their "left" or to their "right"
	std::array<Shbool, THREAD_COUNT + 2> isValToTheRight; // + 2 <=> the 2 borders
	isValToTheRight.front() = true;
	isValToTheRight.back() = false;

	std::array<VecSizeT, THREAD_COUNT> thrdsPos;	// position for each thread

	#pragma omp parallel default(none) shared(l, r, thrdsPos, isValToTheRight, arr, val)
	{
		const unsigned thrdID = omp_get_thread_num();
		VecSizeT& currThrdPos = thrdsPos[thrdID];
		Shbool& currThrdSaysItsRight = isValToTheRight[thrdID + 1];
		const Shbool& prevThrdSaysItsRight = isValToTheRight[thrdID];
		const Shbool& nextThrdSaysItsRight = isValToTheRight[thrdID + 2];

		while (l < r)
		{
			const VecSizeT segm_size = (r - l + 1) / (THREAD_COUNT + 1) + 1;
			currThrdPos = l + segm_size * (thrdID + 1) - 1;
			currThrdSaysItsRight = (arr[currThrdPos] <= val);

			#pragma omp barrier

			if (currThrdSaysItsRight != prevThrdSaysItsRight)
			{
				r = currThrdPos - 1;
				if (thrdID != 0)
				{
					l = thrdsPos[thrdID - 1];
				}
			}

			#pragma omp single
			{
				if (isValToTheRight[THREAD_COUNT] != isValToTheRight[THREAD_COUNT + 1])
				{
					l = thrdsPos.back();
				}
			}
		}
	}

	// now l == r

	// if "val" should've been here but it isn't
	if (arr[l] != val)
		return -1;

	return l;
}

template<typename VecType, typename VecSizeT/* = unsigned long long*/>
VecSizeT BinarySearch(const std::vector<VecType>& arr, VecType val)
{
	VecSizeT l = 0, r = arr.size() - 1, m;
	while (l < r)
	{
		m = l + (r - l) / 2 + 1;
		if (arr[m] <= val)
		{
			l = m;
		}
		else
		{
			r = m - 1;
		}
	}

	// now l == r

	// if "val" should've been here but it isn't
	if (arr[l] != val)
		return -1;

	return l;
}

}

