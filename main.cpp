#include <iostream>
#include "jw_vector.hpp"

using std::cout;
using std::endl;

int main()
{
    jw::vector<int> vi;
    cout << vi.size() << vi.capacity() << endl;

    int a[] = { 1,2,3,4,5,6,7,8,9 };
    vi.push_back(3);
    vi.push_back(4);
    vi.push_back(5);
    jw::vector<int> vi2(8, 0);
    jw::vector<int> vi3(a, a + 9);
    for (const auto& i : vi) {
        cout << i << " ";
    }
    cout << endl;
    
    for (const auto& i : vi2) {
        cout << i << " ";
    }
    cout << endl;

    for (const auto& i : vi3)
        cout << i << " ";
    cout << endl;
    return 0;
}
