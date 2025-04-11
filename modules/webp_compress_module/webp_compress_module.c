#include <webp/encode.h>
#include <webp/decode.h>

#include "module.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Define module config
Module ModuleConfig = {
    .mversion = MODULE_MANAGER_VERSION,
    .name = "webp_compress_module",
    .type = MODULE_TYPE_MID,
    .version = "1.0.0",
};

__attribute__((constructor)) void _module_init()
{
    // optional
}

__attribute__((destructor)) void _module_exit()
{
    // define your cleanup function
    // called before module exit
    return;
}

__attribute__((unused)) ModuleMsg _module_call(ModuleMsg *input)
{
    if (input->input_type != MODULE_INP_TYPE_STR)
    {
        return (ModuleMsg){
            .input_type = MODULE_INP_TYPE_INT,
            .data.i = 0,
            .status = MODULE_STATUS_ERR,
            .opt_msg = "Invalid input type - expected string path",
        };
    }

    module_printf("Processing image: %s", input->data.str);

    // Check file extension
    const char *ext = strrchr(input->data.str, '.');
    if (!ext)
    {
        return (ModuleMsg){
            .input_type = MODULE_INP_TYPE_INT,
            .data.i = 0,
            .status = MODULE_STATUS_ERR,
            .opt_msg = "File has no extension",
        };
    }

    // Handle WebP format
    if (strcasecmp(ext, ".webp") == 0)
    {
        // Read the WebP file
        FILE *file = fopen(input->data.str, "rb");
        if (!file)
        {
            return (ModuleMsg){
                .input_type = MODULE_INP_TYPE_INT,
                .data.i = 0,
                .status = MODULE_STATUS_WARN,
                .opt_msg = "Failed to open WebP file",
            };
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        uint8_t *file_data = (uint8_t *)malloc(file_size);
        if (!file_data)
        {
            fclose(file);
            return (ModuleMsg){
                .input_type = MODULE_INP_TYPE_INT,
                .data.i = 0,
                .status = MODULE_STATUS_WARN,
                .opt_msg = "Memory allocation failed",
            };
        }

        if (fread(file_data, 1, file_size, file) != (size_t)file_size)
        {
            free(file_data);
            fclose(file);
            return (ModuleMsg){
                .input_type = MODULE_INP_TYPE_INT,
                .data.i = 0,
                .status = MODULE_STATUS_WARN,
                .opt_msg = "Failed to read WebP data",
            };
        }
        fclose(file);

        // Decode WebP
        int width, height;
        uint8_t *image_data = WebPDecodeRGB(file_data, file_size, &width, &height);
        free(file_data);

        if (!image_data)
        {
            return (ModuleMsg){
                .input_type = MODULE_INP_TYPE_INT,
                .data.i = 0,
                .status = MODULE_STATUS_WARN,
                .opt_msg = "Failed to decode WebP image",
            };
        }

        return (ModuleMsg){
            .input_type = MODULE_INP_TYPE_PTR,
            .data.ptr = image_data,
            .status = MODULE_STATUS_OK,
            .opt_msg = "",
        };
    }
    // Handle JPEG/PNG formats using stb_image
    else if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0 || strcasecmp(ext, ".png") == 0)
    {
        int width, height, channels;
        uint8_t *image_data = stbi_load(input->data.str, &width, &height, &channels, 4); // Force 4 channels (RGBA)

        if (!image_data)
        {
            ModuleMsg msg = (ModuleMsg){
                .input_type = MODULE_INP_TYPE_INT,
                .data.i = 0,
                .status = MODULE_STATUS_WARN,
            };

            strncpy(msg.opt_msg, stbi_failure_reason(), ModuleInputBufMax);
        }

        // Convert to WebP format in memory if needed
        // (This example just returns the raw image data)
        return (ModuleMsg){
            .input_type = MODULE_INP_TYPE_PTR,
            .data.ptr = image_data,
            .status = MODULE_STATUS_OK,
            .opt_msg = "",
        };
    }
    else
    {
        return (ModuleMsg){
            .input_type = MODULE_INP_TYPE_INT,
            .data.i = 0,
            .status = MODULE_STATUS_WARN,
            .opt_msg = "Unsupported image format",
        };
    }
}
