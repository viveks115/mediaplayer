#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h> // For Windows API functions
#include <conio.h>   // For _kbhit and _getch on Windows
#else
#include <unistd.h>  // For sleep function on Linux
#include <termios.h> // For terminal I/O
#include <fcntl.h>   // For file control options
#endif
#include "mediaplayer.h"

// Global variables for current track index, elapsed time, and playback status
static int currentTrackIndex = 0;
static int elapsedTime = 0;
static int isPlaying = 1; // 0: Paused, 1: Playing
static int shuffleEnabled = 0;
static float playbackSpeed = 1.0;
// Function to format duration in hh:mm:ss
void formatDuration(int durationInSeconds, char *formattedDuration)
{
    int hours = durationInSeconds / 3600;
    int minutes = (durationInSeconds % 3600) / 60;
    int seconds = durationInSeconds % 60;

    snprintf(formattedDuration, 9, "%02d:%02d:%02d", hours, minutes, seconds);
}

// Function to check if a key is pressed
#ifdef _WIN32
int isKeyPressed()
{
    return _kbhit(); // Returns a non-zero value if a key is pressed on Windows
}
#else
int isKeyPressed()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Save current terminal attributes
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Set the terminal to non-blocking mode
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

    // Set the file status flags to non-blocking
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // Restore terminal attributes and file status flags
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

void playTrack(const struct Track *playlist)
{
    char formattedDuration[9];
    int repeatTrack = 0;
    do
    {
        formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);
        printf("\nPlaying: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
        printf("Duration: %s\n", formattedDuration);
        // printf("RepeatTrack=%d\n", repeatTrack);

        while (elapsedTime < getDurationInSeconds(&playlist[currentTrackIndex]) && isPlaying)
        {
            char formattedElapsedTime[9];
            formatDuration(elapsedTime, formattedElapsedTime);
            printf("Repeat=%d Shuffle=%d Elapsed Time: %s\r", repeatTrack, shuffleEnabled, formattedElapsedTime);
            fflush(stdout);
            int sleepDuration = (int)(1000 / playbackSpeed);

#ifdef _WIN32
            Sleep(sleepDuration); // Wait for 1 second (Windows sleep function)
#else
            sleep(1); // Wait for 1 second on Linux
#endif

            // Check for key press and toggle playback
            if (isKeyPressed())
            {
                char key = getchar();
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
                else if (key == 'R' || key == 'r')
                {
                    if (repeatTrack == 0)
                    {
                        // printf("                   Repeat ON");
                        repeatTrack = 1;
                    }
                    else
                    {
                        // printf("                   Repeat OFF");
                        repeatTrack = 0;
                    }
                }
                else if (key == 's' || key == 'S')
                {

                    if (shuffleEnabled == 0)
                    {
                        shuffleEnabled = 1;
                    }
                    else
                    {
                        shuffleEnabled = 0;
                    }
                }
                else if (key == 'x' || 'X')
                {
                    togglePlaybackSpeed();
                }
            }

            elapsedTime++;
        }

        printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time

        // Reset playback status and elapsed time for the next playback
        isPlaying = 1;
        elapsedTime = 0;
        formatDuration(0, formattedDuration);
    } while (repeatTrack);

    if ((currentTrackIndex != NUMBEROFTRACK - 1) || shuffleEnabled == 1)
    {
        nextTrack(playlist);
        playTrack(playlist);
    }
}

