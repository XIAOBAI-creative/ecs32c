
// We are going to build a binary search tree with an explicit
// stack based iterator.  We are doing this as a "header-only"
// c++ implementation, as it is templated and therefore needs
// to be included to be used.  This provides basically the same
// high level functionality as std::map, that is, a key/value
// data store with a sorted order

// We need to include the following headers...

#include <memory>
#include <stack>
#include <utility>

#define HERE {std::cout << "IMPLEMENT HERE\n";}

// C++ require declaration before use, so we define
// our three classes here.
template <class K, class V>
class BinaryTree;
template <class K, class V>
class BinaryTreeIterator;
template <class K, class V>
class BinaryTreeNode;

// This iterator is returned for both start and
// end but only the start iterator matters, the end 
// iterator is effectively ignored.

// It is considered "undefined behavior" (that is,
// things are allowed to crash in obscure ways if)

// A: The iterator has a longer lifetime than the 
// underlying tree (translation: don't return the iterator)

// B: Nodes are added or removed from the tree
// (translation: its OK to use the iterator to change
// the data associated with keys using the iterator in a 
// for loop, but it is not OK to
// add new keys or remove keys)
template <class K, class V>
class BinaryTreeIterator : public std::input_iterator_tag
{
    friend class BinaryTree<K, V>;

    // The constructor.  For end you don't actually
    // need to do anything (the end is effectively a dummy).
    // For the start iterator, you should set current to root
    // and then call incr() so that the iterator is ready to 
    // actually start the traversal
    BinaryTreeIterator(BinaryTreeNode<K, V> *root, bool start)
    {
         if (start && root != nullptr) {//start by most left node if iterator point smallest(left) element and root node not empty
            current = root;//start for traver
            pushLeft(root);
    }
     else
        {
            current = nullptr;
        }
    }
public:
    // This should return TRUE if there is still
    // items left in the iteration:
    // Basically, check to see if the stack is empty
    // and current is null, and, if not, there is 
    // still more work to do...
    bool operator!=(BinaryTreeIterator<K, V> &other)
    {
         return current != other.current;//check if iterator continue or not
    }

    // This is the heart of the tree traversal algorithm.
    // It needs to be called once in the constructor and 
    // once by each invocation of the ++ operator.
    // The idea in english...
    //
    // While current is not null, push the current
    // pointer onto the stack and take current->left 
    // and make it the new current.
    // 
    // Once current is null, then take the top of the stack
    // and set it to current...
    void incr()
    {
        if (current == nullptr && !working_stack.empty())//if current is null and stack not empry,we take a node in stack and assign to current, then pop this node from stack
        //push all node to stack from current right node follow left path
        {
            current = working_stack.top();
            working_stack.pop();
            pushLeft(current->right);
        }
        else if (current != nullptr)
        {
            pushLeft(current->right);
        }
    }

    // This should just call incr
    void operator++()
    {
        incr();
    }

    // And this visits the node itself...
    // In visiting the node, it should 
    // both set current to be the current node's right
    // and then return a pair constructed from the old 
    // current node's key and value.
    std::pair<K, V> operator*()
    {

        // This isn't actually what you want to return,
        // its just a placeholder so the compiler doesn't complain.
        return std::pair<K,V>(current->key, current->value);
    }

private:
    void pushLeft(BinaryTreeNode<K, V> *node)//node and its left node push to stack
        {
            while (node != nullptr)
            {
                working_stack.push(node);
                node = node->left;
            }
            if (!working_stack.empty())
            {
                current = working_stack.top();
                working_stack.pop();
            }
            else
            {
                current = nullptr;
            }
        }
    // A pointer to the current node
    BinaryTreeNode<K, V> *current;

    // And a stack for the traversal visit of the tree
    // nodes.
    std::stack<BinaryTreeNode<K, V> *> working_stack;
};


// The class for the binary tree itself.
template <class K, class V>
class BinaryTree
{
public:
    BinaryTree() : root(nullptr)
    {
    }
    
    // The [] operation is for both getting and setting.
    // If the key exists in the tree a reference to the
    // associated value is returned.  Otherwise, it will
    // create a new tree node and return that value.

