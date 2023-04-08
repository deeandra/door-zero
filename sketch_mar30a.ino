#include <TFT_eSPI.h>
// #include <SD.h>
// #include <FS.h>
// #include <SPI.h>

#include "bg_data.h"
#include "sprites_array.h"
#include "game.h"
#include "rooms.h"

// #define SD_CS  5
// #define TFT_CS 15
// #define MOSI   23
// #define MISO   19
// #define SCK    18


TFT_eSPI tft = TFT_eSPI();

// SDLib::SDClass SD;
// SD.begin(5);

unsigned short currentRoom;
Room thisRoom;

File saveFile;
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

TFT_eSprite chara = TFT_eSprite(&tft);
TFT_eSprite bg = TFT_eSprite(&tft);
TFT_eSprite arrow = TFT_eSprite(&tft);

void moveChara();
void idle_animation();
void start();
void roomInit();

void patchBg(unsigned short positionX, unsigned short positionY)
{
    unsigned int count = 0;
    unsigned short cropWidth = SPRITE_WIDTH + STEP_SIZE;
    unsigned short cropHeight = SPRITE_HEIGHT + STEP_SIZE;
    unsigned short croppedBg[cropHeight * cropWidth];

    for (int y = 0; y < cropHeight; y++)
    {
        for (int x = 0; x < cropWidth; x++)
        {
            if((y % 2 == 0) && (x % 2 == 0)){  
                count = x + (y*cropWidth);

                croppedBg[count] = thisRoom.background[(SCREEN_WIDTH/2 * (y/2 + positionY/2)) + (x/2 + positionX/2)];
                croppedBg[count+1] = thisRoom.background[(SCREEN_WIDTH/2 * (y/2 + positionY/2)) + (x/2 + positionX/2)];
                croppedBg[count+cropWidth] = thisRoom.background[(SCREEN_WIDTH/2 * (y/2 + positionY/2)) + (x/2 + positionX/2)];
                croppedBg[count+cropWidth+1] = thisRoom.background[(SCREEN_WIDTH/2 * (y/2 + positionY/2)) + (x/2 + positionX/2)];
            }
        }
    }
    bg.pushImage(0, 0, cropWidth, cropHeight, croppedBg);
    // tft.pushImage(positionX, positionY, cropWidth, cropHeight, newArr);
    return;
}

bool collisionDetected(const unsigned short collisions[][2], unsigned short collisionNum){
    unsigned short charaBoxX = charaX; 
    unsigned short charaBoxY = charaY + 32; // untuk kaki nya aja
    unsigned short charaBoxW = SPRITE_WIDTH;
    unsigned short charaBoxH = SPRITE_HEIGHT - 32;
    unsigned short collisionWH = 16;

    for(int i=0; i<collisionNum; i++){
        if(collisions[i][0] == 0 && collisions[i][1] == 0){
            return false;
        }
        if (charaBoxX < collisions[i][0] + collisionWH &&
            charaBoxX + charaBoxW > collisions[i][0] &&
            charaBoxY < collisions[i][1] + collisionWH &&
            charaBoxY + charaBoxH > collisions[i][1]){
                return true;
            }
    }
    if (charaX < 0 ||
        charaY < 0 ||
        charaX + SPRITE_WIDTH > SCREEN_WIDTH ||
        charaY + SPRITE_HEIGHT > SCREEN_HEIGHT){
            return true;
        }
    return false;
}

void bIsPushed() {

}

void loadingScreen() {

}

void showDialogueBox() {

}

void readRoomLetter() {

}

void playRoomMusic() {

}

void chooseDoor() {
    if (thisRoom.roomNumber==6){
        currentRoom = 5;
        loadingScreen();
        roomInit();
    }

    showDialogueBox();

    while(true){

    }
}

