#include <catch2/catch_test_macros.hpp>

#include "geBitmapWriter.h"

using namespace geEngineSDK;

#pragma pack(push, 1)
struct BMPFileHeader
{
  uint16 bfType;      // 'BM'
  uint32 bfSize;
  uint16 bfReserved1;
  uint16 bfReserved2;
  uint32 bfOffBits;   // 54
};

struct BMPInfoHeader
{
  uint32 biSize;          // 40
  int32  biWidth;
  int32  biHeight;        // + => bottom-up
  uint16 biPlanes;        // 1
  uint16 biBitCount;      // 24/32
  uint32 biCompression;   // 0 (BI_RGB)
  uint32 biSizeImage;
  int32  biXPelsPerMeter; // 3780 (96 DPI)
  int32  biYPelsPerMeter; // 3780 (96 DPI)
  uint32 biClrUsed;
  uint32 biClrImportant;
};
#pragma pack(pop)

static_assert(sizeof(BMPFileHeader) == 14, "BMPFileHeader must be 14 bytes");
static_assert(sizeof(BMPInfoHeader) == 40, "BMPInfoHeader must be 40 bytes");

static inline const BMPFileHeader*
asFileH(const Vector<uint8>& buf) {
  REQUIRE(buf.size() >= sizeof(BMPFileHeader));
  return reinterpret_cast<const BMPFileHeader*>(buf.data());
}

static inline const BMPInfoHeader*
asInfoH(const Vector<uint8>& buf) {
  REQUIRE(buf.size() >= sizeof(BMPFileHeader) + sizeof(BMPInfoHeader));
  return reinterpret_cast<const BMPInfoHeader*>(buf.data() + sizeof(BMPFileHeader));
}

static inline const uint8*
pixelData(const Vector<uint8>& buf) {
  auto* fh = asFileH(buf);
  REQUIRE(buf.size() >= fh->bfOffBits);
  return buf.data() + fh->bfOffBits;
}

static inline uint32
calcRowPitch(uint32 width, uint32 bytesPerPixel) {
  const uint32 raw = width * bytesPerPixel;
  const uint32 pad = (4u - (raw & 3u)) & 3u;
  return raw + pad;
}

TEST_CASE("BitmapWriter::rawPixelsToBMP - writes valid headers (96 DPI = 3780)") {
  BitmapWriter w;

  const uint32 W = 3, H = 2;
  const uint32 srcBpp = 3; // RGB
  const bool force32 = false;

  Vector<uint8> src(W * H * srcBpp, 0x7F);
  const uint32 expectedSize = w.getBMPSize(W, H, srcBpp, force32);

  Vector<uint8> out(expectedSize, 0);
  const uint32 written =
    w.rawPixelsToBMP(src.data(), W, H, srcBpp, out.data(), (uint32)out.size(),
      PIXEL_ORDER::RGB, force32);

  REQUIRE(written == expectedSize);

  const auto* fh = asFileH(out);
  const auto* ih = asInfoH(out);

  REQUIRE(fh->bfType == 0x4D42); // 'BM'
  REQUIRE(fh->bfOffBits == 54);
  REQUIRE(fh->bfSize == expectedSize);
  REQUIRE(fh->bfReserved1 == 0);
  REQUIRE(fh->bfReserved2 == 0);

  REQUIRE(ih->biSize == 40);
  REQUIRE(ih->biWidth == (int32)W);
  REQUIRE(ih->biHeight == (int32)H);
  REQUIRE(ih->biPlanes == 1);
  REQUIRE(ih->biCompression == 0);

  // 24-bit expected (srcBpp=3, no force32)
  REQUIRE(ih->biBitCount == 24);

  // 96 DPI
  REQUIRE(ih->biXPelsPerMeter == 3780);
  REQUIRE(ih->biYPelsPerMeter == 3780);
}

TEST_CASE("BitmapWriter::rawPixelsToBMP - 24-bit padding is correct for odd widths") {
  BitmapWriter w;

  // width=1 => row bytes = 3, padding=1, rowPitch=4
  const uint32 W = 1, H = 2;
  const uint32 srcBpp = 3; // RGB
  const uint32 outBpp = 3; // expect 24-bit
  const uint32 rowPitch = calcRowPitch(W, outBpp);

  // Two pixels in RGB, top row then bottom row (in memory)
  // top:   (R=10,G=20,B=30)
  // bottom:(R=40,G=50,B=60)
  Vector<uint8> src = {
    10, 20, 30,
    40, 50, 60
  };

  const uint32 size = w.getBMPSize(W, H, srcBpp, false);
  Vector<uint8> out(size, 0xCD);

  const uint32 written =
    w.rawPixelsToBMP(src.data(), W, H, srcBpp, out.data(), (uint32)out.size(),
      PIXEL_ORDER::RGB, false);

  REQUIRE(written == size);

  const auto* ih = asInfoH(out);
  REQUIRE(ih->biBitCount == 24);

  const uint8* px = pixelData(out);

  // Bottom-up: first row in file = bottom row from src
  // Pixel stored as BGR in BMP.
  // File row 0 (bottom): B=60,G=50,R=40 plus padding 0
  REQUIRE(px[0] == 60);
  REQUIRE(px[1] == 50);
  REQUIRE(px[2] == 40);
  REQUIRE(px[3] == 0); // padding byte must be zeroed

  // File row 1 (top): B=30,G=20,R=10 plus padding 0
  const uint8* row1 = px + rowPitch;
  REQUIRE(row1[0] == 30);
  REQUIRE(row1[1] == 20);
  REQUIRE(row1[2] == 10);
  REQUIRE(row1[3] == 0);
}

