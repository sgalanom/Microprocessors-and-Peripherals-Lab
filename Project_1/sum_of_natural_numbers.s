    .global sum_of_natural_numbers
    .p2align 2
    .type sum_of_natural_numbers, %function
    
sum_of_natural_numbers:
    .fnstart
    
    PUSH {r0, r1, lr}                   // Save the values of registers r0, r1, and lr onto the stack
    MOV r1, r0                          // Move the value of 'n' into register r1
    MOV r0, #0                          // Initialize result to 0
	loop_start:
	ADDS r0, r0, r1                    // Add r1 to r0 and store the result in r0
    SUBS r1, r1, #1                    // Decrement r1 by 1
    CMP r1, #0                         // Compare r1 with 0
    BGT LOOP_START                     // Branch if greater than to LOOP_START if r1 is greater than 0
    BX lr                               // Return by branching to the address in lr
    POP {r0, r1, pc}                    // Restore the values of registers r0, r1, and pc (program counter)
    
    .fnend
