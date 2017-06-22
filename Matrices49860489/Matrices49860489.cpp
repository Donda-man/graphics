// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define ENEMY_NUM 10
#define ENEMY_COLOR_NUM 3
#define BULLET_NUM 10

bool Timer3On = false;
int timer = 0;


// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface

						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite

LPDIRECT3DTEXTURE9 sprite_enemy;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_blue;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy_yellow;    // the pointer to the sprite

LPDIRECT3DTEXTURE9 sprite_bullet;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet_blue;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet_yellow;    // the pointer to the sprite

LPDIRECT3DTEXTURE9 sprite_background;    // the pointer to the sprite

LPDIRECT3DTEXTURE9 sprite_TScore;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_OScore;    // the pointer to the sprite

									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);

int t_xPos = 1;
int o_xPos = 1;
int Score = 0;
int T_Score = 0;
int O_Score = 0;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;

};

//주인공 클래스 
class Hero :public entity
{
public:
	void move(int i);
	void init(float x, float y);

	int state_bullet = 0;
};

void Hero::init(float x, float y)
{

	x_pos = x;
	y_pos = y;
	state_bullet = 0;
}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 6;
		break;

	case MOVE_DOWN:
		y_pos += 6;
		break;

	case MOVE_LEFT:
		x_pos -= 6;
		break;

	case MOVE_RIGHT:
		x_pos += 6;
		break;
	}
}

Hero hero;

// 적 클래스 
class Enemy :public entity {

public:
	void init(float x, float y);
	void move();

	float w_Life = 100;
	float f_Life = 100;
	float b_Life = 100;
};

void Enemy::init(float x, float y)
{
	x_pos = x;
	y_pos = y;
}


void Enemy::move()
{
	//y_pos += 2;
	x_pos -= 4;
}

class Enemy_blue :public entity {

public:
	void init(float x, float y);
	void move();
};

void Enemy_blue::init(float x, float y)
{
	x_pos = x;
	y_pos = y;
}


void Enemy_blue::move()
{
	//y_pos += 2;
	x_pos -= 4;
}

class Enemy_yellow :public entity {

public:
	void init(float x, float y);
	void move();
};

void Enemy_yellow::init(float x, float y)
{
	x_pos = x;
	y_pos = y;
}


void Enemy_yellow::move()
{
	//y_pos += 2;
	x_pos -= 4;
}

// 총알 클래스 
class Bullet :public entity
{
public:
	bool bShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();
	bool check_collision(float x, float y);

};

void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

}

bool Bullet::show()
{
	return bShow;
}

void Bullet::active()
{
	bShow = true;
}

void Bullet::move()
{
	switch (hero.state_bullet)
	{
	case 0:
		x_pos += 8;
		break;
	case 1:
		x_pos += 16;
		break;
	case 2:
		x_pos += 4;
		break;
	}
}

void Bullet::hide()
{
	bShow = false;
}


//객체 생성 
Enemy enemy[ENEMY_NUM];
Enemy_blue enemy_blue[ENEMY_COLOR_NUM];
Enemy_yellow enemy_yellow[ENEMY_COLOR_NUM];
Bullet bullet[BULLET_NUM];

bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
	{
		Score = Score + 1;
		return true;
	}
	else
		return false;
}

bool Bullet::check_collision(float x, float y)
{
	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_EX_TOPMOST | WS_POPUP, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);

	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:
	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		do_game_logic();

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);

		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Panel3.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite
	
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"hero.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_blue.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_blue);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy_yellow.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy_yellow);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"bullet.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Bullet_blue.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet_blue);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Bullet_yellow.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet_yellow);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"background.jpg",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_background);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"OScore.png",    // the file name
		720,    // default width
		77,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_OScore);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"TScore.png",    // the file name
		720,    // default width
		77,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_TScore);    // load to sprite

	return;
}

void init_game(void)
{
	//객체 초기화 
	hero.init(150, 300);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		//enemy[i].init((float)(rand() % 300), rand() % 200 - 300);
		enemy[i].init((float)(rand() % 640+500), (float)(rand() % 300 + 50));
	}

	for (int i = 0; i<ENEMY_COLOR_NUM; i++)
	{
		enemy_blue[i].init((float)(rand() % 640 + 550), (float)(rand() % 300 + 50));
	}
}

