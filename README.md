### Tested with:
```
  using VecType = unsigned long;
  using VecSizeT = std::vector<VecType>::size_type;
  
  /*number of elements inside the sorted vector*/ 
  N = std::numeric_limits<unsigned long>::max() - 1; // = 4'294'967'294;
  
  START_VALUE = 0; // => sortedVec becomes:
  sortedVec = {0, 1, 2, ... N-1}  // => val X will always be found on index X, for any X in [0, N)
  
  seed = 5768; // for the number generator
``` 
This configuration uses 16GB of RAM (4'294'967'294numbers * 4bytes)

On a system with 32GB DDR3, i7-4790K @ 4GHz - 8 logical processors (4 cores)

### Output (portion of it taken after letting it run for >10'000 iterations):
```
...

Searching for: 2597163767
Binary Search found it at:         2597163767   in: 0.000002400 seconds avg : 0.000003187
Parallel Search found it at:       2597163767   in: 0.000015000 seconds avg : 0.000021596
Shorter Par Search found it at:    2597163767   in: 0.000010700 seconds avg : 0.000017240
Bin Search In Par found it at:     2597163767   in: 0.000002800 seconds avg : 0.000004866

Searching for: 1802179795
Binary Search found it at:         1802179795   in: 0.000002800 seconds avg : 0.000003187
Parallel Search found it at:       1802179795   in: 0.000015700 seconds avg : 0.000021596
Shorter Par Search found it at:    1802179795   in: 0.000011800 seconds avg : 0.000017240
Bin Search In Par found it at:     1802179795   in: 0.000006400 seconds avg : 0.000004866

Searching for: 93048401
Binary Search found it at:           93048401   in: 0.000004700 seconds avg : 0.000003187
Parallel Search found it at:         93048401   in: 0.000034300 seconds avg : 0.000021596
Shorter Par Search found it at:      93048401   in: 0.000029900 seconds avg : 0.000017240
Bin Search In Par found it at:       93048401   in: 0.000004300 seconds avg : 0.000004866

...
```
