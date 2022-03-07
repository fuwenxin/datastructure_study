#ifndef RBTREE
#define RBTREE
#include <utility>
#include <iostream>
#include <typeinfo>
using namespace std;

typedef bool rb_tree_color_type;
const rb_tree_color_type rb_tree_color_red = false;
const rb_tree_color_type rb_tree_color_black = true;

struct rb_tree_base_node{
    typedef rb_tree_color_type color_type;
    typedef rb_tree_base_node * link_type;

    color_type color;
    link_type parent;
    link_type left;
    link_type right;
    static link_type minimum(link_type x){
        while (x->left != 0) x = x->left; 
        return x;
    }

    static link_type maximum(link_type x){
        while (x->right != 0) x = x->right; 
        return x;
    }
};

template <class Value>
struct rb_tree_node: public rb_tree_base_node{
    typedef rb_tree_node<Value> *link_type;
    Value value_field;
};

struct rb_tree_iterator_base{
    rb_tree_base_node *node_ptr;
    void increment();
    void decrement();
};

template <class Value,class Ref,class Ptr> 
struct rb_tree_iterator: public rb_tree_iterator_base{
    typedef Value value_type;
    typedef Ref reference;
    typedef Ptr pointer;
    typedef rb_tree_iterator<Value, Value&, Value*> iterator;
    typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef rb_tree_iterator<Value, Ref, Ptr> self;
    typedef rb_tree_node<Value> *link_type;
    rb_tree_iterator(){}
    rb_tree_iterator(link_type x){ node_ptr = x; }
    rb_tree_iterator(const iterator &it){ node_ptr = it.node_ptr; }

    reference operator*() const { return link_type(node_ptr)->value_field; }
    pointer operator->() const { return &(operator*()); }

    self &operator++(){ increment(); return *this;}
    self operator++(int){
        self tmp = *this;
        increment();
        return tmp;
    }

    self &operator--(){ decrement(); return *this;}
    self operator--(int){
        self tmp = *this;
        decrement();
        return tmp;
    }

    bool operator==(const iterator &it) const{
        return (operator*()) == (*it);
    }
};

// template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
template <class Key, class Value, class KeyOfValue, class Compare>
class rb_tree{
protected:
    typedef void* void_pointer;
    typedef rb_tree_base_node *base_ptr_type;
    typedef rb_tree_node<Value> rb_tree_node_type;
    // typedef simple_alloc<rb_tree_node_type, Alloc> rb_tree_node_allocator;
    typedef rb_tree_color_type color_type;
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef value_type *pointer;
    typedef const value_type* const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef rb_tree_node_type *link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;  
protected:
    // link_type get_node() { /* TO-DO */ }
    // void put_node(link_type p) { /* TO-DO */ }
    link_type create_node(const value_type& x){
        link_type tmp = new rb_tree_node<value_type>();
        tmp->value_field = x;
        return tmp;
    }

    link_type clone_node(link_type x){
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = 0;
        tmp->right = 0;
        return tmp;
    }

    void destroy_node(link_type p){
        delete &(p->value_field);
        delete p;
    }

protected:
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type &root() const{
        return (link_type &)(header->parent);
    }

    link_type &leftmost() const{
        return (link_type &)(header->left);
    }
    
    link_type &rightmost() const{
        return (link_type &)(header->right);
    }
    
    static link_type &left(link_type x){
        return (link_type &)(x->left);
    }

    static link_type &right(link_type x){
        return (link_type &)(x->right);
    }
    
    static link_type &parent(link_type x){
        return (link_type &)(x->parent);
    }

    static reference value(link_type x){
        return x->value_field;  
    }

    static const key_type &key(link_type x){
        return KeyOfValue()(value(x));  // KeyOfValue 仿函数，用于从实值value中取出键值key, （用户定义并传入的）
    }

    static color_type &color(link_type x){
        return (color_type &)(x->color);
    }

    static link_type &left(base_ptr_type x){
        return (link_type &)(x->left);
    }

    static link_type &right(base_ptr_type x){
        return (link_type &)(x->right);
    }
    static link_type &parent(base_ptr_type x){
        return (link_type &)(x->parent);
    }
    static reference value(base_ptr_type x){
        return ((link_type)x)->value_field;
    }
    static const key_type &key(base_ptr_type x){
        return KeyOfValue()(value((link_type)x));  // KeyOfValue 仿函数，用于从实值value中取出键值key, （用户定义并传入的）
    }
    static color_type &color(base_ptr_type x){
        return (color_type &)((link_type)x->color);
    }

    static link_type minimum(link_type x){
        return (link_type) rb_tree_base_node::minimum(x);
    }

    static link_type maximum(link_type x){
        return (link_type) rb_tree_base_node::maximum(x);
    }

public:
    typedef rb_tree_iterator<value_type, reference, pointer> iterator;

private:
    iterator _insert(base_ptr_type x, base_ptr_type y, const value_type &v); // 插入点为x，父节点为y，参数v为新值,即插入在x和y之间
    link_type _copy(link_type, link_type); 
    void _erase(link_type);
    void init(){
        header = new rb_tree_node<Value>();
        color(header) = rb_tree_color_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }

public:
    rb_tree(const Compare& comp = Compare()): node_count(0), key_compare(comp) {
        init();
    }
    ~rb_tree(){
        // clear();
        delete header;
    }
    rb_tree<Key, Value, KeyOfValue, Compare> & operator=(const rb_tree<Key, Value, KeyOfValue, Compare>);

public:
    Compare key_comp() const{return key_compare;}
    iterator begin(){ return leftmost();}
    iterator end(){ return header;}    
    bool empty()const{ return node_count == 0; }
    size_type size() const{ return node_count; }
    size_type max_size() const{ return size_type(-1); }

public:
    pair<iterator, bool> insert_unique(const value_type&);

