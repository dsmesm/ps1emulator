//
// Created by Dima on 9/8/2020.
//

#ifndef PS1EMULATOR_GPU_H
#define PS1EMULATOR_GPU_H


#include <unordered_map>
#include <queue>

struct Gpu {
    Gpu();

    std::vector<std::vector<unsigned short>> vram = std::vector(1024, std::vector<unsigned short>(512));

    std::unordered_map<unsigned int, std::tuple<void(Gpu::*)(unsigned int), unsigned int>> gp0Opcodes;
    std::unordered_map<unsigned int, void(Gpu::*)(unsigned int)> gp1Opcodes;

    std::queue<unsigned int> gpuBuffer;
    std::queue<unsigned int> gpuReadBuffer;

    bool loadingImage = false;
    std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> imageProps = std::tuple(0,0,0,0);
    unsigned int horCount = 0;
    unsigned int verCount = 0;
    unsigned int wordsToLoad = 0;

    unsigned int texturePageX_Base = 0;
    unsigned int texturePageY_Base = 0;
    unsigned int semiTransparencyMode = 0;
    unsigned int texturePageColors = 0;
    unsigned int ditheringMode = 0;
    unsigned int drawingToDisplay = 0;
    unsigned int maskBit = 0;
    unsigned int drawPixels = 0;
    unsigned int interlaceField = 1;
    unsigned int reverseFlag = 0;
    unsigned int textureDisable = 0;
    unsigned int horizontalResolution2 = 0;
    unsigned int horizontalResolution1 = 0;
    unsigned int verticalResolution = 0;
    unsigned int videoMode = 0;
    unsigned int colorDepth = 0;
    unsigned int verticalInterlace = 0;
    unsigned int displayEnable = 1;
    unsigned int interruptRequest = 0;
    unsigned int dmaRequest = 0;
    unsigned int readyToReceiveCommand = 1;
    unsigned int readyToReceiveDma = 1;
    unsigned int readyToSendVram = 1;
    unsigned int dmaDirection = 0;
    unsigned int interlaceLines = 0;

    unsigned int texturedRectangleX_Flip = 0;
    unsigned int texturedRectangleY_Flip = 0;

    unsigned int displayStartVramX = 0;
    unsigned int displayStartVramY = 0;

    unsigned int displayHorizontalStart = 0;
    unsigned int displayHorizontalEnd = 0;
    unsigned int displayVerticalStart = 0;
    unsigned int displayVerticalEnd = 0;

    unsigned int textureWindowMaskX = 0;
    unsigned int textureWindowMaskY = 0;
    unsigned int textureWindowOffsetX = 0;
    unsigned int textureWindowOffsetY = 0;

    unsigned int drawingAreaTopX = 0;
    unsigned int drawingAreaTopY = 0;
    unsigned int drawingAreaBottomX = 0;
    unsigned int drawingAreaBottomY = 0;

    unsigned int drawingOffsetX = 0; //both offset values are signed
    unsigned int drawingOffsetY = 0;

    unsigned int getGpuStat();
    unsigned int gpuRead = 0;

    void setDmaDirection(unsigned int value);

    void parseGp0Opcode(unsigned int data);
    void parseGp1Opcode(unsigned int data);

    void setDrawMode(unsigned int instruction);
    void setDisplayMode(unsigned int instruction);
    void setDma(unsigned int instruction);
    void nop(unsigned int instruction);
    void setDrawingAreaTop(unsigned int instruction);
    void setDrawingAreaBottom(unsigned int instruction);
    void setDrawingOffset(unsigned int instruction);
    void setTextureWindow(unsigned int instruction);
    void maskBitSet(unsigned int instruction);
    void drawMonochromeQuadrOpaque(unsigned int instruction);
    void drawShadedQuadrOpaque(unsigned int instruction);
    void drawShadedTriOpaque(unsigned int instruction);
    void clearCache(unsigned int instruction);
    void loadImageFromCpu(unsigned int instruction);
    void loadImageToCpu(unsigned int instruction);
    void drawTexturedQuadrOpaqueBlended(unsigned int instruction);
    void fillRectangle(unsigned int instruction);

    void resetGpu(unsigned int instruction);
    void setDisplayStartVram(unsigned int instruction);
    void setHorizontalDisplayRange(unsigned int instruction);
    void setVerticalDisplayRange(unsigned int instruction);
    void setDisplayEnable(unsigned int instruction);
    void acknowledgeGpuInterrupt(unsigned int instruction);
    void resetCommandBuffer(unsigned int instruction);
};


#endif //PS1EMULATOR_GPU_H
