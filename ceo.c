#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>
#include "basics.h"
#include "vec2d.h"
#include "transform.h"



float WALL_L = 0;
float WALL_R = 0;
float FLOOR_Y = 0;


typedef struct {

    int direcao;
    int pulo;

    vec2d pos;
    vec2d vel;
    SDL_FRect hitbox;
    float altura_agachada;

    bool no_ar;
    bool no_controle;// true = estou no controle do meu corpo, 
                     // false = meu corpo foi arremessado por forcas externas

    float walkspeed;
    float jumppower;


} Fighter;



vec2d desloc_from_controls( Fighter *F, bool cu, bool cd, bool cl, bool cr ){
    vec2d desloc = (vec2d){0};
    if( cl ){
        desloc.x = -1;
    }
    if( cr ){
       desloc.x = 1;
    }
    if( cu ){
        if( !(F->no_ar)  ){// previne double jump
            F->vel = v2d( 0, F->jumppower );
            F->no_ar = true;
            F->no_controle = true;
        }
    }
    if( cd ){
        //F->hitbox.h = F->altura_agachada;
        
    }
    return desloc;
}


void fighter_movement( Fighter *F, vec2d desloc ){
    if( F->no_ar ){
        if( F->no_controle ){
            F->pos.x += desloc.x * F->walkspeed;
            F->direcao = desloc.x;
        }
        F->pos.y += F->vel.y;
        F->vel.y += 1;

        if( F->pos.y > FLOOR_Y ){
            F->pos.y = FLOOR_Y;
            F->no_ar = false;
        }
    }
    else{// no chao
        F->pos.x += desloc.x * F->walkspeed;
        F->direcao = desloc.x;
    }

    if( F->pos.x - 0.5 * F->hitbox.w < WALL_L ){// colisao com a parede Left
        F->pos.x = WALL_L + 0.5 * F->hitbox.w;
    }
    if( F->pos.x + 0.5 * F->hitbox.w > WALL_R ){// colisao com a parede Left
        F->pos.x = WALL_R - 0.5 * F->hitbox.w;
    }

    F->hitbox.x = F->pos.x - 0.5 * F->hitbox.w;
    F->hitbox.y = F->pos.y - F->hitbox.h;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~O~~~~~~~~~~| M A I N |~~~~~~~~~~~O~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char *argv[]){

    SDL_Window *window;
    SDL_Renderer *R;
    int width = 600;
    int height = 400;
    int cx, cy;
    int loop = 1;


    if( !SDL_Init(SDL_INIT_VIDEO) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }
    if( !SDL_CreateWindowAndRenderer( "CEO_Clash", width, height, 
                                      SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, 
                                      &window, &R ) ){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }
    SDL_GetWindowSize( window, &width, &height );
    cx = width / 2;
    cy = height / 2;

    WALL_R = width;
    FLOOR_Y = height - 100;

    // prime the random number generator
    SDL_srand(0);

    Transform T = (Transform){256,256,cx,cy,1,1};
    int scaleI = 0;


    
    

    Fighter P1 = {0};
    P1.pos = v2d( 100, FLOOR_Y );
    P1.hitbox.w = 200;
    P1.hitbox.h = 350;
    P1.hitbox.x = P1.pos.x - 0.5 * P1.hitbox.w;
    P1.hitbox.y = P1.pos.y - P1.hitbox.h;
    P1.walkspeed = 5;
    P1.jumppower = -20;

    Fighter P2 = {0};
    P2.pos = v2d( width-100, FLOOR_Y );
    P2.hitbox.w = 250;
    P2.hitbox.h = 300;
    P2.hitbox.x = P2.pos.x - 0.5 * P2.hitbox.w;
    P2.hitbox.y = P2.pos.y - P2.hitbox.h;
    P2.walkspeed = 5;
    P2.jumppower = -20;


    bool p1u = 0, p1d = 0, p1l = 0, p1r = 0; // up down left right
    bool p2u = 0, p2d = 0, p2l = 0, p2r = 0;

    
    //our desired frame period
    int frame_period = SDL_roundf( 1000 / 60.0 );

    SDL_Log("<<Entering Loop>>");
    while ( loop ) { //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        
        SDL_Event event;
        while( SDL_PollEvent(&event) ){
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    loop = 0;
                    break;
                case SDL_EVENT_KEY_DOWN:
                         if( event.key.key == 'w' ) p1u = 1;
                    else if( event.key.key == 's' ) p1d = 1;
                    else if( event.key.key == 'a' ) p1l = 1;
                    else if( event.key.key == 'd' ) p1r = 1;
                    else if( event.key.key == SDLK_UP    ) p2u = 1;
                    else if( event.key.key == SDLK_DOWN  ) p2d = 1;
                    else if( event.key.key == SDLK_LEFT  ) p2l = 1;
                    else if( event.key.key == SDLK_RIGHT ) p2r = 1;
                    break;
                case SDL_EVENT_KEY_UP:
                         if( event.key.key == 'w' ) p1u = 0;
                    else if( event.key.key == 's' ) p1d = 0;
                    else if( event.key.key == 'a' ) p1l = 0;
                    else if( event.key.key == 'd' ) p1r = 0;
                    else if( event.key.key == SDLK_UP    ) p2u = 0;
                    else if( event.key.key == SDLK_DOWN  ) p2d = 0;
                    else if( event.key.key == SDLK_LEFT  ) p2l = 0;
                    else if( event.key.key == SDLK_RIGHT ) p2r = 0;
                    break;
            }
        }

        SDL_SetRenderDrawColor( R, 200,200,200,255 );
        SDL_RenderClear(R);

        SDL_SetRenderDrawColor( R, 0,0,0,255 );
        SDL_RenderLine( R, 0, FLOOR_Y, width, FLOOR_Y );


        vec2d desloc1 = desloc_from_controls( &P1, p1u, p1d, p1l, p1r );
        vec2d desloc2 = desloc_from_controls( &P2, p2u, p2d, p2l, p2r );
        
        fighter_movement( &P1, desloc1 );
        fighter_movement( &P2, desloc2 );
        
        if( SDL_FRect_overlap( &(P1.hitbox), &(P2.hitbox) ) ){
            Fighter *LF;
            Fighter *RF;
            if( P1.pos.x < P2.pos.x ){
                LF = &P1;
                RF = &P2;
            } else {
                LF = &P2;
                RF = &P1;
            }
            int overlap = (LF->hitbox.x + LF->hitbox.w)-(RF->hitbox.x);
            LF->pos.x -= 0.5 * overlap;
            RF->pos.x += 0.5 * overlap;
            LF->hitbox.x = LF->pos.x - 0.5 * LF->hitbox.w;
            RF->hitbox.x = RF->pos.x - 0.5 * RF->hitbox.w;
        }

        SDL_SetRenderDrawColor( R, 0,0,255,255 );
        SDL_RenderRect( R, &(P1.hitbox) );
        SDL_SetRenderDrawColor( R, 255,0,0,255 );
        SDL_RenderRect( R, &(P2.hitbox) );


        // throw things up onscreen
        SDL_RenderPresent(R);
        // try to maintain constant framerate
        SDL_framerateDelay( frame_period );

    }//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> / L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    SDL_DestroyRenderer(R);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

