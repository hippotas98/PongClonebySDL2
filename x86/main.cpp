#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "cmath"
#include "cstdlib"
#include "ctime"
#include "sstream"
#include "iostream"
#include "windows.h"

using namespace std;

//Khai bao bien

SDL_Event occur;
SDL_Window *window =NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *image_ball = NULL;
SDL_Surface *image_paddle = NULL;
SDL_Surface *PlayerScoreSur;
SDL_Surface *AIScoreSur;
SDL_Surface *Background = NULL;
SDL_Rect PlayerPaddle;
SDL_Rect AIPaddle;
SDL_Rect Scribe;
SDL_Rect Ball;
SDL_Rect PlayerScoreRect;
SDL_Rect AIScoreRect;
SDL_Rect Backrect = { 0,0,800,600 };
TTF_Font *times = NULL;
SDL_Color white = { 255,255,255 };
SDL_Color red = { 255,0,0 };
Mix_Chunk *paddlesound, *scoresound, *hitsound;
Mix_Music *backgroundsong, *finalsong;
SDL_Texture *Backtext = NULL;
double second;
int xvel = -2;
int yvel = -2;
int PlayerScore = 0, AIScore = 0;
const int winscore = 5;
const int ball_x = 390;
const int ball_y = 290;
bool winner = 0;
bool running = true;
bool stop = 0;

bool BallinRect(int, int, SDL_Rect);
bool CheckCollision(SDL_Rect, SDL_Rect);
void RenderText(SDL_Texture, SDL_Surface, SDL_Rect);
void SetUp();
void LoadGame();
void Quit();
int GameMenu();
int PauseGame();
int SelectInput();
int InitGame();
int RandomNumber();
void Collisions();
void Game(int);
void DrawScore();
void DrawScreen();
void Instructor(int, int);
void PaddleMove();
void Player2PaddleMove();
void PaddleMoveByMouse();
void SetPaddleY(int);
void PrintResultPvP();
void PrintResultAI();
void LogicAI(int);
void LogicPvP();
void ResetBall(int ,int );
void AIPaddleMove();
void ContinueGame();

int main(int agrc, char *agrs[]) {
	int menu = InitGame();
	SDL_Delay(500);
	Game(menu);
	return 0;
}

void RenderText(SDL_Texture *text, SDL_Surface *sur, SDL_Rect *Rect) {
	text = SDL_CreateTextureFromSurface(renderer, sur);
	SDL_RenderCopy(renderer, text, NULL, Rect);
	SDL_DestroyTexture(text);
	SDL_FreeSurface(sur);
}

int RandomNumber() {
	srand(unsigned int(time(NULL)));
	int k = rand() % 2;
	if (k == 1) return -1;
	else return 1;
}

void SetUp() {
	PlayerPaddle.x = 20;
	PlayerPaddle.y = 250;
	PlayerPaddle.w = 20;
	PlayerPaddle.h = 100;

	AIPaddle.x = 760;
	AIPaddle.y = 250;
	AIPaddle.w = 20;
	AIPaddle.h = 100;

	Ball.x = ball_x;
	Ball.y = ball_y;
	Ball.w = 20;
	Ball.h = 20;

	PlayerScoreRect.x = 100;
	PlayerScoreRect.y = 0;
	PlayerScoreRect.w = 40;
	PlayerScoreRect.h = 80;

	AIScoreRect.x = 660;
	AIScoreRect.y = 0;
	AIScoreRect.w = 40;
	AIScoreRect.h = 80;

}

void LoadGame() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Init(SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		cout << ("could not create window: %s/n", SDL_GetError());
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Renderer init error", SDL_GetError(), window);
	}
	//screen = SDL_GetWindowSurface(window);
	SetUp();

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	backgroundsong = Mix_LoadMUS("backgroundsong.mp3");
	if (backgroundsong == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Music init error", SDL_GetError(), window);
	}
	finalsong = Mix_LoadMUS("victory.mp3");

	paddlesound = Mix_LoadWAV("paddle_hit.wav");
	if (paddlesound == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Music init error", SDL_GetError(), window);
	}

	scoresound = Mix_LoadWAV("score_update.wav");
	if (scoresound == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Music init error", SDL_GetError(), window);
	}

	hitsound = Mix_LoadWAV("wall_hit.wav");
	if (hitsound == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Music init error", SDL_GetError(), window);
	}

	times = TTF_OpenFont("times.ttf", 80);
	if (times == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Font init error", SDL_GetError(), window);
	}
	Mix_PlayMusic(finalsong, -1);
}

