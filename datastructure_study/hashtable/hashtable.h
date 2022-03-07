//
// Created by 付文欣 on 2021/11/3.
//
#include <vector>
#include <utility>
#ifndef HASHTABLE_HASHTABLE_H
#define HASHTABLE_HASHTABLE_H

static const int _stl_num_primes = 28;
static const unsigned long _stl_prime_list[_stl_num_primes] =
{
        53,         97,           193,         389,       769,
        1543,       3079,         6151,        12289,     24593,
        49157,      98317,        196613,      393241,    786433,
        1572869,    3145739,      6291469,     12582917,  25165843,
        50331653,   100663319,    201326611,   402653189, 805306457,
        1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long _std_next_prime(unsigned long n){
    for(int i=0; i<_stl_num_primes; ++i)
        if(n < _stl_prime_list[i])
            return _stl_prime_list[i];
    return _stl_prime_list[_stl_num_primes - 1];
}

//hash table 的基本结点
template<class Value>
class _hashtable_node{
public:
    _hashtable_node* next;
    Value val;
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable;

//hash table 的迭代器
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
struct _hashtable_iterator{
    typedef hashtable<Value,Key,HashFcn,ExtractKey,EqualKey> hashtable;
    typedef _hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey> iterator;
    typedef _hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey> const_iterator;
    typedef _hashtable_node<Value> node;
    
    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node *cur;
    hashtable* ht;

    _hashtable_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}
    _hashtable_iterator() {}
    reference operator*() const { return cur->val; }
    pointer &operator->() const { return &(operator*()); }
    iterator &operator++();
    iterator operator++(int);
    bool operator==(const iterator &it) const { return cur == it->cur; }
    bool operator!=(const iterator &it) const { return cur != it->cur; }
};


template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable{
public:
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef Value value_type;
    typedef Key key_type;
    typedef _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;
    typedef _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> const_iterator;
private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef _hashtable_node<Value> node;

    std::vector<node *> buckets;
    size_type num_elements;
    void initialize_buckets(size_type);
    size_type bkt_num(const value_type &obj, size_type n) const{
        return bkt_num_key(get_key(obj), n);
    }

    size_type bkt_num(const value_type &obj) const{
        return bkt_num_key(get_key(obj));
    }
    size_type bkt_num_key(const key_type &key, size_type n) const{
        return hash(key) % n;
    }
    size_type bkt_num_key(const key_type &key) const{
        return bkt_num_key(key, bucket_count());
    }

    void resize(size_type new_size);

public:
    hashtable(size_type n, const HashFcn &hf, const key_equal &eql)
            : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0){
    }

    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const { return _stl_prime_list[_stl_num_primes - 1];}

    node *new_node(const value_type &obj){
        node *tmp = new node();
        tmp->next = 0;
        tmp->val = obj;
        return tmp;
    }

    void delete_node(node *n){
        delete &(n->val);
        delete n;
    }

public:
    std::pair<iterator, bool> insert_unique(const value_type &obj);
    std::pair<iterator, bool> insert_unique_noresize(const value_type &obj);

    iterator insert_equal(const value_type &obj);
    iterator insert_equal_noresize(const value_type &obj);

    iterator find(const key_type &obj);
    size_type count(const key_type &obj);
    size_type size() const { return num_elements; }
    int elems_in_buckets(const int i);
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
int hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::elems_in_buckets(const int i) {
    int tmp = 0;
    node *n = buckets[i];
    while(n){
        tmp ++;
        n = n->next;
    }
    return tmp;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> &
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++() {
    const node *old = cur;
    cur = cur->next;
    if( !cur ){
        size_type loc = ht->bkt_num(old->val);
        while(!cur && ++loc < ht->bucket_count())
            cur = ht->buckets[loc];
    }
    return *this;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>
_hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++(int) {
    iterator tmp = *this;
    ++ (*this);
    return tmp;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::initialize_buckets(size_type n) {
    const size_type bucket_n = _std_next_prime(n);
    buckets.reserve(bucket_n);
    buckets.insert(buckets.end(), bucket_n, (node *)0);
    num_elements = 0;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_equal(const value_type &obj) {
    resize(num_elements + 1);
    return insert_equal_noresize(obj);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_equal_noresize(const value_type &obj) {
    const size_type new_loc = bkt_num(obj);
    node *first = buckets[new_loc];
    while(first){
        if(first->val == obj){
            node *tmp = new_node(obj);
            tmp->next = first->next;
            first->next = tmp;
            num_elements ++;
            return std::pair<iterator, bool> (iterator(first, this), true);
        }
        first = first->next;
    }
    node *tmp = new_node(obj);
    tmp->next = buckets[new_loc];
    buckets[new_loc] = tmp;
    ++ num_elements;
    return std::pair<iterator, bool>(iterator(tmp, this), true);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_unique(const value_type &obj) {
    resize(num_elements + 1);
    return insert_unique_noresize(obj);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_unique_noresize(const value_type &obj) {
    const size_type new_loc = bkt_num(obj);
    node *first = buckets[new_loc];
    while(first){
        if(first->val == obj)
            return std::pair<iterator, bool>(iterator(first, this), false);
        first = first->next;
    }
    node *tmp = new_node(obj);
    tmp->next = buckets[new_loc];
    buckets[new_loc] = tmp;
    ++ num_elements;
    return std::pair<iterator, bool>(iterator(tmp, this), true);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::resize(size_type new_size) {
    const size_type old_size = bucket_count();
    if(new_size > old_size){
        const size_type n = _std_next_prime(old_size);
        std::vector<node *> tmp (n, (node *)0);
        for(size_type i = 0; i < old_size; ++ i){
            node *first = buckets[i];
            while(first) {
                size_type new_loc = bkt_num(first->val, new_size);
                buckets[i] = first->next;
                first->next = tmp[new_loc];
                tmp[new_loc] = first;
                first = buckets[i];
            }
        }
        buckets.swap(tmp);
    }
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::find(const key_type &key) {
    size_type loc = bkt_num_key(key);
    node *n = buckets[loc];
    while(n){
        if(equals(get_key(n->val), key))
            break;
        n = n->next;
    }
    return iterator (n, this);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::count(const key_type &key) {
    size_type loc = bkt_num_key(key);
    node *n = buckets[loc];
    int result = 0;
    while(n){
        if(equals(get_key(n->val), key))
            result ++;
        n = n->next;
    }
    return result;
}

#endif //HASHTABLE_HASHTABLE_H