    template<class InputIterator>
    void insert_unique(const InputIterator&, const InputIterator&);

    iterator insert_equal(const value_type&);

    template<class InputIterator>
    void insert_equal(const InputIterator&, const InputIterator&);

    iterator find(const Key &k);

};


inline void _rb_tree_rotate_left(rb_tree_base_node *x, rb_tree_base_node* &root){
    rb_tree_base_node *y = x->right;
    x->right = y->left;
    if(y->left){
        y->left->parent = x;
    }
    y->parent = x->parent;
    if(x == root){
        root = y;
    }
    else if(x == x->parent->left){
        x->parent->left = y;
    }
    else{    
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

inline void _rb_tree_rotate_right(rb_tree_base_node *x, rb_tree_base_node* &root){
    rb_tree_base_node *y = x->left;
    x->left = y->right;
    if(y->right){
        y->right->parent = x;
    }
    y->parent = x->parent;
    if(x == root){
        root = y;
    }
    else if(x == x->parent->right){
        x->parent->right = y;
    }
    else{    
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

inline void _rb_tree_rebalance(rb_tree_base_node* x, rb_tree_base_node* &root){
    x->color = rb_tree_color_red;
    while(x != root && x->parent->color == rb_tree_color_red){
        if(x->parent == x->parent->parent->left){
            rb_tree_base_node *y = x->parent->parent->right;
            if(y && y->color == rb_tree_color_red){
                x->parent->color = rb_tree_color_black;
                y->color = rb_tree_color_black;
                x->parent->parent->color = rb_tree_color_red;
                x = x->parent->parent;
            }
            else{
                if(x == x->parent->right){
                    x = x->parent;
                    _rb_tree_rotate_left(x, root);
                }
                x->parent->color = rb_tree_color_black;
                x->parent->parent->color = rb_tree_color_red;
                _rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        else{
            rb_tree_base_node *y = x->parent->parent->left;
            if(y && y->color == rb_tree_color_red){
                x->parent->color = rb_tree_color_black;
                y->color = rb_tree_color_black;
                x->parent->parent->color = rb_tree_color_red;
                x = x->parent->parent;
            }
            else{
                if(x == x->parent->left){
                    x = x->parent;
                    _rb_tree_rotate_right(x, root);
                }
                x->parent->color = rb_tree_color_black;
                x->parent->parent->color = rb_tree_color_red;
                _rb_tree_rotate_left(x->parent->parent, root);
            }
        }
    }
    root->color = rb_tree_color_black;
}

template <class Key, class Value, class KeyOfValue, class Compare>
template <class InputIterator>
void rb_tree<Key, Value, KeyOfValue, Compare>::insert_equal(const InputIterator &first, const InputIterator &last){
    InputIterator it = first;
    while(it != last){
        insert_equal(*it);
        it ++;
    }
}

template  <class Key, class Value, class KeyOfValue, class Compare>
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator rb_tree<Key, Value, KeyOfValue, Compare>::insert_equal(const Value &v){
    link_type y = header;
    link_type x = root();
    while( x != 0 ){
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    return _insert(x, y, v);
}

template <class Key, class Value, class KeyOfValue, class Compare>
template <class InputIterator>
void rb_tree<Key, Value, KeyOfValue, Compare>::insert_unique(const InputIterator &first, const InputIterator &last){
    InputIterator it = first;
    while(it != last){
        insert_unique(*it);
        it ++;
    }
}

template  <class Key, class Value, class KeyOfValue, class Compare>
pair <typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator, bool> rb_tree<Key, Value, KeyOfValue, Compare>::insert_unique(const Value &v){
    link_type y = header;
    link_type x = root();
    bool cmp = true;
    while( x != 0 ){
        y = x;
        cmp = key_compare(KeyOfValue()(v), key(x)); 
        x = cmp ? left(x) : right(x);
    }
    iterator it = iterator(y);
    if(cmp)
        if(it == begin())
            return pair<iterator, bool>(_insert(x, y, v), true);
        else
            it--;
    if(key_compare(key(it.node_ptr), KeyOfValue()(v)))    
        return pair<iterator, bool>(_insert(x, y, v), true);
    return pair<iterator, bool>(it, false);
}

template  <class Key, class Value, class KeyOfValue, class Compare>
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare>::_insert(base_ptr_type x, base_ptr_type y, const value_type &v){
    link_type _x = (link_type)x;
    link_type _y = (link_type)y;
    link_type z;
    if(_y == header || _x != 0 || key_compare(KeyOfValue()(v), key(_y))){
        z = create_node(v);
        left(_y) = z;
        if(_y == header){
            root() = z;
            rightmost() = z;
        }
        else if(_y == leftmost()){
            leftmost() = z;
        }
    }
    else{
        z = create_node(v);
        right(_y) = z;
        if(_y == rightmost())
            rightmost() = z;
    }
    parent(z) = _y;
    left(z) = 0;
    right(z) = 0;
    _rb_tree_rebalance(z, header->parent);
    node_count ++;
    return iterator(z);
}

template <class Key, class Value, class KeyOfValue, class Compare>
typename rb_tree<Key, Value, KeyOfValue, Compare>::iterator
rb_tree<Key, Value, KeyOfValue, Compare>::find(const Key &k) {
    link_type y = header;
    link_type x = root();
    while(x != 0)
        if(!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    iterator it = iterator(y);
    return (it == end() || key_compare(k, key(y)))? end() : it;
}

#endif