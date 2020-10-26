#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Dima on 9/8/2020.
//

#include "Gpu.h"
#include "Renderer.h"


unsigned int Gpu::getGpuStat() {
    unsigned int registerValue = 0;

    registerValue |= texturePageX_Base;
    registerValue |= texturePageY_Base << 4;
    registerValue |= (unsigned int) semiTransparencyMode << 5;
    registerValue |= (unsigned int) texturePageColors << 7;
    registerValue |= (unsigned int) ditheringMode << 9;
    registerValue |= (unsigned int) drawingToDisplay << 10;
    registerValue |= (unsigned int) maskBit << 11;
    registerValue |= (unsigned int) drawPixels << 12;
    registerValue |= (unsigned int) interlaceField << 13;
    registerValue |= (unsigned int) reverseFlag << 14;
    registerValue |= (unsigned int) textureDisable << 15;
    registerValue |= (unsigned int) horizontalResolution2 << 16;
    registerValue |= (unsigned int) horizontalResolution1 << 17;
    registerValue |= (unsigned int) verticalResolution << 19;
    //registerValue |= (unsigned int) 0 << 19;
    registerValue |= (unsigned int) videoMode << 20;
    registerValue |= (unsigned int) colorDepth << 21;
    registerValue |= (unsigned int) verticalInterlace << 22;
    registerValue |= (unsigned int) displayEnable << 23;
    registerValue |= (unsigned int) interruptRequest << 24;
    registerValue |= dmaRequest << 25;
    registerValue |= (unsigned int) readyToReceiveCommand << 26;
    registerValue |= (unsigned int) readyToSendVram << 27;
    registerValue |= (unsigned int) readyToReceiveDma << 28;
    registerValue |= (unsigned int) dmaDirection << 29;
    registerValue |= (unsigned int) interlaceLines << 31;

    return registerValue;
}

void Gpu::setDmaDirection(unsigned int value) {
    dmaDirection = value;
    switch (value)
    {
        case 0:
            dmaRequest = 0;
            break;
        case 1:
            dmaRequest = 1; // depends on FIFO state
            break;
        case 2:
            dmaRequest = readyToReceiveDma;
            break;
        case 3:
            dmaRequest = readyToSendVram;
            break;
    }
}

void Gpu::parseGp0Opcode(unsigned int data) {
    loadingImage &= wordsToLoad != 0;

    if (loadingImage)
    {

        if (horCount + 1 < std::get<2>(imageProps))
        {
            vram[std::get<0>(imageProps) + horCount][ std::get<1>(imageProps) + verCount] = (unsigned short)data;
            horCount++;
            vram[std::get<0>(imageProps) + horCount][ std::get<1>(imageProps) + verCount] = (unsigned short)(data >> 16);
            horCount++;
        }
        else
        {
            horCount = 0;
            verCount++;

            vram[std::get<0>(imageProps) + horCount][ std::get<1>(imageProps) + verCount] = (unsigned short)data;
            horCount++;
            vram[std::get<0>(imageProps) + horCount][ std::get<1>(imageProps) + verCount] = (unsigned short)(data >> 16);
            horCount++;
        }
        wordsToLoad--;
        if (wordsToLoad == 0)
        {
            imageProps = std::tuple(0, 0, 0, 0);
            horCount = 0;
            verCount = 0;
        }
    }
    else
    {
        auto tuple = gp0Opcodes[(data & 0xff000000) >> 24];
        (this->*std::get<0>(tuple))(data);
    }
}

Gpu::Gpu() {
    gp0Opcodes = {
            {0xe1, std::tuple(&Gpu::setDrawMode, 1)},
            {0x00, std::tuple(&Gpu::nop, 1)},
            {0xe3, std::tuple(&Gpu::setDrawingAreaTop, 1)},
            {0xe4, std::tuple(&Gpu::setDrawingAreaBottom, 1)},
            {0xe5, std::tuple(&Gpu::setDrawingOffset, 1)},
            {0xe2, std::tuple(&Gpu::setTextureWindow, 1)},
            {0xe6, std::tuple(&Gpu::maskBitSet, 1)},
            {0x28, std::tuple(&Gpu::drawMonochromeQuadrOpaque, 5)},
            {0x38, std::tuple(&Gpu::drawShadedQuadrOpaque, 8)},
            {0x30, std::tuple(&Gpu::drawShadedTriOpaque, 6)},
            {0x2c, std::tuple(&Gpu::drawTexturedQuadrOpaqueBlended, 9)},
            {0x01, std::tuple(&Gpu::clearCache, 1)},
            {0xa0, std::tuple(&Gpu::loadImageFromCpu, 3)},
            {0xc0, std::tuple(&Gpu::loadImageToCpu, 3)},
            {0x02, std::tuple(&Gpu::fillRectangle, 3)},
    };

    gp1Opcodes ={
            {0x00, &Gpu::resetGpu},
            {0x08, &Gpu::setDisplayMode},
            {0x04, &Gpu::setDma},
            {0x05, &Gpu::setDisplayStartVram},
            {0x06, &Gpu::setHorizontalDisplayRange},
            {0x07, &Gpu::setVerticalDisplayRange},
            {0x03, &Gpu::setDisplayEnable},
            {0x02, &Gpu::acknowledgeGpuInterrupt},
            {0x01, &Gpu::resetCommandBuffer},
    };
}

