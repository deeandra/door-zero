#include "Tone32.hpp"
#include "pitches.h"

#define BUZZZER_PIN  25 

Tone32 _tone32(25, 0);
bool isMute = true;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};



void playSong() {
    if(isMute == false) {
        for (int thisNote = 0; thisNote < 8; thisNote++) {
            int noteDuration = 1000 / noteDurations[thisNote];
            _tone32.playTone(melody[thisNote], noteDuration);

            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
        //    _tone32.stopPlaying();
        }
        _tone32.stopPlaying();
    }
}


void playIfNotMute(double freq, unsigned long duration) {
    if(isMute == false) {
        _tone32.playTone(freq, duration);
    }
}

void stopIfNotMute() {
    if(isMute == false) {
        _tone32.stopPlaying();
    }
}