void pauseTrack(const struct Track *playlist)
{
    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("\nPausing: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    // printf("Duration: %s\n", formattedDuration);
    // printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time
    isPlaying = 0; // Set playback status to paused

    while (!isPlaying)
    {
        if (isKeyPressed())
        {
           char key = getchar();
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
    srand(time(NULL));
    if (shuffleEnabled)
    {
        int randomIndex = currentTrackIndex;
        while (randomIndex == currentTrackIndex)
        {
            randomIndex = rand() % NUMBEROFTRACK;
        }
        currentTrackIndex = randomIndex;
    }
    else
    {
        // Move to the next track
        currentTrackIndex++;

        // Check if we reached the end of the playlist, loop back to the beginning
        if (currentTrackIndex == NUMBEROFTRACK)
        {
            currentTrackIndex = 0;
        }
    }
    // Reset elapsed time for the new track
    elapsedTime = 0;

    char formattedDuration[9];
    formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

    printf("\nSwitched to next track: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
    printf("Duration: %s\n", formattedDuration);
    printf("Playback Speed: %.2fx\n", playbackSpeed);
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
    printf("Playback Speed: %.2fx\n", playbackSpeed);
}
void togglePlaybackSpeed()
{
    // Toggle between 1x, 1.5x, and 2x speeds
    if (playbackSpeed == 1.0)
    {
        playbackSpeed = 1.5;
    }
    else if (playbackSpeed == 1.5)
    {
        playbackSpeed = 2.0;
    }
    else
    {
        playbackSpeed = 1.0;
    }

    printf("Playback Speed: %.2fx\n", playbackSpeed);
}

///// for Linux

// #include <stdio.h>
// #include <unistd.h>  // For sleep and POSIX API functions
// #include <termios.h> // For terminal I/O
// #include "mediaplayer.h"

// // Global variables for current track index, elapsed time, and playback status
// static int currentTrackIndex = 0;
// static int elapsedTime = 0;
// static int isPlaying = 1; // 0: Paused, 1: Playing

// // Function to format duration in hh:mm:ss
// void formatDuration(int durationInSeconds, char *formattedDuration)
// {
//     int hours = durationInSeconds / 3600;
//     int minutes = (durationInSeconds % 3600) / 60;
//     int seconds = durationInSeconds % 60;

//     snprintf(formattedDuration, 9, "%02d:%02d:%02d", hours, minutes, seconds);
// }

// // Function to check if a key is pressed
// int isKeyPressed()
// {
//     struct termios oldt, newt;
//     int ch;
//     int oldf;

//     // Save current terminal attributes
//     tcgetattr(STDIN_FILENO, &oldt);
//     newt = oldt;

//     // Set the terminal to non-blocking mode
//     newt.c_lflag &= ~(ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//     oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

//     // Set the file status flags to non-blocking
//     fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

//     ch = getchar();

//     // Restore terminal attributes and file status flags
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//     fcntl(STDIN_FILENO, F_SETFL, oldf);

//     if (ch != EOF)
//     {
//         ungetc(ch, stdin);
//         return 1;
//     }

//     return 0;
// }

// void playTrack(const struct Track *playlist)
// {
//     char formattedDuration[9];
//     formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

//     printf("Playing: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
//     printf("Duration: %s\n", formattedDuration);

//     while (elapsedTime < getDurationInSeconds(&playlist[currentTrackIndex]) && isPlaying)
//     {
//         char formattedElapsedTime[9];
//         formatDuration(elapsedTime, formattedElapsedTime);
//         printf("Elapsed Time: %s\r", formattedElapsedTime);
//         fflush(stdout);

//         sleep(1); // Wait for 1 second (Linux sleep function)

//         // Check for key press and toggle playback
//         if (isKeyPressed())
//         {
//             char key = getchar();
//             if (key == '2' || key == 'P') // Check for '2' or 'P' to pause
//             {
//                 pauseTrack(playlist);
//             }
//             else if (key == '3') // Check for '3' to go to the next track
//             {
//                 nextTrack(playlist);
//             }
//             else if (key == '4') // Check for '4' to go to the previous track
//             {
//                 prevTrack(playlist);
//             }
//         }

//         elapsedTime++;
//     }

//     printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time

//     // Reset playback status and elapsed time for the next playback
//     isPlaying = 1;
//     elapsedTime = 0;
//     formatDuration(0, formattedDuration);
// }

// void pauseTrack(const struct Track *playlist)
// {
//     char formattedDuration[9];
//     formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

//     printf("Pausing: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
//     // printf("Duration: %s\n", formattedDuration);
//     // printf("Elapsed Time: %s\n", formattedDuration); // Display final elapsed time
//     isPlaying = 0; // Set playback status to paused

//     while (!isPlaying)
//     {
//         if (isKeyPressed())
//         {
//             char key = getchar();
//             if (key == '1') // Check for '1' to resume playback
//             {
//                 isPlaying = 1;
//                 break;
//             }
//         }
//     }
// }

// void nextTrack(const struct Track *playlist)
// {
//     // Move to the next track
//     currentTrackIndex++;

//     // Check if we reached the end of the playlist, loop back to the beginning
//     if (currentTrackIndex == 3)
//     {
//         currentTrackIndex = 0;
//     }

//     // Reset elapsed time for the new track
//     elapsedTime = 0;

//     char formattedDuration[9];
//     formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

//     printf("Switched to next track: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
//     printf("Duration: %s\n", formattedDuration);
// }

// void prevTrack(const struct Track *playlist)
// {
//     // Move to the previous track
//     currentTrackIndex--;

//     // Check if we reached the beginning of the playlist, loop to the end
//     if (currentTrackIndex < 0)
//     {
//         currentTrackIndex = 2;
//     }

//     // Reset elapsed time for the new track
//     elapsedTime = 0;

//     char formattedDuration[9];
//     formatDuration(getDurationInSeconds(&playlist[currentTrackIndex]), formattedDuration);

//     printf("Switched to previous track: %s (Track ID: %d)\n", getTrackName(&playlist[currentTrackIndex]), getTrackId(&playlist[currentTrackIndex]));
//     printf("Duration: %s\n", formattedDuration);
// }
