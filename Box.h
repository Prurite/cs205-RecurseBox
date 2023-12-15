#include <string>
using std::string;

class Box {
private:
    enum Destination { NOT_DESTINATION, PLAYER, BOX };

    int player_id; // 0 not player; >0: the order it moves
    bool is_wall;
    Destination dest_type;
    Box** subspace; // NULL: no subspace
    int height, width;
    Box* clone_of; // NULL: not a clone
    Box* parent; 

    Box();
    Box(string);
    ~Box();
    string Serialize();

    enum Direction { UP, DOWN, LEFT, RIGHT };
    void Move(Direction d);
    bool Movable(Direction d);
    bool isComplete();

};