/*!
 * @file set_map.cpp
 * @author tom96da
 * @brief setとmapの最大値取得
 * @date 2023-05-30
 */

#include <set>
#include <map>

using namespace std;

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
    return 0;
}