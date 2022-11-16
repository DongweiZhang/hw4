#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int height(Node * root) {
    if (root == nullptr) return 0;

    int lh = height(root->left);
    int rh = height(root->right);

    return lh > rh ? lh + 1 : rh + 1;
}

bool equalPaths(Node * root) {
    if (root == nullptr) return true;

    if (root->left && root->right) {
        int lh = height(root->left);
        int rh = height(root->right);

        if (lh != rh) {
            return false;
        } else {
            if (lh == 1) return true;
            return equalPaths(root->left) && equalPaths(root->right);
        }
    } else if (root->left) {
        return equalPaths(root->left);
    } else if (root->right) {
        return equalPaths(root->right);
    } else {
        return true;
    }
}

