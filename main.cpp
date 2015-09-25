
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<map>
#include<vector>
#include<SDL2/SDL_mixer.h>

#include "TinyXml/tinyxml.h"

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event Event;
SDL_Texture *background, *background1, *background2, *background3, *texture_npc, *texture_tile, *texture_flag, *texture_flagm;
SDL_Rect rect_background, rect_background1, rect_background2, rect_background3, rect_character, rect_npc, rect_tile, rect_tileset, rect_flag, rect_flagm;

class Warp
{
public:
    int x;
    int y;
    string mapa;
    SDL_Rect rect;
    Warp(int x,int y,string mapa,SDL_Rect rect)
    {
        this->x=x;
        this->y=y;
        this->mapa=mapa;
        this->rect = rect;
    }
};

bool collision(SDL_Rect r1, SDL_Rect r2)
{
    if(r1.x+r1.w <= r2.x)
        return false;
    if(r1.x >= r2.x+r2.w)
        return false;
    if(r1.y+r1.h<=r2.y)
        return false;
    if(r1.y >= r2.y+r2.h)
        return false;

    return true;
}

vector<Warp*> getWarps(string archivo)
{
    TiXmlDocument doc(archivo.c_str());
    bool loadOkay = doc.LoadFile();
    TiXmlElement *map_node = doc.FirstChild("map")->ToElement();
    TiXmlNode*objectgroup_node = map_node->FirstChild("objectgroup");

    vector<Warp*> respuesta;

    if(objectgroup_node==NULL)
        return respuesta;

    for(TiXmlNode*object_node = objectgroup_node->FirstChild("object");
        object_node!=NULL;
        object_node=object_node->NextSibling("object"))
    {
        int x,y;
        string mapa;
        SDL_Rect rect;
        rect.x = atoi(object_node->ToElement()->Attribute("x"));
        rect.y = atoi(object_node->ToElement()->Attribute("y"));
        rect.w = atoi(object_node->ToElement()->Attribute("width"));
        rect.h = atoi(object_node->ToElement()->Attribute("height"));

        TiXmlNode*properties = object_node->FirstChild("properties");
        for(TiXmlNode*property = properties->FirstChild("property");
            property!=NULL;
            property=property->NextSibling("property"))
        {
            if(strcmp(property->ToElement()->Attribute("name"),"x")==0)
            {
                cout<<"X:"<<property->ToElement()->Attribute("value")<<endl;
                x=atoi(property->ToElement()->Attribute("value"));
            }
            if(strcmp(property->ToElement()->Attribute("name"),"y")==0)
            {
                y=atoi(property->ToElement()->Attribute("value"));
            }
            if(strcmp(property->ToElement()->Attribute("name"),"mapa")==0)
            {
                mapa=property->ToElement()->Attribute("value");
            }
        }
        Warp*warp = new Warp(x,y,mapa,rect);
        cout<<warp->x<<endl;
        cout<<warp->y<<endl;
        cout<<warp->mapa<<endl;
        cout<<warp->rect.x<<endl;
        cout<<warp->rect.y<<endl;
        cout<<warp->rect.w<<endl;
        cout<<warp->rect.h<<endl;
        respuesta.push_back(warp);
    }

    return respuesta;
}

vector<int> getMapa(string archivo,int layer)
{
    vector<int> mapa;
    TiXmlDocument doc(archivo.c_str());
    bool loadOkay = doc.LoadFile();
    TiXmlElement *map_node = doc.FirstChild("map")->ToElement();
    TiXmlNode*layer_node_temp = map_node->FirstChild("layer");
    for(int i=1;i<layer;i++)
        layer_node_temp=layer_node_temp->NextSibling("layer");

    TiXmlElement *layer_node = layer_node_temp->ToElement();

    for(TiXmlNode *tile_node = layer_node->FirstChild("data")->FirstChild("tile");
        tile_node!=NULL;
        tile_node=tile_node->NextSibling("tile"))
    {
        mapa.push_back(atoi(tile_node->ToElement()->Attribute("gid")));
    }
    return mapa;
}

