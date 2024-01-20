#include "box.h"
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
/*
format
[subspace id] [len] [inner space] [subBoxes] [mirrored 0/1]
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
    string identity;
    ss>>identity;
    int parentId, playerId;
    ss>>parentId>>playerId;
    setParent(parentId);
    setPlayerId(playerId);
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
string Subspace::toString(){
    string s = "Subspace ";
    s += to_string(getParent()) + " ";
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
/*
format:
[boxes length] [boxes format] [playerBoxed length] [playerBoxed id]
*/
void Map::loadFromString(string s){
    stringstream ss(s);
    loadFromString(ss);
}
void Map::loadFromString(stringstream &ss){
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
            int parentId, playerId, subspaceId;
            ss>>parentId>>playerId>>subspaceId;
            size_t length;
            ss>>length;
            Subspace subspace = Subspace(*this, parentId, playerId,length, ss, false);
            subspace.loadFromString(ss);
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
}
string Map::toString(){
    string s = "";
    s += to_string(boxes.size()) + " ";
    for(int i=0;i<boxes.size();i++){
        s += boxes[i].toString() + " ";
    }
    return s;
}