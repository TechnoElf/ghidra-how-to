#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool check_passcode(const char *passcode) {
    return strcmp(passcode, "1337") == 0;
}

int main(void) {
    printf("Enter passcode: ");

    char passcode[5];
    if (!scanf("%4s", passcode)) {
        printf("\nInvalid input\n");
        return 1;
    }

    if (!check_passcode(passcode)) {
        printf("Incorrect passcode\n");
        return 1;
    }
    printf("Correct passcode\n");
    return 0;
}
