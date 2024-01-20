#include "SDL.h"
#include <io.h>
#include "SDL_ttf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <cstring>
using namespace std;

#define windowheight 800
#define windowwidth 800

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
std::string text;
int the_numberofgame = 0;
int main_table = 0;
int whole_game = 18;

//copy a bmp to renderer
int bmp_copy(const char *name,int x,int y,int w,int h){
    SDL_Surface* bmp_surf = SDL_LoadBMP(name);
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={x,y,w,h};
    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);
    SDL_FreeSurface(bmp_surf); 
    SDL_DestroyTexture(texture);
    
    return 0;
}

//draw the text
void DrawFont(SDL_Renderer* renderer,TTF_Font* font, const std::string& text,int x,int y){
    if(text.empty()) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font,text.c_str(),SDL_Color{0,0,0});
    if(!surface){
        SDL_Log(TTF_GetError());
    } 
    SDL_Rect dsRect = {x ,y ,surface->w, surface->h};
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    if(!texture){
        SDL_Log(TTF_GetError());
    } 
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer,texture,NULL,&dsRect);
    SDL_DestroyTexture(texture);
}

//load map form the stream **unfinished
int loadmap(SDL_Rect wind,std::string stream){
    return 0;
}

int isload(std::string text){
    if(!access(text.c_str(),4)){
        return 0;
    }
    return -1;
}
//     *** unfinished  **//
int savemap(std::string &text,SDL_Rect window_rect){
    ifstream ifs;
    ifs.open(text.c_str(),ios::in);
    char c;
    ifs >> c;
    std::ofstream outputFile(text.c_str());
    if (outputFile.is_open() && ifs.eof()) {
        //write the data into the file 


        outputFile.close();
        int size = 25*(window_rect.h)/600;
        TTF_Font *font= TTF_OpenFont("../../a.ttf",size);
        SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Data saved",{0,255,0});
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
        SDL_Rect font_bmp ={(window_rect.w-txt_font->w)/2,window_rect.h*6/14,txt_font->w,txt_font->h};
        SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
                                    
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(txt_font);
        TTF_CloseFont(font);
        SDL_RenderPresent(renderer);
        ifs.close();
    } else {
        int size = 25*(window_rect.h)/600;
        TTF_Font *font= TTF_OpenFont("../../a.ttf",size);
        SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Wrong saving",{255,0,0});
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
        SDL_Rect font_bmp ={(window_rect.w-txt_font->w)/2,window_rect.h*6/14,txt_font->w,txt_font->h};
        SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
                                    
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(txt_font);
        TTF_CloseFont(font);
        SDL_RenderPresent(renderer);
        SDL_StopTextInput();
    }
    ifs.close();

    return 0;
}