void dibujarLayer(SDL_Renderer* renderer,vector<int>mapa)
{
    int x_pantalla = 0;
    int y_pantalla = 0;
    for(int i=0;i<mapa.size();i++)
    {
        int x = 0;
        int y = 0;
        for(int acum = 1;acum<mapa[i];acum++)
        {
            x+=32;
            if(acum%16==0)
            {
                y+=32;
                x=0;
            }
        }

    //            rect_tile.x = 32*(mapa[i]%16-1);
    //            rect_tile.y = 32*(mapa[i]/16);
        rect_tile.x = x;
        rect_tile.y = y;
        rect_tile.w = 32;
        rect_tile.h = 32;

        //cout<<rect_tile.x<<","<<rect_tile.y<<endl;

        rect_tileset.x = x_pantalla;
        rect_tileset.y = y_pantalla;

        if(mapa[i]!=0)
            SDL_RenderCopy(renderer, texture_tile, &rect_tile, &rect_tileset);

        x_pantalla+=32;
        if(x_pantalla>=320)
        {
            x_pantalla=0;
            y_pantalla+=32;
        }
    }
}

bool collisionLayer(vector<int>collision_map,SDL_Rect rect_personaje)
{
//    rect_personaje.x+=1;
//    rect_personaje.y+=1;
//    rect_personaje.w-=2;
//    rect_personaje.h-=2;

    int x_pantalla = 0;
    int y_pantalla = 0;
    for(int i=0;i<collision_map.size();i++)
    {
        int x = 0;
        int y = 0;
        for(int acum = 1;acum<collision_map[i];acum++)
        {
            x+=32;
            if(acum%16==0)
            {
                y+=32;
                x=0;
            }
        }

    //            rect_tile.x = 32*(mapa[i]%16-1);
    //            rect_tile.y = 32*(mapa[i]/16);
        rect_tile.x = x;
        rect_tile.y = y;
        rect_tile.w = 32;
        rect_tile.h = 32;

        //cout<<rect_tile.x<<","<<rect_tile.y<<endl;

        rect_tileset.x = x_pantalla;
        rect_tileset.y = y_pantalla;
        rect_tileset.w = 32;
        rect_tileset.h = 32;

        if(collision_map[i]!=0)
        {
            if(collision(rect_personaje,rect_tileset))
            {
                return true;
            }
        }

        x_pantalla+=32;
        if(x_pantalla>=320)
        {
            x_pantalla=0;
            y_pantalla+=32;
        }
    }
    return false;
}