void Quit() {
	Mix_FreeChunk(hitsound);
	Mix_FreeChunk(paddlesound);
	Mix_FreeChunk(scoresound);
	Mix_FreeMusic(finalsong);
	Mix_FreeMusic(backgroundsong);
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void DrawScore() {
	stringstream pscore;
	stringstream ascore;
	pscore << PlayerScore;
	ascore << AIScore;
	SDL_Surface *playersur = TTF_RenderText_Solid(times, pscore.str().c_str(), white);
	SDL_Surface *aisur = TTF_RenderText_Solid(times, ascore.str().c_str(), white);
	SDL_Texture *ptext = NULL;
	SDL_Texture *atext = NULL;
	RenderText(ptext, playersur, &PlayerScoreRect);
	RenderText(atext, aisur, &AIScoreRect);
	SDL_RenderPresent(renderer);
}

void BallMove() {
	Ball.x += (xvel);
	Ball.y += (yvel);
	SDL_Delay(5);
	//Sleep(1);
}

void ResetBall(int a, int b) {
	Ball.x = ball_x;
	Ball.y = ball_y;
	xvel = a;
	yvel = b;
	BallMove();
}
//kiem ra bong va cham 
bool BallinRect(int x, int y, SDL_Rect rect) {
	if (x > rect.x &&
		x < rect.x + rect.w &&
		y > rect.y &&
		y < rect.y + rect.h
		)
	{
		return true;
	}

	return false;
}

bool CheckCollision(SDL_Rect r1, SDL_Rect r2)
{
	if (BallinRect(r1.x, r1.y, r2) == true ||
		BallinRect(r1.x + r1.w, r1.y, r2) == true ||
		BallinRect(r1.x, r1.y + r1.h, r2) == true ||
		BallinRect(r1.x + r1.w, r1.y + r1.w, r2) == true)
		return true;
	return false;

}
//di chuyen bang chuot
void SetPaddleY(int a) {
	int newy;
	if (a < 1) {
		newy = 1;
	}
	else if (a + PlayerPaddle.h > 599) {
		newy = 599 - PlayerPaddle.h;
	}
	else newy = a;
	PlayerPaddle.y = newy;
}

void PaddleMoveByMouse() {
	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);
	SetPaddleY(mousey - PlayerPaddle.h / 2);
}

void Collisions() {
	int i = 1;
	//kiem tra tren duoi
	if (Ball.y < 1)
	{
		Ball.y = 1;
		yvel = -yvel;
		Mix_PlayChannel(-1, hitsound, 0);
		BallMove();
	}
	if (Ball.y + Ball.h > 599)
	{
		Ball.y = 600 - Ball.h;
		yvel = -yvel;
		Mix_PlayChannel(-1, hitsound, 0);
		BallMove();
	}
	//kiem tra Paddle
	if (CheckCollision(Ball, PlayerPaddle))
	{
		xvel = -xvel;
		Ball.x = PlayerPaddle.x + PlayerPaddle.w + 1;
		Mix_PlayChannel(-1, paddlesound, 0);
		BallMove();

	}
	if (CheckCollision(Ball, AIPaddle))
	{
		xvel = -xvel;
		Ball.x = AIPaddle.x - Ball.w;
		Mix_PlayChannel(-1, paddlesound, 0);
		BallMove();
	}
	int a = RandomNumber();
	if (Ball.x + Ball.w > 781) {
		//Player 1 scores
		PlayerScore += 1;
		Mix_PlayChannel(-1, scoresound, 0);
		DrawScore();
		ResetBall(2, 2 * a); //sua -1 -1 = 1 -1 
	}
	else if (Ball.x < 19) {
		//Player 2 scores
		AIScore += 1;
		Mix_PlayChannel(-1, scoresound, 0);
		DrawScore();
		ResetBall(-2, 2 * a); // sua -1 1 = -1 -1
	}
}

