#include "RBTree.h"
#include <iostream>
using namespace std;

template <class T>
class identity{
public:
    const T& operator()(const T& x) const{
        return x;
    }
}; 

int main(){
    rb_tree<int, int, identity<int> ,less<int>> itree;
    cout << itree.size() << endl;
    itree.insert_unique(10);
    itree.insert_unique(7);
    itree.insert_unique(8);


    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);

    itree.insert_unique(13);
    itree.insert_unique(12);

    itree.insert_unique(12);

    for(auto it = itree.begin(); !(it == itree.end()) ; ++it)
        cout << *it << '(' << rb_tree_iterator_base(it).node_ptr->color << ")" << endl;
    
    cout << itree.size() << endl;
}