#include <TFT_eSPI.h>
#include <EEPROM.h>

#include "bg_data.h"
#include "sprites_array.h"
#include "game.h"
#include "game_sound.h"
#include "rooms.h"

void moveChara();
void idle_animation();
void start();
void roomInit();
void showDialogueBox();
void hideDialogueBox();
void printDialogue(std::string dialogue);

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

unsigned short* arrayBgCustom(unsigned short positionX, unsigned short positionY, unsigned short cropWidth, unsigned short cropHeight)
{
    unsigned int count = 0;
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
    // bg.pushImage(0, 0, cropWidth, cropHeight, croppedBg);
    // tft.pushImage(positionX, positionY, cropWidth, cropHeight, croppedBg);
    return croppedBg;
}


bool collisionDetected() {
    unsigned short charaBoxX = charaX; 
    unsigned short charaBoxY = charaY + 32; // untuk kaki nya aja
    unsigned short charaBoxW = SPRITE_WIDTH;
    unsigned short charaBoxH = SPRITE_HEIGHT - 32;
    unsigned short collisionWH = 16;

    for(int i=0; i<thisRoom.collisionNum; i++){
        if(thisRoom.collisionZones[i][0] == 0 && thisRoom.collisionZones[i][1] == 0){
            return false;
        }
        if (charaBoxX < thisRoom.collisionZones[i][0] + collisionWH &&
            charaBoxX + charaBoxW > thisRoom.collisionZones[i][0] &&
            charaBoxY < thisRoom.collisionZones[i][1] + collisionWH &&
            charaBoxY + charaBoxH > thisRoom.collisionZones[i][1]){
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

void readRoomLetter() {
    showDialogueBox();
    short index = 0;
    printDialogue(thisRoom.letter[index]);
    delay(300);

    while (true) {
        if (digitalRead(BTN_LOG) == LOW) {
            hideDialogueBox();
            return;
        }
        else if (digitalRead(BTN_A) == LOW) {
            index++;
            if (index==20 || thisRoom.letter[index].empty()){
                hideDialogueBox();
                return;
            }
            printDialogue(thisRoom.letter[index]);
            delay(300);
        }
    }
}

void playRoomMusic() {

}

void showDialogueBox() {
    int y = 0;
    while(true){
        if(millis() - lastAnimationTime > 50){
            y += 8;
            tft.pushImage(0, SCREEN_HEIGHT-y, SCREEN_WIDTH, 80, dialogue_box);
            lastAnimationTime = millis();
            if(y==80){
                break;
            }
        }
    }
    dialogueBoxIsOpen = true;
}

void hideDialogueBox() {
    dialogueBox.createSprite(SCREEN_WIDTH, 80);
    dialogueBox.setSwapBytes(true);

    tft.fillRect(20, 180, 280, 40, TFT_WHITE);

    int y = 80;
    while(true){
        if(millis() - lastAnimationTime > 50){
            dialogueBox.pushImage(0, 80-y, SCREEN_WIDTH, 16, arrayBgCustom(0, SCREEN_HEIGHT-y, SCREEN_WIDTH, 16));
            y -= 8;
            if (directionId == 1) { // UP
                chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_up[lastPose]);
            }
            else if (directionId == 2) { // DOWN
                chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[lastPose]);
            }
            else if (directionId == 3) { // RIGHT
                chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_right[lastPose]);
            }
            else if (directionId == 4) { // LEFT
                chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_left[lastPose]);
            }
            chara.pushToSprite(&dialogueBox, charaX, charaY-160, TFT_BLACK);
            dialogueBox.pushImage(0, 80-y, SCREEN_WIDTH, 80, dialogue_box);
            dialogueBox.pushSprite(0, 160);
            lastAnimationTime = millis();
            if (y==0) {
                break;
            }
        }
    }
    dialogueBox.deleteSprite();
    dialogueBoxIsOpen = false;
}

void printDialogue(std::string dialogue) {
    tft.setTextColor(0x39CA);
    tft.setTextSize(2);
    
    short x = 22;
    short stepX = 12;
    short y = 182;
    short stepY = 18;

    short count = 0;
    short countWordChar;
    bool newline = false;

    tft.fillRect(20, 180, 280, 40, TFT_WHITE);

    for (short i=0; i<dialogue.length(); i++) {
        if (x + (count+1)*stepX > 298 || newline==true) {
            count = 0;
            y = y + stepY;
            newline = false;
            if (std::isspace(dialogue[i])) {
                continue;
            }
        }
        if (std::isspace(dialogue[i])) {
            countWordChar = 0;
            short index = 1;
            while (true) {
                if(i+index == dialogue.length()) {
                    break;
                }
                if (std::isspace(dialogue[i+index])) {
                    break;
                }
                index++;
                countWordChar++;
            }
            if (x + (count+1+countWordChar)*stepX > 298) {
                newline = true;
            }
        }
        tft.setCursor(x + (count*stepX), y, 1);
        tft.print(dialogue[i]);
        playIfNotMute(NOTE_A4, 25);
        delay(25);
        count++;
    }
    stopIfNotMute();
}

void startTutorial() {
    showDialogueBox();
    printDialogue("Hello world! What a wonderful day!");
    while(true){}
    
}

void chooseDoor() {
    if (thisRoom.roomNumber==6){
        currentRoom = 5;
        // loadingScreen();
        roomInit();
        // startTutorial();
    }

    // showDialogueBox();

    // while(true){

    // }
}