void PaddleMove() {
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_UP]) PlayerPaddle.y -= 8;
	if (state[SDL_SCANCODE_DOWN]) PlayerPaddle.y += 8;
	if (PlayerPaddle.y < 1) PlayerPaddle.y = 1;
	if (PlayerPaddle.y + PlayerPaddle.h > 599)
	{
		PlayerPaddle.y = 599 - PlayerPaddle.h;
	}
}

void AIPaddleMove() {
	if (AIScore + PlayerScore < 2) {
		if (AIPaddle.y + AIPaddle.h * 0.5 >= Ball.y + Ball.h * 0.5) {
			AIPaddle.y -= 1;
		}
		else AIPaddle.y += 1;
	}
	else {
		if (AIPaddle.y + AIPaddle.h * 0.5 >= Ball.y + Ball.h * 0.5) {
			AIPaddle.y -= xvel;
		}
		else AIPaddle.y += yvel;
	}
	if (AIPaddle.y < 1) AIPaddle.y = 1;
	if (AIPaddle.y + AIPaddle.h > 599)
	{
		AIPaddle.y = 599 - AIPaddle.h;
	}
}

void Player2PaddleMove()
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_W]) AIPaddle.y -= 8;
	if (state[SDL_SCANCODE_S]) AIPaddle.y += 8;
	if (AIPaddle.y < 1) AIPaddle.y = 1;
	if (AIPaddle.y + AIPaddle.h > 599)
	{
		AIPaddle.y = 599 - AIPaddle.h;
	}
}

void ContinueGame() {
	const Uint8 *con = SDL_GetKeyboardState(NULL);
	if (con[SDL_SCANCODE_SPACE]) {
		running = true;
		winner = false;
		PlayerScore = AIScore = 0;
		ResetBall(2, 2);
		SDL_DestroyRenderer(renderer);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SetUp();
	}
	if (con[SDL_SCANCODE_ESCAPE]) {
		running = false;
		Quit();
	}
}

void PrintResultAI() {
	IMG_Init(IMG_INIT_PNG);
	SDL_Surface *Message = NULL;
	SDL_Texture *Mess_text = NULL;
	SDL_Rect Rect = { 180,250,500,80 };
	SDL_DestroyRenderer(renderer);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Renderer init error", SDL_GetError(), window);
	}
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//SDL_RenderClear(renderer);
	SDL_Texture *picture = NULL;
	SDL_Surface *picture_sur = NULL;
	SDL_Rect result = { 0,0,800,600 };
	picture_sur = IMG_Load("menubackground.png");
	picture = SDL_CreateTextureFromSurface(renderer, picture_sur);
	SDL_RenderCopy(renderer, picture, NULL, &result);

	if (PlayerScore == winscore) {
		xvel = 0, yvel = 0;
		Message = TTF_RenderText_Solid(times, "VICTORY !!", white);
		RenderText(Mess_text, Message, &Rect);
	}
	else if (AIScore == winscore) {
		xvel = 0, yvel = 0;
		Message = TTF_RenderText_Solid(times, "DEFEATED !!", white);
		RenderText(Mess_text, Message, &Rect);
	}
	SDL_Surface *Continue = NULL;
	SDL_Texture *Con_text = NULL;
	SDL_Rect con = { 200,500,400,20 };
	Continue = TTF_RenderText_Solid(times, "Press Space to continue or ESC to exit.", white);
	RenderText(Con_text, Continue, &con);
	SDL_RenderPresent(renderer);
	ContinueGame();
}

