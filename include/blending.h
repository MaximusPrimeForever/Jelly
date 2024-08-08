#pragma once
#define RGBA_SIZE (4)
#define GL_CLAMP_TO_EDGE 0x812F

#include "defs.h"
#include <stdint.h>

#include "superglue.h"

typedef struct T_Image
{
    int width;
    int height;
    float opacity;
    unsigned char* image_data;
} Image;

typedef enum e_BlendingMode
{
    ADD = 0,
    SCREEN,
    MULTIPLY,
    NORMAL,
    BLENDING_MODE_COUNT
} BlendingMode;

bool LoadTextureFromStbBuffer(
    IN unsigned char* image_data,
    IN int img_width,
    IN int img_height,
    OUT GLuint* out_texture
);

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

bool BlendTextures(GLuint* in_texture_1, GLuint* in_texture_2, GLuint* out_texture);
