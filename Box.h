#include <string>
using std::string;
enum Identity { WALL , PLAYER, DESTINATION, SUB_MAP};
enum Direction { UP, DOWN, LEFT, RIGHT };

class Box {
    private:
    Identity Identity;
    public:
        Box();
        Box(string);
        ~Box();
        string Serialize();
        virtual void Move(Direction d);
        virtual bool Movable(Direction d);
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
        void Move(Direction d);
        bool Movable(Direction d);
        bool isComplete();
};
class destination : public Box {
    private:

    public:
        destination();
        ~destination();
        void Move(Direction d);
        bool Movable(Direction d);
        bool isComplete();
};
class Map : public Box {
    private:
        int index_of_map;
        Box **map;
        int x_range, y_range;
    public:
        void Move(Direction d);
        bool Movable(Direction d);
        bool isComplete();
};
