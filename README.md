# Hash-Tag-Counter-Using-Fibonacci-Heap
### Problem Statement:
We are required to implement a system to find the n most frequent hashtags that appear on Facebook or Twitter like social media.

### Implementation Idea:
The idea is to use a max priority structure like Fibonacci Heap to determine the most frequent hashtags. If there are millions of hashtags, the number of calls to the increase key functionality will be very high. Since, Fibonacci heap has an amortized complexity of O(1) for the increase key operation, it is best to implement it.

### Data structures used for the implementation:
1. **Max Fibonacci Heap**: Hashtag objects are stored ordered by the number of times it appeared in the input file. If there are multiple hashtags with the same number of times, we have chosen to keep the first appeared hashtag first.
2. **Hashmap**: A hashmap table to keep track of the hashtags that already appeared and to increase its value if it appears again later in the input.

### Execution of program:
1. The project is implemented in c++ language. Compiler used: g++.
2. Clone the repository.
3. Open command prompt and enter command **make** to execute the makefile and build the binary.
4. Execute the binary with the input file by running the command:
    * ./hashtagcounter *input_file_name* *output_file_name*.
5. Output is written into the output file name in the same directory.