void aIsPushed_room() {
    short interactNum;
    short (*interactZones)[2];
    
    if (directionId==1) {
        short zones[2][2] = {{charaX, charaY+32-TILE_SIZE}, {charaX+TILE_SIZE, charaY+32-TILE_SIZE}};
        interactZones = zones;
        interactNum = 2;
    }
    else if (directionId==2) {
        short zones[2][2] = {{charaX, charaY+SPRITE_HEIGHT}, {charaX+TILE_SIZE, charaY+SPRITE_HEIGHT}};
        interactNum = 2;
    }
    else if (directionId==3) {
        short zones[3][2] = {{charaX-TILE_SIZE, charaY}, {charaX-TILE_SIZE, charaY+TILE_SIZE}, {charaX-TILE_SIZE, charaY+(2*TILE_SIZE)}};
        interactNum = 3;
    }
    else { //directionId==4
        short zones[3][2] = {{charaX+SPRITE_WIDTH, charaY}, {charaX+SPRITE_WIDTH, charaY+TILE_SIZE}, {charaX+SPRITE_WIDTH, charaY+(2*TILE_SIZE)}};
        interactNum = 3;
    }

    //letter
    for (int i=0; i<interactNum; i++) {
        for (int j=0; j<5; j++){
            if ((interactZones[i][0]==0 && interactZones[i][1]==0) || 
                (thisRoom.letterXY[j][0]==0 && thisRoom.letterXY[j][1]==0)) {
                continue;
            }
            if((interactZones[i][0]==thisRoom.letterXY[j][0]) && (interactZones[i][1]==thisRoom.letterXY[j][1])){
                readRoomLetter();
            }
        }
    }

    //music
    for (int i=0; i<interactNum; i++) {
        for (int j=0; j<5; j++){
            if ((interactZones[i][0]==0 && interactZones[i][1]==0) || 
                (thisRoom.musicXY[j][0]==0 && thisRoom.musicXY[j][1]==0)) {
                continue;
            }
            if((interactZones[i][0]==thisRoom.musicXY[j][0]) && (interactZones[i][1]==thisRoom.musicXY[j][1])){
                playRoomMusic();
            }
        }
    }

    //door
    for (int i=0; i<interactNum; i++) {
        for (int j=0; j<7; j++){
            if ((interactZones[i][0]==0 && interactZones[i][1]==0) || 
                (thisRoom.doorXY[j][0]==0 && thisRoom.doorXY[j][1]==0)) {
                continue;
            }
            if((interactZones[i][0]==thisRoom.doorXY[j][0]) && (interactZones[i][1]==thisRoom.doorXY[j][1])){
                chooseDoor();
            }
        }
    }
}

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

void roomInit()
{
    Serial.println("roomInit start");
    thisRoom = rooms[currentRoom];
    charaX = thisRoom.startingXY[0];
    charaY = thisRoom.startingXY[1];
    directionId = thisRoom.startingDirection;
    Serial.println("RoomInit 2");

    // bgFile = SD.open("/anam.bmp", FILE_READ);

    // getBgArray();
    // thisRoom.background = anam_room;

    // tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, thisRoom.background);
    pushBg2x();
    Serial.println("RoomInit after pushbg2x");
    
    bg.createSprite(SPRITE_WIDTH+STEP_SIZE, SPRITE_HEIGHT+STEP_SIZE);
    bg.setSwapBytes(true);

    chara.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
    // chara.setSwapBytes(true);
// 
    //  bg.pushImage(0, 0, bgWidth, bgHeight, home);
    patchBg(charaX, charaY);
    chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[0]);
    //  chara.pushToSprite(&bg, charaX, charaY, TFT_BLACK);
    chara.pushToSprite(&bg, 0, 0, TFT_BLACK);

    bg.pushSprite(charaX, charaY);
    Serial.println("RoomInit end");
}

void readSaveFile() {
    // digitalWrite(TFT_CS, HIGH);
    // digitalWrite(SD_CS, LOW);

    // SPI.begin(SCK, MISO, MOSI, SD_CS);
    // if(!SD.begin(SD_CS, SPI)){
    //     Serial.println("Card Mount Failed (read)");
    //     return;
    // }

    // String line;
    // saveFile = SD.open("/save.txt", FILE_READ);

    // line = saveFile.readStringUntil('\n');
    // currentRoom = atoi(line.c_str());
    // line = saveFile.readStringUntil('\n');
    // charaX = atoi(line.c_str());
    // line = saveFile.readStringUntil('\n');
    // charaY = atoi(line.c_str());
    // line = saveFile.readStringUntil('\n');
    // directionId = atoi(line.c_str());
    // for(short i=0; i<ROOM_COUNT; i++){ 
    //     line = saveFile.readStringUntil('\n');
    //     visitedRooms[i] = atoi(line.c_str());
    // }

    // saveFile.close();
    // Serial.println("read done");
    // SD.end();
    // SPI.endTransaction();
    // SPI.end();

    // digitalWrite(SD_CS, HIGH);
    // digitalWrite(TFT_CS, LOW);
}

void writeSaveFile() {
    // digitalWrite(TFT_CS, HIGH);
    // digitalWrite(SD_CS, LOW);

    // SPI.begin(SCK, MISO, MOSI, SD_CS);
    // if(!SD.begin(SD_CS, SPI)){
    //     Serial.println("Card Mount Failed (write)");
    //     return;
    // }

    // saveFile = SD.open("/save.txt", FILE_WRITE);

    // saveFile.println(currentRoom); //currentRoom
    // saveFile.println(charaX); //charaX
    // saveFile.println(charaY); //charaY
    // saveFile.println(directionId);
    // for(short i=0; i<ROOM_COUNT; i++){ //visitedRooms
    //     saveFile.println(visitedRooms[i]); 
    // }

    // saveFile.close();
    // Serial.println("write done");
    // SD.end();
    // SPI.endTransaction();
    // SPI.end();

    // digitalWrite(SD_CS, HIGH);
    // digitalWrite(TFT_CS, LOW);
}

