#ifndef Bitmap__cpp
#define Bitmap__cpp

#include <stdint.h>
#include <stdio.h>

//
// Struct
//

#pragma pack(push, 1)
struct Bitmap_File_Header {
  uint16_t type = 'MB';
  uint32_t size = 0;
  uint16_t reserved1 = 0;
  uint16_t reserved2 = 0;
  uint32_t offset_bits = 0;
};

struct Ciexyz {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct Ciexyz_Triple {
  Ciexyz ciexyz_red;
  Ciexyz ciexyz_green;
  Ciexyz ciexyz_blue;
};

struct Bitmap_Info_Header {
  uint32_t      size             = 0;
  int32_t       width            = 0;
  int32_t       height           = 0;
  uint16_t      planes           = 1;
  uint16_t      bit_count        = 24;
  uint32_t      compression      = 0; // BI_RGB
  uint32_t      size_image       = 0;
  int32_t       XPelsPerMeter    = 3780;
  int32_t       YPelsPerMeter    = 3780;
  uint32_t      colour_used      = 0;
  uint32_t      colour_important = 0;
};
#pragma pack(pop)


struct Bitmap_Data {
    Bitmap_File_Header header;
    Bitmap_Info_Header header_v5;
    uint8_t *data = nullptr;
};




//
// Load from disc
//

static bool write_bmp_to_disc(uint32_t width, uint32_t height, uint8_t *input, char const *path_and_name) {
    //
    // Open file
    FILE *file_handle = nullptr;
    {
        errno_t error = fopen_s(&file_handle, path_and_name, "w+b");
        if (!file_handle) {
            // TODO: handle error
            char buffer[512];
            _snprintf_s(buffer, 512, _TRUNCATE, "%s inn %s, failed in creating a new file", __FUNCTION__, __FILE__);
            perror(buffer);
            error;
            return false;
        }
    }


    //
    // Write headers
    uint32_t row_size = 3 * width; // We only support 24-bit BMP
    uint32_t padding = 4 - (row_size % 4); // Every row in the bitmap must be aligned to DWORD (4-byte)
    uint32_t padded_row_size = 3 * width + padding;

    uint32_t data_size = padded_row_size * height;

    {
        Bitmap_File_Header header {};
        header.offset_bits = sizeof(Bitmap_File_Header) + sizeof(Bitmap_Info_Header);
        header.size = header.offset_bits + data_size;

        size_t written = fwrite(&header, sizeof(Bitmap_File_Header), 1, file_handle);
        if (written != 1) {
            // TODO: handle error
            char buffer[512];
            _snprintf_s(buffer, 512, _TRUNCATE, "%s inn %s, failed in writing the Bitmap_File_Header", __FUNCTION__, __FILE__);
            perror(buffer);

            return false;
        }

        Bitmap_Info_Header bi {};
        bi.size = sizeof(Bitmap_Info_Header);
        bi.width = width;
        bi.height = -static_cast<int32_t>(height); // Top-down, i.e. the origin is in the upper-left corner
        bi.size_image = data_size;

        written = fwrite(&bi, sizeof(Bitmap_Info_Header), 1, file_handle);
        if (written != 1) {
            // TODO: handle error
            char buffer[512];
            _snprintf_s(buffer, 512, _TRUNCATE, "%s inn %s, failed in writing the Bitmap_v5_Header", __FUNCTION__, __FILE__);
            perror(buffer);

            return false;
        }
    }


    //
    // Write data
    {
        struct Pixel {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
        };

        #define kColour
        #ifdef kColour
        Pixel constexpr colour[] {
            #if 0
            {255, 255, 255},
            { 15, 100, 100},
            {  0, 170, 170},
            { 35, 255, 255},
            #else
            {  0,   0,   0},
            {  0, 167, 150},
            {170,   8, 118},
            {  0, 214, 255},
            #endif
        };
        #endif

        uint8_t *data = new uint8_t[data_size];
        uint32_t src_index = 0;
        uint32_t dst_index = 0;

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                uint8_t value = input[src_index++];
                #ifdef kColour
                *reinterpret_cast<Pixel *>(data + dst_index) = colour[value];
                #else
                Pixel *p = reinterpret_cast<Pixel *>(data + dst_index);
                p->red   = 85 * value;
                p->green = 85 * value;
                p->blue  = 85 * value;
                #endif
                dst_index += sizeof(Pixel);
            }
            dst_index += padding;
        }

        size_t written = fwrite(data, data_size, 1, file_handle);
        if (written != 1) {
            // TODO: handle error
            char buffer[512];
            _snprintf_s(buffer, 512, _TRUNCATE, "%s inn %s, failed in writing the data", __FUNCTION__, __FILE__);
            perror(buffer);

            return false;
        }

        delete[] data;
    }

    fclose(file_handle);

    return true;
}




//
// Read from disc
//

static bool load_bmp_from_disc(char const *path_and_name, Bitmap_Data *output) {
    FILE *file_handle = nullptr;
    errno_t error = fopen_s(&file_handle, path_and_name, "rb");
    if (error) {
        // TODO: handle error
        char buffer[512];
        _snprintf_s(buffer, 512, _TRUNCATE, "%s inn %s, failed in opening the file %s", __FUNCTION__, __FILE__, path_and_name);
        perror(buffer);
        error;
    }
    else {
        fseek(file_handle, 0, SEEK_END);
        size_t size = ftell(file_handle);
        fseek(file_handle, 0, SEEK_SET);

        uint8_t *data = new uint8_t[size];
        size_t read = fread(data, size, 1, file_handle);
        if(read == 1) {
            size_t s16 = sizeof(uint16_t);
            size_t s32 = sizeof(uint32_t);
            size_t sa = sizeof(Bitmap_File_Header);
            size_t sb = sizeof(Bitmap_Info_Header);

            s16;
            s32;
            sa;
            sb;

            Bitmap_File_Header *header = reinterpret_cast<Bitmap_File_Header *>(data);
            Bitmap_Info_Header *info   = reinterpret_cast<Bitmap_Info_Header *>(data + sizeof(Bitmap_File_Header));

            int a = 0;
            ++a;

            header;
            info;
        }

        if (data)  delete[] data;
    }

    return true;
}

#endif