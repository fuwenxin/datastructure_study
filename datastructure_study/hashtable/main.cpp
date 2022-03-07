#include <iostream>
#include <utility>
#include "hashtable.h"
using namespace std;

template <class T>
class identity{
public:
    const T& operator()(const T& x) const{
        return x;
    }
};

int main() {
    hashtable<int, int, hash<int>, identity<int>, equal_to<int>>
            ht_test(50, hash<int>(), equal_to<int>());
    cout << ht_test.bucket_count() << endl;
    cout << ht_test.max_bucket_count() << endl;
    cout << ht_test.size() << endl;

    for(int i=0; i<200;++i)
       ht_test.insert_unique(i * i);

    for(int i=0; i<100;++i)
        ht_test.insert_unique(i * i);

    for(int i=0;i<ht_test.bucket_count();++i)
        cout << "bucket[" << i << "] has " << ht_test.elems_in_buckets(i) << endl;

    cout << ht_test.size() << endl;
    cout << ht_test.bucket_count() << endl;
    cout << ht_test.count(16) << endl;
    return 0;
}
