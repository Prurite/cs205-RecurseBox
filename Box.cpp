#include "Box.h"
#include <stack>
using std::stack;

// TODO: loop; infinity

bool operator==(const Map::MoveLog& a, const Map::MoveLog& b) {
    return a.type == b.type && a.parentId == b.parentId && a.boxId == b.boxId && a.d == b.d;
}

bool Map::inMovingLoop(MoveLog log) {
    for (size_t i = 0; i < moveLogs.size(); ++i)
        if (moveLogs[i] == log)
            return true;
    return false;
}

// DEBUG: print the move logs
void printMoveLogs(vector<Map::MoveLog> &log) {
    string type, dir;
    for (size_t i = 0; i < log.size(); ++i) {
        switch (log[i].type) {
            case 0: type = "Move"; break;
            case 1: type = "Insert"; break;
            case 2: type = "Enter"; break;
            case 3: type = "Exit"; break;
        }
        switch (log[i].d) {
            case UP: dir = "UP"; break;
            case DOWN: dir = "DOWN"; break;
            case LEFT: dir = "LEFT"; break;
            case RIGHT: dir = "RIGHT"; break;
        }
        cout << type << ", parent " << log[i].parentId
            << ", box " << log[i].boxId << ", " << dir << endl;
    }
}

MoveResult Subspace::move(int x, int y, Direction d) {
    // Try to move the box at x row, y col to a direction d by 1 step
    int dx, dy;
    double p;
    switch (d) {
        case UP: dx = -1; dy = 0; p = 1.0 * y / len; break;
        case DOWN: dx = 1; dy = 0; p = 1.0 * y / len; break;
        case LEFT: dx = 0; dy = -1; p = 1.0 * x / len; break;
        case RIGHT: dx = 0; dy = 1; p = 1.0 * x / len; break;
    }
    // If (x,y) is not a movable box, return false
    if (innerSpace[x][y] == WALL) return FAIL;
    if (subBoxes[x][y] == EMPTY) return FAIL;

    int curId = subBoxes[x][y];
    Map::MoveLog log = {0, id, curId, d};
    if (map->inMovingLoop(log)) {
        map->loopBorder = log;
        printMoveLogs(map->moveLogs);
        return SUCCESS;
    }
    map->moveLogs.push_back(log);

    MoveResult res = FAIL;

    // 4 cases: 1) Edge 2) Wall 3) Space 4) Another box
    int nx = x + dx, ny = y + dy;
    if (nx < 0 || nx >= len || ny < 0 || ny >= len) {
    // 1) Edge: push it out of the subspace
        res = exit(curId, d, p, x, y);
    } else if (isWall(nx, ny)) {
    // 2) Wall
        res = FAIL;
    } else if (isTileEmpty(nx, ny)) {
    // 3) Space
        subBoxes[nx][ny] = curId;
        res = SUCCESS;
    } else {
    // 4) Another box
        res = move(nx, ny, d);
        if (res == SUCCESS) {
            // a) It's movable
            subBoxes[nx][ny] = curId;
        } else if (res == LOOP) {
            // b) A loop happened
            res = LOOP;
        } else if (isSubspace(nx, ny)) {
            res = map->getSubspace(subBoxes[nx][ny])->enter(curId, d);
            // b) It's enterable
        } else if (isSubspace(x, y)) {
            res = map->getSubspace(curId)->enter(subBoxes[nx][ny], invert(d));
            // c) It's eatable
            subBoxes[nx][ny] = curId;
        }
    }
    if (res == SUCCESS && subBoxes[x][y] == curId)
        subBoxes[x][y] = EMPTY;
    if (res == SUCCESS && log == map->loopBorder) {
        cout << "Loop end" << endl;
        res = LOOP;
    }
    cout << "Move " << id << ' ' << x << ' ' << y << ' ' << d << ' ' << res << endl;
    map->moveLogs.pop_back();
    return res;
}

// The move function attempts to move a box to a direction d
// Below are intermediate functions operating on a specific box and do not process loops

