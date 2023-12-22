
#ifndef TRACK_H
#define TRACK_H

// Structure to hold track information
#define NUMBEROFTRACK 8
struct Track
{
    int trackId;
    char trackName[50];
    int durationInSeconds;
};

// Function prototypes for getter and setter functions
void setTrackId(struct Track *track, int trackId);
int getTrackId(const struct Track *track);

void setTrackName(struct Track *track, const char *trackName);
const char *getTrackName(const struct Track *track);

void setDurationInSeconds(struct Track *track, int duration);
int getDurationInSeconds(const struct Track *track);
void *handleUserInput(void *arg);
#endif // TRACK_H