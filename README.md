# Boolean Equation Minimization
## Quine-McClusky Algorithm
The QMA works by iteratively grouping similar minterms together until a set of maximally condensed minterms is produced. Then, by constructing an Essential Prime Implicants table, we identify which minterms from the condensed set are essential for covering all of the original minterms in the truth table. In some cases, the condensed minterms will not be able to cover all of the original minterms. Although there are more elegant solutions, this implementation simply solves for the optimal solution from this point using brute force (if necessary at all). This algorithm's complexity is exponential.

### Compile
```
g++ -std=c++11 -Wall -Wextra -o minimize main.cpp minimizer.cpp minterm.cpp
```

### Dependencies
```
    main.cpp

    minterm.cpp
    minterm.h

    minimizer.cpp
    minimizer.h
```

### Usage
```
./minimize [n-bit string] ... where n is a power of 2
```
Note: presently, don't-care-values (X's) are not supported -- only use 1's and 0's.
### Example
```
./minimize 00110011111100000000010101110100
```
is equivalent to solving the truth table where the input string above is the same as the output column in the table below: 
```
Truth Table (5 variables)
0 0 0 0 0 | 0
0 0 0 0 1 | 0
0 0 0 1 0 | 1
0 0 0 1 1 | 1
0 0 1 0 0 | 0
0 0 1 0 1 | 0
0 0 1 1 0 | 1
0 0 1 1 1 | 1
0 1 0 0 0 | 1
0 1 0 0 1 | 1
0 1 0 1 0 | 1
0 1 0 1 1 | 1
0 1 1 0 0 | 0
0 1 1 0 1 | 0
0 1 1 1 0 | 0
0 1 1 1 1 | 0
1 0 0 0 0 | 0
1 0 0 0 1 | 0
1 0 0 1 0 | 0
1 0 0 1 1 | 0
1 0 1 0 0 | 0
1 0 1 0 1 | 1
1 0 1 1 0 | 0
1 0 1 1 1 | 1
1 1 0 0 0 | 0
1 1 0 0 1 | 1
1 1 0 1 0 | 1
1 1 0 1 1 | 1
1 1 1 0 0 | 0
1 1 1 0 1 | 1
1 1 1 1 0 | 0
1 1 1 1 1 | 0
```
Result:
```
A!BCE+B!CD+!AB!C+AB!DE+!A!BD
```
