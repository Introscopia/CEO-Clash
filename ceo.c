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
    int ultima_direcao;
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


    bool atacando;
    int ataque_timer;
    int ataque_cooldown; // tempo até poder atacar novamente



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


void fighter_movement(Fighter *F, vec2d desloc) {
    if (F->no_ar) {
        if (F->no_controle) {
            F->pos.x += desloc.x * F->walkspeed;
            F->direcao = desloc.x;
            if (F->direcao != 0) F->ultima_direcao = F->direcao;
        }
        F->pos.y += F->vel.y;
        F->vel.y += 1;

        if (F->pos.y > FLOOR_Y) {
            F->pos.y = FLOOR_Y;
            F->no_ar = false;
        }
    } else { // no chão
        F->pos.x += desloc.x * F->walkspeed;
        F->direcao = desloc.x;
        if (F->direcao != 0) F->ultima_direcao = F->direcao;
    }

    if (F->pos.x - 0.5 * F->hitbox.w < WALL_L) {
        F->pos.x = WALL_L + 0.5 * F->hitbox.w;
    }
    if (F->pos.x + 0.5 * F->hitbox.w > WALL_R) {
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
    int c2x, c2y;
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

    SDL_srand(0);



    //TEXTURAS

    SDL_Texture *Fundo = IMG_LoadTexture(R,"Assets/Cenas/InvDez/ceu1.png");
    float fundow, fundoh;
    SDL_GetTextureSize(Fundo, &fundow, &fundoh);

    SDL_Texture *Fundo2 = IMG_LoadTexture(R,"Assets/Cenas/InvDez/ceu2.png");
    float fundo2w, fundo2h;
    SDL_GetTextureSize(Fundo2, &fundo2w, &fundo2h);

    SDL_Texture *Fundo3 = IMG_LoadTexture(R,"Assets/Cenas/InvDez/morros3.png");
    float fundo3w, fundo3h;
    SDL_GetTextureSize(Fundo3, &fundo3w, &fundo3h);

    SDL_Texture *Fundo4 = IMG_LoadTexture(R,"Assets/Cenas/InvDez/pred4.png");
    float fundo4w, fundo4h;
    SDL_GetTextureSize(Fundo4, &fundo4w, &fundo4h);

    SDL_Texture *Fundo5 = IMG_LoadTexture(R,"Assets/Cenas/InvDez/pred5.png");
    float fundo5w, fundo5h;
    SDL_GetTextureSize(Fundo5, &fundo5w, &fundo5h);
    



     //TEXTURAS PERSONAGENS

    SDL_Texture *Piwis = IMG_LoadTexture(R, "Assets/Pinto_outline.png");
    float fw, fh;
    SDL_GetTextureSize(Piwis, &fw, &fh);

    Transform T = (Transform){256,256,cx,cy,1,1};
    int scaleI = 0;

    SDL_Texture *Piwispez = IMG_LoadTexture(R, "Assets/piwispez.png");
    SDL_GetTextureSize(Piwispez, &fw, &fh);



    SDL_Texture *Melon = IMG_LoadTexture(R, "Assets/Sus.png");
    float cyberw, cyberh;
    SDL_GetTextureSize(Melon, &cyberw, &cyberh);

    Transform I = (Transform){256,256,c2x,c2y,1,1};
    int scaleT = 0;
    
    SDL_Texture *pat1 = IMG_LoadTexture(R,"Assets/piwisAT1.png");
    float pat1w, pat1h;
    SDL_GetTextureSize(pat1, &pat1w, &pat1h);




    SDL_Texture *Fundo6 = IMG_LoadTexture(R,"Assets/Cenas/InvDez/pred6.png");
    float fundo6w, fundo6h;
    SDL_GetTextureSize(Fundo6, &fundo6w, &fundo6h);


//  HITBOXES PERSONAGENS


    Fighter P1 = {0};
    P1.pos = v2d( 100, FLOOR_Y );
    P1.hitbox.w = fw*0.2;
    P1.hitbox.h = fh*0.2;
    P1.hitbox.x = P1.pos.x - 0.5 * P1.hitbox.w;
    P1.hitbox.y = P1.pos.y - P1.hitbox.h;
    P1.walkspeed = 5;
    P1.jumppower = -30;
    P1.direcao = 0;
    P1.ultima_direcao = 1;
    P1.atacando = false;
    P1.ataque_timer = 0;
    P1.ataque_cooldown = 0;

    Fighter P2 = {0};
    P2.pos = v2d( width-100, FLOOR_Y );
    P2.hitbox.w = cyberw*0.5;
    P2.hitbox.h = cyberh*0.5;
    P2.hitbox.x = P2.pos.x - 0.5 * P2.hitbox.w;
    P2.hitbox.y = P2.pos.y - P2.hitbox.h;
    P2.walkspeed = 5;
    P2.jumppower = -40;
    P2.direcao = 1;
    P2.ultima_direcao = -1;
    P2.atacando = false;
    P2.ataque_timer = 0;
    P2.ataque_cooldown = 0;


    bool p1u = 0, p1d = 0, p1l = 0, p1r = 0; // up down left right
    bool p2u = 0, p2d = 0, p2l = 0, p2r = 0;

    
    int frame_period = SDL_roundf( 1000 / 60.0 );


// ATAQUE

    void processar_ataque(Fighter *A, Fighter *B){
        
    if (A->atacando) {
        A->ataque_timer--;
        if (A->ataque_timer <= 0) {
            A->atacando = false;
            A->ataque_cooldown = 30; // 0.5s de recarga a 60fps
        }

        SDL_FRect ataque_hitbox = A->hitbox;
        ataque_hitbox.w *= 0.80;
        ataque_hitbox.h *= 0.60;

        if (A->ultima_direcao < 0) ataque_hitbox.x -= ataque_hitbox.w * 0.5;     //direção própria do ataque (diferente da do player) útil para ataques traseiros.
        else ataque_hitbox.x += ataque_hitbox.w * 1;
        
            //ataque ficar visivel
            SDL_SetRenderDrawColor(R, 0, 250, 0, 100);
            SDL_RenderFillRect(R, &ataque_hitbox);

        //Verifica colisão com o outro personagem
        if (SDL_FRect_overlap(&ataque_hitbox, &(B->hitbox))) {
            B->pos.x += B->vel.x; B->vel.x = A->ultima_direcao * 10; // knockback
            B->no_controle = false;
            //efeito do ataque
            SDL_SetRenderDrawColor(R, 255, 0, 0, 100);
            SDL_RenderFillRect(R, &(B->hitbox));
        }
    }

    if (A->ataque_cooldown > 0) {
        A->ataque_cooldown--;
    }
}



//EXECUTANDO



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
                    else if( event.key.key == SDLK_SPACE ) {
                         if (!P1.atacando && P1.ataque_cooldown == 0) {
                              P1.atacando = true;
                              P1.ataque_timer = 10; // duração do ataque em frames
                            }
                               }
                    else if( event.key.key == SDLK_RETURN ) {
                         if (!P2.atacando && P2.ataque_cooldown == 0) {
                              P2.atacando = true;
                              P2.ataque_timer = 10;
                            }
                               }
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




        SDL_FRect fundodest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo, NULL, &fundodest);

        SDL_FRect fundo2dest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo2, NULL, &fundo2dest);

        SDL_FRect fundo3dest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo3, NULL, &fundo3dest);

        SDL_FRect fundo4dest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo4, NULL, &fundo4dest);

        SDL_FRect fundo5dest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo5, NULL, &fundo5dest);




        vec2d desloc1 = desloc_from_controls( &P1, p1u, p1d, p1l, p1r );
        vec2d desloc2 = desloc_from_controls( &P2, p2u, p2d, p2l, p2r );
        
        fighter_movement( &P1, desloc1 );
        fighter_movement( &P2, desloc2 );

        processar_ataque(&P1, &P2);
        processar_ataque(&P2, &P1);
        
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

        int flip1;
        if( P1.direcao < 0 ) flip1 = 1;
        else if( P1.direcao > 0 ) flip1 = 0;
        SDL_RenderTextureRotated(R, Piwis, NULL, &(P1.hitbox), 0, NULL, flip1);

        int flip2;
        if( P2.direcao > 0 ) flip2 = 1;
        else if( P2.direcao < 0 ) flip2 = 0;
        SDL_RenderTextureRotated(R, Melon, NULL, &(P2.hitbox), 0, NULL, flip2);

    
        SDL_FRect fundo6dest = {0,0,width,height};
        SDL_RenderTexture(R, Fundo6, NULL, &fundo6dest);

        
        SDL_RenderPresent(R);
        SDL_framerateDelay( frame_period );

    }//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> / L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    SDL_DestroyRenderer(R);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

