#include <iostream>
#include <fstream>
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
    // Read the save game from file
    ifstream fin("game.txt");
    string s;
    getline(fin, s, '\0');
    Game game(s);
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