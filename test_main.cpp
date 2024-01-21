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
    cout << game.toString() << endl;
    cout << game.show() << endl;
    cout<<"Link Start? (y/n)"<<endl;
    string c;
    cin>>c;
    while(true){
        if(c == "n") break;
        cout<<"Input your move: "<<endl;
        cin>>c;
        if(c == "z") cout<<game.undo()<<endl;
        else if(c == "x") cout<<game.redo()<<endl;
        else if(c == "r") cout<<game.reset()<<endl;
        else if (c == "q") break;
        else cout<<game.move(DirectionMap[c])<<endl;
        cout<<game.show()<<endl;
        cout<<game.getMap().isComplete()<<endl;
    }
}