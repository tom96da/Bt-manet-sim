#include <future>
#include <iostream>
#include <thread>
#include <vector>

int foo() {
    std::cout << "executes foo()\n";
    return 3;
}

int main() {
    std::vector<std::future<int>> future;
    int result = 0;
    for (int i = 0; i < 100; i++) {
        future.emplace_back(std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return 3;
        }));
    }

    for (auto &&f : future) {
        result += f.get();
    }

    std::cout << "foo() = " << result << std::endl;

    return 0;
}