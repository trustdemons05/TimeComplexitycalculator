#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>

// Original source code

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}


int main() {
    srand(time(NULL));

    // Test with size 100
    {
        int* arr;
        arr = new int[100];
        for(int i = 0; i < 100; i++) {
            arr[i] = rand() % 1000;
        }
        int n = rand() % 1000;

        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(arr, 100);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "SIZE:" << 100 << ",TIME:" << duration.count() << std::endl;

        delete[] arr;
    }

    // Test with size 500
    {
        int* arr;
        arr = new int[500];
        for(int i = 0; i < 500; i++) {
            arr[i] = rand() % 1000;
        }
        int n = rand() % 1000;

        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(arr, 500);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "SIZE:" << 500 << ",TIME:" << duration.count() << std::endl;

        delete[] arr;
    }

    // Test with size 1000
    {
        int* arr;
        arr = new int[1000];
        for(int i = 0; i < 1000; i++) {
            arr[i] = rand() % 1000;
        }
        int n = rand() % 1000;

        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(arr, 1000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "SIZE:" << 1000 << ",TIME:" << duration.count() << std::endl;

        delete[] arr;
    }

    // Test with size 5000
    {
        int* arr;
        arr = new int[5000];
        for(int i = 0; i < 5000; i++) {
            arr[i] = rand() % 1000;
        }
        int n = rand() % 1000;

        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(arr, 5000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "SIZE:" << 5000 << ",TIME:" << duration.count() << std::endl;

        delete[] arr;
    }

    // Test with size 10000
    {
        int* arr;
        arr = new int[10000];
        for(int i = 0; i < 10000; i++) {
            arr[i] = rand() % 1000;
        }
        int n = rand() % 1000;

        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(arr, 10000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "SIZE:" << 10000 << ",TIME:" << duration.count() << std::endl;

        delete[] arr;
    }

    return 0;
}
