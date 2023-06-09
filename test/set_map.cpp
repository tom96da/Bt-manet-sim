/*!
 * @file set_map.cpp
 * @author tom96da
 * @brief setとmapの習得
 * @date 2023-05-30
 */

#include <set>
#include <map>

using namespace std;

class MyClass
{
private:
    int id_;

public:
    MyClass(int id)
        : id_{id} {}

    bool operator<(const MyClass &right) const
    {
        return this->id_ < right.id_;
    }
};

int main()
{
    set<int> mySet{1, 5, 4, 2, 3};
    auto s = mySet.emplace(6);
    auto maxSet = *mySet.rbegin();

    map<int, double> myMap{
        {1, 3.1},
        {5, 1.5},
        {4, 4.2},
        {2, 1.1},
        {3, 4.0}};
    auto t = myMap.emplace(6, 7.4);
    auto maxMap = (*myMap.rbegin()).first;

    set<MyClass> myClassSet;
    myClassSet.emplace(1);
    myClassSet.emplace(3);

    map<MyClass, int> myClassMap;
    myClassMap.emplace(MyClass(1), 2);
    myClassMap.emplace(MyClass(3), 4);
    auto l = myClassMap.at(MyClass(3));

    return 0;
}