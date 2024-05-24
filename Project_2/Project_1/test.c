#include <stdio.h>

extern const int table[26];
extern int compute_hash(char *str);
extern int compute_single_digit(int hash);
extern int sum_of_natural_numbers(int n);


// Lookup table for the corresponding integers
const int table[26] = {10, 42, 12, 21, 7, 5, 67, 48, 69, 2, 36, 3, 19, 1, 14, 51, 71, 8, 26, 54, 75, 15, 6, 59, 13, 25};
	
int main() {
    // Define the string
    char str[] = "PouTaVitsos8"; //"Ar, PE 2!W";
		printf("%s\n", str);
    // Call assembly routine to compute the hash of the string
    int hash = compute_hash(str);

    // Print the hash value
    printf("Hash of the string: %d\n", hash);

    // Call assembly routine to compute the single digit of the hash
    int single_digit_hash = compute_single_digit(hash);

    // Print the single digit hash value
    printf("Single digit of the hash: %d\n", single_digit_hash);

    // Call assembly routine to compute sum_of_natural_numbers
    int sum_result = sum_of_natural_numbers(single_digit_hash);

    // Print the result of sum_of_natural_numbers
    printf("Result of sum_of_natural_numbers: %d\n", sum_result);

    return 0;
}

