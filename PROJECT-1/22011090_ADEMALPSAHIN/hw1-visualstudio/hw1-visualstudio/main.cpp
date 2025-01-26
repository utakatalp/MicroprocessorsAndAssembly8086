#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

#define pixel_max(a) ((a) <= 255 ? (a) : 255)
#define pixel_min(a) ((a) >= 0 ? (a) : 0)

// Function to read an image in grayscale
unsigned char* readImage(const char* filename, int& width, int& height, int& channels) {
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 1); // Load as grayscale
    if (!image) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    std::cout << "Image loaded successfully!" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << channels << std::endl;
    return image;
}

// Function to write an image to a PNG file
bool writeImage(const char* filename, unsigned char* image, int width, int height) {
    if (!image) {
        std::cerr << "Image data is null before writing!" << std::endl;
        return false;
    }
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions: width = " << width << ", height = " << height << std::endl;
        return false;
    }
    // For grayscale images, stride is the same as the width
    int stride = width;
    if (stbi_write_png(filename, width, height, 1, image, stride) == 0) {
        std::cerr << "Failed to write the image to file: " << filename << std::endl;
        return false;
    }
    std::cout << "Image written successfully to: " << filename << std::endl;
    return true;
}

int main() {
    // Input and output file paths
    const char* inputFilename = "lena.png";
    const char* outputFilename1 = "output_image1.png";
    const char* outputFilename2 = "output_image2.png";

    // Image data variables
    int width, height, channels; // channels = 1 (grayscale)
    unsigned int number_of_pixels;

    // Read the input image
    unsigned char* image = readImage(inputFilename, width, height, channels);
    if (!image) 
        return -1; // Exit if the image failed to load

    // Allocate memory for the output image
    unsigned char* outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

    // image is 1d array 
    // with length = width * height
    // pixels can be used as image[i] 
    // pixels can be updated as image[i] = 100, etc.
    // a pixel is defined as unsigned char
    // so a pixel can be 255 at max, and 0 at min.

    /* -------------------------------------------------------- QUESTION-1 -------------------------------------------------------- */
    
    /* Q-1 Inverse the colors of image. 
    Inverse -> pixel_color = 255 - pixel_color */

    number_of_pixels = width * height;
    
    __asm {

        
                MOV ECX, number_of_pixels

                MOV ESI, image
                MOV EDI, outputImage
                XOR AL, AL 

                outerLoop:                    
                    MOV AL, byte ptr[ESI] // take the index pixel into AL, AL = image[i] 
                    NOT AL            // inverse instead of 255 - pixel_color
                    MOV byte ptr [EDI], AL // store the inversed pixel in outputImage[i]
                    INC ESI // i++
                    INC EDI
                LOOP outerLoop

        // Your assembly code here...
        
    }

    // Write the modified image as output_image1.png
    if (!writeImage(outputFilename1, outputImage, width, height)) {
        stbi_image_free(image);
        return -1;
    }
    stbi_image_free(outputImage); // Clear the outputImage.

    /* -------------------------------------------------------- QUESTION-2 -------------------------------------------------------- */
    /* Histogram Equalization */

    outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

        unsigned int* hist = (unsigned int*)malloc(sizeof(unsigned int) * 256);
        unsigned int* cdf = (unsigned int*)malloc(sizeof(unsigned int) * 256);

        // Check if memory allocation succeeded
        if (hist == NULL) {
            std::cerr << "Memory allocation for hist failed!" << std::endl;
            return -1;
        }
        if (cdf == NULL) {
            std::cerr << "Memory allocation for cdf failed!" << std::endl;
            free(hist);
            return -1;
        }

        // Both hist and cdf are initialized as zeros.
        for (int i = 0; i < 256; i++) {
            hist[i] = 0;
            cdf[i] = 0;
        }

        // You can define new variables here... As a hint some variables are already defined.
        unsigned int min_cdf, range;
        number_of_pixels = width * height;

        // Q-2 (a) - Compute the histogram of the input image.
        __asm {
            // Your assembly code here...
            MOV ECX, number_of_pixels
            MOV ESI, hist
            MOV EDI, image
            XOR EBX, EBX
            L1: 
                
                MOV BL, [EDI] // image pixel's byte value assigned into BL to be used in OFSET
                INC DWORD PTR [ESI + EBX * 4] // Incremented the index in hist array
                INC EDI // i++ for next pixel

            LOOP L1

        }
    printf("Histogram\n");
    for (int i = 0; i < 256; i++) {
        printf("%d - %d\n", i, hist[i]);
    }

    /* Q-2 (b) - Compute the Cumulative Distribution Function cdf
                    and save it to cdf array which is defined above. */

    // CDF Calculation (cdf[i] = cdf[i-1] + hist[i])
    cdf[0] = hist[0]; // initial condition
    __asm {
        // Your assembly code here...
        MOV ESI, cdf
        ADD ESI, 4   //first index is already used
        MOV EDI, hist
        ADD EDI, 4   //first index is already used
        MOV ECX, 256 // loop counter, working with bytes
        DEC ECX // first index is already used
        L2 :
            MOV EAX, [ESI-4] // cdf[i-1]
            ADD EAX, [EDI] // cdf[i-1] + hist[i]
            MOV [ESI], EAX // cdf[i] = cdf[i-1] + hist[i]
            ADD ESI, 4 // i++
            ADD EDI, 4 // i++
            LOOP L2
        
    }
    printf("CDF\n");
    for (int i = 0; i < 256; i++) {
        printf("%d - %d\n", i, cdf[i]);
    }

    /* Q-2 (c) - Normalize the Cumulative Distribution Funtion 
                    that you have just calculated on the same cdf array. */
    int i = 0;
    while (cdf[i] == 0) // find the min cdf value except 0 to be used in range
    {
        i++;
    }
    min_cdf = cdf[i];
    // Normalized cdf[i] = ((cdf[i] - min_cdf) * 255) / range
    range = (number_of_pixels - min_cdf);

    __asm {
        // Your assembly code here...
        MOV ECX, 256 // max index  count of cdf array, working with bytes
        MOV ESI, cdf


    L3:
        MOV EAX, [ESI]   // cdf[i]
        CMP EAX, 0       // condition for preventing - value, if cdf[i] = 0 that be a problem
        JE CON
        SUB EAX, min_cdf // cdf[i]-min_cdf 
        MOV EBX, 255
        MUL EBX          // (cdf[i]-min_cdf) * 255 
        MOV EBX, DWORD PTR range
        DIV EBX   // EAX = (cdf[i]-min_cdf) * 255/ range
        
        
    CON:
        MOV [ESI], EAX // cdf[i] = (cdf[i]-min_cdf) * 255/ range
        ADD ESI, 4

        LOOP L3
        
    }
    printf("Normalized\n");
    for (int i = 0; i < 256; i++) {
        printf("%d - %d\n", i, cdf[i]);
    }

    /* Q-2 (d) - Apply the histogram equalization on the image.
                    Write the new pixels to outputImage. */
	// Here you only need to get a pixel from image, say the value of pixel is 107
	// Then you need to find the corresponding cdf value for that pixel
	// The output for the pixel 107 will be cdf[107]
	// Do this for all the pixels in input image and write on output image.
    __asm {
        // Your assembly code here...
        MOV ESI, cdf
        MOV EDI, image
        MOV EBX, outputImage
        MOV ECX, number_of_pixels
        L5:
            XOR AX, AX
            MOV AL, [EDI] // AL = image[i]
        
            SHL EAX, 2    // AL *=4 AL for working with DWORD

            PUSH EBX // storing EBX 
            XOR EBX, EBX
            MOV EBX, EAX // EDX = EAX (OFSET)
       
            MOV EAX, DWORD PTR [ESI + EBX]  // EAX = cdf[outputImage[i]]
            POP EBX

            MOV [EBX], AL   // outputImage[i]= cdf[outputImage[i]]
            INC EDI         // i++
            INC EBX         // i++
        LOOP L5

    }


    // Write the modified image
    if (!writeImage(outputFilename2, outputImage, width, height)) {
        stbi_image_free(image); 
        return -1;
    }

    // Free the image memory
    stbi_image_free(image);
    stbi_image_free(outputImage);

    return 0;
}
