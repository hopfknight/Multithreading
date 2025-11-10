#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <numeric>

void find_sum(std::vector<int> *elements, long long *result, int start, int end)
{
    long long sum = 0;
    for (int i = start; i < end; i++)
        sum += (*elements)[i];
    *result = sum;
}

int main()
{
    int size = 1000000;
    std::vector<int> elements(size);

    for (int i = 0; i < size; i++)
        elements[i] = std::rand() % 100;

    auto t1_start = std::chrono::high_resolution_clock::now();

    long long sum = std::accumulate(elements.begin(), elements.end(), 0LL);

    auto t1_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time1 = t1_end - t1_start;

    std::cout << "Сумма(однопоточная): " << sum << " за " << time1.count() << " мс\n";

    auto t2_start = std::chrono::high_resolution_clock::now();

    long long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int quarter = size / 4;

    std::thread t1(find_sum, &elements, &sum1, 0, quarter);
    std::thread t2(find_sum, &elements, &sum2, quarter, 2 * quarter);
    std::thread t3(find_sum, &elements, &sum3, 2 * quarter, 3 * quarter);
    std::thread t4(find_sum, &elements, &sum4, 3 * quarter, size);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    long long total = sum1 + sum2 + sum3 + sum4;

    auto t2_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time2 = t2_end - t2_start;

    std::cout << "Сумма(многопоточная): " << total << " за " << time2.count() << " мс\n";

    double speed = time1.count() / time2.count();
    std::cout << "Ускорение: " << speed << "x\n";

    return 0;
}
