.global compute_single_digit            // Declare the function globally
.p2align 2                              // Align the function to a 4-byte boundary
.type compute_single_digit, %function   // Specify the type of the function

compute_single_digit:
    .fnstart                            // Start of the function

    PUSH {r1, r2, r3, r4, r5, r6, lr}   // Save the values of registers onto the stack
    MOV r1, r0                          // Move the value of 'number' into register r1
    CMP r1, #0                          // Check if number is negative
    BLT negative_case                   // If number is negative, jump to negative_case
    MOV r2, #0                          // Initialize sum to 0
    MOV r4, #10                         // Load the value 10 into register r4
    B continue_loop                     // Jump to continue the loop

negative_case:
    RSBS r1, r1, #0                    // Calculate the absolute value of r1 (number)
    MOV r2, #0                         // Initialize sum to 0
    MOV r4, #10                        // Load the value 10 into register r4

continue_loop:
    UDIV r3, r1, r4                    // Divide number by 10 and store the quotient in r3 (number /= 10)
    MUL r5, r3, r4                     // Multiply the quotient by 10 and store the result in r5 (quotient * 10)
    SUB r5, r1, r5                     // Subtract the result from the original value to get the remainder (number - quotient * 10)
    ADD r2, r2, r5                     // Add the remainder to sum (r2)
    CMP r1, #0                         // Check if quotient is not zero
    MOV r1, r3                         // Move the quotient into r1
    BNE continue_loop                  // If quotient is not zero, continue the loop
    
    CMP r2, #9                         // Check if sum > 9
    BLE end_loop                       // If sum <= 9, end the loop
    MOV r1, r2                         // Move the value of 'sum' into register r1
    MOV r2, #0                         // Reset sum to 0
    B continue_loop                    // Repeat the loop with the new sum

end_loop:
    MOV r0, r2                         // Move the final sum into output variable (r0)
    POP {r1, r2, r3, r4, r5, r6, pc}   // Restore the values of registers and return
    
    .fnend                             // End of the function
