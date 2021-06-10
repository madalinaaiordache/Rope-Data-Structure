// @Profeanu Ioana, Iordache Madalina Gabriela 313CA
#ifndef __ROPE__
#define __ROPE__
#define EMPTY ""

typedef struct RopeNode RopeNode;

// structure which contains the metadata
// of a rope tree's node
struct RopeNode {
    RopeNode* left;
    RopeNode* right;
    const char* str;
    int weight;
};

// structure which contains the root of a rope tree
typedef struct RopeTree RopeTree;

struct RopeTree {
    RopeNode* root;
};

// structure which contains the pair of roots
// coresponding to the right and left side of the
// split rope tree
typedef struct SplitPair SplitPair;

struct SplitPair {
    RopeNode* left;
    RopeNode* right;
};

RopeNode* makeRopeNode(const char* str);

RopeTree* makeRopeTree(RopeNode* root);

void printRopeNode(RopeNode* rn);

void printRopeTree(RopeTree* rt);

void debugRopeNode(RopeNode* rn, int indent);

RopeTree* concat(RopeTree* rt1, RopeTree* rt2);

char indexRope(RopeTree* rt, int idx);

char* search(RopeTree* rt, int start, int end);

SplitPair split(RopeTree* rt, int idx);

RopeTree* insert(RopeTree* rt, int idx, const char* str);

RopeTree* delete(RopeTree* rt, int start, int len);

#endif  // __ROPE__
