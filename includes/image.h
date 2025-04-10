#pragma once

#include "common.h"

typedef struct
{
    const char filename[FILENAME_BUFFER_SIZE];
    const char fileext[EXT_BUFFER_SIZE];
    size_t size;
    const char *mime_type;
    int width;
    int height;
    const char *md5_hash;
} ImageMeta;

typedef struct
{
    void *data;
    ImageMeta meta;
} Image;

int dump_image(const Image *image, char *path);