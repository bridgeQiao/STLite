#include <iostream>
#include <algorithm>
#include <list>
#include "jw_vector.hpp"
#include "jw_list.hpp"
#include "jw_deque.hpp"
#include "jw_stack.hpp"
#include "jw_queue.hpp"
#include "jw_rb_tree.hpp"

using std::cout;
using std::endl;

template<typename T>
void print(const T& x)
{
    for (const auto& i : x)
        std::cout << i << " ";
    std::cout << std::endl;
}

void test_rb_tree();

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

    cout << "\n-------------------   Test list\n";
    jw::list<int> ilist(a, a + 9);
    print(ilist);
    ilist.pop_back();
    print(ilist);
    ilist.reverse();
    print(ilist);
    auto pos = std::find(ilist.begin(), ilist.end(), 3);
    cout << *pos << endl;
    jw::list<int> ilist2;
    ilist2.push_back(10);
    ilist2.push_back(20);
    ilist.splice(pos, ilist2);
    print(ilist);
    jw::list<int> ilists1 {1,2,3,4,5,5,6,7};
    jw::list<int> ilists2{ 1,5,7,8,4,6,3,5 };
    ilists2.sort();
    //jw::list<int> ilists2{ 1,3,4,5,5,6,7,8 };
    print(ilists2);
    ilists1.merge(ilists2);
    print(ilists1);
    cout << ilists1.size();

    cout << "\n-----------------   Test deque\n";
    jw::deque<int> ideque1;
    ideque1.push_back(1);
    print(ideque1);
    ideque1.push_front(9);
    ideque1.push_back(8);
    ideque1.push_back(3);
    ideque1.push_back(4);
    print(ideque1);
    ideque1.pop_back();
    ideque1.pop_front();
    ideque1.pop_front();
    print(ideque1);
    auto iter = ideque1.begin() + 1;
    ideque1.insert(iter, 10);
    print(ideque1);

    cout << "\n-----------------   Test stack & queue\n";
    jw::stack<int> istack;
    jw::queue<int> iqueue;
    istack.push(3);
    istack.push(4);
    iqueue.push(3);
    iqueue.push(4);
    printf("stack size: %d, queue size: %d\n", istack.size(), iqueue.size());
    printf("stack top: %d, queue front: %d\n", istack.top(), iqueue.front());

    cout << "\n-----------------   rb tree\n";
    test_rb_tree();
    return 0;
}

template<typename T>
struct IDentity {
    const T& operator()(const T& x) { return x; }
};

void test_rb_tree() {
    jw::rb_tree<int, int, IDentity<int>, std::less<int>> itree;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);
    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);
    print(itree);
    auto iter = itree.find(8);
    cout << "8 is " << *iter << endl;
    itree.erase(iter);
    print(itree);
}