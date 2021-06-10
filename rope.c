// @Profeanu Ioana, Iordache Madalina Gabriela 313CA
#include "./rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define EMPTY ""

const char *strdup(const char *s);

// function which creates a rope tree node
RopeNode* makeRopeNode(const char* str) {
    // allocate memory for the node
    RopeNode* Node = calloc(1, sizeof(RopeNode));
    DIE(Node == NULL, "Calloc failed!");

    // initialise its metadata
    Node->left = NULL;
    Node->right = NULL;
    Node->weight = 0;

    Node->str = str;
    return Node;
}

// function which creates a rope tree
RopeTree* makeRopeTree(RopeNode* root) {
    // allocate memory for the tree
    RopeTree* Tree = calloc(1, sizeof(RopeTree));
    DIE(Tree == NULL, "Calloc failed!");

    Tree->root = root;
    return Tree;
}

// recursive function which prints each node
void printRopeNode(RopeNode* rn) {
    if (!rn)
        return;

    if (!(rn->left) && !(rn->right)) {
        printf("%s", rn->str);
        return;
    }

    printRopeNode(rn->left);
    printRopeNode(rn->right);
}

// function which prints the rope tree
void printRopeTree(RopeTree* rt) {
    if (rt && rt->root) {
        printRopeNode(rt->root);
        printf("%s", "\n");
    }
}

// recursive function for debugging the rope tree
void debugRopeNode(RopeNode* rn, int indent) {
    if (!rn)
        return;

    for (int i = 0; i < indent; ++i)
        printf("%s", " ");

    if (!strcmp(rn->str, EMPTY))
        printf("# %d\n", rn->weight);
    else
        printf("%s %d\n", rn->str, rn->weight);

    debugRopeNode(rn->left, indent+2);
    debugRopeNode(rn->right, indent+2);
}


// function for checking if a node is leaf
int isLeaf(RopeNode *Node)
{
    if (Node == NULL)
       return 0;

    if (Node->left == NULL && Node->right == NULL)
       return 1;

    return 0;
}

// function which returns the total weight
// starting from an initial node
int getTotalWeight(RopeNode* rt) {
    if (!rt)
        return 0;

    return rt->weight + getTotalWeight(rt->right);
}

// function which concatenates two rope trees
RopeTree* concat(RopeTree* rt1, RopeTree* rt2) {
    // create a new root node which has a weight equal to the sum of
    // weights of the leaf nodes in the first tree
    RopeNode* newRoot = makeRopeNode(strdup(EMPTY));

    // allocate memory for the concatenated tree
    RopeTree* newTree = makeRopeTree(newRoot);

    newRoot->left = rt1->root;
    newRoot->right = rt2->root;

    // change the weight of the root
    newRoot->weight = getTotalWeight(newRoot);

    return newTree;
}

// function which recursively finds a char in the rope tree
void findChar(RopeNode* Node, int idx, char *character)
{
    // if the weight of the current node is lower than the value of idx
    if (Node->weight <= idx && Node->right != NULL) {
        // we subtract the weight from idx and move to the right
        findChar(Node->right, idx - Node->weight, character);
        return;
    }

    // if the weight is less than value of idx we move to the left
    if (Node->left != NULL) {
        findChar(Node->left, idx, character);
        return;
    }

    // continue until reaching a leaf
    character[0] = Node->str[idx];
    return;
}

// function which returns the character located at a given index
char indexRope(RopeTree* rt, int idx) {
    char* character = calloc(1, sizeof(char));
    DIE(character == NULL, "Calloc failed!");

    // recursive search
    findChar(rt->root, idx, character);

    // dealocate memory for the idx-th character
    char auxiliary_character = character[0];
    free(character);

    // return character at position idx
    return auxiliary_character;
}

// function which returns the substring that starts
// and ends at the given indexes
char* search(RopeTree* rt, int start, int end) {
    // get the size of the searched substring
    size_t size = end - start + 1;

    // allocate memory for the substring in [start, end) interval
    char* searched = calloc(size, sizeof(char));
    DIE(searched == NULL, "Calloc failed!");

    int i = 0;
    for (int j = start; j < end; j++) {
        // call indexRope function to return characters between
        // [start, end), then store them in an array
        searched[i] = indexRope(rt, j);
        i++;
    }

    // return the array which contains the searched substring
    return searched;
}

// recursive function which clones all nodes
// of a rope tree and returns its root
RopeNode* cloneNode(RopeNode* root) {
    if (root == NULL)
        return NULL;

    // allocate memory for the node's string, which will be stored
    // in the newly created node
    char* auxiliaryStr = calloc(strlen(root->str) + 1, sizeof(char));
    DIE(auxiliaryStr == NULL, "Calloc failed!");

    // copy the value
    strcpy(auxiliaryStr, root->str);

    // allocate memory for the new node
    RopeNode* newNode = makeRopeNode(auxiliaryStr);

    // copy the new node's metadata
    newNode->weight = root->weight;

    newNode->left = cloneNode(root->left);
    newNode->right = cloneNode(root->right);

    return newNode;
}

