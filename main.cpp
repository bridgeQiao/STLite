#include <iostream>
#include "jw_algorithm.hpp"
#include "jw_vector.hpp"
#include "jw_list.hpp"
#include "jw_deque.hpp"
#include "jw_stack.hpp"
#include "jw_queue.hpp"
#include "jw_rb_tree.hpp"
#include "jw_set.hpp"
#include "jw_map.hpp"
#include "jw_hashtable.hpp"

using std::cout;
using std::endl;

template<typename T>
void print(const T& x)
{
    for (const auto& i : x)
        std::cout << i << " ";
    std::cout << std::endl;
}

void test_vector();
void test_list();
void test_deque();
void test_rb_tree();
void test_hashtable();

int main()
{
    test_vector();
    test_list();
    test_deque();
    cout << "\n-----------------   Test stack & queue\n";
    jw::stack<int> istack;
    jw::queue<int> iqueue;
    istack.push(3);
    istack.push(4);
    iqueue.push(3);
    iqueue.push(4);
    printf("stack size: %d, queue size: %d\n", int(istack.size()), int(iqueue.size()));
    printf("stack top: %d, queue front: %d\n", istack.top(), iqueue.front());

    test_rb_tree();
    cout << "\n-----------------   Test set\n";
    jw::set<int> iset{ 3,7,9,4,6,7,8,2,3 };
    iset.insert(3);
    iset.insert(45);
    print(iset);
    auto elem = iset.begin();
    iset.erase(elem);
    print(iset);
    cout << "\n-----------------   Test map\n";
    jw::map<int, char> imap;
    imap.insert({ 1, 'a' });
    imap.insert({ 2,'b' });
    for (const auto& i : imap)
        cout << i.first << " " << i.second << endl;

    test_hashtable();
    return 0;
}

void test_vector() {
    cout << "\n-----------------   Test vector\n";
    jw::vector<int> vi;
    cout << vi.size() << vi.capacity() << endl;

    int a[] = { 1,2,3,4,5,6,7,8,9 };

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
}

void test_list() {
    cout << "\n-------------------   Test list\n";
    int a[] = { 1,2,3,4,5,6,7,8,9 };
    jw::list<int> ilist(a, a + 9);
    print(ilist);
    ilist.pop_back();
    print(ilist);
    ilist.reverse();
    print(ilist);
    auto pos = jw::find(ilist.begin(), ilist.end(), 3);
    cout << *pos << endl;
    jw::list<int> ilist2;
    ilist2.push_back(10);
    ilist2.push_back(20);
    ilist.splice(pos, ilist2);
    print(ilist);
    jw::list<int> ilists1{ 1,2,3,4,5,5,6,7 };
    const jw::list<int> ilists2{ 1,5,7,8,4,6,3,5 };
    //ilists2.sort();
    ////jw::list<int> ilists2{ 1,3,4,5,5,6,7,8 };
    //print(ilists2);
    //ilists1.merge(ilists2);
    //print(ilists1);
    //cout << ilists1.size();
    auto iter_list = ilists2.end();
}

void test_deque() {
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
}

template<typename T>
struct IDentity {
    const T& operator()(const T& x) { return x; }
};

void test_rb_tree() {
    cout << "\n-----------------   Test rb tree\n";
    jw::rb_tree<int, int, IDentity<int>, std::less<int>> itree;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(9);
    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);
    itree.insert_unique(8);
    print(itree);
    // test: 1. delete root( 9 ), test root
    // 2. delete 8 and 7(or 5), test while loop in rebalance
    auto iter = itree.find(8);
    itree.erase(iter);
    print(itree);
    iter = itree.find(5);
    cout << "The number is " << *iter << endl;
    itree.erase(iter);
    print(itree);

    jw::rb_tree<int, int, IDentity<int>, std::less<int>> itree2(itree);
    print(itree2);
}

void test_hashtable() {
    cout << "\n-----------------   Test hashtable\n";
    using value_type = std::pair<int, int>;
    jw::hashtable<int, int, std::hash<int>, jw::identity<int>, std::equal_to<int>> 
        iht(50, std::hash<int>(), std::equal_to<int>());
    iht.insert_unique(3);
    iht.insert_unique(5);
    print(iht);
}