void PrintResultPvP() {
	SDL_Surface *Message1 = NULL;
	SDL_Texture *Mess1_text = NULL;
	SDL_Rect Rect1 = { 100,250,250,80 };
	SDL_Surface *Message2 = NULL;
	SDL_Texture *Mess2_text = NULL;
	SDL_Rect Rect2 = { 500,250,250,80 };
	SDL_DestroyRenderer(renderer);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Renderer init error", SDL_GetError(), window);
	}
	SDL_Texture *picture = NULL;
	SDL_Surface *picture_sur = NULL;
	SDL_Rect result = { 0,0,800,600 };
	picture_sur = IMG_Load("menubackground.png");
	picture = SDL_CreateTextureFromSurface(renderer, picture_sur);
	SDL_RenderCopy(renderer, picture, NULL, &result);
	int j = 0;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 1; i <= 25; ++i) {
		Scribe.x = 398;
		Scribe.y = j;
		Scribe.h = 20;
		Scribe.w = 4;
		SDL_RenderFillRect(renderer, &Scribe);
		j = j + 30;
	}
	if (PlayerScore == winscore) {
		xvel = 0, yvel = 0;
		Message1 = TTF_RenderText_Solid(times, "Player 1 WIN !!", white);
		RenderText(Mess1_text, Message1, &Rect1);
		Message2 = TTF_RenderText_Solid(times, "Player 2 LOSE !!", white);
		RenderText(Mess2_text, Message2, &Rect2);
	}
	else if (AIScore == winscore) {
		xvel = 0, yvel = 0;
		Message1 = TTF_RenderText_Solid(times, "Player 1 LOSE !!", white);
		RenderText(Mess1_text, Message1, &Rect1);
		Message2 = TTF_RenderText_Solid(times, "Player 2 WIN !!", white);
		RenderText(Mess2_text, Message2, &Rect2);
	}
	SDL_Surface *Continue = NULL;
	SDL_Texture *Con_text = NULL;
	SDL_Rect con = { 200,500,400,20 };
	Continue = TTF_RenderText_Solid(times, "Press Space to continue or ESC to exit.", white);
	RenderText(Con_text, Continue, &con);
	SDL_RenderPresent(renderer);
	ContinueGame();
}