// function which creates the cloned rope tree
RopeTree* cloneRopeTree(RopeTree* rt) {
    if (!rt)
        return NULL;

    RopeTree* newTree = makeRopeTree(cloneNode(rt->root));
    return newTree;
}

// function which extracts a substring from a given string
char* getSubstring(const char *strSrc, int start, int end)
{
    // find length of wanted substring
    int strLength = end - start;

    // allocate memory for the substring
    char *strDest = (char*)calloc((strLength + 1), sizeof(char));
    DIE(strDest == NULL, "Calloc failed!");

    // extract each character between [start, end) from source string
    for (int i = start; i < end && (*(strSrc + i) != '\0'); i++)
    {
        *strDest = *(strSrc + i);
        strDest++;
    }

    // add string terminator
    *strDest = '\0';

    // return substring
    return strDest - strLength;
}

// function which recursively splits a rope tree
void splitTree(RopeTree* rtAux, RopeNode* Node, int idx, int idxInitial,
                SplitPair* splitFinal, SplitPair* splitRightSide,
                SplitPair* splitNodes)
{
    // check if right link of the root has to be deleted
    // (which means that the left split part subtree will be the left part
    // of the initial rope tree, and similarly for the right part)
    if (Node->weight == idx && Node->right != NULL && Node == rtAux->root &&
        isLeaf(Node->right) == 1) {
        // create the final left and right subtrees
        splitFinal->right = Node->right;
        splitFinal->left = rtAux->root;

        // delete the right link of the root
        Node->right = NULL;

        // rebalance weights
        splitFinal->left->weight = getTotalWeight(splitFinal->left);
        splitFinal->right->weight = getTotalWeight(splitFinal->right);

        return;
    }

    // if we should travel down on the right side
    if (Node->weight < idx && Node->right != NULL) {
        // continue travel without deleting any links
        splitTree(rtAux, Node->right, idx - Node->weight,
        idxInitial, splitFinal, splitRightSide, splitNodes);
        return;

    // check if we should travel down on the right side
    } else if (Node->left != NULL) {
        // if the node is the root, then the right side of the
        // right subtree should be the right side of the main tree
        if (Node == rtAux->root && Node->right != NULL) {
            splitRightSide->right = Node->right;
            Node->right = NULL;
        }

        // if the node isn't root and node to the right exists
        // delete the link and add the node in the split nodes pair
        // (it will be an orphan node which will later be concatenated)
        if (Node != rtAux->root && Node->right != NULL) {
            // if up to this point there haven't been any orphan nodes,
            // add the current node to the right of the split nodes pair
            if (splitNodes->right == NULL) {
                splitNodes->right = Node->right;
                // delete the link to the right
                Node->right = NULL;

            } else {
                // if there has already been an orphan node,
                // add the current node to the left of the split nodes pair
                splitNodes->left = Node->right;
                Node->right = NULL;

                // concatenate the two orphan nodes
                RopeNode* merge_nodes = makeRopeNode(strdup(EMPTY));
                merge_nodes->left = splitNodes->left;
                merge_nodes->right = splitNodes->right;

                // on the right side, the split nodes pair will contain
                // the two merged orphan nodes
                splitNodes->right = merge_nodes;
                splitNodes->left = NULL;
            }
        }

        // continue travel on the left side
        splitTree(rtAux, Node->left, idx, idxInitial, splitFinal,
                  splitRightSide, splitNodes);
        return;

    // if the node is a leaf
    } else {
        // if we have to split the leaf
        if (idx != 0) {
            // create the two new nodes containing the split string
            // that the original node contained
            RopeNode* left = makeRopeNode(getSubstring(Node->str, 0, idx));
            int size = strlen(Node->str);
            RopeNode* right = makeRopeNode(getSubstring(Node->str, idx, size));

            // renew the node's links
            Node->left = left;
            Node->right = NULL;

            // renew the string of the node
            free((void *)Node->str);
            Node->str = strdup(EMPTY);
            splitNodes->left = right;

        } else {
            // if the leaf shouldn't be split

            // check if the leaf will be an orphan leaf
            if (idxInitial != rtAux->root->weight) {
                splitNodes->left = Node;
            } else {
                splitNodes->left = NULL;
            }
        }

        // assembly the two final subtrees
        // if there are oprhan nodes
        if (splitNodes->right != NULL && splitNodes->left != NULL) {
            // merge the two orphan nodes
            RopeNode* merge_nodes = makeRopeNode(strdup(EMPTY));
            merge_nodes->left = splitNodes->left;
            merge_nodes->right = splitNodes->right;
            // the left side of the right subtree will be the merged nodes
            splitRightSide->left = merge_nodes;

            // merge the right subtree
            RopeNode* merge_right_side = makeRopeNode(strdup(EMPTY));
            merge_right_side->left = splitRightSide->left;
            merge_right_side->right = splitRightSide->right;

            // create the two split parts
            // the right side will be the merged right side
            splitFinal->right = merge_right_side;

        // if there is only one orphan node
        } else if (splitNodes->right == NULL && splitNodes->left != NULL) {
            // add the orphan leaf to a node
            RopeNode* merge_right_side = makeRopeNode(strdup(EMPTY));
            merge_right_side->left = splitNodes->left;
            merge_right_side->right = splitRightSide->right;
            splitFinal->right = merge_right_side;

        // if there aren't any orphan nodes
        } else {
            splitFinal->right = splitRightSide->right;
        }

        // the left side will always start from the initial tree's root
        splitFinal->left = rtAux->root;

        // update each subtree's weight
        splitFinal->left->weight = getTotalWeight(splitFinal->left);
        splitFinal->right->weight = getTotalWeight(splitFinal->right);
        return;
    }
}

