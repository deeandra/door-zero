#define GAME_H
#include <string>
#include <map>

#define BTN_UP 13
#define BTN_RIGHT 12
#define BTN_LEFT 14
#define BTN_DOWN 27
#define BTN_A 33
#define BTN_B 32
#define BTN_MENU 35
#define BTN_LOG 34
#define SPEAKER 25

const unsigned short SCREEN_WIDTH = 320;
const unsigned short SCREEN_HEIGHT = 240;
const unsigned short SPRITE_WIDTH = 32;
const unsigned short SPRITE_HEIGHT = 48;

const unsigned short ROOM_COUNT = 9;

struct Room {

    unsigned short roomNumber;
	std::string roomName;
	const unsigned short *background; 
	unsigned short startingXY[2]; 
    unsigned short startingDirection;
	const unsigned short (*collisionZones)[2];
	unsigned short letterXY[5][2]; 
	std::string letter[20];	
	unsigned short musicXY[5][2];  
	std::string musicList[5]; 
    unsigned short doorXY[7][2];
    unsigned short doors[4];
    // 0 north /up
	// 1 south /down
	// 2 east /right
	// 3 west /left
    // 999 klo kosong
	
    void setStartingXY(unsigned short *source) {
        this->startingXY[0] = source[0];
        this->startingXY[1] = source[1];
    }

    // void setCollisionZones(unsigned short source[][2], unsigned short boxNum) {
    //     for(int i = 0; i < boxNum; ++i) {
    //         for(int j = 0; j < 2; ++j) {
    //             this->collisionZones[i][j] = source[i][j]; 
    //         }
    //     }
    // }

    void setLetterXY(unsigned short source[][2], unsigned short boxNum) {
        for(int i = 0; i < boxNum; ++i) {
            for(int j = 0; j < 2; ++j) {
                this->letterXY[i][j] = source[i][j]; 
            }
        }
    }

    void setLetter(std::string *source, unsigned short n) {
        for (int i = 0; i < n; i++) {
            this->letter[i] = source[i];
        }
    }

    void setMusicXY(unsigned short source[][2], unsigned short boxNum) {
        for(int i = 0; i < boxNum; ++i) {
            for(int j = 0; j < 2; ++j) {
                this->musicXY[i][j] = source[i][j]; 
            }
        }
    }

    void setMusicList(std::string *source, unsigned short n) {
        for (int i = 0; i < n; i++) {
            this->musicList[i] = source[i];
        }
    }

    void setDoorXY(unsigned short source[][2], unsigned short boxNum) {
        for(int i = 0; i < boxNum; ++i) {
            for(int j = 0; j < 2; ++j) {
                this->doorXY[i][j] = source[i][j]; 
            }
        }
    }

    void setDoors(unsigned short *source) {
        for (int i = 0; i < 4; i++) {
            this->doors[i] = source[i];
        }
    }
};



bool isMuted;
bool isWifiConnected;

enum GameState {
	START,
	MENUPAGE,
	ROOM,
	DIALOGUE,
	ROOM_LOADING,
	WIFI_PAUSE
};

enum RoomState {
	IDLE,
	WALKING
};

enum StartPageState {
	OPTION_NEWGAME,
	OPTION_CONTINUE
};

enum MenuState {
	OPTION_SAVE,
	OPTION_MUTE_UN,
	OPTION_WM, //wifi manager
	OPTION_LOG //list room apa aja yg dh dimasukin
};

enum Button {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    A,
    B,
    MENU,
    LOG
};

std::map<Button, unsigned short> pin = {
    {UP, 13}, 
    {DOWN, 27},
    {RIGHT, 12}, 
    {LEFT, 14},
    {A, 33}, 
    {B, 32},
    {MENU, 35}, 
    {LOG, 34},
};

std::map<Button, unsigned short> direction = {
    {UP, 1}, 
    {DOWN, 2},
    {RIGHT, 3}, 
    {LEFT, 4}
};

