/* *************************** RayCasting SDL**********************   */
/* Author: velorek | inspired by: 3DSage  	       		      */
/* Date : 2021				               		      */
/* [+] STRUCTURE: 		|-------------------v		      */
/* [MAIN] -> [NEWGAME] -> [MAIN_LOOP] ->[UPDATEGAME && HANDLE_EVENTS] */  
/*				^	            |	              */
/*                     [DRAWTOSCREEN] <-      [MOVEOBJECTS]           */
/* *****************************************************************  */
/* 3DSage Video part 1 [https://www.youtube.com/watch?v=gYRrGTC7GtA]  */
/* *****************************************************************  */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <math.h>
//GAME CONSTANTS
#define PI 3.1415926535
#define P2 PI/2 //90º
#define P3 3*PI/2 //270º
#define SCREEN_W 1024
#define SCREEN_H 512
#define MAX_KEY 1000
//MESSAGES
#define STS_MSG0 "SDL Graphics loaded successfully.\n"
#define ERR_MSG0 "SDL Graphics could not be loaded. \n"
#define ERR_MSG1 "Failed to load asset. \n"
#define TRUE 1
#define FALSE 0
#define DR 0.0174533 //one dgree in radians
typedef short BOOL; 

/* GLOBALS */
//SDL_Surface *background,*playerSprite;
SDL_Event ev;
SDL_Renderer *ren1;
SDL_Window *win1;
long KeyState[MAX_KEY];
BOOL Running=TRUE;
BOOL keypressed=FALSE;
time_t t;
float px, py, pdx, pdy, pa; //player values

int mapX=8, mapY = 8, mapS = 64;
int map[]=
{
 1, 1, 1, 1, 1, 1, 1, 1,
 1, 0, 0, 0, 0, 0, 0, 1, 
 1, 0, 0, 2, 0, 0, 0, 1, 
 1, 0, 0, 2, 0, 0, 0, 1, 
 1, 0, 0, 2, 0, 0, 0, 1, 
 1, 1, 0, 0, 0, 0, 1, 1, 
 1, 0, 0, 0, 0, 0, 0, 1, 
 1, 1, 1, 1, 1, 1, 1, 1 
};

/* ---------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ---------------------------------------------- */
/*SDL Related */
BOOL  InitVideo();
BOOL  InitAudio();
void  ToggleFullscreen(SDL_Window* Window);
void  CleanMemory();
/* EVENTS */
BOOL Key(long K);
void HandleKey(long Sym, BOOL Down);
void HandleEvents();

/* Game engine */

//void  LoadAssets();
void  NewGame();
void  UpdateGame();
void  Main_Loop();
/* Drawing */
void Draw(int X, int Y, SDL_Surface *Img);
void DrawScreen();
void drawMap2D();
void drawRays2D();
float dist(float ax, float ay, float bx, float by); 
/* ---------------------------------------------- */
/* MAIN CODE */ 
/* ---------------------------------------------- */
int main(){
  InitVideo();
  NewGame();
  Main_Loop(); //UPDATE EVENTS AND DRAW TO SCREEN
  CleanMemory();
  return 0;
}

/* FUNCTIONS */

//SDL Initialization
BOOL InitVideo(){
   SDL_Init(SDL_INIT_VIDEO);
#ifdef __linux__ 
   win1 = SDL_CreateWindow(" > Game 2 <", 50,0,SCREEN_W,SCREEN_H,SDL_WINDOW_SHOWN);
#elif _WIN32
   win1 = SDL_CreateWindow(" > Game 2 <", 50,0,SCREEN_W,SCREEN_H,SDL_WINDOW_FULLSCREEN);
#else
#endif
   ren1 = SDL_CreateRenderer(win1, -1, 0);
   SDL_SetWindowBordered(win1,SDL_TRUE);
   return (ren1 != NULL) && (win1 != NULL);
}


void ToggleFullscreen(SDL_Window* Window) {
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    BOOL IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
    SDL_ShowCursor(IsFullscreen);
}

void CleanMemory(){
  SDL_DestroyRenderer(ren1);
  SDL_DestroyWindow(win1);
  SDL_Quit();
} 

//Events Functions

BOOL Key(long K){
  if ((K>= 0) && (K <= MAX_KEY)) 
    return KeyState[K]; 
  else 
    return FALSE;
}

