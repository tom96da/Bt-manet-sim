#include <iostream>
#include <algorithm>
#include <array>
#include <thread>
#include <chrono>

int main()
{
    std::array<bool, 100> v{false};
    int max{100};
    auto t = std::thread(
        [&]()
        {
            std::cout << "\e[?25l";
            std::cout << "  0 1 2 3 4 5 6 7 8 9" << std::endl;
            int tmp{0};
            do
            {
                for (int i = 0; i < 10; i++)
                {
                    std::cout << i << " ";
                    for (int j = 0; j < 10; j++)
                    {
                        char s = (v[i * 10 + j] == 0) ? 'x' : 'o';
                        std::cout << s << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "\e[11A" << std::endl;
                tmp = std::count_if(v.begin(), v.end(), [](bool value)
                                    { return value; });
            } while (tmp < max);
            std::cout << "\e[10B\e[?25h";
        });

    for (int i = 0; i < 100; i++)
    {
        v[i] = i % 3 ? false : true;
    }
    int tmp{0}, k{0};
    do
    {
        v[k] += 1;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        k++;
        tmp = std::count_if(v.begin(), v.end(), [](bool value)
                            { return value; });
    } while (tmp < max);
    t.join();
    return 0;
}