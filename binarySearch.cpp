#include <stdio.h>

// An iterative binary search function.
int binarySearch(int **arr, int low, int high, int x)
{
    while (low <= high) {
        int mid = low + (high - low) / 2;

        // Check if x is present at mid
        if (arr[mid][0] == x)
            return mid;

        // If x greater, ignore left half
        if (arr[mid][0] < x)
            low = mid + 1;

        // If x is smaller, ignore right half
        else
            high = mid - 1;
    }

    // If we reach here, then element was not present
    return -1;
}
 
