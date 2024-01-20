#include "Box.h"
#include <stack>
using std::stack;

// TODO: loop; infinity


bool Subspace::move(int x, int y, Direction d) {
    // Try to move the box at x row, y col to a direction d by 1 step
    // Return true if the move is successful and (x,y) is now vacant, false otherwise
    int dx, dy;
    switch (d) {
        case UP: dx = -1; dy = 0; break;
        case DOWN: dx = 1; dy = 0; break;
        case LEFT: dx = 0; dy = -1; break;
        case RIGHT: dx = 0; dy = 1; break;
    }
    // If (x,y) is not a movable box, return false
    if (innerSpace[x][y] != EMPTY) return false;
    if (subBoxes[x][y] == EMPTY) return false;

    int curId = subBoxes[x][y];
    // 4 cases: 1) Edge 2) Wall 3) Space 4) Another box
    int nx = x + dx, ny = y + dy;
    if (nx < 0 || nx >= len || ny < 0 || ny >= len) {
    // 1) Edge
        // a) Infinity
        if (getParentId() == 0 || getParentId() == subspaceId) {
            // TODO: Check infinity
            return false;
        }
        // b) Normal
        int posx, posy; // Find the position of the leaving subspace in the outer space
        int parentId = getParentId();
        map.getSubspace(parentId).getBoxXY(subspaceId, posx, posy);
        if (map.getSubspace(parentId).enter(subBoxes[x][y], d, posx + dx, posy + dy)) {
        // Try to push the current box to the outer area
            subBoxes[x][y] = EMPTY;
            return true;
        } else
            return false;
    } else if (isWall(nx, ny))
    // 2) Wall
        return false;
    else if (isTileEmpty(nx, ny)) {
    // 3) Space
        subBoxes[nx][ny] = curId;
        if (subBoxes[x][y] == curId) subBoxes[x][y] = EMPTY;
        return true;
    }
    // 4) Another box
    if (move(nx, ny, d)) {
        // a) It's movable
        subBoxes[nx][ny] = curId;
        if (subBoxes[x][y] == curId) subBoxes[x][y] = EMPTY;
        return true;
    } else if (isSubspace(nx, ny)
            && map.getSubspace(subBoxes[nx][ny]).enter(curId, d)) {
        // b) It's enterable
        if (subBoxes[x][y] == curId) subBoxes[x][y] = EMPTY;
        return true;
    } else if (isSubspace(x, y)
            && map.getSubspace(curId).enter(subBoxes[nx][ny], invert(d))) {
        // c) It's eatable
        subBoxes[nx][ny] = curId;
        if (subBoxes[x][y] == curId) subBoxes[x][y] = EMPTY;
        return true;
    }
    return false;
}

// The move function attempts to move a box to a direction d
// Returns true when anything is moved, but due to loops the space may not be empty
// Below are intermediate functions operating on a specific box and do not process loops

bool Subspace::enter(int boxId, Direction d, double p) {
    // Push a box into the subspace in a direction d, with relative position p
    // Returns true if the box is successfully pushed into the subspace, false otherwise
    // On success, the box is inserted into the subspace, but the original box is not cleared

    int nx, ny; // The position of the entering box in the subspace
    switch (d) {
        case UP: nx = len - 1; ny = len * p; break;
        case DOWN: nx = 0; ny = len * p; break;
        case LEFT: nx = len * p; ny = len - 1; break;
        case RIGHT: nx = len * p; ny = 0; break;
    }
    if (isWall(nx, ny)) return false;
    // 1) Wall
    else if (isTileEmpty(nx, ny)) {
    // 2) Space
        subBoxes[nx][ny] = boxId;
        return true;
    }
    // 3) Another box
    if (move(nx, ny, d)) {
        // a) It's movable; check if a loop occurred
        if (subBoxes[nx][ny] != EMPTY) return false;
        subBoxes[nx][ny] = boxId;
        return true;
    } else if (isSubspace(nx, ny)) {
        double base_p;
        switch (d) {
            case UP: case DOWN: base_p = 1.0 * ny / len; break;
            case LEFT: case RIGHT: base_p = 1.0 * nx / len; break;
        }
        Subspace& subspace = map.getSubspace(subBoxes[nx][ny]);
        p = (p - base_p) * len / subspace.getLen();
        if (subspace.enter(boxId, d, p))
        // b) It's enterable
            return true;
    }
    return false;
}

bool Subspace::exit(int boxId, Direction d, double p) {
    // Pop a box out of the subspace in a direction d, with relative position p
    // x, y is the expected leaving position (x or y outside the border)
    // Returns true if the box is successfully popped out of the subspace, false otherwise
    // On success, the box is inserted into the outer space, but the original box is not cleared

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

    if (n_px < 0 || n_px >= parentLen || n_py < 0 || n_py >= parentLen)
        // It's also leaving the outer space
        return parent.exit(boxId, d, base_p + p * len / parentLen);
    else
        // Insert it into the outer space
        return parent.insert(boxId, d, n_px, n_py);
}

bool Subspace::insert(int boxId, Direction d, int x, int y) {
    // Try to insert a box into the subspace at (x,y) in a direction d
    // The box may be one that the inner space pushes out or one that the outer space pushes in
    // Returns true if the box is successfully inserted into the subspace, false otherwise
    // On success, the box is inserted into the subspace, but the original box is not cleared

    // x, y in [0, len)
    if (isWall(x, y)) return false;
    // 1) Wall
    else if (isTileEmpty(x, y)) {
    // 2) Space
        subBoxes[x][y] = boxId;
        return true;
    }
    // 3) Another box
    if (move(x, y, d)) {
        // a) It's movable; check if a loop occurred
        if (subBoxes[x][y] != EMPTY) return false;
        subBoxes[x][y] = boxId;
        return true;
    } else if (isSubspace(x, y)
            && map.getSubspace(subBoxes[x][y]).enter(boxId, d))
        // b) It's enterable
        return true;
    return false;
}