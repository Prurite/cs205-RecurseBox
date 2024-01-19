#include <string>
using std::string;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum Destination { NONE, PLAYER, BOX };

class Box {
    // The origin for everything
    // Stands for a single block in game
    enum Identity {EMPTY, PLAYER, WALL, SUBSPACE};

    Identity identity;
    Box* parent; // The parent box

};

class EmptyBlock : public Box {
    // An open space in a box
    // Can be the destination of a player or another box

    Destination destType;
};

class Block : public Box {
    // A regular movable block
};

class Subspace : public Block {
    // A subspace block containing internal structures

    size_t len; // Length of its edge
    Box ***subBoxes; // A 2-dim array of its sub-boxes
    // Positions start from 0

    bool move(int x, int y, Direction d); // Move the box at (x,y) in a direction d
};

class CopyOfSubspace : public Subspace {
    // A copy of a subspace, that is enterable but not exitable

    Subspace *original; // The original subspace
};

class SolidBlock : public Block {
    // A solid block containing no internal structures

    int playerId; // 0 if it's not a player; otherwise the order of player moving.
};

class Wall : public Box {
    // A wall block that can not be moved

};

class Game {
    // The class for an active game
    Subspace *root; // The root subspace
}