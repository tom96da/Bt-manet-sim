/*!
 * @file finde.cpp
 * @author tom96da
 * @brief finde()のテスト
 * @date 2023-05-13
 */

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v{1, 2, 9, 7};

    auto it = std::find(v.begin(), v.end(), 7);
    if (it == v.end())
    {
        std::cout << "not found." << std::endl;
    }
    else
    {
        std::cout << "found: " << *it << std::endl;
    }
}
