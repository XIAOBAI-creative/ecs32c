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
#include <algorithm> // for std::max

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
        if (start && root != nullptr)
        {
            current = nullptr;
            pushLeft(root);
            incr(); // 确保 current 被正确初始化
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
    bool operator!=(const BinaryTreeIterator<K, V> &other) const
    {
        return current != other.current;
    }

    // This is the heart of the tree traversal algorithm.
    // It needs to be called once in the constructor and 
    // once by each invocation of the ++ operator.
    void incr()
    {
        if (!working_stack.empty())
        {
            current = working_stack.top();
            working_stack.pop();
            pushLeft(current->right);
        }
        else
        {
            current = nullptr;
        }
    }

    // This should just call incr
    void operator++()
    {
        incr();
    }

    // And this visits the node itself...
    std::pair<K, V> operator*() const
    {
        return std::pair<K, V>(current->key, current->value);
    }

private:
    void pushLeft(BinaryTreeNode<K, V> *node)
    {
        while (node != nullptr)
        {
            working_stack.push(node);
            node = node->left;
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

    V &operator[](const K &key)
    {
        if (root == nullptr)
        {
            root = new BinaryTreeNode<K, V>(key);
            return root->value;
        }
        else
        {
            root = root->find(key); // 更新 root
            return root->find(key)->value;
        }
    }

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

    void erase(const K &key)
    {
        if (root != nullptr)
        {
            root = root->erase(key);
        }
    }

    ~BinaryTree()
    {
        if (root != nullptr)
        {
            root->freetree();
            root = nullptr;
        }
    }

    BinaryTreeIterator<K, V> begin()
    {
        return BinaryTreeIterator<K, V>(root, true);
    }
    BinaryTreeIterator<K, V> end()
    {
        return BinaryTreeIterator<K, V>(nullptr, false);
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
    BinaryTreeNode(const K &keyin) : key(keyin), left(nullptr), right(nullptr), height(1)
    {
    }

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
        }
        delete this;
    }

protected:
    int getHeight(BinaryTreeNode<K, V> *node)
    {
        return node ? node->height : 0;
    }

    int getBalance()
    {
        return getHeight(left) - getHeight(right);
    }

    // 左旋
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

    // 右旋
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
    {
        BinaryTreeNode<K, V> *current = this;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    BinaryTreeNode<K, V> *erase(const K &k)
    {
        if (k < key)
        {
            if (left != nullptr)
                left = left->erase(k);
        }
        else if (k > key)
        {
            if (right != nullptr)
                right = right->erase(k);
        }
        else
        {
            if ((left == nullptr) || (right == nullptr))
            {
                BinaryTreeNode<K, V> *temp = left ? left : right;

                if (temp == nullptr)
                {
                    delete this;
                    return nullptr;
                }
                else
                {
                    *this = *temp;
                    delete temp;
                }
            }
            else
            {
                BinaryTreeNode<K, V> *temp = right->minValueNode();
                key = temp->key;
                value = temp->value;
                right = right->erase(temp->key);
            }
        }

        // 更新高度
        height = 1 + std::max(getHeight(left), getHeight(right));

        // 获取平衡因子
        int balance = getBalance();

        // 左左情况
        if (balance > 1 && left && left->getBalance() >= 0)
            return rightRotate();

        // 左右情况
        if (balance > 1 && left && left->getBalance() < 0)
        {
            left = left->leftRotate();
            return rightRotate();
        }

        // 右右情况
        if (balance < -1 && right && right->getBalance() <= 0)
            return leftRotate();

        // 右左情况
        if (balance < -1 && right && right->getBalance() > 0)
        {
            right = right->rightRotate();
            return leftRotate();
        }

        return this;
    }

    BinaryTreeNode<K, V>* find(const K& k)
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
        }
        else
        {
            if (right == nullptr)
            {
                right = new BinaryTreeNode<K, V>(k);
            }
            else
            {
                right = right->find(k);
            }
        }

        // 更新高度
        height = 1 + std::max(getHeight(left), getHeight(right));

        // 获取平衡因子
        int balance = getBalance();

        // 左左情况
        if (balance > 1 && left && k < left->key)
            return rightRotate();

        // 左右情况
        if (balance > 1 && left && k > left->key)
        {
            left = left->leftRotate();
            return rightRotate();
        }

        // 右右情况
        if (balance < -1 && right && k > right->key)
            return leftRotate();

        // 右左情况
        if (balance < -1 && right && k < right->key)
        {
            right = right->rightRotate();
            return leftRotate();
        }

        return this;
    }

    bool contains(const K &k)
    {
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
    int height;
};
