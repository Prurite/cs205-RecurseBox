#include <string>
#include <vector>
using std::string;
enum Identity { WALL , PLAYER, SUB_MAP, EMPTY, BOX};
enum Direction { UP, DOWN, LEFT, RIGHT };

class Box {
    private:

    public:
        Identity Identity = BOX;
        Box();
        Box(string);
        ~Box();
        string Serialize();
        bool isEnterable(Direction d);
        operator Map();
};

class player : public Box {
    private:

    public:
        player();
        ~player();
};
class wall : public Box {
    private:

    public:
        wall();
        ~wall();
};
/*
        y       
 |------------------
 |
 |
 |
x|
 |
 |
 |
*/
class Map : public Box {
    private:
        int index_of_map;
        int x_player = -1, y_player = -1;
        int x_length,y_length;
        int x_parent = -1, y_parent = -1; //recording the location in the parent map, move out
        std::vector<int> x_destination;
        std::vector<int> y_destination;
        Box **map;
    public:
        Map();
        Map(int index_of_map, int x_length, int y_length, int x_player, int y_player, 
            std::vector<int> x_destination, std::vector<int> y_destination, Box **map);
        ~Map();
        
        bool hasPlayer();
        /*
        @todo
        check if this direction is enterable
        */
        bool isEnterable(Direction d, Map &submap);
        /*
        @todo
        check if the final position is empty or submap
        @return
        0: not movable
        1: movable with empty or cycle
        2: movable with submap
        */
       
        int Movable(Direction d, int x_temp, int y_temp, Map &submap);
        /*
        @todo
        check as movable
        if the final position is empty
        then swap from empty to player
        else if the final position is submap
        then check if the submap is enterable
        */
        bool Move(Direction d, Map &submap);
        void printMap();
        bool isComplete();
};