// function which allocates memory for the subtrees
// used for splitting the initial tree
// the type parameter is used to know if we use the function
// for the split function or for insert and delete
SplitPair makeSplit(RopeTree* rt, int idx, int ifSplit)
{
    // create a copy of the initial rope tree
    RopeTree* rtAux = cloneRopeTree(rt);

    // create the split pair for the final two subtrees
    SplitPair* splitFinal = calloc(1, sizeof(SplitPair));
    DIE(splitFinal == NULL, "Calloc failed!");
    splitFinal->left = NULL;
    splitFinal->right = NULL;

    // create the split part for the right and left substrees
    // of the final right side subtree
    SplitPair* splitRightSide = calloc(1, sizeof(SplitPair));
    DIE(splitRightSide == NULL, "Calloc failed!");
    splitRightSide->left = NULL;
    splitRightSide->right = NULL;

    // create the split pair for the orphan nodes met along the
    // splitting process which will be concatenated together
    SplitPair* splitNodes = calloc(1, sizeof(SplitPair));
    DIE(splitNodes == NULL, "Calloc failed!");
    splitNodes->left = NULL;
    splitNodes->right = NULL;

    // if the index is 0, the right final split subtree will
    // be the initial tree
    if (idx == 0) {
        if (ifSplit == 1) {
            printf("\n");
        }
        splitFinal->left = NULL;
        splitFinal->right = rtAux->root;

    } else {
        // otherwise, split the tree using a recursive funciton
        splitTree(rtAux, rtAux->root, idx, idx, splitFinal,
                  splitRightSide, splitNodes);
    }

    // create the final result (with stack allocation)
    SplitPair resultSplit;
    resultSplit.right = splitFinal->right;
    resultSplit.left = splitFinal->left;

    free(splitFinal);
    free(splitRightSide);
    free(splitNodes);
    free(rtAux);

    // return the final split pair
    return resultSplit;
}

SplitPair split(RopeTree* rt, int idx) {
    return makeSplit(rt, idx, 1);
}

// function which inserts a string in the initial rope string
RopeTree* insert(RopeTree* rt, int idx, const char* str) {
    // split the tree at idx
    SplitPair firstSplit = makeSplit(rt, idx, 0);

    // create a tree with each splitted subtree
    RopeTree* leftSubtree = makeRopeTree(firstSplit.left);
    RopeTree* rightSubtree = makeRopeTree(firstSplit.right);

    // create the node which will be inserted
    RopeNode* newNode = makeRopeNode(str);
    // make it be the root of a new tree
    RopeTree* newTree = makeRopeTree(newNode);

    // concatenate the left splitted subtree with the to be inserted node
    RopeTree* firstConcat = concat(leftSubtree, newTree);
    // concatenate the result with the right splitted subtree
    RopeTree* finalConcat = concat(firstConcat, rightSubtree);

    // free the memory
    free(leftSubtree);
    free(rightSubtree);
    free(newTree);
    free(firstConcat);

    return finalConcat;
}

// recursive function which returns the length of the
// string stored by the rope tree
void getLength(RopeNode* Node, int *stringLen) {
    // traverse the tree recursively and
    // add the length of the string stored by leaves
    if (Node == NULL)
        return;

    // if the node is a leaf, add its length to the result
    if (isLeaf(Node) == 1) {
        *stringLen += strlen(Node->str);
        return;
    }

    getLength(Node->left, stringLen);
    getLength(Node->right, stringLen);
}

// function which deletes substring from initial rope string
RopeTree* delete(RopeTree* rt, int start, int len) {
    // get the length of the string stored by the rope tree
    int stringLen = 0;
    getLength(rt->root, &stringLen);

    // check the three cases
    if (stringLen < start + len && start < stringLen) {
        // split the rope tree from start and return the left side
        return makeRopeTree(makeSplit(rt, start, 0).left);

    } else if (stringLen < start + len && start >= stringLen) {
        // return the entire tree
        return makeRopeTree(rt->root);

    } else {
        // split the rope start and from start + len
        SplitPair rightSplit =  makeSplit(rt, start, 0);

        SplitPair leftSplit = makeSplit(rt, start + len, 0);

        // concatenate the left side of the first split
        // and the right side of the second split
        // and return it
        return concat(makeRopeTree(rightSplit.left),
                      makeRopeTree(leftSplit.right));
    }
}