    // What you should do is first check if the root is null
    // and, if so, just create a new node.
    // then simply invoke root->find(key) and return
    // that value as find is already set up to return
    // a reference.
    V &operator[](const K &key)
    {
 
        // THis is just to keep the compiler happy
        // so your code compiles, this is not what you
        // actually want to return
         {
        root = new BinaryTreeNode<K, V>(key);
        return root->value;
        }
        else
        {
        root = root->find(key);
        return root->find(key)->value;
        }
    }

    // This should return false if the tree
    // is empty, otherwise return root->contains
    bool contains(const K &key)
    {
      if (root == nullptr)
        {
            return false;
        }
        else
        {
            return root->contains(key);
        }
    }
    

    // Erases a node if a key matches.  If the
    // key does not match it simply is an operation
    // that does nothing.
    void erase(const K &key)
    {
        if (root != nullptr)
        {
            root = root->erase(key);
        }
    }

    // And the destructor for the binary tree.
    // In order to prevent memory leaks and keep with
    // the C++ "RAII" convention, it should see
    // if there is a root and, if so, call freetree()
    // on the root.
    ~BinaryTree()
    {
          if (root != nullptr)
        {
            root->freetree();
            root = nullptr;
        }
    }

    // This returns the iterators.
    BinaryTreeIterator<K, V> begin()
    {
        return BinaryTreeIterator(root, true);
    }
    BinaryTreeIterator<K, V> end()
    {
        return BinaryTreeIterator(root, false);
    }

protected:
    BinaryTreeNode<K, V> *root;
};

// And the class for the binary tree node itself.
template <class K, class V>
class BinaryTreeNode
{
    friend class BinaryTree<K, V>;
    friend class BinaryTreeIterator<K, V>;

public:
    // The constructor, it simply setts the key and the left/right pointers.
    // Data defaults to whatever the default value is for the data type.
    BinaryTreeNode(const K &keyin) : key(keyin), left(nullptr), right(nullptr),height(1)//height for avl
    {
    }

    // This should recursively free the tree.
    // It should call freetree on left and 
    // right and then, as the last act,
    // call delete on this.  

    // Yes, you can "suicide" an object in C++,
    // and this is a case where you want to do it.
    void freetree()
    {
         if (left != nullptr)
        {
            left->freetree();
            left = nullptr;
        }
        if (right != nullptr)
        {
            right->freetree();
            right = nullptr;
        }delete this;
    }

protected:
    int getHeight(BinaryTreeNode<K, V> *node)//if node not nullptr then return height, if not exist return 0
    {
        return node ? node->height : 0;
    }
    int getBalance()//just left - right to see balance
    {
        return getHeight(left) - getHeight(right);
    }

    // left rotate
    BinaryTreeNode<K, V> *leftRotate()
    {
        BinaryTreeNode<K, V> *y = right;
        BinaryTreeNode<K, V> *T2 = y->left;
        y->left = this;
        right = T2;
        height = 1 + std::max(getHeight(left), getHeight(right));
        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        return y;
    }

    // right rotate
    BinaryTreeNode<K, V> *rightRotate()
    {
        BinaryTreeNode<K, V> *x = left;
        BinaryTreeNode<K, V> *T2 = x->right;
        x->right = this;
        left = T2;
        height = 1 + std::max(getHeight(left), getHeight(right));
        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        return x;
    }
    BinaryTreeNode<K, V> *minValueNode()
    {//follow left node go down until leftest
        BinaryTreeNode<K, V> *current = this;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }
    // Removing a node from a binary tree, returning
    // a pointer to the now modified tree.

    // If you aren't at the node you are looking for, just set
    // your new left or right pointer (as approriate, and if it exists) 
    // to the  result of recursively calling erase, and then return this.

    // If this node does match the key however, check the
    // child nodes.  If there is no left, assign the right node to a temporary
    // value, delete this, and return the right node.
    // If there is no right node, do the same thing.
    // If there is both a left and right node, check if the left node
    // doesn't have a right node of its own.  If so, set that node's right
    // to the current right, assign left to a temporary, and delete this and
    // return that left node.

    // Finally, if the left node DOES have a right node, keep
    // following the right hand side of that subtree until you
    // find a node without a right hand branch.  In that case set
    // the node above it's right to that node's left, assign that
    // node to a temporary, have its left point to the current node's left
    // its right to the current node's right, delete this and return that
    // node.

