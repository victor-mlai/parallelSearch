### Tested with:
```
  using VecType = unsigned long;
  using VecSizeT = std::vector<VecType>::size_type;
  
  /*number of elements inside the sorted vector*/ 
  N = 1'000'000'000ul; // MAX is std::numeric_limits<unsigned long>::max() - 1 = 4'294'967'294; = 16GB of RAM (4'294'967'294numbers * 4bytes)
  
  START_VALUE = 0; // => sortedVec becomes:
  sortedVec = {0, 1, 2, ... N-1}  // => val X will always be found on index X, for any X in [0, N)
  
  seed = 5768; // for the number generator
``` 
This configuration uses 4GB of RAM

System used: 16GB DDR4 3200 MHz, AMD Ryzen 7 3700X - 16 logical processors (8 cores)

### Output (portion of it taken after letting it run for >10'000 iterations):
```
Searching for:    688379645 | found at |    in    (seconds)   avg
       Binary Search  |     688379645  |  0.000004400  |  0.000004400
         Lower Bound  |     688379645  |  0.000002400  |  0.000002400
     Parallel Search  |     688379645  |  0.000811100  |  0.000811100   // Here first search takes the most because omp initializes its memory pool
  Shorter Par Search  |     688379645  |  0.000022600  |  0.000022600   // Still uses omp but the threads are already initialized
   Bin Search In Par  |     688379645  |  0.000008700  |  0.000008700
      Cache Friendly  |     688379645  |  0.000016700  |  0.000016700

Searching for:    321494064 | found at |    in    (seconds)   avg
       Binary Search  |     321494064  |  0.000003700  |  0.000004050
         Lower Bound  |     321494064  |  0.000001300  |  0.000001850
     Parallel Search  |     321494064  |  0.000023900  |  0.000417500
  Shorter Par Search  |     321494064  |  0.000017000  |  0.000019800
   Bin Search In Par  |     321494064  |  0.000005000  |  0.000006850
      Cache Friendly  |     321494064  |  0.000015900  |  0.000016300

Searching for:     64345949 | found at |    in    (seconds)   avg
       Binary Search  |      64345949  |  0.000002900  |  0.000003667
         Lower Bound  |      64345949  |  0.000001700  |  0.000001800
     Parallel Search  |      64345949  |  0.000023500  |  0.000286167
  Shorter Par Search  |      64345949  |  0.000017700  |  0.000019100
   Bin Search In Par  |      64345949  |  0.000005000  |  0.000006233
      Cache Friendly  |      64345949  |  0.000016300  |  0.000016300
      
...


Searching for:    651847836 | found at |    in    (seconds)   avg
       Binary Search  |     651847836  |  0.000002100  |  0.000002400
         Lower Bound  |     651847836  |  0.000001300  |  0.000001230
     Parallel Search  |     651847836  |  0.000022000  |  0.000027174
  Shorter Par Search  |     651847836  |  0.000030500  |  0.000020482
   Bin Search In Par  |     651847836  |  0.000003800  |  0.000003564
      Cache Friendly  |     651847836  |  0.000016200  |  0.000019616

Searching for:    921062309 | found at |    in    (seconds)   avg
       Binary Search  |     921062309  |  0.000002200  |  0.000002400
         Lower Bound  |     921062309  |  0.000000800  |  0.000001230
     Parallel Search  |     921062309  |  0.000023400  |  0.000027174
  Shorter Par Search  |     921062309  |  0.000017300  |  0.000020482
   Bin Search In Par  |     921062309  |  0.000002800  |  0.000003564
      Cache Friendly  |     921062309  |  0.000015000  |  0.000019616

Searching for:    812075906 | found at |    in    (seconds)   avg
       Binary Search  |     812075906  |  0.000001800  |  0.000002400
         Lower Bound  |     812075906  |  0.000000800  |  0.000001230
     Parallel Search  |     812075906  |  0.000022500  |  0.000027174
  Shorter Par Search  |     812075906  |  0.000020300  |  0.000020482
   Bin Search In Par  |     812075906  |  0.000002300  |  0.000003564
      Cache Friendly  |     812075906  |  0.000015800  |  0.000019616
...
```
### Conclusion:
Lower Bound/Binary Search is still the best way to go.
Parallel search takes too much time because of all the syncronization between threads + the memory bandwidth used to transfer information between threads such as whether the value is to their left/right. Good thing that omp uses a Thread Pool in the background, because otherwise the times would have been 400x larger. (You can see that only the first iteration takes 400x more than the binary search. Afterwards it only takes ~0.000027000 +-0.000010000, this is why in the above snippet it converged to 0.000027174