void do_game_logic(void)
{

	//주인공 처리 
	if (KEY_DOWN(0x57))
		hero.move(MOVE_UP);

	if (KEY_DOWN(0x53))
		hero.move(MOVE_DOWN);

	if (KEY_DOWN(0x41))
		hero.move(MOVE_LEFT);

	if (KEY_DOWN(0x44))
		hero.move(MOVE_RIGHT);

	if (KEY_DOWN(0x46))
		hero.state_bullet = 0;

	if (KEY_DOWN(0x51))
		hero.state_bullet = 1;

	if (KEY_DOWN(0x45))
		hero.state_bullet = 2;
	
	//적들 처리 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		if (enemy[i].x_pos < 0)
		enemy[i].init((float)(rand() % 640+550), (float)(rand() % 300 + 50));
		else
		enemy[i].move();
	}

	for (int i = 0; i<ENEMY_COLOR_NUM; i++)
	{
		if (enemy_blue[i].x_pos < 0)
			enemy_blue[i].init((float)(rand() % 640 + 550), (float)(rand() % 300 + 50));
		else
			enemy_blue[i].move();
	}

	for (int i = 0; i<ENEMY_COLOR_NUM; i++)
	{
		if (enemy_yellow[i].x_pos < 0)
			enemy_yellow[i].init((float)(rand() % 640 + 550), (float)(rand() % 300 + 50));
		else
			enemy_yellow[i].move();
	}

	if (Timer3On)
		timer++;

	if (timer >= 3)
	{
		Timer3On = false;
		timer = 0;
	}

	//총알 처리 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].show() == false && Timer3On == false)
		{
			if (KEY_DOWN(0x01))
			{
				bullet[i].active();
				bullet[i].init(hero.x_pos + 60, hero.y_pos);
				Timer3On = true;

				return;
			}
		}
		if (bullet[i].show() == true)
		{
			if (bullet[i].x_pos > 640)
				bullet[i].hide();
			else
				bullet[i].move();

			//충돌 처리 
			for (int i = 0; i < ENEMY_NUM; i++)
			{
				for (int j = 0; j < BULLET_NUM; j++)
				{
					if (bullet[j].check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
					{
						enemy[i].init((float)(rand() % 640+600), (float)(rand() % 300 + 50));
						bullet[j].hide();
					}
				}
			}

			for (int i = 0; i < ENEMY_COLOR_NUM; i++)
			{
				for (int j = 0; j < BULLET_NUM; j++)
				{
					if (bullet[j].check_collision(enemy_blue[i].x_pos, enemy_blue[i].y_pos)== true)
					{
						enemy_blue[i].init((float)(rand() % 640 + 600), (float)(rand() % 300 + 50));
						//bullet[j].hide();
					}
				}
			}

			for (int i = 0; i < ENEMY_COLOR_NUM; i++)
			{
				for (int j = 0; j < BULLET_NUM; j++)
				{
					if (bullet[j].check_collision(enemy_yellow[i].x_pos, enemy_yellow[i].y_pos) == true)
					{
						enemy_yellow[i].init((float)(rand() % 640 + 600), (float)(rand() % 300 + 50));
						//bullet[j].hide();
					}
				}
			}
		}
	}	
}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 /*
											 static int frame = 21;    // start the program on the final frame
											 if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 // calculate the x-position
											 int xpos = frame * 182 + 1;

											 RECT part;
											 SetRect(&part, xpos, 0, xpos + 181, 128);
											 D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));
											 */

	D3DXMATRIX matWorldTemp;

	ZeroMemory(&matWorldTemp, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(&matWorldTemp);

	RECT part0;
	SetRect(&part0, 0, 0, 640, 480);
	D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position0(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth

	d3dspt->SetTransform(&matWorldTemp);
	d3dspt->Draw(sprite_background, &part0, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));

	////주인공 
	RECT part;
	SetRect(&part, 0, 0, 64, 64);
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
	
	////총알 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		switch (hero.state_bullet)
		{
		case 0:
			if (bullet[i].bShow == true)
			{
				RECT part1; // 
				SetRect(&part1, 0, 0, 64, 64);
				D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 position1(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			break;
		case 1:
			if (bullet[i].bShow == true)
			{
				RECT part2; // 
				SetRect(&part2, 0, 0, 64, 64);
				D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 position2(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet_blue, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			break;
		case 2:
			if (bullet[i].bShow == true)
			{
				RECT part3; // 
				SetRect(&part3, 0, 0, 64, 64);
				D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 position3(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet_yellow, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			break;
		}		
	}

	////에네미 
	RECT part4;
	SetRect(&part4, 0, 0, 64, 64);
	D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		D3DXVECTOR3 position4(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_enemy, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	RECT part5;
	SetRect(&part5, 0, 0, 64, 64);
	D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	for (int i = 0; i<ENEMY_COLOR_NUM; i++)
	{
		D3DXVECTOR3 position5(enemy_blue[i].x_pos, enemy_blue[i].y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_enemy_blue, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	RECT part6;
	SetRect(&part6, 0, 0, 64, 64);
	D3DXVECTOR3 center6(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	for (int i = 0; i<ENEMY_COLOR_NUM; i++)
	{
		D3DXVECTOR3 position6(enemy_yellow[i].x_pos, enemy_yellow[i].y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_enemy_yellow, &part6, &center6, &position6, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	T_Score = Score / 10;
	O_Score = Score % 10;

	RECT part7;
	t_xPos = t_xPos * T_Score;
	SetRect(&part7, (T_Score)*72, 0, ((T_Score+1) * 72), 77);
	D3DXVECTOR3 center7(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position7(570.0f-68, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_TScore, &part7, &center7, &position7, D3DCOLOR_ARGB(255, 255, 255, 255));

	RECT part8;
	o_xPos = o_xPos * O_Score;
	if(O_Score == 0)
		SetRect(&part8, (O_Score) * 72, 0, ((O_Score + 1) * 72), 77);
	else
		SetRect(&part8, ((O_Score-1) * 72), 0, (O_Score * 72), 77);
	D3DXVECTOR3 center8(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position8(570.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_OScore, &part8, &center8, &position8, D3DCOLOR_ARGB(255, 255, 255, 255));
	

	////
	d3dspt->End();    // end sprite drawing
	d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero->Release();
	sprite_enemy->Release();
	sprite_enemy_blue->Release();
	sprite_enemy_yellow->Release();
	sprite_bullet->Release();
	sprite_bullet_blue->Release();
	sprite_bullet_yellow->Release();

	return;
}