int GameMenu() {
	Uint32 time;
	int mousex, mousey;
	const int NumofMenu = 3;
	const char *Menu[NumofMenu] = { "Player VS Computer", "Player VS Player","Exit" };
	SDL_Surface *MenuSur[NumofMenu];
	SDL_Texture *MenuText[NumofMenu];
	SDL_Rect MenuRect[NumofMenu];
	bool Selected[NumofMenu];

	for (int i = 0; i < NumofMenu; ++i) {
		Selected[i] = 0;
	}

	MenuRect[0].x = 200;
	MenuRect[1].x = 250;
	MenuRect[2].x = 350;
	MenuRect[0].y = 300;
	MenuRect[1].y = 400;
	MenuRect[2].y = 500;

	MenuRect[0].h = MenuRect[1].h = MenuRect[2].h = 60;
	MenuRect[0].w = 400;
	MenuRect[1].w = 300;
	MenuRect[2].w = 100;

	MenuSur[0] = TTF_RenderText_Solid(times, Menu[0], white);
	MenuSur[1] = TTF_RenderText_Solid(times, Menu[1], white);
	MenuSur[2] = TTF_RenderText_Solid(times, Menu[2], white);

	IMG_Init(IMG_INIT_PNG);
	Background = IMG_Load("menubackground.png");
	Backtext = SDL_CreateTextureFromSurface(renderer, Background);
	SDL_RenderCopy(renderer, Backtext, NULL, &Backrect);

	MenuText[0] = SDL_CreateTextureFromSurface(renderer, MenuSur[0]);
	MenuText[1] = SDL_CreateTextureFromSurface(renderer, MenuSur[1]);
	MenuText[2] = SDL_CreateTextureFromSurface(renderer, MenuSur[2]);
	for (int i = 0; i < 3; ++i)
		SDL_RenderCopy(renderer, MenuText[i], NULL, &MenuRect[i]);
	SDL_RenderPresent(renderer);
	SDL_Event menuevent;
	while (true)
	{
		time = SDL_GetTicks();
		while (SDL_PollEvent(&menuevent)) {
			switch (menuevent.type)
			{
			case SDL_QUIT:
				SDL_FreeSurface(MenuSur[0]);
				SDL_DestroyTexture(MenuText[0]);
				SDL_FreeSurface(MenuSur[1]);
				SDL_DestroyTexture(MenuText[1]);
				SDL_FreeSurface(MenuSur[2]);
				SDL_DestroyTexture(MenuText[2]);
				SDL_FreeSurface(Background);
				SDL_DestroyTexture(Backtext);
				return 2;
			case SDL_MOUSEMOTION:
				mousex = menuevent.motion.x;
				mousey = menuevent.motion.y;
				for (int i = 0; i < NumofMenu; ++i)
				{
					if (MenuRect[i].x < mousex &&
						MenuRect[i].x + MenuRect[i].w > mousex &&
						MenuRect[i].y < mousey &&
						MenuRect[i].y + MenuRect[i].h > mousey)
					{
						if (Selected[i] == 0)
						{
							Selected[i] = 1;
							SDL_DestroyTexture(MenuText[i]);
							SDL_Surface *Temp = TTF_RenderText_Solid(times, Menu[i], red);
							MenuText[i] = SDL_CreateTextureFromSurface(renderer, Temp);
							SDL_FreeSurface(Temp);
						}
					}
					else
					{
						if (Selected[i] == 1)
						{
							Selected[i] = 0;
							SDL_DestroyTexture(MenuText[i]);
							SDL_Surface *Temp = TTF_RenderText_Solid(times, Menu[i], white);
							MenuText[i] = SDL_CreateTextureFromSurface(renderer, Temp);
							SDL_FreeSurface(Temp);
						}
					}
					SDL_RenderCopy(renderer, MenuText[i], NULL, &MenuRect[i]);
					SDL_RenderPresent(renderer);
					SDL_DestroyTexture(MenuText[i]);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			{
				mousex = menuevent.motion.x;
				mousey = menuevent.motion.y;
				for (int i = 0; i < NumofMenu; ++i)
				{
					if (MenuRect[i].x < mousex &&
						MenuRect[i].x + MenuRect[i].w > mousex &&
						MenuRect[i].y < mousey &&
						MenuRect[i].y + MenuRect[i].h > mousey)
					{
						SDL_FreeSurface(MenuSur[0]);
						SDL_DestroyTexture(MenuText[0]);
						SDL_FreeSurface(MenuSur[1]);
						SDL_DestroyTexture(MenuText[1]);
						SDL_FreeSurface(MenuSur[2]);
						SDL_DestroyTexture(MenuText[2]);
						SDL_FreeSurface(Background);
						SDL_DestroyTexture(Backtext);
						return i;
					}

				}
				break;
			}
			}
		}
	}

	if (30 > (SDL_GetTicks() - time)) {
		SDL_Delay(30 > (SDL_GetTicks() - time));
	}
	//SDL_Delay(3000);
	return 0;
}

void LogicAI(int method) {
	if (method == 0)
		PaddleMove();
	else
		PaddleMoveByMouse();
	AIPaddleMove();
	BallMove();
	Collisions();
	if (AIScore + PlayerScore > 6) {
		xvel = xvel * 2;
		yvel = yvel * 2;
	}
	if (PlayerScore == winscore || AIScore == winscore) winner = true;
}

void LogicPvP()
{
	PaddleMove();
	Player2PaddleMove();
	BallMove();
	Collisions();
	if (AIScore + PlayerScore > 8) {
		xvel = xvel * 2;
		yvel = yvel * 2;
	}
	if (PlayerScore == winscore || AIScore == winscore) winner = true;
}

void DrawScreen() {
	int j = 0;
	//SDL_FillRect(screen, NULL, 0);
	SDL_Rect Top = { 0,0,800,10 };
	SDL_Rect Bottom = { 0,590,800,10 };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 1; i <= 25; ++i) {
		Scribe.x = 398;
		Scribe.y = j;
		Scribe.h = 20;
		Scribe.w = 4;
		SDL_RenderFillRect(renderer, &Scribe);
		j = j + 30;
	}
	SDL_RenderFillRect(renderer, &Top);
	SDL_RenderFillRect(renderer, &Bottom);
	IMG_Init(IMG_INIT_PNG);
	image_ball = IMG_Load("ball.png");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image_ball);
	SDL_RenderCopy(renderer, texture, 0, &Ball);
	image_paddle = IMG_Load("paddle.png");
	SDL_Texture *texture_paddle = SDL_CreateTextureFromSurface(renderer, image_paddle);
	SDL_RenderCopy(renderer, texture_paddle, 0, &PlayerPaddle);
	SDL_RenderCopy(renderer, texture_paddle, 0, &AIPaddle);
	DrawScore();
	SDL_RenderPresent(renderer);
	SDL_FreeSurface(image_ball);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(texture_paddle);
}

