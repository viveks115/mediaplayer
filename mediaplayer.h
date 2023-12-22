

#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include "track.h"

// Function prototypes for media player functionalities
void playTrack(const struct Track *playlist);
void pauseTrack(const struct Track *playlist);
void nextTrack(const struct Track *playlist);
void prevTrack(const struct Track *playlist);
void togglePlaybackSpeed();

#endif // MEDIAPLAYER_H