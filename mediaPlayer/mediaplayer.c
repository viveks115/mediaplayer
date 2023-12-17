// mediaplayer.c

// mediaplayer.c

#include <stdio.h>
#include <windows.h> // For sleep and Windows API functions
#include <conio.h>   // For _getch
#include "mediaplayer.h"

// Global variables for current track index, elapsed time, and playback status
static int currentTrackIndex = 0;
static int elapsedTime = 0;
static int isPlaying = 1; // 0: Paused, 1: Playing

// Function to format duration in hh:mm:ss
void formatDuration(int durationInSeconds, char *formattedDuration)
{
    int hours = durationInSeconds / 3600;
    int minutes = (durationInSeconds % 3600) / 60;
    int seconds = durationInSeconds % 60;

    snprintf(formattedDuration, 9, "%02d:%02d:%02d", hours, minutes, seconds);
}

// Function to check if a key is pressed
int isKeyPressed()
{
    return _kbhit(); // Returns a non-zero value if a key is pressed
}

void playTrack(const struct Track *playlist)
{
    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("Playing: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    printf("Duration: %s\n", formattedDuration);

    while (elapsedTime < getDurationInSeconds(&playlist[currentTrackIndex]) && isPlaying)
    {
        char formattedElapsedTime[9];
        formatDuration(elapsedTime, formattedElapsedTime);
        printf("Elapsed Time: %s\r", formattedElapsedTime);
        fflush(stdout);

        Sleep(1000); // Wait for 1 second (Windows sleep function)

        // Check for key press and toggle playback
        if (isKeyPressed())
        {
            char key = _getch();
            if (key == '2' || key == 'P') // Check for '2' or 'P' to pause
            {
                pauseTrack(playlist);
            }
            else if (key == '3') // Check for '3' to go to the next track
            {
                nextTrack(playlist);
            }
            else if (key == '4') // Check for '4' to go to the previous track
            {
                prevTrack(playlist);
            }
        }

        elapsedTime++;
    }

    printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time

    // Reset playback status and elapsed time for the next playback
    isPlaying = 1;
    elapsedTime = 0;
    formatDuration(0, formattedDuration);
}

void pauseTrack(const struct Track *playlist)
{
    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("Pausing: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    // printf("Duration: %s\n", formattedDuration);
    // printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time
    isPlaying = 0; // Set playback status to paused

    while (!isPlaying)
    {
        if (isKeyPressed())
        {
            char key = _getch();
            if (key == '1') // Check for '1' to resume playback
            {
                isPlaying = 1;
                break;
            }
        }
    }
}

void nextTrack(const struct Track *playlist)
{
    // Move to the next track
    currentTrackIndex++;

    // Check if we reached the end of the playlist, loop back to the beginning
    if (currentTrackIndex == 3)
    {
        currentTrackIndex = 0;
    }

    // Reset elapsed time for the new track
    elapsedTime = 0;

    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("Switched to next track: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    printf("Duration: %s\n", formattedDuration);
}

void prevTrack(const struct Track *playlist)
{
    // Move to the previous track
    currentTrackIndex--;

    // Check if we reached the beginning of the playlist, loop to the end
    if (currentTrackIndex < 0)
    {
        currentTrackIndex = 2;
    }

    // Reset elapsed time for the new track
    elapsedTime = 0;

    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("Switched to previous track: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    printf("Duration: %s\n", formattedDuration);
}