int SelectInput() {
	int input;
	const SDL_MessageBoxButtonData button[] = {
		{ 0,0,"Keyboard" },
		{ 0,1,"Mouse" }
	};
	const SDL_MessageBoxColorScheme colorScheme[5] = {
		{ 255,255,255 }, //background color
		{ 0,0,0 }, //Text color
		{ 255,255,0 }, //button color
		{ 255,255,255 }, //button background
		{ 0,0,0 } //button text
	};
	const SDL_MessageBoxData messageboxdata{
		SDL_MESSAGEBOX_INFORMATION,
		NULL,
		"Select Input Method",
		"Keyboard or Mouse",
		2,
		button,
		&colorScheme[5],
	};
	if (SDL_ShowMessageBox(&messageboxdata, &input) < 0) {
		SDL_Log("error displaying message box");
		return -1;
	}
	else return input;
}

int PauseGame() {
	int pause;
	const SDL_MessageBoxButtonData button[] = {
		{ 0,0,"Continue" },
		{ 0,1,"Exit" }
	};
	const SDL_MessageBoxColorScheme colorScheme[5] = {
		{ 255,255,255 }, //background color
		{ 0,0,0 }, //Text color
		{ 255,255,0 }, //button color
		{ 255,255,255 }, //button background
		{ 0,0,0 } //button text
	};
	const SDL_MessageBoxData messageboxdata{
		SDL_MESSAGEBOX_INFORMATION,
		NULL,
		"Stop the Game",
		"Click continue to continue the game, Exit to quit",
		2,
		button,
		&colorScheme[5],
	};
	if (SDL_ShowMessageBox(&messageboxdata, &pause) < 0) {
		SDL_Log("error displaying message box");
		return -1;
	}
	else return pause;
}

void Instructor(int menu, int inputmethod) {
	if (menu == 0)
		if (inputmethod != 1)
			SDL_ShowSimpleMessageBox(0, "How to play :D", "Press Up or Down to move the paddle\nPress SPACE to pause", window);
		else
			SDL_ShowSimpleMessageBox(0, "How to play :D", "Using mouse to move the paddle\nPress SPACE to pause", window);
	else
		SDL_ShowSimpleMessageBox(0, "How to play :D", "Player 1 press Up or Down to move the paddle \nPlayer 2 Press W or S to move the paddle\nPress SPACE to pause", window);
}

int InitGame() {
	IMG_Init(IMG_INIT_PNG);
	LoadGame();
	int menu = 0;
	menu = GameMenu();
	return menu;
}

void Game(int menu) {
	SDL_DestroyRenderer(renderer);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_ShowSimpleMessageBox(0, "Render init error", SDL_GetError(), window);
	}
	second = SDL_GetTicks() / 1000;
	if (menu == 0) {
		int inputmethod = SelectInput();
		Instructor(menu, inputmethod);
		while (running == true) {
			SDL_PollEvent(&occur);
			if (occur.type == SDL_QUIT) {
				running = false;
				Quit();
			}
			if (occur.type == SDL_KEYDOWN)
			{
				switch (occur.key.keysym.sym)
				{
				case SDLK_SPACE:
					int pause = PauseGame();
					stop = true;
					if (pause == 0)
					{
						stop = false;
						break;
					}
					if (pause == 1) {
						running = false;
						Quit();
						break;
					}
				}
			}
			if (stop == false) {
				LogicAI(inputmethod);
				if (winner == false) {
					DrawScreen();
				}
				else {
					PrintResultAI();
				}
			}
		}
	}
	else if (menu == 1) {
		Instructor(menu, 0);
		while (running == true) {
			SDL_PollEvent(&occur);
			if (occur.type == SDL_QUIT)
			{
				Quit();
			}
			else if (occur.type == SDL_KEYDOWN)
			{
				switch (occur.key.keysym.sym)
				{
				case SDLK_SPACE:
					int pause = PauseGame();
					stop = true;
					if (pause == 0)
					{
						stop = false;
						break;
					}
					if (pause == 1) {
						running = false;
						Quit();
						break;
					}
				}
			}
			if (stop == false) {
				LogicPvP();
				if (winner == false)
					DrawScreen();
				else PrintResultPvP();
			}
		}
	}
	else if (menu == 2)
	{
		Quit();
	}
}