TEST_CASE("BitmapWriter::rawPixelsToBMP - 32-bit BGRA output preserves alpha (no padding)") {
  BitmapWriter w;

  const uint32 W = 2, H = 2;
  const uint32 srcBpp = 4; // RGBA -> should become 32-bit
  const bool force32 = false;

  // src memory order: row0(top), row1(bottom)
  // top-left    RGBA = (1,2,3,4)
  // top-right   RGBA = (5,6,7,8)
  // bottom-left RGBA = (9,10,11,12)
  // bottom-rightRGBA = (13,14,15,16)
  Vector<uint8> src = {
    1,2,3,4,   5,6,7,8,
    9,10,11,12, 13,14,15,16
  };

  const uint32 size = w.getBMPSize(W, H, srcBpp, force32);
  Vector<uint8> out(size, 0);

  const uint32 written =
    w.rawPixelsToBMP(src.data(), W, H, srcBpp, out.data(), (uint32)out.size(),
      PIXEL_ORDER::RGBA, force32);

  REQUIRE(written == size);

  const auto* ih = asInfoH(out);
  REQUIRE(ih->biBitCount == 32);

  const uint32 outBpp = 4;
  const uint32 rowPitch = calcRowPitch(W, outBpp);
  REQUIRE(rowPitch == W * outBpp); // 2*4=8 => aligned already

  const uint8* px = pixelData(out);

  // Bottom-up: first row in file corresponds to bottom row in src:
  // bottom-left RGBA(9,10,11,12) => BGRA(11,10,9,12)
  REQUIRE(px[0] == 11);
  REQUIRE(px[1] == 10);
  REQUIRE(px[2] == 9);
  REQUIRE(px[3] == 12);

  // bottom-right RGBA(13,14,15,16) => BGRA(15,14,13,16)
  REQUIRE(px[4] == 15);
  REQUIRE(px[5] == 14);
  REQUIRE(px[6] == 13);
  REQUIRE(px[7] == 16);

  // Second row in file corresponds to top row in src:
  const uint8* row1 = px + rowPitch;

  // top-left RGBA(1,2,3,4) => BGRA(3,2,1,4)
  REQUIRE(row1[0] == 3);
  REQUIRE(row1[1] == 2);
  REQUIRE(row1[2] == 1);
  REQUIRE(row1[3] == 4);

  // top-right RGBA(5,6,7,8) => BGRA(7,6,5,8)
  REQUIRE(row1[4] == 7);
  REQUIRE(row1[5] == 6);
  REQUIRE(row1[6] == 5);
  REQUIRE(row1[7] == 8);
}

TEST_CASE("BitmapWriter::rawPixelsToBMP - returns 0 when dstCapacity is too small") {
  BitmapWriter w;

  const uint32 W = 4, H = 4;
  const uint32 srcBpp = 4;

  Vector<uint8> src(W * H * srcBpp, 0xAA);
  const uint32 need = w.getBMPSize(W, H, srcBpp, false);

  Vector<uint8> out(need - 1, 0); // intentionally too small
  const uint32 written =
    w.rawPixelsToBMP(src.data(), W, H, srcBpp, out.data(), (uint32)out.size(),
      PIXEL_ORDER::RGBA, false);

  REQUIRE(written == 0);
}

TEST_CASE("BitmapWriter::getBMPSize matches rawPixelsToBMP written bytes") {
  BitmapWriter w;

  const uint32 W = 17, H = 9;
  const uint32 srcBpp = 3;

  Vector<uint8> src(W * H * srcBpp, 123);

  const uint32 size = w.getBMPSize(W, H, srcBpp, false);
  Vector<uint8> out(size, 0);

  const uint32 written =
    w.rawPixelsToBMP(src.data(), W, H, srcBpp, out.data(), (uint32)out.size(),
      PIXEL_ORDER::RGB, false);

  REQUIRE(written == size);

  // sanity: biSizeImage matches expected pitch * height
  const auto* ih = asInfoH(out);
  const uint32 rowPitch = calcRowPitch(W, 3);
  REQUIRE(ih->biSizeImage == rowPitch * H);
}
