#if 1
#include <TJpg_Decoder.h>
#include "SPI.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include "elink3.h"

SPISettings spi_settings(10000000, MSBFIRST, SPI_MODE0);

uint16_t jpgWidth, jpgHeight; // 记录当前JPG的宽高
uint8_t (*bmp8)[16 + 1];      // 创建像素抖动缓存二维数组（先不定长度），JPG最大的输出区块16+1行缓存（缓存上一次的最后一行）
uint16_t blockCount_x = 0;    // x轴区块计数
boolean FirstLineJitterStatus = 1;  // 首行抖动状态 1-可以抖动 0-已抖动过
boolean getXYstate = 1;             // 获取绘制像素点XY像素初始坐标
int16_t x_p = 0;                    // 绘制像素点的x轴坐标
int16_t y_p = 0;                    // 绘制像素点的y轴坐标
int16_t x_start;                    // 绘制像素点的x轴坐标初始值记录
int16_t y_start;                    // 绘制像素点的y轴坐标初始值记录
uint8_t ScreenOrientation = 0;      // 屏幕方向

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
#define baise  GxEPD_WHITE  //白色
#define heise  GxEPD_BLACK  //黑色
//#define ENABLE_GxEPD2_GFX 1

//GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=D8*/ 7, /*DC=D3*/ 6, /*RST=D4*/ 10, /*BUSY=D2*/ 11));
//GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=*/ 7, /*DC=*/ 6, /*RST=*/ 10, /*BUSY=*/ 11));
//GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=*/ 7, /*DC=*/ 6, /*RST=*/ 10, /*BUSY=*/ 11));  //合宙
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(GxEPD2_154_Z90c(/*CS=*/ 4, /*DC=*/ 6, /*RST=*/ 7, /*BUSY=*/ 8));


#include "../.pio/libdeps/esp32-c3-devkitm-1/Adafruit GFX Library/Fonts/FreeSansBold24pt7b.h"
//#include "../.pio/libdeps/esp32-c3-devkitm-1/Adafruit GFX Library/Fonts/FreeSansBold40pt7b.h"

#include "../.pio/libdeps/esp32-c3-devkitm-1/Adafruit GFX Library/Fonts/FreeSansBold9pt7b.h"

void elink3_setup()
{
  Serial.println("\n\n 测试TJpg_Decoder库");
  Serial.println("\n\n 测试墨水屏显示，抖动显示");

  //屏幕初始化
  SPI.end();
  //SPI.begin(2, 4, 3, 7);  //合宙
  SPI.begin(3, 9, 5, 4);
  display.init(115200);

    Serial.println("enter elink_hello_world");
    display.setRotation(2);
    display.firstPage();

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSansBold24pt7b);

  //display.init(0, 0, 10, 1); // 串口使能 初始化完全刷新使能 复位时间 ret上拉使能

  u8g2Fonts.begin(display);             // 将u8g2过程连接到Adafruit GFX
  u8g2Fonts.setFontMode(1);             // 使用u8g2透明模式（这是默认设置）
  u8g2Fonts.setFontDirection(0);        // 从左到右（这是默认设置）
  u8g2Fonts.setForegroundColor(heise);  // 设置前景色
  u8g2Fonts.setBackgroundColor(baise);  // 设置背景色
  u8g2Fonts.setFont(u8g2_font_t0_15_tf);

  //display.setRotation(ScreenOrientation); // 设置方向
  //display.firstPage();
  
  do{
      //display.setCursor(32, 100);
      display.setCursor(0, 150);
      display.print("12:56");
      display.display();
  } while (display.nextPage());
  Serial.print("elink_hello_world");

  // jpeg图像可以按1、2、4或8的比例缩放
  //TJpgDec.setJpgScale(1);
  //TJpgDec.setSwapBytes(0);//我们需要交换颜色字节（字节次序 ）

  //提供回调函数，jpg输出流会在回调函数里输出，图像处理也在这里
  //TJpgDec.setCallback(epd_output);

  //SPIFFS.begin(true);
  //Serial.println("\r\n初始化已完成.");

}

void elink_display_int(int16_t x, int16_t y, int i)
{
  display.setFont(&FreeSansBold9pt7b);
  do{
      //display.setCursor(32, 100);
      display.setCursor(x, y);
      display.print(i);

      display.setCursor(10, 20);
      display.print(123);

      display.display();
  } while (display.nextPage());
  Serial.print("elink_hello_world");
}