void overwriteSaveFile() {
    // digitalWrite(TFT_CS, HIGH);
    // digitalWrite(SD_CS, LOW);

    // SPI.begin(SCK, MISO, MOSI, SD_CS);
    // if(!SD.begin(SD_CS, SPI)){
    //     Serial.println("Card Mount Failed (overwrite))");
    //     return;
    // }

    // saveFile = SD.open("/save.txt", FILE_WRITE);

    // saveFile.println(6); //currentRoom
    // saveFile.println(rooms[6].startingXY[0]); //charaX
    // saveFile.println(rooms[6].startingXY[1]); //charaY
    // saveFile.println(rooms[6].startingDirection);
    // for(short i=0; i<ROOM_COUNT; i++){ //visitedRooms
    //     if (i==6) {
    //         saveFile.println(1);
    //     }
    //     else {
    //         saveFile.println(0);
    //     }
    // }
    // saveFile.close();
    // Serial.println("overwrite done");
    // SD.end();
    // SPI.endTransaction();
    // SPI.end();

    // digitalWrite(SD_CS, HIGH);
    // digitalWrite(TFT_CS, LOW);
}

void setup()
{
    loadRoomData();

    Serial.begin(115200);
    if(EEPROM.read(0) )

    // // pinMode(TFT_CS, OUTPUT);
    // // pinMode(SD_CS, OUTPUT);
    // digitalWrite(TFT_CS, HIGH);
    // digitalWrite(SD_CS, LOW);

    // SPI.begin(SCK, MISO, MOSI, SD_CS);
    // if(!SD.begin(SD_CS, SPI)){
    //     Serial.println("Card Mount Failed (setup)");
    //     return;
    // }


    // saveFile = SD.open("/save.txt", FILE_READ);

    // if (!saveFile) {
    //     saveFile.close();
    //     overwriteSaveFile();
    // }

    // // readSaveFile();
    // SD.end();
    // SPI.endTransaction();
    // SPI.end();

    // digitalWrite(SD_CS, HIGH);
    // digitalWrite(TFT_CS, LOW);
    
    
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_MENU, INPUT_PULLUP);
    pinMode(BTN_LOG, INPUT_PULLUP);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, start_bg_full);
    start();
}

void loop()
{
    if (gameState == ROOM)
    {
        //animasi idle
        if (roomState == IDLE)
        {
            if (millis() - lastAnimationTime > 100)
            {
                idle_animation();
                lastAnimationTime = millis();
            }
        }

        // animasi jalan
        for (Button b : {Button::UP, Button::DOWN, Button::RIGHT, Button::LEFT}) {
            if (digitalRead(pin[b]) == LOW){
                directionId = direction[b];
                moveChara();
            }
        }

        // process pencet A 
        if (digitalRead(pin[A]) == LOW) {
            // aIsPushed_room();
            // if(millis() - lastButtonPress > 1000){
            //     Serial.println("A is clicked!");
            //     writeSaveFile();
            // }
        }
        // if (digitalRead(pin[B]) == LOW) {
        //     writeSaveFile();
        // }
        // else if (digitalRead(pin[B]) == LOW) {
        //     bIsPushedWhileRoom();
        // }
    }
    else if (gameState == START)
    {
        
      for (Button b : {Button::UP, Button::DOWN, Button::RIGHT, Button::LEFT, Button::A, Button::B, Button::MENU, Button::LOG}) {
            if (digitalRead(pin[b]) == LOW){
                if(b == UP || b == DOWN) {
                    if (millis() - lastButtonPress > 500){
                        start();
                        lastButtonPress = millis();
                        break;
                    }
                }
                else if (b == A) {
                    arrow.deleteSprite();
                    tft.fillScreen(TFT_BLACK);
                    if (startPageState == OPTION_CONTINUE)
                    {
                        gameState = ROOM;

                        readSaveFile();

                        Serial.println(currentRoom);
                        Serial.println(charaX);
                        Serial.println(charaY);
                        Serial.println(directionId);
                        Serial.println(visitedRooms[1]);
                        Serial.println(visitedRooms[6]);

                        // currentRoom = 6;
                        // charaX = 144;
                        // charaY = 96;
                        // directionId = 2;

                        delay(1000);
                        roomInit();
                    }
                    else if (startPageState == OPTION_NEWGAME)
                    {
                        gameState = ROOM;
                        overwriteSaveFile();
                        readSaveFile();
                        roomInit();
                    }
                }
            }
        }
    }
}

