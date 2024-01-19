#include "Box.h"
#include <stack>
using std::stack;

bool Subspace::move(int x, int y, Direction d) {
    // Move the box at x row, y col to a direction d
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

    // Search in the direction d, till an empty space, a wall, or end of the subspace
    // Priority: Move > Enter (A -into-> B) > Eat (A <-into- B)
    int nx = x + dx, ny = y + dy;
    while (1) {
        if (nx < 0 || nx >= len || ny < 0 || ny >= len) {
            // When the push goes out of current subspace
            // TODO
        } else if (innerSpace[nx][ny] == WALL) {
            // When a wall is found on path
            // Moving is not possible, check back if any subspace is enterable
            int lastx = nx - dx, lasty = ny - dy;
            // 1) Check push-in: (nx-dx, nx-dy) into (nx, ny)
            nx = lastx, ny = lasty;
            while (nx != x && ny != y) {
                int curId = subBoxes[nx][ny], prevId = subBoxes[nx - dx][ny - dy];
                if (curId > SUBSPACE_ID) // A subspace is found
                    if (map.getSubspace(curId).enter(prevId, d)) {
                    // Box at (nx-dx, ny-dy) moves into the subspace,
                    // then break to let other boxes in the subspace move
                        nx -= dx; ny -= dy;
                        break;
                    }
                nx -= dx; ny -= dy;
            }
            // 2) Check eating: (nx-dx, nx-dy) eats (nx, ny)
            nx = lastx, ny = lasty;
            while (nx != x && ny != y) {
                int curId = subBoxes[nx][ny], prevId = subBoxes[nx - dx][ny - dy];
                if (prevId > SUBSPACE_ID)
                    if (map.getSubspace(prevId).enter(curId, invert(d))) {
                    // Box at (nx, ny) is eaten by (nx-dx, nx-dy)
                    // Break to let other boxes in the subspace move
                        break;
                    }
            }
        } else if (subBoxes[nx][ny] != EMPTY) {
            // When a movable box is found on path, continue search in the direction d
            nx += dx; ny += dy;
        } else {
            // A space is found, move the boxes
            break;
        }
    }
    // Move boxes searching from (nx, ny) back
    while (nx != x && ny != y) {
        subBoxes[nx][ny] = subBoxes[nx - dx][ny - dy];
        subBoxes[nx - dx][ny - dy] = EMPTY;
        nx -= dx; ny -= dy;
    }
    return true;
}