// initialize the program
int Init(){
    if(SDL_Init(SDL_INIT_EVERYTHING) !=0){
        SDL_Log("sdl init failed");
        return -1;
    }
    if(TTF_Init()<0){
        SDL_Log("failed:%s",TTF_GetError());
        return 0;
    }
    
    window =SDL_CreateWindow("Recurse Box",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,windowwidth,windowheight,SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    renderer =SDL_CreateRenderer(window,-1,0);

    return 0;
}

// initialize the welcome part
int Init_loadgame(SDL_Rect wind={0,0,windowwidth,windowheight}){
        
    SDL_Surface* bmp_surf = SDL_LoadBMP("../../imags/background.bmp");
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={0,0,wind.w,wind.h};
    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);

    int size = 36*(wind.h)/600;
    TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
    TTF_Font *font2 = TTF_OpenFont("../../a.ttf",size*2);
    if(font==NULL){
        SDL_Log("a failed");
        return -1;
    }
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Please press enter to start",{255,0,0});
    SDL_Surface *txt_font2 = TTF_RenderUTF8_Blended(font2," Recurse Box",{255,255,255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer,txt_font2);
    SDL_Rect font_bmp ={(wind.w-txt_font->w)/2,wind.h*2/3,txt_font->w,txt_font->h};
    SDL_Rect font_bmp2 ={(wind.w-txt_font->w)/2,wind.h/3,txt_font2->w,txt_font2->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    SDL_RenderCopy(renderer,tex2,NULL,&font_bmp2);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(tex2);
    SDL_FreeSurface(txt_font);
    SDL_FreeSurface(txt_font2);
    TTF_CloseFont(font);
    TTF_CloseFont(font2);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(bmp_surf); 
    SDL_DestroyTexture(texture);
    
    return 0;
}

// load the gemetable part where we can choose game or loading memories
int main_gametable(SDL_Rect wind={0,0,windowwidth,windowheight},int ii=0){
    SDL_Surface* bmp_surf = SDL_LoadBMP("../../imags/background.bmp");
    if (bmp_surf == NULL ) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    if (texture == NULL ) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={0,0,wind.w,wind.h};

    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);

    int k=255;
    if(ii==1){
        k = 0;
    }

    int size = 36*(wind.h)/600;
    TTF_Font *font = TTF_OpenFont("../../a.ttf",size*2);
    TTF_Font *font2 = TTF_OpenFont("../../a.ttf",size*2);
    TTF_Font *font3 = TTF_OpenFont("../../a.ttf",size/2);
    if(font==NULL || font3==NULL || font3==NULL ){
        SDL_Log("a failed");
        return -1;
    }
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Choose Level",{255,(Uint8)(255-k),255});
    SDL_Surface *txt_font2 = TTF_RenderUTF8_Blended(font2,"Load Game",{255,(Uint8)k,255});
    SDL_Surface *txt_font3 = TTF_RenderUTF8_Blended(font3,"Press Esc to end the game and ",{255,0,0});
    SDL_Surface *txt_font4 = TTF_RenderUTF8_Blended(font3,"press up and down key to choose.",{255,0,0});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer,txt_font2);
    SDL_Texture *tex3 = SDL_CreateTextureFromSurface(renderer,txt_font3);
    SDL_Texture *tex4 = SDL_CreateTextureFromSurface(renderer,txt_font4);
    SDL_Rect font_bmp ={(wind.w-txt_font->w)/2,wind.h*1/7,txt_font->w,txt_font->h};
    SDL_Rect font_bmp2 ={(wind.w-txt_font2->w)/2,wind.h*3/7,txt_font2->w,txt_font2->h};
    SDL_Rect font_bmp3 ={(wind.w-txt_font3->w)/2,wind.h*5/7,txt_font3->w,txt_font3->h};
    SDL_Rect font_bmp4 ={(wind.w-txt_font4->w)/2,wind.h*11/14,txt_font4->w,txt_font4->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    SDL_RenderCopy(renderer,tex2,NULL,&font_bmp2);
    SDL_RenderCopy(renderer,tex3,NULL,&font_bmp3);
    SDL_RenderCopy(renderer,tex4,NULL,&font_bmp4);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(tex2);
    SDL_DestroyTexture(tex3);
    SDL_DestroyTexture(tex4);
    SDL_FreeSurface(txt_font);
    SDL_FreeSurface(txt_font2);
    SDL_FreeSurface(txt_font3);
    SDL_FreeSurface(txt_font4);
    TTF_CloseFont(font);
    TTF_CloseFont(font2);
    TTF_CloseFont(font3);


    SDL_RenderPresent(renderer);
    SDL_FreeSurface(bmp_surf); 
    SDL_DestroyTexture(texture);

    
    return 0;
}

int winning_table(SDL_Rect wind={0,0,windowwidth,windowheight}){
    SDL_Surface* bmp_surf = SDL_LoadBMP("../../imags/background.bmp");
    SDL_Surface* bmp_surf2 = SDL_LoadBMP("../../imags/fireworks.bmp");
    if (bmp_surf == NULL || bmp_surf2 == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, bmp_surf2);
    if (bmp_surf == NULL || bmp_surf2 == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={0,0,wind.w,wind.h};
    SDL_Rect re_bmp2 ={wind.w/5,0,400,400};
    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);
    SDL_RenderCopy(renderer,texture2, NULL, &re_bmp2);

    int size = 36*(wind.h)/600;
    TTF_Font *font = TTF_OpenFont("../../a.ttf",size*2);
    TTF_Font *font2 = TTF_OpenFont("../../a.ttf",size/2);
    if(font==NULL){
        SDL_Log("a failed");
        return -1;
    }
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Congratulations",{255,255,255});
    SDL_Surface *txt_font2 = TTF_RenderUTF8_Blended(font2,"Please press enter to return to last interface",{255,0,0});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer,txt_font2);
    SDL_Rect font_bmp ={(wind.w-txt_font->w)/2,wind.h/2,txt_font->w,txt_font->h};
    SDL_Rect font_bmp2 ={(wind.w-txt_font2->w)/2,wind.h*4/5,txt_font2->w,txt_font2->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    SDL_RenderCopy(renderer,tex2,NULL,&font_bmp2);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(tex2);
    SDL_FreeSurface(txt_font);
    SDL_FreeSurface(txt_font2);
    TTF_CloseFont(font);
    TTF_CloseFont(font2);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(bmp_surf); 
    SDL_FreeSurface(bmp_surf2); 
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(texture2);

    return 0;
}

int game_table(SDL_Rect wind={0,0,windowwidth,windowheight},int num=1){
    SDL_Surface* bmp_surf = SDL_LoadBMP("../../imags/background.bmp");
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={0,0,wind.w,wind.h};
    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);

    int size = 60*(wind.h)/600;
    TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
    if(font==NULL){
        SDL_Log("a failed");
        return -1;
    }
    
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Game Level",{255,255,255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Rect font_bmp ={(wind.w-txt_font->w)/2,wind.h*1/12,txt_font->w,txt_font->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(txt_font);
    TTF_CloseFont(font);
    TTF_Font *font2 = TTF_OpenFont("../../a.ttf",size/3);
    if(font==NULL){
        SDL_Log("a failed");
        return -1;
    }
    
    SDL_Surface *txt_font2 = TTF_RenderUTF8_Blended(font2,"In game ,you can press enter to save.",{255,0,0});
    SDL_Texture *tex2 = SDL_CreateTextureFromSurface(renderer,txt_font2);
    SDL_Rect font_bmp2 ={(wind.w-txt_font2->w)/2,wind.h*1/5,txt_font2->w,txt_font2->h};
    SDL_RenderCopy(renderer,tex2,NULL,&font_bmp2);
    SDL_DestroyTexture(tex2);
    SDL_FreeSurface(txt_font2);
    TTF_CloseFont(font2);
        for (int i=1;i<=10;i++){
            int size = 36*(wind.h)/600;
            TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
            std::string mystring = "Level ";
                if(font==NULL){
                    SDL_Log("a failed");
                    return -1;
                }
            std::string istring = std::to_string(i);
            mystring = mystring +istring;
            SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,mystring.c_str(),{255,255,255});
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
            SDL_Rect font_bmp ={(wind.w/2-txt_font->w)/2,wind.h*(i+2)/13,txt_font->w,txt_font->h};
            SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(txt_font);
            TTF_CloseFont(font);
        } 
        for (int i=11;i<=whole_game;i++){
            int size = 36*(wind.h)/600;
            TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
            std::string mystring = "Level ";
                if(font==NULL){
                    SDL_Log("a failed");
                    return -1;
                }  
            std::string istring = std::to_string(i);
            mystring = mystring +istring;
            SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,mystring.c_str(),{255,255,255});
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
            SDL_Rect font_bmp ={(wind.w*3/2-txt_font->w)/2,wind.h*(i-8)/13,txt_font->w,txt_font->h};
            SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(txt_font);
            TTF_CloseFont(font);
        } 
        if(num>=11){
            int size = 36*(wind.h)/600;
            TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
            std::string mystring = "Level ";
                if(font==NULL){
                    SDL_Log("a failed");
                    return -1;
                }  
            std::string istring = std::to_string(num);
            mystring = mystring +istring;
            SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,mystring.c_str(),{255,0,255});
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
            SDL_Rect font_bmp ={(wind.w*3/2-txt_font->w)/2,wind.h*(num-8)/13,txt_font->w,txt_font->h};
            SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(txt_font);
            TTF_CloseFont(font);
        }else{
            int size = 36*(wind.h)/600;
            TTF_Font *font = TTF_OpenFont("../../a.ttf",size);
            std::string mystring = "Level ";
                if(font==NULL){
                    SDL_Log("a failed");
                    return -1;
                }
            std::string istring = std::to_string(num);
            mystring = mystring +istring;
            SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,mystring.c_str(),{255,0,255});
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
            SDL_Rect font_bmp ={(wind.w/2-txt_font->w)/2,wind.h*(num+2)/13,txt_font->w,txt_font->h};
            SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(txt_font);
            TTF_CloseFont(font);
        }

    
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(bmp_surf); 
    SDL_DestroyTexture(texture);
    
    return 0;
}

