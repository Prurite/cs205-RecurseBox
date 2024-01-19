#include <string>
#include <vector>
#include <memory>
using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum Destination { NONE, PLAYER, BLOCK };

class Box {
    // The origin for everything, stands for a single block in game

    shared_ptr<Box> parent; // The parent box
    int playerId; // 0 if it's not a player; -1, if it's not active; otherwise the order of player moving.

public:
    Box(shared_ptr<Box> p, int pid = 0) : parent(p), playerId(pid);
    shared_ptr<Box> getParent() { return parent; };
    int getPlayerId() { return playerId; };
    int setPlayerId(int p) { playerId = p; };
};

class EmptyBlock : public Box {
    // An open space in a box, can be the destination of a player or another box

    Destination destType;

public:
    EmptyBlock(shared_ptr<Box> p, Destination dt) : Box(p), destType(dt) {};
    Destination getDestType() { return destType; };
};

class Block : public Box {
    // A regular movable block

public:
    Block(shared_ptr<Box> p, int pid = 0) : Box(p, pid) {};
};

class Subspace : public Block {
    // A subspace block containing internal structures

    int id; // A unique id for each subspace in a map
    size_t len; // Length of its edge
    vector< vector< shared_ptr<Box> > > subBoxes; // A 2-dim array of its sub-boxes
    // Positions start from 0
    bool mirrored; // Whether it's mirrored

public:
    Subspace(int id, shared_ptr<Box> p, size_t l, string s, bool m, int pid = 0) :
            id(id), Block(p, pid), len(l), mirrored(m) {
        loadFromString(s);
    };

    string toString(); // Return a string representation of the subspace
    void loadFromString(string s); // Load a subspace from a string
    // The map is saved per each subspace, and references to other subspaces are described by spaceid.
    bool move(int x, int y, Direction d); // Move the box at (x,y) in a direction d
    bool checkComplete(); // Check if all boxes are in the right place
};

class CopyOfSubspace : public Block {
    // A copy of a subspace, that is enterable but not exitable

    shared_ptr<Box> original; // The original subspace
    bool mirrored; // Whether it's mirrored

public:
    CopyOfSubspace(shared_ptr<Box> p, shared_ptr<Box> o, bool m, int pid = 0) :
            Block(p, pid), original(o), mirrored(m) {};
};

class SolidBlock : public Block {
    // A solid block containing no internal structures

public:
    SolidBlock(shared_ptr<Box> p, int pid = 0) : Block(p, pid) {};
};

class Wall : public Box {
    // A wall block that can not be moved

public:
    Wall(shared_ptr<Box> p, int pid = 0) : Box(p, pid) {};
};

class Game {
    // The class for an active game
    unique_ptr<Subspace> root; // The root subspace
};