MoveResult Subspace::enter(int boxId, Direction d, double p) {
    // Push a box into the subspace in a direction d, with relative position p
    // On success, the box is inserted into the subspace, but the original box is not cleared

    Map::MoveLog log = {2, id, boxId, d};
    if (map->inMovingLoop(log)) {
        map->loopBorder = log;
        printMoveLogs(map->moveLogs);
        return SUCCESS;
    }
    map->moveLogs.push_back(log);

    MoveResult res = FAIL;

    int nx, ny; // The position of the entering box in the subspace
    switch (d) {
        case UP: nx = len - 1; ny = len * p; break;
        case DOWN: nx = 0; ny = len * p; break;
        case LEFT: nx = len * p; ny = len - 1; break;
        case RIGHT: nx = len * p; ny = 0; break;
    }
    if (isWall(nx, ny)) res = FAIL;
    // 1) Wall
    else if (isTileEmpty(nx, ny)) {
    // 2) Space
        subBoxes[nx][ny] = boxId;
        map->getBox(boxId)->setParentId(id);
        res = SUCCESS;
    } else {
    // 3) Another box
        res = move(nx, ny, d);
        if (res == SUCCESS) {
            // a) It's movable
            subBoxes[nx][ny] = boxId;
            map->getBox(boxId)->setParentId(id);
        } else if (res == LOOP) {
            // b) It's a loop
            res = LOOP;
        } else if (isSubspace(nx, ny)) {
            // c) Not movable, onsider entering
            double base_p;
            switch (d) {
                case UP: case DOWN: base_p = 1.0 * ny / len; break;
                case LEFT: case RIGHT: base_p = 1.0 * nx / len; break;
            }
            Subspace* subspace = map->getSubspace(subBoxes[nx][ny]);
            p = (p - base_p) * len;
            res = subspace->enter(boxId, d, p);
        }
    }
    if (res == SUCCESS && log == map->loopBorder)
        res = LOOP;
    map->moveLogs.pop_back();
    cout << "Enter " << id << ' ' << boxId << ' ' << d << ' ' << p << ' ' << res << endl;
    return res;
}

MoveResult Subspace::exit(int boxId, Direction d, double p, int x, int y) {
    // Pop a box out of the subspace in a direction d, with relative position p
    // When x and y are not -1, (x,y) is the position of the original box being pushed out
    // Otherwise, the box is passed from the inner space
    // On success, the box is inserted into the outer space, and the original box is cleared

    Map::MoveLog log = {3, id, boxId, d};
    if (map->inMovingLoop(log)) {
        map->loopBorder = log;
        printMoveLogs(map->moveLogs);
        return SUCCESS;
    }
    map->moveLogs.push_back(log);

    MoveResult res = FAIL;

    int px, py, n_px, n_py;
    size_t parentLen;
    double base_p = 0;
    if (getParentId() <= 0) return FAIL;
    // TODO: the default space
    // Get the position of the leaving subspace in the outer space, in (px, py)
    Subspace* parent = map->getSubspace(getParentId());
    parent->getBoxXY(id, px, py);
    parentLen = map->getSubspace(getParentId())->getLen();
    switch(d) {
        case UP: n_px = px - 1; n_py = py; base_p = 1.0 * py / parentLen; break;
        case DOWN: n_px = px + 1; n_py = py; base_p = 1.0 * py / parentLen; break;
        case LEFT: n_px = px; n_py = py - 1; base_p = 1.0 * px / parentLen; break;
        case RIGHT: n_px = px; n_py = py + 1; base_p = 1.0 * px / parentLen; break;
    }
    if (x != -1 && y != -1)
        // Calculate p from x and y
        switch (d) {
            case UP: case DOWN: p = 1.0 * y / len; break;
            case LEFT: case RIGHT: p = 1.0 * x / len; break;
        }

    if (n_px < 0 || n_px >= parentLen || n_py < 0 || n_py >= parentLen) {
        // It's also leaving the outer space
        // Check for infinity
        double p0 = base_p + p * len / parentLen;
        // TODO: infinity
        res = parent->exit(boxId, d, p0);
    } else {
        // Insert it into the outer space
        res = parent->insert(boxId, d, n_px, n_py, p);
        if (res == SUCCESS && x != -1 && y != -1)
            subBoxes[x][y] = EMPTY;
    }
    if (res == SUCCESS && log == map->loopBorder)
        res = LOOP;
    cout << "Exit " << id << ' ' << boxId << ' ' << d << ' ' << p << ' ' << x << ' ' << y << ' ' << res << endl;
    map->moveLogs.pop_back();
    return res;
}

MoveResult Subspace::insert(int boxId, Direction d, int x, int y, double p) {
    // Try to insert a box into the subspace at (x,y) in a direction d
    // The box may be one that the inner space pushes out or one that the outer space pushes in
    // On success, the box is inserted into the subspace, but the original box is not cleared

    Map::MoveLog log = {1, id, boxId, d};
    if (map->inMovingLoop(log)) {
        map->loopBorder = log;
        printMoveLogs(map->moveLogs);
        return SUCCESS;
    }
    map->moveLogs.push_back(log);

    MoveResult res = FAIL;

    // x, y in [0, len)
    if (isWall(x, y)) res = FAIL;
    // 1) Wall
    else if (isTileEmpty(x, y)) {
    // 2) Space
        subBoxes[x][y] = boxId;
        map->getBox(boxId)->setParentId(id);
        res = SUCCESS;
    } else {
    // 3) Another box
        res = move(x, y, d);
        if (res == SUCCESS) {
            // a) It's movable;
            subBoxes[x][y] = boxId;
            cout << boxId << " set parent " << id << " and pos " << x << " " << y << endl;
            map->getBox(boxId)->setParentId(id);
        } else if (res == LOOP) {
            // b) It's a loop
            res = LOOP;
        } else if (isSubspace(x, y)) {
            // c) Not movable, consider entering
            res = map->getSubspace(subBoxes[x][y])->enter(boxId, d, p);
        }
    }
    if (res == SUCCESS && log == map->loopBorder)
        res = LOOP;
    map->moveLogs.pop_back();
    cout << "Insert " << id << ' ' << boxId << ' ' << d << ' ' << x << ' ' << y << ' ' << p << ' ' << res << endl;
    return res;
}