void Gpu::setDrawMode(unsigned int instruction) {
    texturePageX_Base = instruction & 0xf;
    texturePageY_Base = (instruction & 0x10) >> 4;
    semiTransparencyMode = (instruction & 0x60) >> 5;
    texturePageColors = (instruction & 0x180) >> 7;
    ditheringMode = (instruction & 0x200) >> 9;
    drawingToDisplay = (instruction & 0x400) >> 10;
    //drawingToDisplay = 1;
    textureDisable = (instruction & 0x800) >> 11;

    texturedRectangleX_Flip = (instruction & 0x1000) >> 12;
    texturedRectangleY_Flip = (instruction & 0x2000) >> 13;
}

void Gpu::parseGp1Opcode(unsigned int data) {
    (this->*gp1Opcodes[(data & 0xff000000) >> 24])(data);
}

void Gpu::resetGpu(unsigned int instruction) {
    //clear fifo
    interruptRequest = 0;
    displayEnable = 1;
    setDmaDirection(0);

    displayStartVramX = 0;
    displayStartVramY = 0;

    displayHorizontalStart = 0x200;
    displayHorizontalEnd = 0xc00;
    displayVerticalStart = 0x10;
    displayVerticalEnd = 0x100;

    setDisplayMode(0);

    setDrawMode(0);

    textureWindowMaskX = 0;
    textureWindowMaskY = 0;
    textureWindowOffsetX = 0;
    textureWindowOffsetY = 0;

    drawingAreaTopX = 0;
    drawingAreaTopY = 0;
    drawingAreaBottomX = 0;
    drawingAreaBottomY = 0;

    drawingOffsetX = 0;
    drawingOffsetY = 0;

    maskBit = 0;
    drawPixels = 0;
}

void Gpu::setDisplayMode(unsigned int instruction) {
    horizontalResolution1 = instruction & 0x3;
    verticalResolution = (instruction & 0x4) >> 2;
    videoMode = (instruction & 0x8) >> 3;
    colorDepth = (instruction & 0x10) >> 4;
    verticalInterlace = (instruction & 0x20) >> 5;
    horizontalResolution2 = (instruction & 0x40) >> 6;
    reverseFlag = (instruction & 0x80) >> 7;
}

void Gpu::setDma(unsigned int instruction) {
    setDmaDirection(instruction & 0x3);
}

void Gpu::nop(unsigned int instruction) {
    int c = 0;
}

void Gpu::setDrawingAreaTop(unsigned int instruction) {
    drawingAreaTopX = instruction & 0x3ff;
    drawingAreaTopY = (instruction & 0x3fc00) >> 10;
}

void Gpu::setDrawingAreaBottom(unsigned int instruction) {
    drawingAreaBottomX = instruction & 0x3ff;
    drawingAreaBottomY = (instruction & 0x3fc00) >> 10;
}

void Gpu::setDrawingOffset(unsigned int instruction) {
    drawingOffsetX = instruction & 0x7ff;
    drawingOffsetY = (instruction >> 11) & 0x7ff;
    //_drawing = true;
    Renderer::draw();
    Renderer::verticesCount = 0;
    //vertBuffer = new uint[MaxBufferLength];
}

void Gpu::setTextureWindow(unsigned int instruction) {
    textureWindowMaskX = instruction & 0x1f;
    textureWindowMaskY = (instruction >> 5) & 0x1f;
    textureWindowOffsetX = (instruction >> 10) & 0x1f;
    textureWindowOffsetY = (instruction >> 15) & 0x1f;
}

void Gpu::maskBitSet(unsigned int instruction) {
    maskBit = instruction & 0x1;
    drawPixels = (instruction >> 1) & 0x1;
}

void Gpu::setDisplayStartVram(unsigned int instruction) {
    displayStartVramX = instruction & 0x3ff;
    displayStartVramY = (instruction >> 10) & 0x3ff;
}