int load_game_table(){
    SDL_Rect load_rect ={0,0,windowwidth,windowheight};
    SDL_GetWindowSize(window, &load_rect.w, &load_rect.h);

    bmp_copy("../../imags/background.bmp",0,0,load_rect.w,load_rect.h);
    bmp_copy("../../imags/text.bmp",100,500,600,50);

    
    int size = 36*(load_rect.h)/600;
    TTF_Font *font= TTF_OpenFont("../../a.ttf",size);
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Please input the name of the file.",{255,0,255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Rect font_bmp ={(load_rect.w-txt_font->w)/2,load_rect.h*5/14,txt_font->w,txt_font->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(txt_font);
    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
    return 0;
}

int save_game_table(){
    SDL_Rect load_rect ={0,0,windowwidth,windowheight};
    SDL_GetWindowSize(window, &load_rect.w, &load_rect.h);

    bmp_copy("../../imags/background.bmp",0,0,load_rect.w,load_rect.h);
    bmp_copy("../../imags/text.bmp",100,500,600,50);

    
    int size = 36*(load_rect.h)/600;
    TTF_Font *font= TTF_OpenFont("../../a.ttf",size);
    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Please input the name of the file.",{255,0,255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
    SDL_Rect font_bmp ={(load_rect.w-txt_font->w)/2,load_rect.h*5/14,txt_font->w,txt_font->h};
    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
    
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(txt_font);
    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
    return 0;
}

