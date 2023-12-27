#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "cJSON.h"
#include "track.h"
#include "mediaplayer.h"

#define FILENAME "playlist.json" // Replace with your JSON file name
int totalTracks = 0;
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
    // Initialize Jansson
    cJSON *root = NULL;
    cJSON *playlistArray = NULL;
    cJSON *trackObj = NULL;

    // Open the JSON file
    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening JSON file.\n");
        return 1;
    }

    // Read the contents of the file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    fread(fileContent, 1, fileSize, file);
    fclose(file);

    // Parse the JSON content
    root = cJSON_Parse(fileContent);
    free(fileContent);

    if (!root)
    {
        fprintf(stderr, "Error parsing JSON file.\n");
        return 1;
    }

    // Get the "Playlist" array from the JSON file
    playlistArray = cJSON_GetObjectItemCaseSensitive(root, "Playlist");
    if (!cJSON_IsArray(playlistArray))
    {
        fprintf(stderr, "Error: Playlist is not an array.\n");
        cJSON_Delete(root);
        return 1;
    }

    // Create a playlist with the number of tracks in the array
    size_t numTracks = cJSON_GetArraySize(playlistArray);
    totalTracks = numTracks;
    struct Track *playlist = (struct Track *)malloc(numTracks * sizeof(struct Track));

    // Initialize tracks from JSON
    for (size_t i = 0; i < numTracks; i++)
    {
        trackObj = cJSON_GetArrayItem(playlistArray, i);

        // Get TrackID
        cJSON *trackIdJson = cJSON_GetObjectItemCaseSensitive(trackObj, "TrackID");
        if (!cJSON_IsNumber(trackIdJson))
        {
            fprintf(stderr, "Error: TrackID is not a number.\n");
            cJSON_Delete(root);
            free(playlist);
            return 1;
        }
        int trackId = trackIdJson->valueint;
        setTrackId(&playlist[i], trackId);

        // Get TrackName
        cJSON *trackNameJson = cJSON_GetObjectItemCaseSensitive(trackObj, "TrackName");
        if (!cJSON_IsString(trackNameJson))
        {
            fprintf(stderr, "Error: TrackName is not a string.\n");
            cJSON_Delete(root);
            free(playlist);
            return 1;
        }
        const char *trackName = trackNameJson->valuestring;
        setTrackName(&playlist[i], trackName);

        // Get Duration
        cJSON *durationJson = cJSON_GetObjectItemCaseSensitive(trackObj, "Duration");
        if (!cJSON_IsNumber(durationJson))
        {
            fprintf(stderr, "Error: Duration is not a number.\n");
            cJSON_Delete(root);
            free(playlist);
            return 1;
        }
        int duration = durationJson->valueint;
        setDurationInSeconds(&playlist[i], duration);
    }

    // Close the JSON file and release resources
    cJSON_Delete(root);

    // Playback thread
    pthread_t playbackThread;

    // Start handling user input in a separate thread
    pthread_create(&playbackThread, NULL, handleUserInput, (void *)playlist);

    // Wait for the playback thread to finish
    pthread_join(playbackThread, NULL);

    // Free allocated memory
    free(playlist);

    return 0;
}
