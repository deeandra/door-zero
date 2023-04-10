#define GAME_H
#include <string>
#include <map>

#define BTN_UP 13
#define BTN_RIGHT 12
#define BTN_LEFT 14
#define BTN_DOWN 27
#define BTN_A 33
#define BTN_B 32
#define BTN_MENU 26
#define BTN_LOG 21
#define SPEAKER 25

const unsigned short EEPROM_SIZE = 26;
const unsigned short CURRENT_ROOM_ADDRESS = 0;
const unsigned short CHARA_X_ADDRESS = 2;
const unsigned short CHARA_Y_ADDRESS = 4;
const unsigned short DIRECTION_ID_ADDRESS = 6;
const unsigned short VISITED_ROOM_ADDRESS = 8;

const unsigned short SCREEN_WIDTH = 320;
const unsigned short SCREEN_HEIGHT = 240;
const unsigned short SPRITE_WIDTH = 32;
const unsigned short SPRITE_HEIGHT = 48;

const unsigned short ROOM_COUNT = 9;

TFT_eSPI tft = TFT_eSPI();


struct Room {

    unsigned short roomNumber;
	std::string roomName;
	const unsigned short *background; 
	unsigned short startingXY[2]; 
    unsigned short startingDirection;
	const unsigned short (*collisionZones)[2];
    unsigned short collisionNum;
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


unsigned short currentRoom;
Room thisRoom;
unsigned short visitedRooms[ROOM_COUNT];

unsigned short charaX;
unsigned short charaY;
const unsigned short STEP_SIZE = 8;
const unsigned short TILE_SIZE = 16;

const unsigned short START_ARROW_X = 80;
const unsigned short START_ARROW_Y_CONTINUE = 136;
const unsigned short START_ARROW_Y_NEWGAME = 168;

StartPageState startPageState = OPTION_NEWGAME;
GameState gameState = START;
RoomState roomState = IDLE;

unsigned short directionId;
unsigned short lastPose = 0;
unsigned long lastAnimationTime = 0;
unsigned short lastDirection = 1;
RoomState lastRoomState = IDLE;

unsigned long lastButtonPress = 0;
bool processBtnPress = true;
Button lastBtn;

bool dialogueBoxIsOpen = false;

TFT_eSprite chara = TFT_eSprite(&tft);
TFT_eSprite bg = TFT_eSprite(&tft);
TFT_eSprite arrow = TFT_eSprite(&tft);
TFT_eSprite dialogueBox = TFT_eSprite(&tft);

void pushBg2x() {
    unsigned short count = 0;

    for (int y=0; y<SCREEN_HEIGHT; y++) {
        for (int x=0; x<SCREEN_WIDTH; x++) {
            if((y % 2 == 0) && (x % 2 == 0)){  
                tft.drawPixel(x, y, thisRoom.background[count]);
                tft.drawPixel(x+1, y, thisRoom.background[count]);
                tft.drawPixel(x, y+1, thisRoom.background[count]);
                tft.drawPixel(x+1, y+1, thisRoom.background[count]);
                count++;
            }
        }
    }
}


bool EEPROMIsEmpty() {
    if(EEPROM.read(CHARA_X_ADDRESS) == 0 && EEPROM.read(CHARA_Y_ADDRESS) == 0){
        return true;
    }
    return false;
}

void initializeEEPROM() {
    unsigned short currentRoom_i = 6;
    unsigned short charaX_i = 144;
    unsigned short charaY_i = 96;
    unsigned short directionId_i = 2;
    unsigned short visitedRooms_i[ROOM_COUNT] = {0,0,0,0,0,0,1,0,0};

    short address = 0;

    EEPROM.write(address, currentRoom_i);
    address += sizeof(currentRoom_i);
    Serial.println(address);
    EEPROM.write(address, charaX_i);
    address += sizeof(charaX_i);
    Serial.println(address);
    EEPROM.write(address, charaY_i);
    address += sizeof(charaY_i);
    Serial.println(address);
    EEPROM.write(address, directionId_i);
    address += sizeof(directionId_i);
    Serial.println(address);
    for (short i=0; i<ROOM_COUNT; i++) {
        EEPROM.write(address, visitedRooms_i[i]);
        address += sizeof(visitedRooms_i[i]);
        Serial.println(address);
    }
    EEPROM.commit();

    Serial.println("EEPROM initialized");
}

void readEEPROM() {
    short address = 0;
    currentRoom = EEPROM.read(CURRENT_ROOM_ADDRESS);
    charaX = EEPROM.read(CHARA_X_ADDRESS);
    charaY = EEPROM.read(CHARA_Y_ADDRESS);
    directionId = EEPROM.read(DIRECTION_ID_ADDRESS);
    for (short i=0; i<ROOM_COUNT; i++) {
        visitedRooms[i] = EEPROM.read(VISITED_ROOM_ADDRESS+(i*2));
    }
    Serial.println("EEPROM read");
}

void writeEEPROM() {
    short address = 0;

    EEPROM.write(address, currentRoom);
    address += sizeof(currentRoom);
    Serial.println(address);
    EEPROM.write(address, charaX);
    address += sizeof(charaX);
    Serial.println(address);
    EEPROM.write(address, charaY);
    address += sizeof(charaY);
    Serial.println(address);
    EEPROM.write(address, directionId);
    address += sizeof(directionId);
    Serial.println(address);
    for (short i=0; i<ROOM_COUNT; i++) {
        EEPROM.write(address, visitedRooms[i]);
        address += sizeof(visitedRooms[i]);
        Serial.println(address);
    }
    
    EEPROM.commit();
    Serial.println("EEPROM written");
}