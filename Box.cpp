#include "Box.h"
#include <stack>
using std::stack;

// TODO: loop; infinity

bool Subspace::move(int x, int y, Direction d) {
    // Try to move the box at x row, y col to a direction d by 1 step
    // Return true if the move is successful, false otherwise
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

    // 4 cases: 1) Edge 2) Wall 3) Space 4) Another box
    int nx = x + dx, ny = y + dy;
    if (nx < 0 || nx >= len || ny < 0 || ny >= len) {
    // 1) Edge
        // a) Infinity
        if (getParentId() == 0 || getParentId() == subspaceId) {
            // TODO
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
        subBoxes[nx][ny] = subBoxes[x][y];
        subBoxes[x][y] = EMPTY;
        return true;
    }
    // 4) Another box
    if (move(nx, ny, d)) {
        // a) It's movable
        subBoxes[nx][ny] = subBoxes[x][y];
        subBoxes[x][y] = EMPTY;
        return true;
    } else if (isSubspace(nx, ny)
            && map.getSubspace(subBoxes[nx][ny]).enter(subBoxes[x][y], d)) {
        // b) It's enterable
        subBoxes[x][y] = EMPTY;
        return true;
    } else if (isSubspace(x, y)
            && map.getSubspace(subBoxes[x][y]).enter(subBoxes[nx][ny], invert(d))) {
        // c) It's eatable
        subBoxes[nx][ny] = subBoxes[x][y];
        subBoxes[x][y] = EMPTY;
        return true;
    }
    return false;
}

bool Subspace::enter(int boxId, Direction d, int x, int y) {
}