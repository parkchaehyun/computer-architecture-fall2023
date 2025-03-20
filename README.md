# Computer Architecture Fall 2023  

These are my solutions to the assignments for **Computer Architecture (4190.308)**, taken in Fall 2023 at **Seoul National University**.  

- **Instructor:** [Jae W. Lee](https://iamjaelee.github.io/www/)  

- **Course Overview:** This course introduces the main components of a modern computer system, including the **instruction set, processor, and memory hierarchy**. Topics covered include **pipelining, caching, virtual memory, and parallel architectures**.  

## Programming Assignments  

### **Programming Assignment 1: Writing in Assembly**  
Manually implement given C source code in **32-bit RISC-V assembly**.  

- **Digit Sum** – Compute the sum of digits of two integers.  
- **Fibonacci Sequence** – Implement recursive Fibonacci computation.  
- **Binary Tree Sum** – Traverse a binary tree and compute the sum of all node values.  

### **Programming Assignment 2: Numbers and Bit Manipulations**  
Implement bitwise operations and floating-point manipulations in **C**, following strict constraints on allowed operators.  

- **Bit Manipulations:** Parity check, subtraction validity, two’s complement conversion, nibble reversal, bit masking, and division by 4 with overflow handling.  
- **Bit Counting:** Count leading zeros in a 32-bit integer.  
- **Floating-Point Operations:** Compute absolute value, cast float to int, and compare two floating-point values with NaN handling.  

Modified `bits.c`.  

### **Programming Assignment 3: 64-Bit RISC-V Pipeline**  
Implement and modify a **5-stage RISC-V pipeline** in **Verilog** to support control hazard resolution and data forwarding.  

- **Branch Instruction (BEQ):** Implement BEQ and resolve control hazards using a static **not-taken** branch predictor.  
- **Forwarding Unit:** Implement a forwarding unit to resolve **data hazards** and minimize stalls.  
- **Optimized Branch Decision:** Move branch decision from the **MEM stage** to the **ID stage** to improve efficiency.  

Modified `riscv-pipeline.v`, `control.v`, and `inst_decoder.v`. Tested in **Xilinx ISE or Vivado**.  

### **Programming Assignment 4: Cache Simulator**  
Implement a **2-level inclusive cache simulator** in **C** to analyze memory access patterns and eviction policies.  

- **Cache Structure:** Simulate an **L1 and L2 cache** with an **inclusive policy** and **write-back** mechanism.  
- **Eviction Policies:** Support **Least Recently Used (LRU)** and **First-In First-Out (FIFO)** strategies.  
- **Performance Analysis:** Track **cache hits, misses, and evictions** for both levels.  

Modified `csim.c` and validated results using provided **test traces and reference binaries**.  

### **Programming Assignment 5: MatMul Optimization**  
Optimize **matrix multiplication** for **C = A × B**, where:  

- **A** has dimensions **(m × k) = (1024 × 4096)**  
- **B** has dimensions **(k × n) = (4096 × 1024)**  
- **C** has dimensions **(m × n) = (1024 × 1024)**  

Modified `matmul.c`, implementing the following optimizations:  
- **Blocking:** Process submatrices of size **64×64** to improve cache efficiency.  
- **SIMD (SSE2):** Use **128-bit SIMD intrinsics** to perform **4 multiplications** in parallel.  