void HandleKey(long Sym, BOOL Down){
  if (Sym == SDLK_UP) Sym = SDL_SCANCODE_UP;
  if (Sym == SDLK_DOWN) Sym = SDL_SCANCODE_DOWN;
  if (Sym == SDLK_LEFT) Sym = SDL_SCANCODE_LEFT;
  if (Sym == SDLK_RIGHT) Sym = SDL_SCANCODE_RIGHT;
  if (Sym == SDLK_SPACE) Sym = SDL_SCANCODE_SPACE;
   if ((Sym >= 0) && (Sym <= MAX_KEY)) {
    KeyState[Sym] = Down;
    if (Sym == SDLK_ESCAPE) Running = FALSE;    
  }
} 

void HandleEvents(){
  SDL_Event e;
  if (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      Running = FALSE;
    }
    
    if (e.type == SDL_KEYDOWN){
      keypressed = TRUE;
      HandleKey(e.key.keysym.sym, TRUE);
    }
    if (e.type == SDL_KEYUP){
      keypressed = FALSE;
      HandleKey(e.key.keysym.sym, FALSE);
    }  
  }
}

// Game Engine

void NewGame(){
  SDL_SetRenderDrawColor(ren1, 80,80,80,0);
  SDL_RenderPresent(ren1);
  px = 300; py = 300;
  pdx = cos(pa) *5; pdy = sin(pa) * 5;
}

// Drawing Functions
void drawMap2D(){
  SDL_Rect R;
  int x, y, xo, yo;
  for (y=0;y<mapY;y++){
   for (x=0; x<mapX; x++){
     if (map[y*mapX+x] > 0) { SDL_SetRenderDrawColor(ren1,255,255,255,0);} else {SDL_SetRenderDrawColor(ren1,0,0,0,0);}
    xo = x * mapS; yo = y * mapS;
    R.x = xo+1;
    R.y = yo+1;
    R.w = mapS-1;
    R.h = mapS-1;
    SDL_RenderDrawRect(ren1, &R);
    SDL_RenderFillRect(ren1, &R);  
   }
  }
}

void drawPlayer() {
  SDL_Rect R;
  
  R.x = (int) px;
  R.y = (int) py;
  R.w = 8;
  R.h = 8;
  SDL_SetRenderDrawColor(ren1, 255,255,0,0);
  SDL_RenderDrawRect(ren1, &R);
  SDL_RenderFillRect(ren1, &R);
  SDL_RenderDrawLine(ren1,px+4,py+4,px+4+pdx*5,py+4+pdy*5);  
}

