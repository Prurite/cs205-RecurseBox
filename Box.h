#include <iostream>
using namespace std;

#include <string>
#include <vector>
#include <memory>
#include <sstream>
using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using std::stringstream;

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
    return UP;
}

/* id system:
 * Each subspace and special block has a unique id.
 * Solid movable blocks have id BLOCK_ID + x
 * Subspaces have id SUBSPACE_ID * x
 * Copies of subspace x have id SUBSPACE_ID * x + y, y = 0: original, y > 0: mirrored
 * 
 * Two arrays are used to store the inner structure of a box:
 * 1) The non-movable part
    * 0: open space
    * 1: wall
    * 2: destination for player
    * 3: destination for block
 * 2) The movable part
    * 0: empty
    * id: a movable block
 * All movable things are stored in a Box object.
*/

class Map;

class Box {
protected:
    // A regular movable box
    int id;
    int parentId; // The parent box's id, 0 for root
    int playerId; // 0 if it's not a player; -1, if it's not active; otherwise the order of player moving.
    Map* map; // The map that the box is in, used for visiting other boxes

public:
    Box(Map* map, int id, int parentId, int playerId = 0) :
            map(map), id(id), parentId(parentId), playerId(playerId) {};
    Box(Map* map) : map(map) { id = parentId = playerId = -1; }

    int getId() { return id; }
    int getParentId() { return parentId; };
    int getPlayerId() { return playerId; };
    void setPlayerId(int p) { playerId = p; };
    void setParentId(int p) { parentId = p; };
    virtual string toString() = 0; // Return a string representation of the box
    virtual bool loadFromString(stringstream &ss) = 0; // Load a box from a string
    virtual string show() = 0; // Show the box as a printable string
    void setMap(Map* m) { map = m; };
};

class SolidBlock : public Box {
    // A solid block containing no internal structures

public:
    SolidBlock(Map* map, int id, int parentId, int playerId = 0) :
            Box(map, id, parentId, playerId) {};
    SolidBlock(Map* map) : Box(map) {};
    
    string toString() override;
    bool loadFromString(stringstream &ss) override;
    string show() override;
};

class Subspace : public Box {
    // A subspace block containing internal structures
    size_t len; // Length of its edge
    bool mirrored; // Whether it's mirrored
    int infEpsLevel; // 0: standard, +: inf, -: eps
    vector< vector<Space> > innerSpace; // The inner structure of the subspace
    vector< vector<int> > subBoxes; // The movable boxes in the subspace
    // Positions start from 0

public:
    Subspace(Map *map) : Box(map), len(0), mirrored(0), infEpsLevel(0) {};

    string toString() override; // Return a string representation of the subspace
    bool loadFromString(stringstream &ss) override; // Load a subspace from a string
    // The map is saved per each subspace, and references to other subspaces are described by spaceid.
    string show() override; // Show the subspace as a printable string

    MoveResult move(int x, int y, Direction d);
    // Move the box at (x,y) in a direction d
    MoveResult insert(int boxId, Direction d, int x, int y, double p = 0.5);
    // Try to insert a box into the subspace at (x,y) in a direction d
    MoveResult enter(int boxId, Direction d, double p = 0.5);
    // Push a box into the subspace in a direction d, with relative position p
    MoveResult exit(int boxId, Direction d, double p, int x = -1, int y = -1);
    // Pop a box out of the subspace in a direction d, with relative position p
    // When (x,y) = (-1,-1), the box comes from nested inner subspace.

    bool isComplete(); // Check if all boxes are in the right place, does not go in recursively
    size_t getLen() { return len; };
    const vector< vector<Space> >& getInnerSpace() { return innerSpace; };
    const vector< vector<int> >& getSubBoxes() { return subBoxes; };
    bool getMirrored() { return mirrored; };
    void setMirrored(bool m) { mirrored = m; };
    bool isSubspace(int x, int y) { return subBoxes[x][y] >= SUBSPACE_ID; };
    bool isWall(int x, int y) { return innerSpace[x][y] == WALL; };
    bool isTileEmpty(int x, int y) { return innerSpace[x][y] != WALL && subBoxes[x][y] == EMPTY; };
    bool getBoxXY(int boxId, int& x, int& y); // Get the position of a box
};

class CopyOfSubspace : public Box {
    // A copy of a subspace, that is enterable but not exitable
    int originalId; // The original subspace
    bool mirrored; // Whether it's mirrored
    int infEpsLevel; // 0: standard, +: inf, -: eps

public:
    CopyOfSubspace(Map* map, int id, int parentId, int originalId, bool mirrored, int playerId = 0, int infEpsLevel = 0) :
            Box(map, id, parentId, playerId), originalId(originalId), mirrored(mirrored), infEpsLevel(infEpsLevel) {};
    CopyOfSubspace(Map* map) : Box(map) { originalId = mirrored = infEpsLevel = 0; };

    string toString() override;
    bool loadFromString(stringstream &ss) override;
    string show() override;
    
    int getOriginalId() { return originalId; };
    void setMirrored(bool m) { mirrored = m; };
    bool getMirrored() { return mirrored; };
};

class Map {
public:   
    vector< shared_ptr<Box> > boxes; // Subspaces and other movable boxes
    // Each box has a unique id, which is described at the top of the file.
    vector<int> playerBoxes; // All boxes that are players, the lower the index the higher priority it has
    vector<int> infBoxes, epsBoxes; // Boxes that are inf or eps, the index is its order - 1
    // NOTES: on initializing, the maximum existing order should be read from the string
    // and corresponding inf boxes are allocated

    struct MoveLog { int type, parentId, boxId; Direction d; }; // The log of a move
    // Type: 0: move, 1: insert, 2: enter, 3: exit
    vector<MoveLog> moveLogs; // The temp array for detecting moving loops,
    // it should be cleared before each move
    MoveLog loopBorder;

    Subspace defaultSpace; // The default outer-most space

    Map( );
    Map(const Map& m);
    ~Map( ) { }

    string toString(); // Return a string representation of the map
    bool loadFromString(stringstream &ss); // Load a map from a string
    string show();

    const vector< shared_ptr<Box> >& getBoxes() { return boxes; };
    int getCurrentPlayerBoxId(int playerId = 1); // Get the box id that the current player is
    Subspace* getSubspace(int id); // Get the subspace with id, may be a copy or an original id
    Subspace* getInfSpace(int order = 1);
    Box* getBox(int id) {
        for (size_t i = 0; i < boxes.size(); ++i)
            if (boxes[i]->getId() == id)
                return boxes[i].get();
        return nullptr;
    }
    bool isComplete() const;
    bool inMovingLoop(MoveLog log); // Check if a move is in a loop
};

class Game {
public:
    // The class for an active game
    vector<Map> moves; // Used for storing the history of moves
    int curMove; // Current move id

    Game(string s) { loadFromString(s); }; // Load a game from a string

    string toString(); // Return a string representation of the game
    bool loadFromString(string s); // Load a game from a string
    string show(int curMove = -1); // Show the map of a move
    // string showAll();

    bool move(Direction d); // Move the player in a direction d
    bool undo(); // Undo the last move
    bool redo(); // Redo the last move
    bool reset(); // Reset to the first move

    const Map& getMap() { return moves[curMove]; }; // Get the current map
};