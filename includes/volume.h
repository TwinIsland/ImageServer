// #pragma once

// #include <stdlib.h>
// #include <stdio.h>

// #include "image.h"
// #include "common.h"
// #include "CONFIG.h"

// typedef enum
// {
//     VOL_TYPE_PERSISTENT, // pVol - Disk, long-term storage
//     VOL_TYPE_EPHEMERAL,  // eVol - Disk, cache-like
//     VOL_TYPE_MEMORY      // mVol - RAM, fast ephemeral storage
// } VolumeType;

// typedef struct
// {
//     // pVol
//     char pVolDirPath[PATH_BUFFER_SIZE];
//     size_t pVolSizeLimit;
//     int pVolImageNLimit;

//     // eVol
//     char eVolDirPath[PATH_BUFFER_SIZE];
//     size_t eVolSizeLimit;
//     int eVolImageNLimit;

//     // mVol
//     size_t mVolSizeLimit;
//     int mVolImageNLimit;
// } VolumeConfig;

// // to disable linter warning
// typedef struct VolumeEntry
// {
//     int64_t img_id;
//     ImageMeta meta;
//     union
//     {
//         char disk[PATH_BUFFER_SIZE];
//         void *mem_ptr;
//     } path;
//     char impath[PATH_BUFFER_SIZE];
//     struct VolumeEntry *next;
//     struct VolumeEntry *prev;
// } VolumeEntry;

// typedef struct
// {
//     VolumeEntry *entries;
//     size_t total_size;
//     int entry_count;
//     int last_id;
// } VolumeList;

// extern __thread char volume_err[512];

// // init all three volume
// // return ok or not, err msg in volumeerr
// int volume_init(const VolumeConfig *vol_cfg);
// void volume_cleanup();

// // write volume, return the image id added, -1 if failed
// int volume_write(VolumeType type, const Image *img, int64_t original_id, const char *processing_params, sqlite3 *db);
// int volume_delete(const char *impath);

// // // Advanced operations
// // ImageMeta volume_move(VolumeType dest, VolumeType src, int image_id);
// // ImageMeta volume_copy(VolumeType dest, VolumeType src, int image_id);

// // // Management API
// // size_t volume_usage(VolumeType type);
// // size_t volume_cfg(VolumeType type);

// int volume_mkimpath(VolumeType type, const char *filename, int image_id, char *dest);