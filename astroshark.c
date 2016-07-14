/*Sean Kee*/
/*Astroshark v0.0.4*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#define PI 3.14159265

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280
char windowTitle[18] = {"Astroshark  v0.0.4"}; 																	/*Title of the window*/

enum direction {NORTH = 5, EAST, SOUTH, WEST};

typedef struct {
	SDL_Rect laser_dstrect;																					/*Creates ship's destination rectangle, a.k.a. the ship "object"*/
	SDL_Rect laser_srcrect;
} laserInstance;


void calculateMovement(int *new_posX, int *new_posY, int angle, int speed){											/*Function to calculate movement using my algorithm*/
	int quadrant = 0;																										/*This Algorithm calculates the direction that the player should move if the W(forwards) key is pressed by using the angle and speed*/
	float deltaX;																											/*This algorithm involves trigonometry to calculate (using unit circle)*/
	float deltaY;

	if (angle >= 360)																							/*Checks to see if rotation is greater or less than 0/360*/
		angle -= 360;
	if (angle < 0)
		angle +=360;


	if (angle < 90 && angle > 0) {																							/*Sets correct quadrant*/
		quadrant = 1;
	}
	if (angle < 180 && angle > 90) {
		quadrant = 4;
		angle -= 90;
	}
	if (angle < 270 && angle > 180) {
		quadrant = 3;
		angle -= 180;
	}
	if (angle <= 359 && angle > 270) {
		quadrant = 2;
		angle -= 270;
	}
	if (angle == 0) 																										/*Sets direction if player is orientated towards an axis*/
		quadrant = NORTH;
	if (angle == 90)
		quadrant = EAST;
	if (angle == 180)
		quadrant = SOUTH;
	if (angle == 270)
		quadrant = WEST;

	deltaX = sin(angle*PI/180) * speed;																						/*Calculates the change in x and y using trigonometric functions*/
	deltaY = cos(angle*PI/180) * speed;
	printf("deltaX = %f\ndeltaY = %f\nangle = %d\n", deltaX, deltaY, angle);

	switch(quadrant) {																										/*Updates the changes based on correct orientation and deltaX and deltaY*/
		case 1:
			*new_posX += deltaX;
			*new_posY -= deltaY;
			break;
		case 2:
			*new_posX -= deltaY;
			*new_posY -= deltaX;
			break;
		case 3:
			*new_posX -= deltaX;
			*new_posY += deltaY;
			break;
		case 4:
			*new_posX += deltaY;
			*new_posY += deltaX;
			break;
		case NORTH:
			*new_posY -= speed;
			break;
		case EAST:
			*new_posX += speed;
			break;
		case SOUTH:
			*new_posY += speed;
			break;
		case WEST:
			*new_posX -= speed;
			break;
	}
}

void createLaser(struct SDL_Window **gameWindow, struct SDL_Renderer **renderer, int *w, int *h, struct SDL_Texture **spriteTexture) {  	/*Function to create laser, takes in points because those values have to be used in the game(Double pointers are for pointers to struct pointers)*/
	SDL_Surface *tempSurface = IMG_Load("resources/lasers_spritesheet_160x320.png");															/*Creates a temporary surface and loads the spritesheet*/
	*spriteTexture = SDL_CreateTextureFromSurface(*renderer, tempSurface);																	/*Sets the texture to the spritesheet on the tempSurface*/
	SDL_FreeSurface(tempSurface);																											/*Frees the space allocated to the tempSurface*/
	SDL_QueryTexture(*spriteTexture, NULL, NULL, w, h);																						/*Sets the width and height of the dstrect to the sprite's texture, essentially binding the texture to the dstrect*/
}