float dist(float ax, float ay, float bx, float by){
  return ( sqrt ((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );
}
void drawRays2D(){
  int i,r,mx,my,mp,dof,savemp=0,savemp2=0; float rx=0,ry=0,ra,xo,yo,disT;
  ra=pa-DR*30; if (ra<0){ra+=2*PI;} if (ra>2*PI) {ra-=2*PI;} 
  //Casting one ray
  for (r=0;r<60;r++)
  {
  
    //Check horizontal lines
     dof =0;
     float disH=1000000, hx = px, hy=py;
     float aTan = -1 / tan(ra);
     //1- check if ray is looking up or down
     //looking up
     if (ra>PI) {ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64; xo=-yo*aTan;}
     //looking down
     if (ra<PI) {ry=(((int)py>>6)<<6)+64; rx=(py-ry)*aTan+px; yo=64; xo=-yo*aTan;}
     if (ra==0 || ra==PI) {rx=px; ry=py;dof=8;} //left-right
     while (dof<8){
       //calculate ray in map's position
       mx=(int) (rx)>>6; my=(int) (ry)>>6; mp=my*mapX+mx;
       if (mp> 0 && mp<mapX*mapY && map[mp] > 0) {hx=rx; hy=ry; disH=dist(px,py,hx,hy); dof = 8;savemp2=mp;} //hit a wall
       else{rx+=xo; ry+=yo; dof+=1;} //next line	  	
     }
/*    
  SDL_SetRenderDrawColor(ren1, 0,255,0,0);
  SDL_RenderDrawLine(ren1,px+4,py+4,rx,ry);   
  SDL_RenderDrawLine(ren1,px+4-1,py+4-1,rx-1,ry-1);   
  SDL_RenderDrawLine(ren1,px+4+1,py+4+1,rx+1,ry+1);   
*/
    //Check vertical lines
     dof =0;
     float disV=1000000, vx = px, vy=py;
     float nTan = -tan(ra);
     //1- check if ray is looking up or down
     //looking left
     if (ra>P2 && ra<P3) {rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan;}
     //looking right
     if (ra<P2 || ra>P3) {rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan;}
     if (ra==0 || ra==PI) {rx=px; ry=py;dof=8;} //up down
     while (dof<8){
       //calculate ray in map's position
       mx=(int) (rx)>>6; my=(int) (ry)>>6; mp=my*mapX+mx;
       if (mp>0 && mp<mapX*mapY && map[mp] > 0) {vx=rx; vy=ry; disV=dist(px,py,vx,vy); dof = 8;savemp=mp;} //hit a wall
       else{rx+=xo; ry+=yo; dof+=1;} //next line	  	
     }
  
  
  if (disV<disH) {rx=vx; ry=vy;disT=disV;SDL_SetRenderDrawColor(ren1, 255,0,0,0); if (map[savemp]==2) {SDL_SetRenderDrawColor(ren1, 0,155,0,0);}}
  if (disH<disV) {rx=hx; ry=hy;disT=disH;SDL_SetRenderDrawColor(ren1, 155,0,0,0); if (map[savemp2]==2) {SDL_SetRenderDrawColor(ren1, 0,155,0,0);}}
  //if (map[savemp]==2) {savemp=0;savemp2=0;SDL_SetRenderDrawColor(ren1, 0,155,0,0);}
  //if (map[savemp2]==2) {savemp=0;savemp2=0;SDL_SetRenderDrawColor(ren1, 0,155,0,0);}
  //SDL_SetRenderDrawColor(ren1, 255,0,0,0);
  SDL_RenderDrawLine(ren1,(int) px+4,(int) py+4,(int) rx,(int) ry); 
  //Draw 3D
  float ca=pa-ra; if(ra<0){ ra+=2*PI;} if (ra>2*PI) {ra-=2*PI;} disT=disT*cos(ca); //fix fish-eye

  float lineH=(mapS*320) / disT; if (lineH>320) {lineH=320;} 
  float lineO = 160 - lineH/2;
  //SDL_SetRenderDrawColor(ren1, 0,0,139,0);
  for(i=0; i<8; i++) //line width
   SDL_RenderDrawLine(ren1,r*8+i+530,lineO,r*8+i+530,lineH+lineO);  
   
  ra+=DR; if (ra<0){ra+=2*PI;} if (ra>2*PI) {ra-=2*PI;} 
  }
 // SDL_SetRenderDrawColor(ren1, 255,255,0,0);
 // SDL_RenderDrawLine(ren1,px+4,py+4,px+4+pdx*5,py+4+pdy*5);  
}

void DrawScreen() {
  SDL_SetRenderDrawColor(ren1, 80,80,80,0);
  SDL_RenderClear(ren1);
  drawMap2D();
  drawRays2D(); 
  drawPlayer();
  SDL_RenderPresent(ren1);
}



void UpdateGame(){
  if (Key(SDLK_f)) ToggleFullscreen(win1);
  if (Key(SDL_SCANCODE_LEFT) || Key(SDLK_a)) {pa-= 0.1; if(pa<0) {pa+=2*PI;} pdx = cos(pa) * 5; pdy=sin(pa)*5;}
  if (Key(SDL_SCANCODE_RIGHT) || Key(SDLK_d)) {pa+= 0.1; if(pa>2*PI) {pa-=2*PI;} pdx = cos(pa) * 5; pdy=sin(pa)*5;}
  if (Key(SDL_SCANCODE_UP) || Key(SDLK_w)) {px+=pdx; py+=pdy;}
  if (Key(SDL_SCANCODE_DOWN) || Key(SDLK_s)) {px-=pdx; py-=pdy;}
}

void Main_Loop(){
/* Update + HandleEvents - Draw */
  unsigned int LastTime, CurrentTime;

  LastTime = SDL_GetTicks();
  while (Running==TRUE) {
    	CurrentTime = SDL_GetTicks();
 	if (CurrentTime - LastTime > 1000) LastTime = CurrentTime - 60;
 	while (CurrentTime - LastTime > 1000/30) {
 	  UpdateGame();
          LastTime = LastTime + 30;
 	}  
       HandleEvents();         
       DrawScreen();
    }
} 
/* ---------------------------------------------- */
/* END 						  */
/* ---------------------------------------------- */
