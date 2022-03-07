#include "RBTree.h" 

/*
本红黑树有额外的根节点 end/start：
root->parent == end
end->left == minimum
end->right = maximum
*/
void rb_tree_iterator_base::increment(){
    if (node_ptr->right != 0){
        node_ptr = node_ptr->right;
        while(node_ptr->left != 0)
            node_ptr = node_ptr->left;
    }
    else{
        rb_tree_base_node *p = node_ptr->parent;
        if(p->right == node_ptr){
            p = p->parent;
            node_ptr = node_ptr->parent;
            while(p->right == node_ptr){
                p = p->parent;
                node_ptr = node_ptr->parent;
            }
        }
        if(p->right != node_ptr)  //不满足该条件则指向了end
            node_ptr = p;
    }
}

void rb_tree_iterator_base::decrement(){
    if (node_ptr->left != 0){
        node_ptr = node_ptr->left;
    }
    else{
        rb_tree_base_node *p = node_ptr->parent;
        if(p->right == node_ptr){
            node_ptr = p;
        }
        else{
            node_ptr = p;
            p = node_ptr->parent;
            while(p->left == node_ptr){
                node_ptr = p;
                p = node_ptr->parent;
            }
            if(p->right == node_ptr)
                node_ptr = p;
        }
    }
}
