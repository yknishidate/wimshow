# wimshow

A tiny header-only imshow library for Windows

## Usage

```cpp
#include "wimshow.h"

int main()
{
    int32_t width = 600;
    int32_t height = 400;
    uint8_t* image = new uint8_t[width * height * 3];
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            int32_t offset = (y * width + x) * 3;
            image[offset + 0] = uint8_t(x / float(width) * 255);
            image[offset + 1] = uint8_t(y / float(height) * 255);
            image[offset + 2] = uint8_t(0);
        }
    }

    wis::showImage(image, width, height);
    wis::waitClose();
}
```
