#include <NeoPixelBus.h>

// uncomment one of these that matches your panel pixel layouts
// rotation is ignored for mosaic as it applies a rotation for you
// that is specific to the location of the panel within the mosaic
// to reduce connection lengths

// typedef ColumnMajorAlternatingLayout MyPanelLayout;
// typedef ColumnMajorLayout MyPanelLayout;
// typedef RowMajorAlternatingLayout MyPanelLayout;
// typedef RowMajorLayout MyPanelLayout;
// typedef RowMajorAlternating270Layout MyPanelLayout;
typedef ColumnMajorAlternating180Layout MyPanelLayout;

// make sure to set these panel values to the sizes of yours
const uint8_t PanelWidth = 5; // 10
const uint8_t PanelHeight = 10; //20
const uint8_t TileWidth = 1; // laid out in 4 panels x 2 panels mosaic
const uint8_t TileHeight = 1;

const uint16_t PixelCount = PanelWidth * PanelHeight * TileWidth * TileHeight;
const uint8_t PixelPin = 2;

const String filename = "/tststpy.bin";

const int32_t timeSize = 4;
const int32_t bufSize = PixelCount * 3;
const int32_t headerSize = bufSize + timeSize;
const int32_t fileSampleRateMs = 10;