void Gpu::setHorizontalDisplayRange(unsigned int instruction) {
    displayHorizontalStart = instruction & 0xfff;
    displayHorizontalEnd = (instruction >> 12) & 0xfff;
}

void Gpu::setVerticalDisplayRange(unsigned int instruction) {
    displayVerticalStart = instruction & 0x3ff;
    displayVerticalEnd = (instruction >> 10) & 0x3ff;
}

void Gpu::drawMonochromeQuadrOpaque(unsigned int instruction) {
    auto color = instruction & 0xffffff;
    auto vertex1 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex4 = gpuBuffer.front();
    gpuBuffer.pop();

    std::vector<unsigned int> vertices =
                {
                    vertex1 & 0xffff, vertex1 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    vertex2 & 0xffff, vertex2 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    vertex3 & 0xffff, vertex3 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    vertex2 & 0xffff, vertex2 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    vertex3 & 0xffff, vertex3 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    vertex4 & 0xffff, vertex4 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                };

    if (Renderer::verticesCount + 6 > 1024*128)
    {
        Renderer::draw();
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        Renderer::vertBuffer[(15 * Renderer::verticesCount / 3) + i] = vertices[i];
    }

    Renderer::verticesCount += 6;

    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);
}

void Gpu::setDisplayEnable(unsigned int instruction) {
    displayEnable = instruction & 0x1;
}

void Gpu::drawShadedQuadrOpaque(unsigned int instruction) {
    auto color1 = instruction & 0xffffff;
    auto vertex1 = gpuBuffer.front();
    gpuBuffer.pop();
    auto color2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto color3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto color4 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex4 = gpuBuffer.front();
    gpuBuffer.pop();

    std::vector<unsigned int> vertices =
                   {
                        vertex1 & 0xffff, vertex1 >> 16, color1 & 0xff, (color1 >> 8) & 0xff, (color1 >> 16) & 0xff,
                        vertex2 & 0xffff, vertex2 >> 16, color2 & 0xff, (color2 >> 8) & 0xff, (color2 >> 16) & 0xff,
                        vertex3 & 0xffff, vertex3 >> 16, color3 & 0xff, (color3 >> 8) & 0xff, (color3 >> 16) & 0xff,
                        vertex2 & 0xffff, vertex2 >> 16, color2 & 0xff, (color2 >> 8) & 0xff, (color2 >> 16) & 0xff,
                        vertex3 & 0xffff, vertex3 >> 16, color3 & 0xff, (color3 >> 8) & 0xff, (color3 >> 16) & 0xff,
                        vertex4 & 0xffff, vertex4 >> 16, color4 & 0xff, (color4 >> 8) & 0xff, (color4 >> 16) & 0xff,
                   };

    if (Renderer::verticesCount + 6 > 1024*128)
    {
        Renderer::draw();
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        Renderer::vertBuffer[(15 * Renderer::verticesCount / 3) + i] = vertices[i];
    }

    Renderer::verticesCount += 6;

    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);
}

void Gpu::drawShadedTriOpaque(unsigned int instruction) {
    auto color1 = instruction & 0xffffff;
    auto vertex1 = gpuBuffer.front();
    gpuBuffer.pop();
    auto color2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto color3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex3 = gpuBuffer.front();
    gpuBuffer.pop();

    std::vector<unsigned int> vertices =
                   {
                        vertex1 & 0xffff, vertex1 >> 16, color1 & 0xff, (color1 >> 8) & 0xff, (color1 >> 16) & 0xff,
                        vertex2 & 0xffff, vertex2 >> 16, color2 & 0xff, (color2 >> 8) & 0xff, (color2 >> 16) & 0xff,
                        vertex3 & 0xffff, vertex3 >> 16, color3 & 0xff, (color3 >> 8) & 0xff, (color3 >> 16) & 0xff,
                   };

    if (Renderer::verticesCount + 3 > 1024 * 128)
    {
        Renderer::draw();
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        Renderer::vertBuffer[(15 * Renderer::verticesCount / 3) + i] = vertices[i];
    }

    Renderer::verticesCount += 3;

    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);
}

void Gpu::acknowledgeGpuInterrupt(unsigned int instruction) {
    interruptRequest = 0;
}

void Gpu::resetCommandBuffer(unsigned int instruction) {
    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);

    wordsToLoad = 0;
    loadingImage = false;
}

void Gpu::clearCache(unsigned int instruction) {
    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);

    wordsToLoad = 0;
    loadingImage = false;
}

