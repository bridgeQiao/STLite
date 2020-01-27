#include <iostream>
#include "jw_vector.hpp"
#include "jw_list.hpp"

using std::cout;
using std::endl;

template<typename T>
void print(const T& x)
{
    for (const auto& i : x)
        std::cout << i << " ";
    std::cout << std::endl;
}

int main()
{
    jw::vector<int> vi;
    cout << vi.size() << vi.capacity() << endl;

    int a[] = { 1,2,3,4,5,6,7,8,9 };
    // test vector
    cout << "Test vector\n";
    vi.push_back(3);
    vi.push_back(4);
    vi.push_back(5);
    jw::vector<int> vi2(a, a + 9);
    print(vi);
    print(vi2);
    std::cout << "vi2: size capacity " << vi2.size() << " " << vi2.capacity() << endl;
    vi2.insert(vi2.begin(), 7, 3);
    print(vi2);
    vi2.pop_back();
    print(vi2);
    std::cout << "vi2: size capacity " << vi2.size() << " " << vi2.capacity() << endl;
    // test list
    cout << "Test list\n";
    jw::list<int> ilist(a, a + 9);
    print(ilist);
    ilist.pop_back();
    print(ilist);
    return 0;
}
