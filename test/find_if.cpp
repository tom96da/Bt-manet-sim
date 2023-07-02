/*!
 * @file find_if.cpp
 * @author tom96da
 * @brief map と vector でどちらが find_if が速いか
 * @date 2023-07-02
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>

enum class Attr { NONE, TRUE, FALSE };

class MyClass {
   private:
    int id_;
    Attr attr_;

   public:
    MyClass(int id, Attr attr = Attr::NONE) : id_{id}, attr_{attr} {};
    int getId() const { return id_; }
    Attr getAttr() const { return attr_; }
    void setAttr(const Attr attr) { attr_ = attr; }
};

int main() {
    std::vector<MyClass> v;
    std::map<int, MyClass> m;
    int count = 0;

    for (int i = 0; i < 10000; i++) {
        v.emplace_back(i);
        m.emplace(i, MyClass{i});
        if (i > 6600) {
            v.at(i).setAttr(Attr::TRUE);
            // m.at(i).setAttr(Attr::TRUE);
        }
        if (i % 3 == 0) {
            m.at(i).setAttr(Attr::TRUE);
        }
    }

    auto start = std::chrono::system_clock::now();

    while (true) {
        auto itr = std::find_if(v.rbegin(), v.rend(), [](const MyClass& my) {
            return my.getAttr() == Attr::TRUE;
        });
        if (itr == v.rend()) {
            break;
        }

        auto& my = *itr;
        my.setAttr(Attr::FALSE);
        count++;
    }

    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    auto msec =
        std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    std::cout << msec << "ms, count: " << count << std::endl;

    count = 0;
    start = std::chrono::system_clock::now();

    while (true) {
        auto itr = std::find_if(m.rbegin(), m.rend(),
                                [](const std::pair<int, MyClass> p) {
                                    auto [_, my] = p;
                                    return my.getAttr() == Attr::TRUE;
                                });
        if (itr == m.rend()) {
            break;
        }

        auto& [_, my] = *itr;
        my.setAttr(Attr::FALSE);
        count++;
    }

    end = std::chrono::system_clock::now();
    dur = end - start;
    msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    std::cout << msec << "ms, count: " << count << std::endl;
    return 0;
}