void elink3_loop()
{
  fs::File root;
  root = SPIFFS.open(String("/"), "r");
  //loadFile("/111.jpg");
  //File root = LittleFS.open("/", "r");
#if 1
  while (File file = root.openNextFile())
  {
    String strname = file.name();
    strname = "/" + strname;
    Serial.println(file.name());
    // If it is not a directory and filename ends in .jpg then load it
    if (!file.isDirectory() && (strname.endsWith(".jpg") || strname.endsWith(".jpeg")))
    {
      //clearScreen();//清空屏幕
      uint32_t t = millis();
      loadFile(strname); //绘制
      t = millis() - t;
      Serial.print(t); Serial.println(" ms");
      Serial.println("");
      delay(2000);
    }
  }
  display.hibernate(); //彻底关闭屏幕电源
  Serial.print("\n休眠\n");
  ESP.deepSleep(0);
#endif
}

void loadFile(String name)
{
  //数值初始化
  FirstLineJitterStatus = 1;  // 第一行抖动状态
  getXYstate = 1;             // 获取绘制像素点XY像素初始坐标
  blockCount_x = 0;           // X轴区块计数
  jpgWidth = 0;
  jpgHeight = 0;

  Serial.printf("name=%s", name);
  //获取jpeg的宽度和高度（以像素为单位）
  TJpgDec.getFsJpgSize(&jpgWidth, &jpgHeight, name);
  //jpgWidth = 160;  jpgHeight = 160;
  Serial.print("jpgWidth = "); Serial.print(jpgWidth); Serial.print(", jpgHeight = "); Serial.println(jpgHeight);

  //设置屏幕方向
  display.setRotation(ScreenOrientation); // 用户方向
  //if (jpgWidth != jpgHeight) display.setRotation(jpgWidth > jpgHeight ? 3 : 0);

  //设置缩放 1-2-4-8
  uint16_t scale = 1;
  for (scale = 1; scale <= 8; scale <<= 1)
  {
    if (jpgWidth <= display.width() * scale && jpgHeight <= display.height() * scale)
    {
      if (scale > 1)
      {
        scale = scale >> 1; // 屏幕太小，缩得比屏幕小就看不清了，回到上一个缩放
      }
      break;
    }
  }
  if (scale > 8) scale = 8; //至多8倍缩放
  TJpgDec.setJpgScale(scale);

  Serial.print("scale:"); Serial.println(scale);

  // 重新计算缩放后的长宽
  jpgWidth = jpgWidth / scale;
  jpgHeight = jpgHeight / scale;

  //创建指定长度的二维数值，必须在drawFsJpg之前,getFsJpgSize和重新计算缩放后的长宽之后
  bmp8 = new uint8_t[jpgWidth][16 + 1];

  //自动居中
  int32_t x_center = (display.width() / 2) - (jpgWidth / 2);
  int32_t y_center = (display.height() / 2) - (jpgHeight / 2);
  Serial.print("x_center:"); Serial.println(x_center);
  Serial.print("y_center:"); Serial.println(y_center);

  display.init(0, 0, 10, 1);
  display.setFullWindow();
  display.firstPage();
  do
  {
    uint8_t error;
    error = TJpgDec.drawFsJpg(x_center, y_center, name); // 发送文件和坐标
    String str = "";
    if (error == 1)      str = "Interrupted by output function";                    // 被输出功能中断
    else if (error == 2) str = "Device error or wrong termination of input stream"; // 设备错误或输入流的错误终止
    else if (error == 3) str = "Insufficient memory pool for the image";            // 映像的内存池不足
    else if (error == 4) str = "Insufficient stream input buffer";                  // 流输入缓冲区不足
    else if (error == 5) str = "Parameter error";                                   // 参数错误
    else if (error == 6) str = "Data format error (may be broken data)";            // 数据格式错误（可能是损坏的数据）
    else if (error == 7) str = "Right format but not supported";                    // 格式正确但不受支持
    else if (error == 8) str = "Not supported JPEG standard";                       // 不支持JPEG标准
    if (error != 0)
    {
      display_partialLine(3, str);
      display_partialLine(5, name);
    }
    Serial.println("error:" + String(error) + " " + str);
    Serial.println("");
  } while (display.nextPage());

  //display.powerOff(); //关闭屏幕电源

  delete[] bmp8;//释放内存
}

void clearScreen() //清空屏幕
{
  display.init(0, 0, 10, 1);
  display.setFullWindow();
  display.firstPage();
  display.nextPage();
}
uint16_t getCenterX(String z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z.c_str());  //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);    //计算字符居中的X位置
  return x;
}
uint16_t getCenterX(const char* z) //计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z);        //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);  //计算字符居中的X位置
  return x;
}
uint16_t getCenterX(int32_t z) //计算字符居中的X位置
{
  String zf = String (z);
  uint16_t zf_width = u8g2Fonts.getUTF8Width(zf.c_str());  //获取字符的像素长度
  uint16_t x = (display.width() / 2) - (zf_width / 2);    //计算字符居中的X位置
  return x;
}