void aIsPushed_room() {
    if ((millis() - lastButtonPress) < 300) {
        return;
    }

    lastButtonPress = millis();

    short interactNum;
    short interactZones[3][2];
    
    if (directionId==1) {
        // short zones[2][2] = {{charaX, charaY+32-TILE_SIZE}, {charaX+TILE_SIZE, charaY+32-TILE_SIZE}};
        interactZones[0][0] = charaX;
        interactZones[0][1] = charaY+32-TILE_SIZE;
        interactZones[1][0] = charaX+TILE_SIZE;
        interactZones[1][1] = charaY+32-TILE_SIZE;
        Serial.println(interactZones[0][0]);
        Serial.println(interactZones[0][1]);
        Serial.println(interactZones[1][0]);
        Serial.println(interactZones[1][1]);
        interactNum = 2;
    }
    else if (directionId==2) {
        // short zones[2][2] = {{charaX, charaY+SPRITE_HEIGHT}, {charaX+TILE_SIZE, charaY+SPRITE_HEIGHT}};
        interactZones[0][0] = charaX;
        interactZones[0][1] = charaY+SPRITE_HEIGHT;
        interactZones[1][0] = charaX+TILE_SIZE;
        interactZones[1][1] = charaY+SPRITE_HEIGHT;
        interactNum = 2;
    }
    else if (directionId==3) {
        // short zones[3][2] = {{charaX-TILE_SIZE, charaY}, {charaX-TILE_SIZE, charaY+TILE_SIZE}, {charaX-TILE_SIZE, charaY+(2*TILE_SIZE)}};
        interactZones[0][0] = charaX-TILE_SIZE;
        interactZones[0][1] = charaY;
        interactZones[1][0] = charaX-TILE_SIZE;
        interactZones[1][1] = charaY+TILE_SIZE;
        interactZones[2][0] = charaX-TILE_SIZE;
        interactZones[2][1] = charaY+(2*TILE_SIZE);
        interactNum = 3;
    }
    else if (directionId==4){ //directionId==4
        // short zones[3][2] = {{charaX+SPRITE_WIDTH, charaY}, {charaX+SPRITE_WIDTH, charaY+TILE_SIZE}, {charaX+SPRITE_WIDTH, charaY+(2*TILE_SIZE)}};
        interactZones[0][0] = charaX+SPRITE_WIDTH;
        interactZones[0][1] = charaY;
        interactZones[1][0] = charaX+SPRITE_WIDTH;
        interactZones[1][1] = charaY+TILE_SIZE;
        interactZones[2][0] = charaX+SPRITE_WIDTH;
        interactZones[2][1] = charaY+(2*TILE_SIZE);
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
                return;
            }
        }
    }

    //music
    for (int i=0; i<interactNum; i++) {
        for (int j=0; j<5; j++){
            if ((thisRoom.musicXY[j][0]==0 && thisRoom.musicXY[j][1]==0)) {
                continue;
            }
            if((interactZones[i][0]==thisRoom.musicXY[j][0]) && (interactZones[i][1]==thisRoom.musicXY[j][1])){
                playRoomMusic();
                return;
            }
        }
    }

    //door
    for (int i=0; i<interactNum; i++) {
        for (int j=0; j<7; j++){
            if ((thisRoom.doorXY[j][0]!=0 && thisRoom.doorXY[j][1]!=0)) {
                if((interactZones[i][0]==thisRoom.doorXY[j][0]) && (interactZones[i][1]==thisRoom.doorXY[j][1])){
                    chooseDoor();
                    return;
                }
            } 
        }
    }
}

void roomInit()
{
    tft.fillScreen(TFT_BLACK);

    thisRoom = rooms[currentRoom];
    charaX = thisRoom.startingXY[0];
    charaY = thisRoom.startingXY[1];
    directionId = thisRoom.startingDirection;

    pushBg2x();
    
    bg.createSprite(SPRITE_WIDTH+STEP_SIZE, SPRITE_HEIGHT+STEP_SIZE);
    bg.setSwapBytes(true);

    chara.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
    patchBg(charaX, charaY);
    chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[0]);
    chara.pushToSprite(&bg, 0, 0, TFT_BLACK);

    bg.pushSprite(charaX, charaY);
}

void loadSave()
{
    thisRoom = rooms[currentRoom];

    pushBg2x();
    
    bg.createSprite(SPRITE_WIDTH+STEP_SIZE, SPRITE_HEIGHT+STEP_SIZE);
    bg.setSwapBytes(true);

    chara.createSprite(SPRITE_WIDTH, SPRITE_HEIGHT);
    patchBg(charaX, charaY);
    chara.pushImage(0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, idle_down[0]);
    chara.pushToSprite(&bg, 0, 0, TFT_BLACK);

    bg.pushSprite(charaX, charaY);
}

void setup()
{
    loadRoomData();

    EEPROM.begin(EEPROM_SIZE);

    Serial.begin(115200);
    if (EEPROMIsEmpty()) {
        initializeEEPROM();
    }
    
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
    playSong();
}

void loop()
{
    if(MENU)
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
            aIsPushed_room();
            // if (millis() - lastButtonPress > 1000) {
            //     lastButtonPress = millis();
            //     aIsPushed_room();
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

                        readEEPROM();

                        delay(1000);

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

                        loadSave();
                    }
                    else if (startPageState == OPTION_NEWGAME)
                    {
                        gameState = ROOM;
                        initializeEEPROM();
                        readEEPROM();
                        loadSave();
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
        if (collisionDetected()) {
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
        if (collisionDetected()) {
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
        if (collisionDetected()) {
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
        if (collisionDetected()) {
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
