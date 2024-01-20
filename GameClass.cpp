#include "Box.h"
#include<iostream>
#include<cstring>
#include<sstream>
#include<map>
#include<unordered_map>
using namespace std;
static unordered_map<string, Space> SpaceMap = {
    {"WALL", WALL},
    {"EMPTY", EMPTY},
    {"DEST_PLAYER", DEST_PLAYER},
    {"DEST_BLOCK", DEST_BLOCK}
};
/*
format denote:
# is wall
. is empty
p is player
b is block
= is destination for player
_ is destination for block
[int] is subspace
/[int] is reverse of subspace
+[int] is inf of subspace
-[int] is eps of subspace
[x]|[y] is the yth copy of subspace x
E is invalid

*/
string Box::toString(){
    string s = "b ";
    s += to_string(parentId) + " " + to_string(playerId) + " ";
    return s;
}
void Box::show(){
    cout<<toString()<<endl;
}
/*
format
[parent id] [player id] [subspace id] [len] [inner space] [subBoxes] [mirrored 0/1]
*/
void Subspace::loadFromString(string s){
    stringstream ss(s);
    ss>>subspaceId>>len;
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            Space c;
            string c_temp;
            ss>>c_temp;
            c = SpaceMap[c_temp];
            innerSpace[i].push_back(c);
        }
    }
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            int c;
            ss>>c;
            subBoxes[i].push_back(c);
        }
    }
    ss>>mirrored;
}
void Subspace::loadFromString(stringstream &ss){
    int parentId, playerId;
    ss>>parentId>>playerId>>subspaceId>>len; 
    setParentId(parentId);
    setPlayerId(playerId);   
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            Space c;
            string c_temp;
            ss>>c_temp;
            c = SpaceMap[c_temp];
            innerSpace[i].push_back(c);
        }
    }
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            int c;
            ss>>c;
            subBoxes[i].push_back(c);
        }
    }
    ss>>mirrored;
}
/*
format:
Subspace [parent id] [player id] [subspace id] [len] [inner space] [subBoxes] [mirrored 0/1]

*/
string Subspace::toString(){
    string s = "Subspace ";
    s += to_string(getParentId()) + " ";
    s += to_string(getPlayerId()) + " "; 
    s += to_string(subspaceId) + " ";
    s += to_string(len) + " ";
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            s += to_string(innerSpace[i][j]) + " ";
        }
    }
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            s += to_string(subBoxes[i][j]) + " ";
        }
    }
    s += to_string(mirrored) + " ";
    return s;
}
void Subspace::show(){
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            cout<<innerSpace[i][j]<<" ";
        }
        cout<<endl;
    }
    for(int i=0;i<len;i++){
        for(int j=0;j<len;j++){
            cout<<subBoxes[i][j]<<" ";
        }
        cout<<endl;
    }
}
/*
format:
[loop border] [boxes length] [boxes format] [playerBoxed length] [playerBoxed id] [infBoxes length] [infBoxes format] [epsBoxes length] [epsBoxes format] [movingBoxes length] [movingBoxes format] 
*/
void Map::loadFromString(string s){
    stringstream ss(s);
    loadFromString(ss);
}
void Map::loadFromString(stringstream &ss){
    int loopBorder;
    ss>>loopBorder;
    int boxesLength;
    ss>>boxesLength;
    for(int i=0;i<boxesLength;i++){
        string boxIdentity;
        ss>>boxIdentity;
        if(boxIdentity == "b"){
            int parentId, playerId;
            ss>>parentId>>playerId;
            Box box = Box(*this, parentId, playerId);
            boxes.push_back(box);
        }else{
            Subspace subspace = Subspace(*this, ss);
            boxes.push_back(subspace);
        }
    }
    int playerBoxedLength;
    ss>>playerBoxedLength;
    for(int i=0;i<playerBoxedLength;i++){
        int id;
        ss>>id;
        playerBoxes.push_back(id);
    }
    int infBoxesLength;
    ss>>infBoxesLength;
    for(int i=0;i<infBoxesLength;i++){
        int id;
        ss>>id;
        infBoxes.push_back(id);
    }
    int epsBoxesLength;
    ss>>epsBoxesLength;
    for(int i=0;i<epsBoxesLength;i++){
        int id;
        ss>>id;
        epsBoxes.push_back(id);
    }
    int movingBoxesLength;
    ss>>movingBoxesLength;
    for(int i=0;i<movingBoxesLength;i++){
        int id;
        ss>>id;
        movingBoxes.push_back(id);
    }
}
string Map::toString(){
    string s = "";
    s += to_string(loopBorder) + " ";
    s += to_string(boxes.size()) + " ";
    for(int i=0;i<boxes.size();i++){
        s += boxes[i].toString() + " ";
    }
    s += to_string(playerBoxes.size()) + " ";
    for(int i=0;i<playerBoxes.size();i++){
        s += to_string(playerBoxes[i]) + " ";
    }
    s += to_string(infBoxes.size()) + " ";
    for(int i=0;i<infBoxes.size();i++){
        s += to_string(infBoxes[i]) + " ";
    }
    s += to_string(epsBoxes.size()) + " ";
    for(int i=0;i<epsBoxes.size();i++){
        s += to_string(epsBoxes[i]) + " ";
    }
    s += to_string(movingBoxes.size()) + " ";
    for(int i=0;i<movingBoxes.size();i++){
        s += to_string(movingBoxes[i]) + " ";
    }
    return s;
}
void Map::show(){
    for(int i=0;i<boxes.size();i++){
        cout<<boxes[i].toString()<<endl;
    }
}

/*
format:
[curMove] [number of maps] [map format] 
*/
string Game::toString(){
    string s = "";
    s += to_string(curMove) + " ";
    s += to_string(moves.size()) + " ";
    for(int i=0;i<moves.size();i++){
        s += moves[i].toString() + " ";
    }
    return s;
}
void Game::loadFromString(string s){
    stringstream ss(s);
    loadFromString(ss);
}
void Game::loadFromString(stringstream &ss){
    ss>>curMove;
    int movesLength;
    ss>>movesLength;
    for(int i=0;i<movesLength;i++){
        Map map = Map("");
        map.loadFromString(ss);
    }
}
void Game::show(){
    for(int i=0;i<moves.size();i++){
        moves[i].show();
    }
}