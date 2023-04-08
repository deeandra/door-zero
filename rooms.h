#include <string.h>
#include "collision_data.h"

Room rooms[ROOM_COUNT];

void load8() {
    unsigned short startingXY_s[2] = {144, 184};
    unsigned short letterXY_s[2][2] = {{64,96}, {64,112}};
    std::string letter_s[2] = {"I honestly do miss Anam.", "I hope to one day visit again."};
    unsigned short musicXY_s[2][2] = {{176,80}, {192, 80}};
    std::string musicList_s[2] = {"rickroll", "bensu"};
    unsigned short doorXY_s[4][2] = {{128,224}, {144,224}, {160,224}, {176,224}};
    unsigned short doors_s[4] = {999,999,999,999};

    rooms[8].roomNumber = 18;
    rooms[8].roomName = "Anam International House Room XXX";
    rooms[8].background = anam;
    rooms[8].setStartingXY(startingXY_s);
    rooms[8].startingDirection = 1;
    rooms[8].collisionZones = collision_anam;
    rooms[8].setLetterXY(letterXY_s, 2);
    rooms[8].setLetter(letter_s, 2);
    rooms[8].setMusicXY(musicXY_s, 2);
    rooms[8].setMusicList(musicList_s, 2);
    rooms[8].setDoorXY(doorXY_s, 3);
    rooms[8].setDoors(doors_s);
}

void load6() {
    //start room
    unsigned short startingXY_s[2] = {144, 96};
    unsigned short doorXY_s[4][2] = {{128,48}, {144, 48}, {160,48}, {176, 48}};

    rooms[6].roomNumber = 6;
    rooms[6].roomName = "The Starting Point";
    rooms[6].background = start_room;
    rooms[6].setStartingXY(startingXY_s);
    rooms[6].startingDirection = 2;
    rooms[6].collisionZones = collision_start_room;
    rooms[6].setDoorXY(doorXY_s, 4);
}

void load5() {
    //start2 room
    unsigned short startingXY_s[2] = {144, 192};
    unsigned short letterXY_s[4][2] = {{144, 144}, {144, 160}, {160, 144}, {160, 160}};
    std::string letter_s[3] = {"It's a toilet plunger.", "And it's stuck to the ground.", "'Take this' my ass, what a useless waste of white paint."};
    unsigned short doorXY_s[6][2] = {{128,224}, {144,224}, {160,224}, {176,224}, {144, 48}, {160, 48}};
    unsigned short doors_s[4] = {999,999,8,999};

    rooms[5].roomNumber = 5;
    rooms[5].roomName = "The First Door";
    rooms[5].background = start2_room;
    rooms[5].setStartingXY(startingXY_s);
    rooms[5].startingDirection = 1;
    rooms[5].collisionZones = collision_start2_room;
    rooms[5].setLetterXY(letterXY_s, 4);
    rooms[5].setLetter(letter_s, 3);
    rooms[5].setDoorXY(doorXY_s, 6);
    rooms[5].setDoors(doors_s);
}

void loadRoomData() {
    load8();
    load6();
    load5();

    return;

}

