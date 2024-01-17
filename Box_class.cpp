#include<algorithm>
#include"Box.h"
/*
@todo
implete box
*/

string Box::Serialize(){
    return "Box";
}

/*@todo
implete player*/
player::player(){
    Identity = PLAYER;
}
player::~player(){
    Identity = EMPTY;
}
/*@todo
implete wall*/
wall::wall(){
    Identity = WALL;
}
wall::~wall(){
    Identity = EMPTY;
}
/*@todo
implete map*/
Map::Map(){
    Identity = SUB_MAP;
}
Map::Map(int index_of_map, int x_length, int y_length, int x_player, int y_player, 
        std::vector<int> x_destination, std::vector<int> y_destination, Box **map){
    Identity = SUB_MAP;
    this->index_of_map = index_of_map;
    this->x_length = x_length;
    this->y_length = y_length;
    this->x_player = x_player;
    this->y_player = y_player;
    this->x_destination = x_destination;
    this->y_destination = y_destination;
    this->map = map;
}
Map::~Map(){
    Identity = EMPTY;
}
bool Map::hasPlayer(){
    return x_player != -1 && y_player != -1;
}
// bool Map::isEnterable(Direction d, Map &submap){
//     int x = -1, y = -1;
//     switch(d){
//         case UP:
//             x = 0, y = y_length/2;
//             break;
//         case DOWN:
//             x = x_length-1, y = y_length/2;
//             break;
//         case LEFT:
//             x = x_length/2, y = 0;
//             break;
//         case RIGHT:
//             x = x_length/2, y = y_length-1;
//             break;
//     }
//     if(Movable(d, x, y, submap)){
//         return true;
//     }else{
//         return false;
//     }
// }
/*@param
x y is the source coordinate, d is direction*/
int Map::Movable(Direction d, int x_temp, int y_temp, Map &submap){
    int x = x_temp, y = y_temp;
    int parameter = 0;
    switch(d){
        case UP:
            parameter = -1;
            break;
        case DOWN:
            parameter = 1;
            break;
        case LEFT:
            parameter = -1;
            break;
        case RIGHT:
            parameter = 1;
            break;
    }
    if(d == UP || d == DOWN){
        while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
            x+=parameter;
        }
        if(submap.map[x][y].Identity == EMPTY){
            return 1;
        }else if(submap.map[x][y].Identity == WALL){
            x = x_temp;
            y = y_temp;
            while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
                if(submap.map[x][y].Identity == SUB_MAP){
                    Map m = submap.map[x][y];
                    if(d == UP){
                        int x_enter = 0, y_enter = m.y_length/2;
                        if(m.Movable(d, x_enter, y_enter, m) == 1){
                            return 1;
                        }else if(m.Movable(d, x_enter, y_enter, m) == 2){
                            return 2;
                        }else{
                            return 0;
                        }
                    }else{
                        int x_enter = m.x_length-1, y_enter = m.y_length/2;
                        if(m.Movable(d, x_enter, y_enter, m) == 1){
                            return 1;
                        }else if(m.Movable(d, x_enter, y_enter, m) == 2){
                            return 2;
                        }else{
                            return 0;
                        }
                    }
                }
                x+=parameter;
            }
        }
        return 0;
    }else if(d == LEFT || d == RIGHT){
        while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
            y+=parameter;
        }
        if(submap.map[x][y].Identity == EMPTY){
            return 1;
        }else if(submap.map[x][y].Identity == WALL){
            x = x_temp;
            y = y_temp;
            while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
                if(submap.map[x][y].Identity == SUB_MAP){
                    Map m = submap.map[x][y];
                    if(d == LEFT){
                        int x_enter = m.x_length/2, y_enter = 0;
                        if(m.Movable(d, x_enter, y_enter, m) == 1){
                            return 1;
                        }else if(m.Movable(d, x_enter, y_enter, m) == 2){
                            return 2;
                        }else{
                            return 0;
                        }
                    }else{
                        int x_enter = m.x_length/2, y_enter = m.y_length-1;
                        if(m.Movable(d, x_enter, y_enter, m) == 1){
                            return 1;
                        }else if(m.Movable(d, x_enter, y_enter, m) == 2){
                            return 2;
                        }else{
                            return 0;
                        }
                    }
                }
                y+=parameter;
            }
        }
    }
    return 0;
}
bool Map::Move(Direction d, Map &submap){
    int judge = Movable(d, submap.x_player, submap.y_player, submap);
    int parameter = 0;
    int x = submap.x_player, y = submap.y_player;
    if(judge == 1){
        switch(d){
            case UP:
                parameter = -1;
                break;
            case DOWN:
                parameter = 1;
                break;
            case LEFT:
                parameter = -1;
                break;
            case RIGHT:
                parameter = 1;
                break;
        }
        if(d == UP || d == DOWN){
            while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
                x+=parameter;
            }
            int final_x = x;
            while(parameter != submap.x_player){
                std::swap(submap.map[x][y], submap.map[x-parameter][y]);
                x-=parameter;
            }
            submap.x_player = final_x;
        }else if(d == RIGHT || d == LEFT){
            while(submap.map[x][y].Identity == BOX || submap.map[x][y].Identity == SUB_MAP){
                y+=parameter;
            }
            int final_y = y;
            while(parameter != submap.y_player){
                std::swap(submap.map[x][y], submap.map[x][y-parameter]);
                y-=parameter;
            }
            submap.y_player = final_y;
        }
    }else if(judge == 2){ //find the final empty or cycle then swap backward
        switch(d){
            case UP:
                parameter = -1;
                break;
            case DOWN:
                parameter = 1;
                break;
            case LEFT:
                parameter = -1;
                break;
            case RIGHT:
                parameter = 1;
                break;
        }
        
    }else{
        return false;
    }
}