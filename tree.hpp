#include <iostream>
#include <stack>
#include <utility>
#include <algorithm>

template <class K, class V>
class BinaryTree;
template <class K, class V>
class BinaryTreeIterator;
template <class K, class V>
class BinaryTreeNode;

template <class K, class V>
class BinaryTreeIterator {
    friend class BinaryTree<K, V>;
    BinaryTreeNode<K, V> *current;
    std::stack<BinaryTreeNode<K, V> *> working_stack;

    BinaryTreeIterator(BinaryTreeNode<K, V> *root, bool start) {
        if (start && root != nullptr) {
            current = root;
            pushLeft(current);
        } else {
            current = nullptr;
        }
    }

    void pushLeft(BinaryTreeNode<K, V> *node) {
        while (node != nullptr) {
            working_stack.push(node);
            node = node->left;
        }
    }

public:
    bool operator!=(const BinaryTreeIterator<K, V> &other) const {
        return current != other.current;
    }

    BinaryTreeIterator& operator++() {
        if (!working_stack.empty()) {
            current = working_stack.top();
            working_stack.pop();
            pushLeft(current->right);
        } else {
            current = nullptr;
        }
        return *this;
    }

    std::pair<K, V> operator*() const {
        return {current->key, current->value};
    }
};

template <class K, class V>
class BinaryTree {
    BinaryTreeNode<K, V> *root;

public:
    BinaryTree() : root(nullptr) {}

    ~BinaryTree() {
        if (root != nullptr) {
            root->freetree();
            root = nullptr;
        }
    }

    V &operator[](const K &key) {
        if (root == nullptr) {
            root = new BinaryTreeNode<K, V>(key);
        }
        return root->find(key)->value;
    }

    bool contains(const K &key) const {
        return root != nullptr && root->contains(key);
    }

    void erase(const K &key) {
        if (root != nullptr) {
            root = root->erase(key);
        }
    }

    BinaryTreeIterator<K, V> begin() {
        return BinaryTreeIterator<K, V>(root, true);
    }

    BinaryTreeIterator<K, V> end() {
        return BinaryTreeIterator<K, V>(root, false);
    }
};

template <class K, class V>
class BinaryTreeNode {
    friend class BinaryTree<K, V>;
    friend class BinaryTreeIterator<K, V>;

    K key;
    V value;
    BinaryTreeNode<K, V> *left, *right;
    int height;

    BinaryTreeNode(const K &keyin) : key(keyin), left(nullptr), right(nullptr), height(1) {}

    ~BinaryTreeNode() {
        freetree();
    }

    void freetree() {
        if (left) {
            delete left;
            left = nullptr;
        }
        if (right) {
            delete right;
            right = nullptr;
        }
    }

    int getHeight() const {
        return this ? height : 0;
    }

    int getBalance() const {
        int leftHeight = left ? left->getHeight() : 0;
        int rightHeight = right ? right->getHeight() : 0;
        return leftHeight - rightHeight;
    }

    BinaryTreeNode<K, V>* updateHeight() {
        height = 1 + std::max(left ? left->height : 0, right ? right->height : 0);
        return this;
    }

    BinaryTreeNode<K, V>* leftRotate() {
        BinaryTreeNode<K, V> *y = right;
        right = y->left;
        y->left = this;
        this->updateHeight();
        y->updateHeight();
        return y;
    }

    BinaryTreeNode<K, V>* rightRotate() {
        BinaryTreeNode<K, V> *x = left;
        left = x->right;
        x->right = this;
        this->updateHeight();
        x->updateHeight();
        return x;
    }

    BinaryTreeNode<K, V>* balance() {
        updateHeight();
        int balance = getBalance();
        if (balance > 1 && left->getBalance() >= 0) {
            return rightRotate();
        } else if (balance > 1 && left->getBalance() < 0) {
            left = left->leftRotate();
            return rightRotate();
        } else if (balance < -1 && right->getBalance() <= 0) {
            return leftRotate();
        } else if (balance < -1 && right->getBalance() > 0) {
            right = right->rightRotate();
            return leftRotate();
        }
        return this;
    }

    BinaryTreeNode<K, V>* find(const K& k) {
        BinaryTreeNode<K, V>*& target = (k < key) ? left : right;
        if (k == key) {
            return this;
        } else if (target == nullptr) {
            target = new BinaryTreeNode<K, V>(k);
        }
        return target->find(k);
    }

    bool contains(const K &k) const {
        if (k == key) {
            return true;
        } else if (k < key) {
            return left != nullptr && left->contains(k);
        } else {
            return right != nullptr && right->contains(k);
        }
    }

    BinaryTreeNode<K, V>* erase(const K &k) {
        if (k < key) {
            if (left != nullptr) left = left->erase(k);
        } else if (k > key) {
            if (right != nullptr) right = right->erase(k);
        } else {
            if (!left || !right) {
                BinaryTreeNode<K, V> *temp = left ? left : right;
                if (temp == nullptr) {
                    return nullptr;
                } else {
                    *this = *temp;
                }
                delete temp;
            } else {
                BinaryTreeNode<K, V> *temp = right->minValueNode();
                key = temp->key;
                value = temp->value;
                right = right->erase(temp->key);
            }
        }
        return this->balance();
    }

    BinaryTreeNode<K, V> *minValueNode() {
        BinaryTreeNode<K, V> *current = this;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }
};