void createShip(struct SDL_Window **gameWindow, struct SDL_Renderer **renderer, int *w, int *h, struct SDL_Texture **spriteTexture) {  	/*Function to create ship, takes in points because those values have to be used in the game(Double pointers are for pointers to struct pointers)*/
	SDL_Surface *tempSurface = IMG_Load("resources/playerShip_spritesheet_320x480.png");												/*Creates a temporary surface and loads the spritesheet*/
	*spriteTexture = SDL_CreateTextureFromSurface(*renderer, tempSurface);																/*Sets the texture to the spritesheet on the tempSurface*/
	SDL_FreeSurface(tempSurface);																										/*Frees the space allocated to the tempSurface*/
	SDL_QueryTexture(*spriteTexture, NULL, NULL, w, h);																					/*Sets the width and height of the dstrect to the sprite's texture, essentially binding the texture to the dstrect*/
}

int initializeAstroshark(int *debug) { 																				/*Function for initalizing Astroshark*/
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) !=0) { 																/*Initalizes SDL while testing if it was successful*/
		printf("\n\n***ERROR: Unable to initalize SDL: %s\nEND ERROR***\n", SDL_GetError());
		*debug = 1;
		return 1;
	}
	
	SDL_Window *gameWindow; 																						/*Declares the gameWindow(pointer) with datatype SDL_Window*/
	gameWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0); 		/*Createshttp://stackoverflow.com/questions/21007329/what-is-a-sdl-renderer window at the location of gameWindow pointer*/
	
	if (gameWindow == NULL) { 																						/*Tests to see if window creation was successful or not */
		printf("\n\n***ERROR: Could not create window: %s\nEND ERROR***\n", SDL_GetError());
		*debug = 1;
		return 1;
	}

	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;										/*Creates Renderer flags*/
	SDL_Renderer *renderer = SDL_CreateRenderer(gameWindow, -1, render_flags);												/*Creates Renderer*/
	if(renderer == NULL) {
		printf("\n\n***ERROR: Failed to create renderer: %s\nEND ERROR***\n", SDL_GetError());
		*debug = 1;
		return 1;
	}

	SDL_Rect playerShip_dstrect;																					/*Creates ship's destination rectangle, a.k.a. the ship "object"*/
	SDL_Rect playerShip_srcrect = {0, 0, 320, 480};																	/*TEMPCreates Source rectangle, to highlight the area in which the correct sprite on the spritesheet is located*/
	SDL_Texture *playerShipTexture;																					/*Creates the Ship's texture*/
	createShip(&gameWindow, &renderer, &playerShip_dstrect.w, &playerShip_dstrect.h, &playerShipTexture);			/*Sends the addresses the necessary structs and data to createShip()*/

	playerShip_dstrect.w -= 1600;																					/*Resizes the width of the rectangle to the size of a single sprite*/
	playerShip_dstrect.w /= 10;																						/*Scales down the ship*/
	playerShip_dstrect.h /= 10;

	playerShip_dstrect.x = 0;																						/*TEMPSets location to 0, 0, the top left corner*/
	playerShip_dstrect.y = 0;


	SDL_Texture *laserTexture;
	laserInstance laserI[10];
	int laserCount = 0;


	int close_requested = 0;																						/*close requested variable for controlling closed window*/

	int playerShip_speed = 5;																						/*Default ship speed*/


	int playerShip_moveForward = 0;																					/*Various booleans for different movements*/
	int playerShip_moveLeftStrafe = 0;
	int playerShip_moveBackward = 0;
	int playerShip_moveRightStrafe = 0;

	int playerShip_rotateLeft = 0;
	int playerShip_rotateRight = 0;

	int playerShip_actionShoot = 0;

	int playerShip_rotate = 0;																						/*Variable for ship rotation angle*/

	int playerShip_deltaX = 0;
	int playerShip_deltaY = 0;

	enum playerShip_animation {AT_REST, ENGINE_START, ENGINE_1, ENGINE_2, ENGINE_3, ENGINE_4};
	int playerShip_animationFrame = AT_REST;

	while (!close_requested) {																						/*Runs loop while the close button is not pressed*/
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {																					/*Test for various events, keyboard and mouse*/
				case SDL_QUIT:
					close_requested = 1;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.scancode) {
						case SDL_SCANCODE_W:
							playerShip_moveForward = 1;
							break;
						case SDL_SCANCODE_A:
							playerShip_moveLeftStrafe = 1;
							break;
						case SDL_SCANCODE_S:
							playerShip_moveBackward = 1;
							break;
						case SDL_SCANCODE_D:
							playerShip_moveRightStrafe = 1;
							break;
						case SDL_SCANCODE_SPACE:
							playerShip_actionShoot = 1;
							break;
						case SDL_SCANCODE_LEFT:
							playerShip_rotateLeft = 1;
							break;
						case SDL_SCANCODE_RIGHT:
							playerShip_rotateRight = 1;
							break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.scancode) {
						case SDL_SCANCODE_W:
							playerShip_moveForward = 0;
							break;
						case SDL_SCANCODE_A:
							playerShip_moveLeftStrafe = 0;
							break;
						case SDL_SCANCODE_S:
							playerShip_moveBackward = 0;
							break;
						case SDL_SCANCODE_D:
							playerShip_moveRightStrafe = 0;
							break;
						case SDL_SCANCODE_SPACE:
							playerShip_actionShoot = 0;
							break;
						case SDL_SCANCODE_LEFT:
							playerShip_rotateLeft = 0;
							break;
						case SDL_SCANCODE_RIGHT:
							playerShip_rotateRight = 0;
							break;
					}
					break;

			}
		}
		
		playerShip_deltaX = playerShip_dstrect.x;																	/*Copies ship's position into deltaX and deltaY*/
		playerShip_deltaY = playerShip_dstrect.y;

		if (playerShip_rotateLeft == 1)																				/*Checks for ship rotation based on arrow keys*/
			playerShip_rotate -= 9;
		if (playerShip_rotateRight == 1)
			playerShip_rotate += 9;
		if (playerShip_rotate >= 360)
			playerShip_rotate -= 360;
		if (playerShip_rotate < 0)
			playerShip_rotate +=360;

		

		if (playerShip_moveForward == 1) {																			/*Tests for different key presses*/
			calculateMovement(&playerShip_deltaX, &playerShip_deltaY, playerShip_rotate, playerShip_speed);
			playerShip_dstrect.x = playerShip_deltaX;
			playerShip_dstrect.y = playerShip_deltaY;
			playerShip_animationFrame++;
			
		}
		if (playerShip_moveBackward == 1) {
			calculateMovement(&playerShip_deltaX, &playerShip_deltaY, playerShip_rotate, -1 * playerShip_speed + 3);
			playerShip_dstrect.x = playerShip_deltaX;
			playerShip_dstrect.y = playerShip_deltaY;
		}
		if (playerShip_moveLeftStrafe == 1) {
			calculateMovement(&playerShip_deltaX, &playerShip_deltaY, playerShip_rotate - 90, playerShip_speed - 3);
			playerShip_dstrect.x = playerShip_deltaX;
			playerShip_dstrect.y = playerShip_deltaY;
		}
		if (playerShip_moveRightStrafe == 1) {
			calculateMovement(&playerShip_deltaX, &playerShip_deltaY, playerShip_rotate + 90, playerShip_speed - 3);
			playerShip_dstrect.x = playerShip_deltaX;
			playerShip_dstrect.y = playerShip_deltaY;
		}
		if (playerShip_actionShoot == 1) {
			if (laserCount < 10) {
				createLaser(&gameWindow, &renderer, &laserI[laserCount].laser_dstrect.w, &laserI[laserCount].laser_dstrect.h, &laserTexture);
				laserI[laserCount].laser_dstrect.w -= 320;
				laserI[laserCount].laser_dstrect.w /= 10;
				laserI[laserCount].laser_dstrect.h /= 10;	

				if (playerShip_rotate < 90 && playerShip_rotate > 0) {																							
					laserI[laserCount].laser_dstrect.x = playerShip_dstrect.x + 21;
					laserI[laserCount].laser_dstrect.y = playerShip_dstrect.y - 8;
				}
				if (playerShip_rotate < 180 && playerShip_rotate > 90) {
					laserI[laserCount].laser_dstrect.x = playerShip_dstrect.x + 21;
					laserI[laserCount].laser_dstrect.y = playerShip_dstrect.y + 8;
				}
				if (playerShip_rotate < 270 && playerShip_rotate > 180) {
					laserI[laserCount].laser_dstrect.x = playerShip_dstrect.x - 21;
					laserI[laserCount].laser_dstrect.y = playerShip_dstrect.y + 8;
				}
				if (playerShip_rotate <= 359 && playerShip_rotate > 270) {	
					laserI[laserCount].laser_dstrect.x = playerShip_dstrect.x - 21;
					laserI[laserCount].laser_dstrect.y = playerShip_dstrect.y - 8;
				}

				laserCount++;
			}
		}


		if (playerShip_dstrect.x <= 0)																					/*Collision Detection*/
			playerShip_dstrect.x = 0;
		if (playerShip_dstrect.x >= WINDOW_WIDTH - playerShip_dstrect.w)
			playerShip_dstrect.x = WINDOW_WIDTH - playerShip_dstrect.w ;
		if (playerShip_dstrect.y <= -6)
			playerShip_dstrect.y = -6;
		if (playerShip_dstrect.y >= WINDOW_HEIGHT - playerShip_dstrect.h + 6)
			playerShip_dstrect.y = WINDOW_HEIGHT - playerShip_dstrect.h + 6;


		if (playerShip_moveForward == 0) {
			if (playerShip_animationFrame <= AT_REST)
				playerShip_animationFrame = AT_REST;
			else
				playerShip_animationFrame--;
		}