int main( int argc, char* args[] )
{
    //Init SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return 10;
    }
    //Creates a SDL Window
    if((window = SDL_CreateWindow("Image Loading", 100, 100, 320/*WIDTH*/, 320/*HEIGHT*/, SDL_WINDOW_RESIZABLE | SDL_RENDERER_PRESENTVSYNC)) == NULL)
    {
        return 20;
    }
    //SDL Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL)
    {
        std::cout << SDL_GetError() << std::endl;
        return 30;
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2,2048);

    Mix_Music *music= Mix_LoadMUS("music.mp3");
    Mix_Chunk *win= Mix_LoadWAV("win.wav");
    Mix_Chunk *lose= Mix_LoadWAV("lose.wav");

    //Init textures
    int w=0,h=0;
    background1 = IMG_LoadTexture(renderer,"fondo.png");
    SDL_QueryTexture(background1, NULL, NULL, &w, &h);
    rect_background1.x = 0; rect_background1.y = 0; rect_background1.w = w; rect_background1.h = h;

    char orientation = 'd';// d u l r
    int current_sprite = 0;
    int animation_velocity = 20;
    int velocity;
    if(velocity==0)
        velocity=0;
    else
        velocity=10;
    int npc_velocity = 5;
    int frame = 0;
    map<char,vector<SDL_Texture*> >sprites;
    sprites['u'].push_back(IMG_LoadTexture(renderer, "personaje/up1.png"));
    sprites['u'].push_back(IMG_LoadTexture(renderer, "personaje/up2.png"));
    sprites['d'].push_back(IMG_LoadTexture(renderer, "personaje/down1.png"));
    sprites['d'].push_back(IMG_LoadTexture(renderer, "personaje/down2.png"));
    sprites['l'].push_back(IMG_LoadTexture(renderer, "personaje/left1.png"));
    sprites['l'].push_back(IMG_LoadTexture(renderer, "personaje/left2.png"));
    sprites['r'].push_back(IMG_LoadTexture(renderer, "personaje/right1.png"));
    sprites['r'].push_back(IMG_LoadTexture(renderer, "personaje/right2.png"));

    SDL_QueryTexture(sprites['u'][0], NULL, NULL, &w, &h);
    rect_character.x = 100;
    rect_character.y = 100;
    rect_character.w = w;
    rect_character.h = h;

    texture_npc = IMG_LoadTexture(renderer,"npc.png");
    SDL_QueryTexture(texture_npc, NULL, NULL, &w, &h);
    rect_npc.x = 96;
    rect_npc.y = 0;
    rect_npc.w = w;
    rect_npc.h = h;

    texture_tile = IMG_LoadTexture(renderer,"tile/crypt.png");
    rect_tile.x = 32*4;
    rect_tile.y = 32*5;
    rect_tile.w = 32;
    rect_tile.h = 32;

    texture_flag = IMG_LoadTexture(renderer, "flag.png");
    rect_flag.x =32;
    rect_flag.y =32;
    rect_flag.w =32;
    rect_flag.h =32;

    texture_flagm = IMG_LoadTexture(renderer, "flagm.png");
    rect_flagm.x =32;
    rect_flagm.y =192;
    rect_flagm.w =32;
    rect_flagm.h =32;

    SDL_QueryTexture(texture_npc, NULL, NULL, &w, &h);
    rect_tileset.x = 0;
    rect_tileset.y = 0;
    rect_tileset.w = w;
    rect_tileset.h = h;

    bool start=false;
    bool mover=true;

    background = IMG_LoadTexture(renderer,"fondo1.jpg");
    SDL_QueryTexture(background, NULL, NULL, &w, &h);
    rect_background.x = 0; rect_background.y = 0; rect_background.w = w; rect_background.h = h;

    background2 = IMG_LoadTexture(renderer,"fondo2.png");
    SDL_QueryTexture(background2, NULL, NULL, &w, &h);
    rect_background2.x = 0; rect_background2.y = 0; rect_background2.w = w; rect_background2.h = h;

    background3 = IMG_LoadTexture(renderer,"fondo3.png");
    SDL_QueryTexture(background3, NULL, NULL, &w, &h);
    rect_background3.x = 0; rect_background3.y = 0; rect_background3.w = w; rect_background3.h = h;

    map<string,vector<int> >mapas_down;
    map<string,vector<int> >mapas_down1;
    map<string,vector<int> >mapas_over;
    map<string,vector<int> >mapas_over1;
    map<string,vector<int> >mapas_collision;
    map<string,vector<Warp*> >warps;

    mapas_down["mapa1"]=getMapa("tile/test.tmx",1);
    mapas_down1["mapa1"]=getMapa("tile/test.tmx",4);
    mapas_over["mapa1"]=getMapa("tile/test.tmx",2);
    mapas_over1["mapa1"]=getMapa("tile/test.tmx",5);
    mapas_collision["mapa1"]=getMapa("tile/test.tmx",3);
    warps["mapa1"] = getWarps("tile/test.tmx");

    mapas_down["mapa2"]=getMapa("tile/test2.tmx",1);
    mapas_over["mapa2"]=getMapa("tile/test2.tmx",2);
    mapas_collision["mapa2"]=getMapa("tile/test2.tmx",3);
    warps["mapa2"] = getWarps("tile/test2.tmx");

    mapas_down["mapa3"]=getMapa("tile/test3.tmx",1);
    mapas_over["mapa3"]=getMapa("tile/test3.tmx",2);
    mapas_collision["mapa3"]=getMapa("tile/test3.tmx",3);
    warps["mapa3"] = getWarps("tile/test3.tmx");

    mapas_down["mapa4"]=getMapa("tile/test4.tmx",1);
    mapas_over["mapa4"]=getMapa("tile/test4.tmx",2);
    mapas_collision["mapa4"]=getMapa("tile/test4.tmx",3);
    warps["mapa4"] = getWarps("tile/test4.tmx");

    mapas_down["mapa5"]=getMapa("tile/test5.tmx",1);
    mapas_over["mapa5"]=getMapa("tile/test5.tmx",2);
    mapas_collision["mapa5"]=getMapa("tile/test5.tmx",3);
    warps["mapa5"] = getWarps("tile/test5.tmx");

    mapas_down["mapa6"]=getMapa("tile/test6.tmx",1);
    mapas_over["mapa6"]=getMapa("tile/test6.tmx",2);
    mapas_collision["mapa6"]=getMapa("tile/test6.tmx",3);
    warps["mapa6"] = getWarps("tile/test6.tmx");

    string mapa_actual="mapa2";

    //Main Loop
    while(true)
    {
        while(SDL_PollEvent(&Event))
        {
            if(Event.type == SDL_QUIT)
            {
                return 0;
            }

        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

        if(start && (velocity!=0 || !collision(rect_character,rect_flagm) || !collision(rect_character,rect_flag) || !collision(rect_character,rect_npc)))
        {
            if(currentKeyStates[ SDL_SCANCODE_D ])
            {
                rect_character.x+=velocity;
                while(collisionLayer(mapas_collision[mapa_actual],rect_character))
                {
                    rect_character.x-=1;
                }
                orientation='r';
            }
            if(currentKeyStates[ SDL_SCANCODE_A ])
            {
                rect_character.x-=velocity;
                while(collisionLayer(mapas_collision[mapa_actual],rect_character))
                {
                    rect_character.x+=1;
                }
                orientation='l';
            }
            if(currentKeyStates[ SDL_SCANCODE_S ])
            {
                rect_character.y+=velocity;

                while(collisionLayer(mapas_collision[mapa_actual],rect_character))
                {
                    rect_character.y-=1;
                }
                orientation='d';
            }
            if(currentKeyStates[ SDL_SCANCODE_W ])
            {
                rect_character.y-=velocity;
                while(collisionLayer(mapas_collision[mapa_actual],rect_character))
                {
                    rect_character.y+=1;
                }
                orientation='u';
            }
            if(currentKeyStates[ SDL_SCANCODE_LSHIFT ])
            {
                velocity=6;
                animation_velocity=10;
            }else
            {
                velocity=3;
                animation_velocity=20;
            }
            if(!Mix_PlayingMusic())
                Mix_PlayMusic(music,1);
        }
        if(currentKeyStates[SDL_SCANCODE_SPACE ])
        {
            start=true;
        }
        if(frame%animation_velocity==0)
        {
            current_sprite++;
            if(current_sprite>1)
                current_sprite=0;
        }

        for(int i=0;i<warps[mapa_actual].size();i++)
        {
            if(collision(warps[mapa_actual][i]->rect,rect_character))
            {
                rect_character.x=warps[mapa_actual][i]->x;
                rect_character.y=warps[mapa_actual][i]->y;
                mapa_actual=warps[mapa_actual][i]->mapa;
            }
        }

        SDL_Delay(17);

        if(start==false)
        {
            SDL_RenderCopy(renderer, background, NULL, &rect_background);
        }
        else
        {
            SDL_RenderCopy(renderer, background1, NULL, &rect_background);
            dibujarLayer(renderer,mapas_down[mapa_actual]);
            dibujarLayer(renderer,mapas_down1[mapa_actual]);

            SDL_RenderCopy(renderer, sprites[orientation][current_sprite], NULL, &rect_character);

            dibujarLayer(renderer,mapas_over[mapa_actual]);
            dibujarLayer(renderer,mapas_over1[mapa_actual]);

            //dibujarLayer(renderer,collision_map);


            if(mapa_actual.compare("mapa6")==0)
            {
                SDL_RenderCopy(renderer, texture_npc, NULL, &rect_npc);

                if(mover)
                    rect_npc.y+=npc_velocity;
                if(mover==false)
                    rect_npc.y-=npc_velocity;
                if(rect_npc.y>=128)
                    mover=false;
                if(rect_npc.y<=0)
                    mover=true;
                SDL_RenderCopy(renderer, texture_flag, NULL, &rect_flag);
                if(collision(rect_character,rect_flag)==true)
                {
                    SDL_RenderCopy(renderer, background2, NULL, &rect_background2);
                    Mix_PlayChannel(-1,win,0);
                    velocity=0;
                }
                if(collision(rect_character,rect_npc)==true)
                {
                    for(int cont=0; cont<2; cont++)
                    {
                        velocity=0;
                        npc_velocity=0;
                        Mix_PlayChannel(-1,lose,0);
                        SDL_RenderCopy(renderer, background3, NULL, &rect_background3);
                    }

                }
            }

            if(mapa_actual.compare("mapa3")==0 || mapa_actual.compare("mapa4")==0 || mapa_actual.compare("mapa1")==0)
            {
                SDL_RenderCopy(renderer, texture_flagm, NULL, &rect_flagm);
                if(collision(rect_character,rect_flagm)==true)
                {
                    Mix_PlayChannel(-1,lose,0);
                    SDL_RenderCopy(renderer, background3, NULL, &rect_background3);
                    velocity=0;
                }
            }

        }

        SDL_RenderPresent(renderer);
        frame++;
    }

	return 0;
}
