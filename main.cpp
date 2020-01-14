#include <iostream>
#include <vector>
#include "simplealloc.hpp"
using namespace std;

int main()
{
    vector<int, jw::allocator<int>> vi;
    vi.push_back(3);
    for (const auto& i : vi) {
        cout << i << " ";
    }
    
    // test union
    union obj
    {
        union obj* next;
        char data[1];
    };
    union obj a1;
    union obj* a2;
    cout << "sizeof: obj " << sizeof(a1) << " obj* " << sizeof(a2) << endl;
    char mem[2]{ 0 };
    cout << sizeof(mem) << " " << sizeof(mem + 1) << endl;
    cout << (void*)&mem << "\n" << (void*)&(mem[0]) << "\n" << (void*)&(mem[1]);
    return 0;
}
