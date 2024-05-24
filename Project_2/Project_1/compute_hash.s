.extern table
.global compute_hash
.p2align 2
.type compute_hash, %function

compute_hash:
    .fnstart

    PUSH {r1, r2, r3, r4, r5, lr}    // Save the values of registers onto the stack
    MOV r1, r0                       // Move the address of 'str' into register r1
    MOV r2, #0                       // Initialize hash to 0
    LDR r3, =table                   // Load the address of the table into register r3

loop:
    LDRB r4, [r1], #1               // Load the byte from the memory location pointed by r1 into r4 and increment r1
    CMP r4, #0                       // Check if the byte is null terminator
    BEQ end_loop                     // If byte is null, end the loop
    CMP r4, #'a'                     // Check if byte is lowercase
    BLT check_upper                  // If byte is not lowercase, check if it's uppercase
    CMP r4, #'z'                     // Check if byte is within lowercase range
    BGT check_num                    // If byte is not within lowercase range, check if it's numerical
    SUB r4, r4, #'a'                 // Calculate the index by subtracting the ASCII value of 'a'
    LDR r5, [r3, r4, LSL #2]         // Load the corresponding value from the table
    ADD r2, r2, r5                   // Add the value to the hash
    B loop

check_upper:
    CMP r4, #'A'                     // Check if byte is uppercase
    BLT check_num                    // If byte is not uppercase, check if it's anything else
    CMP r4, #'Z'                     // Check if byte is within uppercase range
    BGT check_other                  // If byte is not within uppercase range, check if it's anything else
    SUB r4, #'A'                     // Calculate the index by subtracting the ASCII value of 'A'
    LDR r5, [r3, r4, LSL #2]         // Load the corresponding value from the table
    ADD r2, r2, r5                   // Add the value to the hash
    B loop

check_num:
    CMP r4, #'0'                     // Check if byte is numerical
    BLT check_other                  // If byte is not numerical, check if it's anything else
    CMP r4, #'9'                     // Check if byte is within numerical range
    BGT check_other                  // If byte is not within numerical range, check if it's anything else
    SUB r4, r4, #'0'                 // Convert ASCII to numerical value
    ADD r2, r2, r4                   // Add the number to the hash
    B loop

check_other:
    B loop                             // If byte is anything else, continue the loop

end_loop:
    MOV r0, r2                         // Move the hash into output variable (r0)
    POP {r1, r2, r3, r4, r5, pc}       // Restore the values of registers and return
    
    .fnend
