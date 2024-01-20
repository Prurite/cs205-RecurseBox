#include "Box.h"
#include <stack>
using std::stack;

// TODO: loop; infinity


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
    if (innerSpace[x][y] != EMPTY) return FAIL;
    if (subBoxes[x][y] == EMPTY) return FAIL;

    int curId = subBoxes[x][y];
    for (size_t i = 0; i < map.movingBoxes.size(); ++i)
        if (map.movingBoxes[i] == curId) {
            map.loopBorder = curId;
            return SUCCESS;
        }

    // 4 cases: 1) Edge 2) Wall 3) Space 4) Another box
    int nx = x + dx, ny = y + dy;
    if (nx < 0 || nx >= len || ny < 0 || ny >= len) {
    // 1) Edge: push it out of the subspace
        return exit(curId, d, p, x, y);
    } else if (isWall(nx, ny))
    // 2) Wall
        return FAIL;
    else if (isTileEmpty(nx, ny)) {
    // 3) Space
        subBoxes[nx][ny] = curId;
        if (subBoxes[x][y] == curId) subBoxes[x][y] = EMPTY;
        return SUCCESS;
    }
    // 4) Another box
    MoveResult res = move(nx, ny, d);
    if (res == SUCCESS) {
        // a) It's movable
        subBoxes[nx][ny] = curId;
        return curId == map.loopBorder ? LOOP : SUCCESS;
    } else if (res == LOOP)
        // b) A loop happened
        return LOOP;
    else if (isSubspace(nx, ny)) {
        res = map.getSubspace(subBoxes[nx][ny]).enter(curId, d);
        // b) It's enterable
        if (res == SUCCESS) subBoxes[x][y] = EMPTY;
        return res;
    } else if (isSubspace(x, y)) {
        res = map.getSubspace(curId).enter(subBoxes[nx][ny], invert(d));
        // c) It's eatable
        subBoxes[nx][ny] = curId;
        if (res == SUCCESS) subBoxes[x][y] = EMPTY;
        return res;
    }
    return FAIL;
}

// The move function attempts to move a box to a direction d
// Below are intermediate functions operating on a specific box and do not process loops

MoveResult Subspace::enter(int boxId, Direction d, double p) {
    // Push a box into the subspace in a direction d, with relative position p
    // On success, the box is inserted into the subspace, but the original box is not cleared

    int nx, ny; // The position of the entering box in the subspace
    switch (d) {
        case UP: nx = len - 1; ny = len * p; break;
        case DOWN: nx = 0; ny = len * p; break;
        case LEFT: nx = len * p; ny = len - 1; break;
        case RIGHT: nx = len * p; ny = 0; break;
    }
    if (isWall(nx, ny)) return FAIL;
    // 1) Wall
    else if (isTileEmpty(nx, ny)) {
    // 2) Space
        subBoxes[nx][ny] = boxId;
        return SUCCESS;
    }
    // 3) Another box
    MoveResult res = move(nx, ny, d);
    if (res == SUCCESS) {
        // a) It's movable
        subBoxes[nx][ny] = boxId;
        return SUCCESS;
    } else if (res == LOOP)
        // b) It's a loop
        return LOOP;
    else if (isSubspace(nx, ny)) {
        // c) Not movable, onsider entering
        double base_p;
        switch (d) {
            case UP: case DOWN: base_p = 1.0 * ny / len; break;
            case LEFT: case RIGHT: base_p = 1.0 * nx / len; break;
        }
        Subspace& subspace = map.getSubspace(subBoxes[nx][ny]);
        p = (p - base_p) * len / subspace.getLen();
        return subspace.enter(boxId, d, p);
    }
    return FAIL;
}

MoveResult Subspace::exit(int boxId, Direction d, double p, int x, int y) {
    // Pop a box out of the subspace in a direction d, with relative position p
    // When x and y are not -1, (x,y) is the position of the original box being pushed out
    // x, y is the expected leaving position (x or y outside the border)
    // On success, the box is inserted into the outer space, and the original box is cleared

    int px, py, n_px, n_py;
    size_t parentLen;
    double base_p = 0;
    // Get the position of the leaving subspace in the outer space, in (px, py)
    Subspace& parent = map.getSubspace(getParentId());
    parent.getBoxXY(subspaceId, px, py);
    parentLen = map.getSubspace(getParentId()).getLen();
    switch(d) {
        case UP: n_px = px - 1; n_py = py; base_p = 1.0 * py / parentLen; break;
        case DOWN: n_px = px + 1; n_py = py; base_p = 1.0 * py / parentLen; break;
        case LEFT: n_px = px; n_py = py - 1; base_p = 1.0 * px / parentLen; break;
        case RIGHT: n_px = px; n_py = py + 1; base_p = 1.0 * px / parentLen; break;
    }

    MoveResult success = FAIL;
    if (n_px < 0 || n_px >= parentLen || n_py < 0 || n_py >= parentLen)
        // It's also leaving the outer space
        success = parent.exit(boxId, d, base_p + p * len / parentLen);
    else
        // Insert it into the outer space
        success = parent.insert(boxId, d, n_px, n_py);
    if (success == SUCCESS && x != -1 && y != -1)
        subBoxes[x][y] = EMPTY;
    return success;
}

MoveResult Subspace::insert(int boxId, Direction d, int x, int y) {
    // Try to insert a box into the subspace at (x,y) in a direction d
    // The box may be one that the inner space pushes out or one that the outer space pushes in
    // On success, the box is inserted into the subspace, but the original box is not cleared

    // x, y in [0, len)
    if (isWall(x, y)) return FAIL;
    // 1) Wall
    else if (isTileEmpty(x, y)) {
    // 2) Space
        subBoxes[x][y] = boxId;
        return SUCCESS;
    }
    // 3) Another box
    MoveResult res = move(x, y, d);
    if (res == SUCCESS) {
        // a) It's movable;
        subBoxes[x][y] = boxId;
        return SUCCESS;
    } else if (res == LOOP)
        // b) It's a loop
        return LOOP;
     else if (isSubspace(x, y))
        // c) Not movable, consider entering
        return map.getSubspace(subBoxes[x][y]).enter(boxId, d);
    return FAIL;
}