void moveChara()
{
    if (millis() - lastAnimationTime < 100)
    {
        return;
    }
    // patchBg(charaX, charaY, SPRITE_WIDTH, SPRITE_HEIGHT, thisRoom.background);

    if (lastRoomState == IDLE || lastDirection != directionId){
        lastPose = 0;
    }else{
        if(lastPose == 5){
            lastPose = 0;
        }else{
            lastPose++;
        }
    }

    if (directionId == 1)
    { // UP
        charaY -= STEP_SIZE;
        if (collisionDetected(thisRoom.collisionZones, 36)) {
            charaY += STEP_SIZE;
        }
        patchBg(charaX, charaY);
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, walk_up[lastPose]);
        chara.pushToSprite(&bg, 0, 0, TFT_BLACK);
        bg.pushSprite(charaX, charaY);
    }
    else if (directionId == 2)
    { // DOWN
        charaY += STEP_SIZE;
        if (collisionDetected(thisRoom.collisionZones, 36)) {
            charaY -= STEP_SIZE;
        }
        patchBg(charaX, charaY-STEP_SIZE);
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, walk_down[lastPose]);
        chara.pushToSprite(&bg, 0, STEP_SIZE, TFT_BLACK);
        bg.pushSprite(charaX, charaY-STEP_SIZE);
        // chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[0]);
    }
    else if (directionId == 3)
    { // RIGHT
        charaX -= STEP_SIZE;
        if (collisionDetected(thisRoom.collisionZones, 36)) {
            charaX += STEP_SIZE;
        }
        patchBg(charaX, charaY);
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, walk_right[lastPose]);
        chara.pushToSprite(&bg, 0, 0, TFT_BLACK);
        bg.pushSprite(charaX, charaY);
        // chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_right[0]);
    }
    else if (directionId == 4)
    { // LEFT
        charaX += STEP_SIZE;
        if (collisionDetected(thisRoom.collisionZones, 36)) {
            charaX -= STEP_SIZE;
        }
        patchBg(charaX-STEP_SIZE, charaY);
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, walk_left[lastPose]);
        chara.pushToSprite(&bg, STEP_SIZE, 0, TFT_BLACK);
        bg.pushSprite(charaX-STEP_SIZE, charaY);
        // chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_left[0]);
    }

   

    //  chara.pushToSprite(&bg, charaX, charaY, TFT_BLACK);
    lastAnimationTime = millis();
    lastRoomState = WALKING;
    lastDirection = directionId;
    //  delay(50);
    //  pushRoomObjects(currentRoom);
    //  bg.pushSprite((SCREEN_WIDTH-bgWidth)/2, (SCREEN_HEIGHT-bgHeight)/2);
}

void idle_animation()
{
    if (lastPose < 5)
    {
        lastPose++;
    }
    else if (lastPose == 5)
    {
        lastPose = 0;
    }

    //  bg.pushImage(0, 0, bgWidth, bgHeight, home);
    patchBg(charaX, charaY);

    if (directionId == 1)
    {
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_up[lastPose]);
    }
    else if (directionId == 2)
    {
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[lastPose]);
    }
    else if (directionId == 3)
    {
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_right[lastPose]);
    }
    else if (directionId == 4)
    {
        chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_left[lastPose]);
    }

    chara.pushToSprite(&bg, 0, 0, TFT_BLACK);
    // chara.pushSprite(charaX, charaY, TFT_BLACK);
    lastRoomState = IDLE;

    bg.pushSprite(charaX, charaY);
}

void start()
{
    unsigned int count = 0;
    unsigned short cropWidth = 32;
    unsigned short cropHeight = 80;
    unsigned short croppedBg[cropHeight * cropWidth];
    unsigned short positionX = 80;
    unsigned short positionY = 128;
    for (int y = 0; y < cropHeight; y++)
    {
        for (int x = 0; x < cropWidth; x++)
        {
            croppedBg[count] = start_bg_full[(SCREEN_WIDTH * (y + positionY)) + (x + positionX)];
            count++;
        }
    }
    tft.pushImage(80, 128, 32, 80, croppedBg);
    // patchBg(80, 128, 32, 80, start_bg_full);


    arrow.createSprite(32, 32);
    arrow.setSwapBytes(true);
    arrow.pushImage(0, 0, 32, 32, start_arrow);
    if (startPageState == OPTION_CONTINUE)
    {
        arrow.pushSprite(START_ARROW_X, START_ARROW_Y_NEWGAME, TFT_BLACK);
        startPageState = OPTION_NEWGAME;
    }
    else if (startPageState == OPTION_NEWGAME)
    {
        arrow.pushSprite(START_ARROW_X, START_ARROW_Y_CONTINUE, TFT_BLACK);
        startPageState = OPTION_CONTINUE;
    }
}