void Gpu::loadImageToCpu(unsigned int instruction) {
    auto source = gpuBuffer.front();
    gpuBuffer.pop();
    auto sourceX = source & 0xffff;
    auto sourceY = (source >> 16) & 0xffff;

    auto resolution = gpuBuffer.front();
    gpuBuffer.pop();
    auto width = resolution & 0xffff;
    auto height = (resolution >> 16) & 0xffff;

    auto imageSize = width * height;
    imageSize += imageSize % 2;

    auto wordCount = imageSize / 2;


    auto i = 0;

    for (int v = 0;v < height;){
        for (int h = 0;h < width;){
            gpuReadBuffer.push(vram[sourceX + h][sourceY + v] | vram[sourceX + h + 1][sourceY + v]);
            h = h + 2;
            i++;
        }
        v++;
    }

    int c = 0;
}

void Gpu::loadImageFromCpu(unsigned int instruction) {
    auto destination = gpuBuffer.front();
    gpuBuffer.pop();
    auto destinationX = destination & 0xffff;
    auto destinationY = (destination >> 16) & 0xffff;

    auto resolution = gpuBuffer.front();
    gpuBuffer.pop();
    auto width = resolution & 0xffff;
    auto height = (resolution >> 16) & 0xffff;

    auto imageSize = width * height;
    imageSize += imageSize % 2;

    imageProps = std::tuple(destinationX, destinationY, width, height);

    auto wordCount = imageSize / 2;
    wordsToLoad = wordCount;
    loadingImage = true;
}

void Gpu::drawTexturedQuadrOpaqueBlended(unsigned int instruction) {
    auto color = instruction & 0xffffff;
    auto vertex1 = gpuBuffer.front();
    gpuBuffer.pop();
    auto texCoord1andPalette = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex2 = gpuBuffer.front();
    gpuBuffer.pop();
    auto texCoord2andTexpage = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto texCoord3 = gpuBuffer.front();
    gpuBuffer.pop();
    auto vertex4 = gpuBuffer.front();
    gpuBuffer.pop();
    auto texCoord4 = gpuBuffer.front();
    gpuBuffer.pop();

    auto texPageInfo = (texCoord2andTexpage & 0xffff0000) >> 16;
    auto paletteInfo = (texCoord1andPalette & 0xffff0000) >> 16;

    auto texPageX = texPageInfo & 0xf;
    auto texPageY = (texPageInfo >> 4) & 0x1;
    auto semiTransparency = (texPageInfo >> 5) & 0x3;
    auto colorMode = (texPageInfo >> 7) & 0x3;

    auto paletteX = paletteInfo & 0xff;
    auto paletteY = (paletteInfo >> 8) & 0xff;

    /*if (colorMode != 2)
    {
        if (colorMode == 0)
        {
            var clut = new ushort[16];
            for (var i = 0; i < 16; i++)
            {
                clut[i] = vram[paletteX + i, paletteY * 4];
            }
        }
        else
        {
            throw new NotImplementedException();
        }
    }*/

    std::vector<unsigned int> vertices =
                   {
                           vertex1 & 0xffff, vertex1 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                           vertex2 & 0xffff, vertex2 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                           vertex3 & 0xffff, vertex3 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                           vertex2 & 0xffff, vertex2 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                           vertex3 & 0xffff, vertex3 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                           vertex4 & 0xffff, vertex4 >> 16, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                   };

    if (Renderer::verticesCount + 6 > 1024*128)
    {
        Renderer::draw();
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        Renderer::vertBuffer[(15 * Renderer::verticesCount / 3) + i] = vertices[i];
    }

    Renderer::verticesCount += 6;


    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);
}

void Gpu::fillRectangle(unsigned int instruction) {
    auto color = instruction & 0xffffff;

    auto topRight = gpuBuffer.front();
    gpuBuffer.pop();
    auto topRightX = topRight & 0xffff;
    auto topRightY = (topRight >> 16) & 0xffff;

    auto resolution = gpuBuffer.front();
    gpuBuffer.pop();
    auto width = resolution & 0xffff;
    auto height = (resolution >> 16) & 0xffff;

    int c = 0;

    std::vector<unsigned int> vertices =
            {
                    topRightX, topRightY, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    topRightX + width, topRightY, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    topRightX, topRightY + height, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    topRightX + width, topRightY, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    topRightX, topRightY + height, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
                    topRightX + width, topRightY + height, color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff,
            };

    if (Renderer::verticesCount + 6 > 1024*128)
    {
        Renderer::draw();
    }

    for (int i = 0; i < vertices.size(); i++)
    {
        Renderer::vertBuffer[(15 * Renderer::verticesCount / 3) + i] = vertices[i];
    }

    Renderer::verticesCount += 6;

    //glDrawArrays(GL_QUADS)

    std::queue<unsigned int> empty;
    std::swap(gpuBuffer, empty);
    //throw std::exception();
}


#pragma clang diagnostic pop