//		if (playerShip_moveBackward == 0)
//			playerShip_deltaY = 0;
//		if (playerShip_moveLeftStrafe == 0)
//			playerShip_deltaY = 0;
//		if (playerShip_moveRightStrafe == 0)
//			playerShip_deltaY = 0;

//		if (playerShip_actionShoot == 0) {

//		}

		if (playerShip_animationFrame > ENGINE_4)
			playerShip_animationFrame = ENGINE_1;

		playerShip_srcrect.x = 320 * (playerShip_animationFrame);

		SDL_RenderClear(renderer);
		if (laserCount > -1) {
			int i;
			for (i = 0; i < laserCount; i++) {
				SDL_RenderCopyEx(renderer, laserTexture, &laserI[i].laser_srcrect, &laserI[i].laser_dstrect, playerShip_rotate, NULL, SDL_FLIP_NONE);																											/*Clears the screen to set color*/
			}
		}
		SDL_RenderCopyEx(renderer, playerShipTexture, &playerShip_srcrect, &playerShip_dstrect, playerShip_rotate, NULL, SDL_FLIP_NONE);		/*Copies the texture onto the rect, and rotates it correctly*/
		SDL_RenderPresent(renderer);																											/*Presents the renderer and draws everything in renderer*/


		SDL_Delay(1000/60);
		
	}









//	SDL_Delay(3000);

	SDL_DestroyTexture(playerShipTexture);																			/*Destroys Texture*/
	SDL_DestroyTexture(laserTexture);
	SDL_DestroyRenderer(renderer);																					/*Destroys Renderer*/
	SDL_DestroyWindow(gameWindow); 																					/*Destroys the window that gameWindow is pointing to*/
	SDL_Quit(); 																									/*Cleans up and quits out of SDL*/

	*debug = 0;
}

int main(void) {
	int debug; 																										/*Variable for testing if initialization was successful*/
	initializeAstroshark(&debug); 																					/*Initalizes Astroshark while referencing the debug variable*/
	if (debug == 1)
		return 1;
	else
		return 0;
}