/*
  display_partialLine()
  发送局部刷新的显示信息到屏幕,带居中
  line        行数（0-7）
  zf          字符内容
  lineRefresh 整行刷新 1-是 0-仅刷新字符长度的区域
*/
void display_partialLine(uint8_t line, String zf) //发送局部刷新的显示信息到屏幕,带居中
{
  display.setPartialWindow(0, line * 16, display.width(), 16);   // 整行刷新
  display.firstPage();
  do
  {
    display.fillScreen(baise);
    u8g2Fonts.setCursor(getCenterX(zf), line * 16 + 13);
    u8g2Fonts.print(zf);
  }
  while (display.nextPage());

  display.powerOff(); //关闭屏幕电源
  // display.hibernate();
}

bool epd_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  //Serial.print("x:"); Serial.println(x);
  //Serial.print("y:"); Serial.println(y);
  //Serial.print("w:"); Serial.println(w);
  //Serial.print("h:"); Serial.println(h);
  // Serial.println(" ");

  yield();
  // 绘制像素点的 x y从哪里开始
  if (getXYstate)
  {
    getXYstate = 0;
    x_start = x;
    y_start = y;
    x_p = x;
    y_p = y;
  }

  for (int16_t j = 0; j < h; j++, y++) //y轴
  {
    int16_t x1 = abs(x_start - x); //计算bmp8的x坐标
    int16_t y1 = j; //计算bmp8的y坐标
    if (FirstLineJitterStatus == 0) y1 += 1; //第一次之后从1行开始

    for (int16_t i = 0; i < w; i++, x1++) //x轴
    {
      uint32_t xh = j * w + i;
      uint8_t num = bitmap[xh];
      bmp8[x1][y1] = num;
    }
  }

  //y轴区块计数
  blockCount_x += w;
  //**** 区块已达到x轴边界，开始抖动和绘制图像
  if (blockCount_x >= jpgWidth)
  {
    blockCount_x = 0;

    //**** 抖动
    int err;
    uint8_t y_max; //抖动多少行，第一次0123456 之后12345678
    if (FirstLineJitterStatus) y_max = h - 2;  //首次0123456
    else                       y_max = h - 1;  //非首次01234567

    //到了最后一行吧剩余的行也一起抖动
    if (y == jpgHeight + y_start) y_max = h - 1;

    for (uint16_t y = 0; y <= y_max; y++) // height width
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
      {
        if (bmp8[x][y] > 127) {
          err = bmp8[x][y] - 255;
          bmp8[x][y] = 255;
        } else {
          err = bmp8[x][y] - 0;
          bmp8[x][y] = 0;
        }
        if (x != jpgWidth - 1)  bmp8[x + 1][y + 0] = colorThresholdLimit(bmp8[x + 1][y + 0] , (err * 7) / 16);
        if (x != 0)             bmp8[x - 1][y + 1] = colorThresholdLimit(bmp8[x - 1][y + 1] , (err * 3) / 16);
        if (1)                  bmp8[x + 0][y + 1] = colorThresholdLimit(bmp8[x + 0][y + 1] , (err * 5) / 16);
        if (x != jpgWidth - 1)  bmp8[x + 1][y + 1] = colorThresholdLimit(bmp8[x + 1][y + 1] , (err * 1) / 16);
      }
    }//像素抖动结束

    uint16_t y_p_max; // 绘制多少行
    if (FirstLineJitterStatus) y_p_max = h - 2; // 首次只到6行 0123456
    else                       y_p_max = h - 1;  // 8-1=7  01234567

    //到了最后一行吧剩余的行也一起绘制
    if (y == jpgHeight + y_start)
    {
      y_p_max += (jpgHeight + y_start - 1) - (y_p + y_p_max); //127-（119+7）
    }

    for (uint16_t y1 = 0; y1 <= y_p_max; y1++, y_p++)
    {
      x_p = x_start; //回到初始位置
      for (uint16_t x1 = 0; x1 < jpgWidth; x1++, x_p++)
      {
        display.drawPixel(x_p, y_p, bmp8[x1][y1]);
        //Serial.printf("bmp8 = %d", bmp8[x1][y1]);
      }
    }

    //倒数第1行移动到第1行
    if (FirstLineJitterStatus) //第一次
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
        bmp8[x][0] = bmp8[x][h - 1];
    }
    else //第一次之后
    {
      for (uint16_t x = 0; x < jpgWidth; x++)
        bmp8[x][0] = bmp8[x][h];
    }
    FirstLineJitterStatus = 0; //第一次抖动结束
  }
  // 返回1以解码下一个块
  return 1;
}

//val1附近的像素，val2误差
uint8_t colorThresholdLimit(uint8_t val1, int8_t val2) // 颜色阈值限制
{
  int16_t val1_int = val1;
  int16_t val2_int = val2;
  int16_t tmp = val1_int + val2_int;
  if (tmp > 255) return 255;
  else if (tmp < 0) return 0;
  else return tmp;
  return 0;
}

#endif