bool Subspace::getBoxXY(int boxId, int& x, int& y) {
    // Get the position of a box
    for (size_t i = 0; i < len; ++i)
        for (size_t j = 0; j < len; ++j)
            if (subBoxes[i][j] == boxId) {
                x = i;
                y = j;
                return true;
            }
    return false;
}

Map::Map( ) : defaultSpace(this) {
    string s = R"(
        Subspace 100000 0 0 7 0 0
        1 1 1 1 1 1 1
        1 0 0 0 0 0 1
        1 0 0 0 0 0 1
        1 0 0 0 0 0 1
        1 0 0 0 0 0 1
        1 0 0 0 0 0 1
        1 1 1 1 1 1 1
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
        0 0 0 0 0 0 0
    )";
    stringstream ss(s);
    defaultSpace.loadFromString(ss);
}

Map::Map(const Map& m) : playerBoxes(m.playerBoxes), infBoxes(m.infBoxes), epsBoxes(m.epsBoxes), defaultSpace(m.defaultSpace) {
    for (size_t i = 0; i < m.boxes.size(); ++i) {
        Subspace* p1 = dynamic_cast<Subspace*>(m.boxes[i].get());
        CopyOfSubspace* p2 = dynamic_cast<CopyOfSubspace*>(m.boxes[i].get());
        SolidBlock* p3 = dynamic_cast<SolidBlock*>(m.boxes[i].get());
        if (p1 != NULL)
            boxes.push_back(make_shared<Subspace>(*p1));
        else if (p2 != NULL)
            boxes.push_back(make_shared<CopyOfSubspace>(*p2));
        else if (p3 != NULL)
            boxes.push_back(make_shared<SolidBlock>(*p3));
        boxes[i]->setMap(this);
    }
}

Subspace* Map::getSubspace(int id) {
    for (size_t i = 0; i < boxes.size(); ++i) {
        Subspace* subspace = dynamic_cast<Subspace*>(boxes[i].get());
        if (subspace != NULL && subspace->getId() == id)
            return subspace;
    }
    return &defaultSpace;
}

int Map::getCurrentPlayerBoxId(int playerId) {
    for (size_t i = 0; i < boxes.size(); ++i)
        if (boxes[i]->getPlayerId() == playerId)
            return boxes[i]->getId();
    return -1;
}

bool Subspace::isComplete() {
    for (size_t i = 0; i < subBoxes.size(); i++)
        for (size_t j = 0; j < subBoxes[i].size(); j++) {
            if (innerSpace[i][j] == DEST_BLOCK && (
                subBoxes[i][j] == EMPTY || map->getBox(subBoxes[i][j])->getPlayerId()
            ) )
                return false;
            if (innerSpace[i][j] == DEST_PLAYER && ( 
                subBoxes[i][j] == EMPTY || map->getBox(subBoxes[i][j])->getPlayerId() == 0
            ) )
                return false;
        }
    return true;
}

bool Map::isComplete() const {
    for (size_t i = 0; i < boxes.size(); ++i) {
        Subspace* subspace = dynamic_cast<Subspace*>(boxes[i].get());
        if (subspace != NULL && !subspace->isComplete())
            return false;
    }
    return true;
}

bool Game::move(Direction d) {
    // Find the subspace the player is in
    Map curMap(moves[curMove]);
    curMap.moveLogs.clear();
    int curId = curMap.getCurrentPlayerBoxId();
    if (curId == -1)
        return false;
    int spaceId = curMap.getBox(curId)->getParentId();
    if (spaceId == -1)
        return false;
    Subspace* space = curMap.getSubspace(spaceId);
    int px, py; // Player's (x, y)
    space->getBoxXY(curId, px, py);
    bool flag = space->move(px, py, d) != FAIL;
    if (flag) {
        moves.erase(moves.begin() + curMove + 1, moves.end());
        moves.push_back(curMap);
        curMove++;
    }
    return flag;
}

bool Game::undo( ) {
    if (curMove == 0)
        return false;
    curMove--;
    return true;
}

bool Game::redo( ) {
    if (curMove == moves.size() - 1)
        return false;
    curMove++;
    return true;
}

bool Game::reset( ) {
    curMove++;
    moves.push_back(moves[0]);
    return true;
}