// track.c
#include <stdio.h>
#include "track.h"

void setTrackId(struct Track *track, int trackId)
{
    track->trackId = trackId;
}

int getTrackId(const struct Track *track)
{
    return track->trackId;
}

void setTrackName(struct Track *track, const char *trackName)
{
    snprintf(track->trackName, sizeof(track->trackName), "%s", trackName);
}

const char *getTrackName(const struct Track *track)
{
    return track->trackName;
}

void setDurationInSeconds(struct Track *track, int duration)
{
    track->durationInSeconds = duration;
}

int getDurationInSeconds(const struct Track *track)
{
    return track->durationInSeconds;
}
