// main.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "track.h"
#include "mediaplayer.h"

// Function to handle user input
void *handleUserInput(void *arg)
{
    struct Track *playlist = (struct Track *)arg;

    char choice;

    do
    {
        printf("\n1. Play\n2. Pause\n3. Next\n4. Previous\n5. Exit\n");
        printf("tools: R= Repeat, X= Speed , S= Shuffle\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
            playTrack(playlist);
            break;
        case '2':
            pauseTrack(playlist);
            break;
        case '3':
            nextTrack(playlist);
            break;
        case '4':
            prevTrack(playlist);
            break;
        case '5':
            printf("Exiting the media player.\n");
            break;
        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }

    } while (choice != '5');

    pthread_exit(NULL);
}

int main()
{
    // Create a playlist with three tracks
    struct Track playlist[NUMBEROFTRACK]; // NUMBEROFTRACK defined  in track.h

    // Initialize tracks
    for (int i = 0; i < NUMBEROFTRACK; i++)
    {
        char result[9];
        setTrackId(&playlist[i], (i + 1));
        sprintf(result, "Track %d", (i + 1));
        setTrackName(&playlist[i], result);
        setDurationInSeconds(&playlist[i], (i + 1) * 10);
    }
    // // setTrackId(&playlist[0], 1);
    // setTrackName(&playlist[0], "Track 1");
    // setDurationInSeconds(&playlist[0], 10);

    // setTrackId(&playlist[1], 2);
    // setTrackName(&playlist[1], "Track 2");
    // setDurationInSeconds(&playlist[1], 15);

    // setTrackId(&playlist[2], 3);
    // setTrackName(&playlist[2], "Track 3");
    // setDurationInSeconds(&playlist[2], 12);

    // Playback thread
    pthread_t playbackThread;

    // Start handling user input in a separate thread
    // pthread_create(&thread, NULL, FunctionName, NULL);
    pthread_create(&playbackThread, NULL, handleUserInput, (void *)playlist);

    // Wait for the playback thread to finish
    pthread_join(playbackThread, NULL);

    return 0;
}
