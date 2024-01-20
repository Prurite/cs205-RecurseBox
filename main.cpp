#include<iostream>
#include <unistd.h>
#include <unordered_map>
#include "Box.h"
using namespace std;
static unordered_map<string, Space> SpaceMap = {
    {"WALL", WALL},
    {"EMPTY", EMPTY},
    {"DEST_PLAYER", DEST_PLAYER},
    {"DEST_BLOCK", DEST_BLOCK}
};
static unordered_map<string, Direction> DirectionMap = {
    {"W", UP},
    {"S", DOWN},
    {"A", LEFT},
    {"D", RIGHT}
};
int main(){
    Map map("1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1");
    Game game("0 1 " + map.toString());
    cout<<"Link Start? (y/n)"<<endl;
    string c;
    cin>>c;
    game.show();
    while(true){
        if(c == "n") break;
        cout<<"Input your move: "<<endl;
        cin>>c;
        if(c == "n") break;
        game.move(DirectionMap[c]);
        game.show();
    }
}