int gamming_table(SDL_Rect wind,int num){
     SDL_Surface* bmp_surf = SDL_LoadBMP("../../imags/background.bmp");
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, bmp_surf);
    if (bmp_surf == NULL) {
        SDL_Log("bmp failed");
        return 0;
    }
    SDL_Rect re_bmp ={0,0,wind.w,wind.h};
    SDL_RenderCopy(renderer,texture, NULL, &re_bmp);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(bmp_surf); 
    SDL_DestroyTexture(texture);
    return 0;
}

//to receive the input command
void gameevent(){
    SDL_Event event;
    bool isquit=false;
    int celect;
    SDL_Rect window_rect ={0,0,windowwidth,windowheight};
    bool isdone =true; //to judge whether past done
    int number; //to store the number which entered
    SDL_StartTextInput();
    while (!isquit){
        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    return;
                case SDL_WINDOWEVENT: 
                    if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                        SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                        Init_loadgame(window_rect);
                        break;
                    }
                case SDL_TEXTINPUT:
                    if (the_numberofgame == 3 ||the_numberofgame == 5)
                        {
                            text += event.text.text;
                            SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                            int size = 36*(window_rect.h)/600;
                            TTF_Font *font= TTF_OpenFont("../../song.ttf",size);
                            DrawFont(renderer,font,text,120,500);
                            SDL_RenderPresent(renderer);
                            TTF_CloseFont(font);
                            break; 
                        }
                case SDL_KEYUP:
                    if(event.key.keysym.scancode){
                    isdone = true;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (isdone){                    
                        isdone = false;
                        number=event.key.keysym.scancode;
                        SDL_Log("%d",celect);
                        SDL_Log("%d %d %d",event.key.keysym.scancode,the_numberofgame,game_table);
                        if (event.key.keysym.scancode == 40){
                            //the enter keyboard
                            if(the_numberofgame == 0){
                                the_numberofgame = 1;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                main_gametable(window_rect,main_table);
                            }else if (the_numberofgame == 1)
                            {
                                printf("%d %d",the_numberofgame,main_table);
                                if (main_table == 0){
                                    the_numberofgame = 2;
                                    celect = 1;
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    game_table(window_rect,celect);
                                    break;
                                }else{
                                    the_numberofgame = 3;
                                    SDL_StartTextInput();
                                    text = "";
                                    load_game_table();
                                    break;
                                }
                            }else if (the_numberofgame == 2)
                            {
                                the_numberofgame = 4;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                gamming_table(window_rect,celect);
                                break;
                            }else if (the_numberofgame == 3)
                            {
                                std::string append = "../../save/";
                                std::string append2 = ".txt";
                                std::string full_text =  append+text ;
                                full_text = full_text +append2;
                                if (isload(full_text) == 0){
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    loadmap(window_rect,full_text);
                                    break;
                                }else{
                                    int size = 25*(window_rect.h)/600;
                                    TTF_Font *font= TTF_OpenFont("../../a.ttf",size);
                                    SDL_Surface *txt_font = TTF_RenderUTF8_Blended(font,"Error file",{255,0,0});
                                    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,txt_font);
                                    SDL_Rect font_bmp ={(window_rect.w-txt_font->w)/2,window_rect.h*6/14,txt_font->w,txt_font->h};
                                    SDL_RenderCopy(renderer,tex,NULL,&font_bmp);
                                    
                                    SDL_DestroyTexture(tex);
                                    SDL_FreeSurface(txt_font);
                                    TTF_CloseFont(font);
                                    SDL_RenderPresent(renderer);
                                    break;
                                }
                            }else if (the_numberofgame == 4){
                                the_numberofgame = 5;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                SDL_StartTextInput();
                                text = "";
                                save_game_table();
                                break;
                            }else if (the_numberofgame == 5)
                            {    
                                std::string append = "../../save/";
                                std::string append2 = ".txt";
                                std::string full_text =  append+text ;
                                full_text = full_text +append2;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                savemap(full_text,window_rect);
                                break;
                            }else if (the_numberofgame == 6){
                                the_numberofgame = 2;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    game_table(window_rect,celect);
                                break;
                            }
                               
                        }else if (event.key.keysym.sym == SDLK_UP)
                        {
                            if(the_numberofgame == 1){
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                main_table = 0;
                                main_gametable(window_rect,main_table);
                            }else if (the_numberofgame == 2 && celect != 1 && celect != 11)
                            {
                                celect = celect - 1;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                game_table(window_rect,celect);
                                break;
                            }
                            break;
                        }else if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            if(the_numberofgame == 1){
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                main_table = 1;
                                main_gametable(window_rect,main_table);
                            }else if (the_numberofgame == 2 && celect != 10 && celect != whole_game)
                            {
                                celect = celect + 1;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                game_table(window_rect,celect);
                            }
                            break;
                        }else if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            if (the_numberofgame == 2 && celect >= 11)
                            {
                                celect = celect - 10;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                game_table(window_rect,celect);
                            }
                            break;
                        }else if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            if (the_numberofgame == 2 && celect < 11 && celect + 10 <= whole_game)
                            {
                                celect = celect + 10;
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                game_table(window_rect,celect);
                            }   
                            break;
                        }else if (event.key.keysym.scancode == 41){
                            //the esc keyboard
                            if (the_numberofgame > 0){
                                if(the_numberofgame == 1){
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    main_table = 0;
                                    the_numberofgame--;
                                    Init_loadgame(window_rect);
                                    break;
                                }else if (the_numberofgame == 2 ){
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    main_table = 0;
                                    the_numberofgame = 1;
                                    main_gametable(window_rect,main_table);
                                    break;
                                }else if ( the_numberofgame == 3){
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    main_table = 1;
                                    the_numberofgame = 1;
                                    main_gametable(window_rect,main_table);
                                    break;
                                }else if (the_numberofgame == 4){
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    the_numberofgame = 2;
                                    game_table(window_rect,celect);
                                    break;
                                }else if (the_numberofgame == 5)
                                {
                                    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                    the_numberofgame = 4;
                                    gamming_table(window_rect,celect);
                                    break;
                                }
                                
                            }else{
                            return;
                            }
                        }else if (event.key.keysym.sym == SDLK_BACKSPACE & (the_numberofgame == 3||the_numberofgame == 5)){
                            //the backspace keyboard  
                            if (!text.empty()){
                                text.pop_back();
                                load_game_table();
                                SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);
                                int size = 36*(window_rect.h)/600;
                                TTF_Font *font= TTF_OpenFont("../../song.ttf",size);
                                DrawFont(renderer,font,text,120,500);
                                SDL_RenderPresent(renderer);
                                TTF_CloseFont(font);
                            }
                            break;
                        }
                    break;
                
                default:
                    break;
                }
            }
        }
        
    }
}

//quit the program
void unInit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return ;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    Init();

    Init_loadgame();
    
    gameevent();
    
    unInit();

    return 0;
}
