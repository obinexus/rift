#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void choose(int* choose_sq, int choose_len, const char** target_sq, int target_len) {
    bool* used = calloc(target_len, sizeof(bool));
    if (!used) {
        perror("Memory allocation failed");
        return;
    }

    for (int i = 0; i < choose_len; i++) {
        int index = choose_sq[i];
        int actual_index = -1;
        int skip_count = 0;

        // Find the actual un-used index
        for (int j = 0; j < target_len; j++) {
            if (!used[j]) {
                if (skip_count == index) {
                    actual_index = j;
                    break;
                }
                skip_count++;
            }
        }

        if (actual_index != -1) {
            printf("%s ", target_sq[actual_index]);
            used[actual_index] = true;
        }
    }

    printf("\n");
    free(used);
}

int main() {
    int choose_sq[] = {0, 1, 1};
    const char* target_sq[] = {"a", "b", "c", "d"};

    int choose_len = sizeof(choose_sq) / sizeof(choose_sq[0]);
    int target_len = sizeof(target_sq) / sizeof(target_sq[0]);

    choose(choose_sq, choose_len, target_sq, target_len);

    return 0;
}
