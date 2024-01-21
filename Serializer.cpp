#include "Box.h"

/* 
    Standard format to store Box:
    Line 1: Box [type] [id] [parentId] [playerId]
    SolidBlock: none
    Subspace:
        [len] [mirrored 0/1] [infEpsLevel]
        [innerSpace]
        [subBoxes]
    CopyOfSubspace:
        [originalId] [mirrored 0/1] [infEpsLevel]

    When reading, the type is not fed into the loader.
*/
string SolidBlock::toString() {
    stringstream ss;
    ss << "SolidBlock " << id << ' ' << parentId << ' ' << playerId << '\n';
    return ss.str();
}

bool SolidBlock::loadFromString(stringstream &ss) {
    ss >> id >> parentId >> playerId;
    return true;
}

string SolidBlock::show() {
    // return "b";
    return "";
}

string Subspace::toString() {
    stringstream ss;
    ss << "Subspace " << id << ' ' << parentId << ' ' << playerId << '\n'
       << len << ' ' << mirrored << ' ' << infEpsLevel << '\n';
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = 0; j < len; ++j)
            ss << innerSpace[i][j] << ' ';
        ss << '\n';
    }
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = 0; j < len; ++j)
            ss << subBoxes[i][j] << ' ';
        ss << '\n';
    }
    return ss.str();
}

bool Subspace::loadFromString(stringstream &ss) {
    ss >> id >> parentId >> playerId >> len >> mirrored >> infEpsLevel;
    innerSpace.resize(len);
    subBoxes.resize(len);
    for (size_t i = 0; i < len; ++i) {
        innerSpace[i].resize(len);
        subBoxes[i].resize(len);
        int x;
        for (size_t j = 0; j < len; ++j) {
            ss >> x;
            innerSpace[i][j] = (Space)x;
        }
    }
    for (size_t i = 0; i < len; ++i)
        for (size_t j = 0; j < len; ++j)
            ss >> subBoxes[i][j];
    return true;
}

/*
    format denote:
     #  is wall
     .  is empty
      p is player
     b  is block
     =  is destination for player
     _  is destination for block
     x  is subspace x
    /x  is reverse of subspace
    +x  is inf of subspace
    -x  is eps of subspace
    x|y is the yth copy of subspace x
     E  is invalid
*/

string Subspace::show() {
    stringstream ss;
    if (id == DEFAULT_SPACE_ID)
        ss << "Default / inf space\n";
    else
        ss << "Subspace " << id / SUBSPACE_ID << '\n';
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = 0; j < len; ++j) {
            int x = subBoxes[i][j];
            if (x > BLOCK_ID && x < SUBSPACE_ID)
                ss << " b";
            else if (x >= SUBSPACE_ID) {
                if (x % SUBSPACE_ID)
                    ss << x / SUBSPACE_ID << "|" << x % SUBSPACE_ID;
                else
                    ss << ' ' << x / SUBSPACE_ID;
            } else if (innerSpace[i][j] == WALL && id != DEFAULT_SPACE_ID)
                ss << " #";
            else if (innerSpace[i][j] == EMPTY)
                ss << " .";
            else if (innerSpace[i][j] == DEST_PLAYER)
                ss << " =";
            else if (innerSpace[i][j] == DEST_BLOCK)
                ss << " _";
            if (!(x >= SUBSPACE_ID && x % SUBSPACE_ID))
                ss << (x > BLOCK_ID && map->getBox(x)->getPlayerId() ? 'p' : ' ');
        }
        ss << '\n';
    }
    return ss.str();
}

string CopyOfSubspace::toString() {
    stringstream ss;
    ss << "CopyOfSubspace " << id << ' ' << parentId << ' ' << playerId << "\n"
       << originalId << ' ' << mirrored << ' ' << infEpsLevel << '\n';
    return ss.str();
}

bool CopyOfSubspace::loadFromString(stringstream &ss) {
    ss >> id >> parentId >> playerId >> originalId >> mirrored >> infEpsLevel;
    return true;
}

string CopyOfSubspace::show() {
    stringstream ss;
    ss << id / SUBSPACE_ID << '|' << id % SUBSPACE_ID;
    // return ss.str();
    return "";
}

string Map::toString() {
    stringstream ss;
    ss << "Map " << boxes.size( ) << '\n';
    for (size_t i = 0; i < boxes.size(); ++i)
        ss << boxes[i]->toString();
    return ss.str();
}

bool Map::loadFromString(stringstream &ss) {
    string type;
    int n;
    ss >> n;
    boxes.clear();
    for (size_t i = 0; i < n; ++i) {
        ss >> type;
        if (type == "SolidBlock")
            boxes.push_back(make_shared<SolidBlock>(this));
        else if (type == "Subspace")
            boxes.push_back(make_shared<Subspace>(this));
        else if (type == "CopyOfSubspace")
            boxes.push_back(make_shared<CopyOfSubspace>(this));
        else
            return false;
        boxes[i]->loadFromString(ss);
    }
    return true;
}

string Map::show() {
    stringstream ss;
    // Check if the default space is empty; if not, show it
    if (defaultSpace.hasSubBoxes())
        ss << defaultSpace.show();
    for (size_t i = 0; i < boxes.size(); ++i)
        ss << boxes[i]->show();
    return ss.str();
}

string Game::toString() {
    stringstream ss;
    ss << "Game " << moves.size() << ' ' << curMove << '\n';
    for (size_t i = 0; i < moves.size(); ++i)
        ss << moves[i].toString();
    return ss.str();
}

bool Game::loadFromString(string s) {
    stringstream ss(s);
    string type;
    int n;
    ss >> type >> n >> curMove;
    moves.clear();
    for (size_t i = 0; i < n; ++i) {
        ss >> type;
        if (type == "Map") {
            moves.push_back(Map());
            moves[i].loadFromString(ss);
        }
        else
            return false;
    }
    return true;
}

string Game::show(int curMove) {
    if (curMove < 0 || curMove >= (int)moves.size())
        curMove = this->curMove;
    return moves[curMove].show();
}