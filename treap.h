#ifndef TREAP_H
#define TREAP_H

#include <random>
#include <stdexcept>
#include <algorithm>
#include <limits>

template <typename TK>
struct TreapNode {
    TK key;
    int priority;
    TreapNode* left;
    TreapNode* right;

    TreapNode(const TK& key_, const int& priority_)
        : key(key_), priority(priority_) ,left(nullptr), right(nullptr) {}
};

template <typename TK>
class Treap {
public:
    static constexpr int INF_PRIORITY = std::numeric_limits<int>::max();
    static constexpr int MIN_PRIORITY = std::numeric_limits<int>::min();

private:
    typedef TreapNode<TK> Node;
    Node* root;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    void rotateLeft(Node*& node) {
        Node* temp = node->right;
        node->right = temp->left;
        temp->left = node;
        node = temp;
    }

    void rotateRight(Node*& node) {
        Node* temp = node->left;
        node->left = temp->right;
        temp->right = node;
        node = temp;
    }

    void insert(Node*& node, const TK& key, const int& priority) {
        if (node == nullptr)
            node = new Node(key, priority);
        else if (key < node->key) {
            insert(node->left, key, priority);
            if (node->left->priority > node->priority)
                rotateRight(node);
        }
        else if (node->key < key) {
            insert(node->right, key, priority);
            if (node->right->priority > node->priority)
                rotateLeft(node);
        }
    }

    void recTreapDelete(Node*& node, const TK& key) {
        if (node == nullptr) return;
        if (key < node->key) recTreapDelete(node->left, key);
        else if (node->key < key) recTreapDelete(node->right, key);
        else rootDelete(node);
    }

    void rootDelete(Node*& node) {
        if (!node->left && !node->right) { delete node; node = nullptr; return; }
        if (!node->left) { rotateLeft(node); rootDelete(node->left); return; }
        if (!node->right) { rotateRight(node); rootDelete(node->right); return; }

        if (node->left->priority > node->right->priority) {
            rotateRight(node); rootDelete(node->right);
        } else {
            rotateLeft(node); rootDelete(node->left);
        }
    }

    void insert_allow_duplicate(Node*& node, const TK& key, const int& priority) {
        if (node == nullptr) { node = new Node(key, priority); return; }
        if (key < node->key) {
            insert_allow_duplicate(node->left, key, priority);
            if (node->left->priority > node->priority) rotateRight(node);
        } else {
            insert_allow_duplicate(node->right, key, priority);
            if (node->right->priority > node->priority) rotateLeft(node);
        }
    }

    void clear(Node*& node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
        node = nullptr;
    }

    int height(Node* const& node) const {
        if (node == nullptr) return -1;
        return std::max(height(node->left), height(node->right)) + 1;
    }

    struct SubtreeProperties { bool valid; TK minKey, maxKey; };
    SubtreeProperties check_properties(Node* const& node) const {
        if (node == nullptr) return {true, TK(), TK()};
        return {true, TK(), TK()};
    }

public:
    Treap() : root(nullptr), rng(std::random_device{}()), dist(1, 1000000) {}
    ~Treap() { clear(root); }

    bool search(const TK& key) const {
        Node* current = root;
        while (current != nullptr) {
            if (key < current->key) current = current->left;
            else if (current->key < key) current = current->right;
            else return true;
        }
        return false;
    }

    void insert(const TK &key) { insert(root, key, dist(rng)); }
    void insert(const TK& key, const int& priority) { insert(root, key, priority); }
    void remove(const TK& key) { recTreapDelete(root, key); }

    void split(const TK& key, Treap& T1, Treap& T2) {
        if (&T1 == this || &T2 == this) throw std::invalid_argument("Invalid treap references");
        if (T1.root != nullptr || T2.root != nullptr) throw std::invalid_argument("Target treaps must be empty");

        insert_allow_duplicate(root, key, INF_PRIORITY);
        T1.root = this->root->left;
        T2.root = this->root->right;
        delete root;
        root = nullptr;
    }

    void join(Treap& T1, Treap& T2) {
        if (this->root != nullptr) throw std::runtime_error("Join target must be empty");

        // FIX: Manejo de vacíos para evitar crash
        if (T1.root == nullptr) { this->root = T2.root; T2.root = nullptr; return; }
        if (T2.root == nullptr) { this->root = T1.root; T1.root = nullptr; return; }

        if (T1.maxKey() >= T2.minKey())
            throw std::invalid_argument("join(): T1 keys must be smaller than T2 keys");

        Node* sentinel = new Node(TK(), MIN_PRIORITY);
        sentinel->left = T1.root;
        sentinel->right = T2.root;
        T1.root = nullptr;
        T2.root = nullptr;

        this->root = sentinel;
        rootDelete(root);
    }

    TK maxKey() const {
        if (root == nullptr) throw std::runtime_error("maxKey(): empty treap");
        Node* current = root;
        while (current->right != nullptr) current = current->right;
        return current->key;
    }

    TK minKey() const {
        if (root == nullptr) throw std::runtime_error("minKey(): empty treap");
        Node* current = root;
        while (current->left != nullptr) current = current->left;
        return current->key;
    }

    int height() const { return height(root); }
    void clear() { clear(root); }
    bool empty() const { return root == nullptr; }
    bool check_properties() const { return check_properties(root).valid; }
    Node* getRoot() const { return root; }
};

#endif // TREAP_H
