// mymap.h
//
// this is a header file that contains the implementation of a map container
// the map contains unique keys that are connected to a value.
// the map uses threading to make use of unused child pointers
//
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        //
        iterator operator++() {
            NODE* prev = nullptr;
            if (curr->isThreaded) {
                curr = curr->right;
                return curr;
            } else {
                curr = curr->right;
                while (curr != nullptr) {
                    prev = curr;
                    curr = curr->left;
                }
                curr = prev;
                return curr;
            }
        }
    };

 public:
    //
    // default constructor:
    //
    // Creates an empty mymap.
    // Time complexity: O(1)
    //
    mymap() {
        this->size = 0;
        this->root = nullptr;
    }

    void preOrderCopy(NODE* otherTree){
        if (otherTree == nullptr)
            return;
        this->put(otherTree->key, otherTree->value);
        preOrderCopy(otherTree->left);
        if (otherTree->isThreaded) {
            return;
        } else
            preOrderCopy(otherTree->right);
        return;
    }
    //
    // copy constructor:
    //
    // Constructs a new mymap which is a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(nlogn), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap(const mymap& other) {
        this->size = 0;
        this->root = new NODE;
        this->root = nullptr;
        preOrderCopy(other.root);
    }

    //
    // operator=:
    //
    // Clears "this" mymap and then makes a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(nlogn), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap& operator=(const mymap& other) {
        this->clear();
        this->size = 0;
        this->root = new NODE;
        this->root = nullptr;
        preOrderCopy(other.root);
        return *this;
    }

    void postOrderDelete(NODE* node) {
        if (node == nullptr) {
            return;
        }
        postOrderDelete(node->left);
        if (node->isThreaded) {
            return;
        } else
            postOrderDelete(node->right);
        delete node;
    }

    // clear:
    //
    // Frees the memory associated with the mymap; can be used for testing.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    void clear() {
        this->size = 0;
        postOrderDelete(this->root);
    }

    //
    // destructor:
    //
    // Frees the memory associated with the mymap.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    ~mymap() {
        clear();
    }
    //
    // inOrderStr(): converts BST to string in order
    //
    void inOrderStr(NODE* node, stringstream& ss) {
        if (node == nullptr)
            return;
        inOrderStr(node->left, ss);
        ss << "key: " << node->key << " value: " << node->value << "\n";
        if (node->isThreaded) {
            return;
        } else
            inOrderStr(node->right, ss);
    }

    //
    // put:
    //
    // Inserts the key/value into the threaded, self-balancing BST based on
    // the key.
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-tree that needs to be re-balanced.
    // Space complexity: O(1)
    //
    void put(keyType key, valueType value) {
        NODE* temp = new NODE;
        temp->key = key;
        temp->value = value;
        temp->nL = 0;
        temp->nR = 0;
        temp->isThreaded = false;
        temp->left = nullptr;
        temp->right = nullptr;

        if (this->root == nullptr) {
            this->root = temp;
            this->size += 1;
        } else {
            NODE* curr = this->root;
            NODE* prev = nullptr;
            while (curr != nullptr) {
                if (key == curr->key) {
                    curr->value = value;
                }
                if (key < curr->key) {
                    prev = curr;
                    curr = curr->left;
                } else {
                    if (curr->isThreaded) {
                        prev = curr;
                        curr = nullptr;
                    } else {
                        prev = curr;
                        curr = curr->right;
                    }
                }
            }
            if (key > prev->key) {
                temp->isThreaded = true;
                temp->right = prev->right;
                prev->right = temp;
                prev->isThreaded = false;
                this->size += 1;
                prev->nR += 1;
            } else {
                temp->isThreaded = true;
                temp->right = prev;
                prev->left = temp;
                this->size += 1;
                prev->nL += 1;
            }
        }
    }

    //
    // contains:
    // Returns true if the key is in mymap, return false if not.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    bool contains(keyType key) {
        NODE* curr = this->root;
        while (curr != nullptr) {
            if (curr->key == key) {
                return true;
            } else if (key < curr->key) {
                curr = curr->left;
            } else {
                if (curr->isThreaded) {
                    curr = nullptr;
                } else {
                    curr = curr->right;
                }
            }
        }
        return false;
    }

    //
    // get:
    //
    // Returns the value for the given key; if the key is not found, the
    // default value, valueType(), is returned (but not added to mymap).
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    valueType get(keyType key) {
        NODE* curr = this->root;
        while (curr != nullptr) {
            if (curr->key == key) {
                return curr->value;
            } else if (key < curr->key) {
                curr = curr->left;
            } else {
                if (curr->isThreaded) {
                    curr = nullptr;
                } else {
                    curr = curr->right;
                }
            }
        }
        return valueType();
    }

    //
    // operator[]:
    //
    // Returns the value for the given key; if the key is not found,
    // the default value, valueType(), is returned (and the resulting new
    // key/value pair is inserted into the map).
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-trees that need to be re-balanced.
    // Space complexity: O(1)
    //
    valueType operator[](keyType key) {
        NODE* curr = this->root;
        NODE* prev = nullptr;
        while (curr != nullptr) {
            if (curr->key == key) {
                return curr->value;
            } else if (key < curr->key) {
                prev = curr;
                curr = curr->left;
            } else {
                if (curr->isThreaded) {
                    prev = curr;
                    curr = nullptr;
                } else {
                    prev = curr;
                    curr = curr->right;
                }
            }
        }
        NODE* temp = new NODE;
        temp->key = key;
        temp->value = valueType();
        temp->nL = 0;
        temp->nR = 0;
        temp->isThreaded = false;
        temp->left = nullptr;
        temp->right = nullptr;
        if (key > prev->key) {
            temp->isThreaded = true;
            temp->right = prev->right;
            prev->right = temp;
            prev->isThreaded = false;
            this->size += 1;
            prev->nR += 1;
        } else {
            temp->isThreaded = true;
            temp->right = prev;
            prev->left = temp;
            this->size += 1;
            prev->nL += 1;
        }
        return valueType();
    }

    //
    // Size:
    //
    // Returns the # of key/value pairs in the mymap, 0 if empty.
    // O(1)
    //
    int Size() {
        return this->size;
    }

    //
    // begin:
    //
    // returns an iterator to the first in order NODE.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    iterator begin() {
        NODE* curr = this->root;
        NODE* prev = nullptr;
        while (curr != nullptr) {
            prev = curr;
            curr = curr->left;
        }
        return prev;
    }

    //
    // end:
    //
    // returns an iterator to the last in order NODE.
    // this function is given to you.
    // 
    // Time Complexity: O(1)
    //
    iterator end() {
        return iterator(nullptr);
    }
    void inOrderPairs(NODE* node, vector<pair<keyType, valueType> >& vec) {
        if (node == nullptr)
            return;
        inOrderPairs(node->left, vec);
        vec.push_back(make_pair(node->key, node->value));
        if (node->isThreaded) {
            return;
        } else
            inOrderPairs(node->right, vec);
    }

    //
    // toString:
    //
    // Returns a string of the entire mymap, in order.
    // Format for 8/80, 15/150, 20/200:
    // "key: 8 value: 80\nkey: 15 value: 150\nkey: 20 value: 200\n
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string toString() {
        stringstream ss;
        inOrderStr(this->root, ss);
        return ss.str();
    }

    //
    // toVector:
    //
    // Returns a vector of the entire map, in order.  For 8/80, 15/150, 20/200:
    // {{8, 80}, {15, 150}, {20, 200}}
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    vector<pair<keyType, valueType> > toVector() {
        vector<pair<keyType, valueType> > vec;
        inOrderPairs(this->root, vec);
        return vec;
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {


        // TODO: write this function.


        return {};
    }
};
