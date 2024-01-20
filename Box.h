#include <string>
#include <vector>
#include <memory>
using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum Destination { NONE, PLAYER, BLOCK };
enum Space { EMPTY, WALL, DEST_PLAYER, DEST_BLOCK };
enum MoveResult { SUCCESS, FAIL, LOOP };
// Success: things are moved; Fail: things are not moved; Loop: things are moved but the space is not vacant
const int BLOCK_ID = 100, SUBSPACE_ID = 1000;
const int INF_ID = 100, EPS_ID = 101;
const double EPS = 1e-5;

inline Direction invert(Direction d) {
    switch (d) {
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
    }
}

/* id system:
 * Each subspace and special block has a unique id.
 * The contents of a subspace are saved as ids.
 * 
 * Two arrays are used to store the inner structure of a box:
 * 1) The non-movable part
    * 0: open space
    * 1: wall
    * 2: destination for player
    * 3: destination for block
 * 2) The movable part
    * 0: empty
    * 100 + x: normal solid block x (may be player)
    * 1000 * x + y: the y-th copy of subspace x, y = 0: original, y > 0: mirrored
    * id's start from 1.
 * All movable things are stored in a Box object.
*/

class Box {
    // A regular movable box
    int parentId; // The parent box's id, 0 for root
    int playerId; // 0 if it's not a player; -1, if it's not active; otherwise the order of player moving.

protected:
    Map& map; // The map that the box is in, used for visiting other boxes

public:
    Box(Map& map, int parentId, int playerId = 0) : map(map), parentId(parentId), playerId(playerId) {};
    int getParentId() { return parentId; };
    int getPlayerId() { return playerId; };
    int setPlayerId(int p) { playerId = p; };
};

class SolidBlock : public Box {
    // A solid block containing no internal structures

public:
    SolidBlock(Map& map, int parentId, int playerId = 0) : Box(map, parentId, playerId) {};
};

class Subspace : public Box {
    // A subspace block containing internal structures
    int subspaceId; // A unique id for each subspace in a map
    size_t len; // Length of its edge
    vector< vector<Space> > innerSpace; // The inner structure of the subspace
    vector< vector<int> > subBoxes; // The movable boxes in the subspace
    // Positions start from 0
    bool mirrored; // Whether it's mirrored

public:
    Subspace(Map& map, int id, int parentId, size_t len, string subBoxes, bool mirrored, int playerId = 0) :
            Box(map, parentId, playerId), subspaceId(id), len(len), mirrored(mirrored) {
        loadFromString(subBoxes);
    };

    string toString(); // Return a string representation of the subspace
    void loadFromString(string s); // Load a subspace from a string
    // The map is saved per each subspace, and references to other subspaces are described by spaceid.

    MoveResult move(int x, int y, Direction d); // Move the box at (x,y) in a direction d
    MoveResult insert(int boxId, Direction d, int x, int y);
    MoveResult enter(int boxId, Direction d, double p = 0.5);
    MoveResult exit(int boxId, Direction d, double p, int x = -1, int y = -1);

    bool checkComplete(); // Check if all boxes are in the right place

    size_t getLen() { return len; };
    const vector< vector<Space> >& getInnerSpace() { return innerSpace; };
    const vector< vector<int> >& getSubBoxes() { return subBoxes; };
    bool getMirrored() { return mirrored; };
    bool isSubspace(int x, int y) { return subBoxes[x][y] >= SUBSPACE_ID; };
    bool isWall(int x, int y) { return innerSpace[x][y] == WALL; };
    bool isTileEmpty(int x, int y) { return innerSpace[x][y] != WALL && subBoxes[x][y] == EMPTY; };
    bool getBoxXY(int boxId, int& x, int& y); // Get the position of a box
};

class CopyOfSubspace : public Box {
    // A copy of a subspace, that is enterable but not exitable
    int originalId; // The original subspace
    int copyId; // The id of the copy
    bool mirrored; // Whether it's mirrored

public:
    CopyOfSubspace (Map& map, int originalId, int copyId, int parentId, bool mirrored, int playerId = 0) :
            Box(map, parentId, playerId), copyId(copyId), originalId(originalId), mirrored(mirrored) {};
    int getOriginalId() { return originalId; };
    void setMirrored(bool m) { mirrored = m; };
    bool getMirrored() { return mirrored; };
};

class InfCopyOfSubspace : public Box {
    // The infinite copies of a subspace
    int originalId; // The original subspace
    bool mirrored; // Whether it's mirrored
    int infLevel; // +: inf, -: eps

public:
    InfCopyOfSubspace (Map& map, int originalId, int infLevel, int parentId, bool mirrored, int playerId = 0) :
            Box(map, parentId, playerId), originalId(originalId), mirrored(mirrored), infLevel(infLevel) {};
    int getOriginalId() { return originalId; };
    void setMirrored(bool m) { mirrored = m; };
    bool getMirrored() { return mirrored; };
    int getInfLevel() { return infLevel; };
};

class InfSpace : public Subspace {
    // The infinite space
    int infLevel; // +: inf, -: eps

public:
    InfSpace(Map& map, int id, int parentId, size_t len, string subBoxes,
                bool mirrored, int infLevel, int playerId = 0) :
            Subspace(map, id, parentId, len, subBoxes, mirrored, playerId), infLevel(infLevel) {};
    int getInfLevel() { return infLevel; };
};

class Map {
public:   
    vector<Box> boxes; // Subspaces and other movable boxes
    vector<int> playerBoxes; // All boxes that are players, the lower the index the higher priority it has
    vector<int> infBoxes, epsBoxes; // Boxes that are inf or eps, the index is its order - 1
    // NOTES: on initializing, the maximum existing order should be read from the string
    // and corresponding inf boxes are allocated

    vector<int> movingBoxes; // The temp array for detecting moving loops,
    // it should be cleared before each move
    int loopBorder; // The id of the border box in a loop

    Map(string s) { loadFromString(s); }; // Load a map from a string
    void loadFromString(string s); // Load a map from a string
    string toString(); // Return a string representation of the map
    const vector<Box>& getBoxes() { return boxes; };
    int getBoxIdCurrentPlayerIn(); // Get the box id that the current player is in
    Subspace& getSubspace(int id); // Get the subspace with id, may be a copy or an original id
    Subspace& getInfSpace();
};

class Game {
public:
    // The class for an active game
    vector<Map> moves; // Used for storing the history of moves
    int curMove; // Current move id

    Game(string s) { loadFromString(s); }; // Load a game from a string
    void loadFromString(string s); // Load a game from a string
    string toString(); // Return a string representation of the game
    bool move(Direction d); // Move the player in a direction d
    bool undo(); // Undo the last move
    bool redo(); // Redo the last move
    bool reset(); // Reset to the first move
    const Map& getMap() { return moves.back(); }; // Get the current map
};