/*!
 * @file hide_thread.cpp
 * @author tom96da
 * @brief LinuxとWindowsで並列処理の隠蔽
 * @date 2023-07-08
 */

#include <chrono>
#include <iostream>
#include <thread>

#ifdef __linux__
// Linux で include する記述
#elif defined _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace chrono_literals;

class MyClass {
   private:
    int n_;

#ifdef __linux__
    thread thread_;
#elif defined _WIN32
    struct ThreadData {
        int i;
        int& n;
        MyClass& myClass;
    };
    HANDLE threadHandle_;
#endif

   public:
    MyClass();
    ~MyClass();

    void print(int i, int& n);
    void close();
    int get();
};

MyClass::MyClass() {}

MyClass::~MyClass() {
#ifdef __linux__
    if (thread_.joinable()) {
        thread_.join();
    }
#elif defined _WIN32
    if (threadHandle_ != NULL) {
        CloseHandle(threadHandle_);
    }
#endif
}

void MyClass::print(int i, int& n) {
#ifdef __linux__
    thread_ = thread([&, i]() {
        while (n < 4) {
            cout << "\r" << i << ":" << n << flush;
            this_thread::sleep_for(0.1s);
        }
        cout << endl;
        n_ = i + n;

        return 0;
    });
#elif defined _WIN32
    threadHandle_ = CreateThread(
        NULL, 0,
        [](LPVOID lpparam) -> DWORD {
            ThreadData* param = reinterpret_cast<ThreadData*>(lpparam);
            int i = param->i;
            int& n = param->n;
            auto& self = param->myClass;

            while (n < 4) {
                cout << "\r" << i << ":" << n << flush;
                this_thread::sleep_for(0.1s);
            }
            cout << endl;
            self.n_ = i + n;

            delete param;

            return 0;
        },
        new ThreadData{i, n, *(this)}, 0, NULL);
#endif
}

void MyClass::close() {
#ifdef __linux__
    thread_.join();
#elif defined _WIN32
    WaitForSingleObject(threadHandle_, INFINITE);
    CloseHandle(threadHandle_);
    threadHandle_ = NULL;
#endif
}

int MyClass::get() { return n_; }

int main() {
    int i = 1;
    int n = 0;
    MyClass myClass{};

    myClass.print(i, n);

    for (; n < 4; n++) {
        this_thread::sleep_for(0.4s);
    }

    myClass.close();

    auto nn = myClass.get();
    cout << nn << endl;

    return 0;
}