    BinaryTreeNode<K, V> *erase(const K &k)
    {
        if (k < key)//check k and key is small then in left, vise versa
        {
            if (left != nullptr)
            {
                left = left->erase(k);
            }
        }
        else if (k > key)
        {
            if (right != nullptr)
            {
                right = right->erase(k);
            }
        }
        else//if equal, then go right tree if node do not have left tree and vise versa then delete current node,
        // if have both left and right and we use lfet side biggest node to instead(right side smallesr also work i think) then if the node we pick do not have right tree then
        //connect currevt node to left node right side and return left node as new tree root node, if has right tree, then use left side right node to instead current node, connect to make node left point to lest, right point to right tree then detele current node
            {
            if ((left == nullptr) || (right == nullptr))
            {
                BinaryTreeNode<K, V> *temp = left ? left : right;
                if (temp == nullptr)//no sub node
                {
                    temp = this;
                    return nullptr;
                }
                else//use sub to instead
                {
                    *this = *temp;
                    delete temp;
                }
            }
            else//if two, get smallest
            {   BinaryTreeNode<K, V> *temp = right->minValueNode();
                key = temp->key;
                value = temp->value;
                right = right->erase(temp->key);
            }
        }
        if (this == nullptr)
            return nullptr;

        height = 1 + std::max(getHeight(left), getHeight(right));//same as next part
        int balance = getBalance();
        if (balance > 1 && left->getBalance() >= 0)
            return rightRotate();
        if (balance > 1 && left->getBalance() < 0)
        {
            left = left->leftRotate();
            return rightRotate();
        }
        if (balance < -1 && right->getBalance() <= 0)
            return leftRotate();
        if (balance < -1 && right->getBalance() > 0)
        {
            right = right->rightRotate();
            return leftRotate();
        }

        return this;
    }


    // This seeks to recursively find a key, creating
    // a new node if necessary.
    // If k == this node's key, return the value.
    // If k < this key it is to the left.
    // If there is no left node, create it with k as the key.
    // Then recursively return find on the left.  Similar for
    // the right. 
    // THis is just to keep the compiler happy
        // so your code compiles, this is not what you
        // actually want to return
    BinaryTreeNode<K, V>* find(const K& k)
    //ccheck k bigger or smaller then key, then to go left/right tree, if null then ky not in tree, and locate at current node left/right sidde
    //so cteate new binarytreenode with key k snd use it as current node's left/right node, baisclly just a recursion
    {    
       if (k == key)
    {
        return this;
    }
    else if (k < key)
    {
        if (left == nullptr)
        {
            left = new BinaryTreeNode<K, V>(k);
        }
        else
        {
            left = left->find(k);
        }
        height = 1 + std::max(getHeight(left), getHeight(right));//updae height
        int balance = getBalance();
        if (balance > 1 && k < left->key)//left left, right right , l r, r l
            return rightRotate();
        if (balance < -1 && k > right->key)
            return leftRotate();
        if (balance > 1 && k > left->key)
        {
            left = left->leftRotate();
            return rightRotate();
        }
        if (balance < -1 && k < right->key)
        {
            right = right->rightRotate();
            return leftRotate();
        }

      return this;
    }
    else
    {
        if (right == nullptr)
        {
            right = new BinaryTreeNode<K, V>(k);
        }
        V &ret = right->find(k);

        // same with last part
        height = 1 + std::max(getHeight(left), getHeight(right));
        int balance = getBalance();
        if (balance > 1 && k < left->key)
            return rightRotate()->value;
        if (balance < -1 && k > right->key)
            return leftRotate()->value;
        if (balance > 1 && k > left->key)
        {
            left = left->leftRotate();
            return rightRotate()->value;
        }
        if (balance < -1 && k < right->key)
        {
            right = right->rightRotate();
            return leftRotate()->value;
        }

        return this;
    }
}
    // And contains is a recursive search that doesn't
    // create new nodes, just checks if the key exists.
    bool contains(const K &k)
    {//use recursion to compare key and the key we want to determine way to search, to see if the key we want in the tree or not
       if (k == key)
        {
            return true;
        }
        else if (k < key && left != nullptr)
        {
            return left->contains(k);
        }
        else if (k > key && right != nullptr)
        {
            return right->contains(k);
        }
        else
        {
            return false;
        }
    }

    K key;
    V value;
    BinaryTreeNode<K, V> *left;
    BinaryTreeNode<K, V> *right;
    int height;//avl
};
