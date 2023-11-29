/*********
  Infered from https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  Capstone project related.
  Program is capable of taking pictures and printing the RGB values of the image
*********/

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"     // SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32
// #include "soc/soc.h"           // Disable brownour problems
// #include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h> // read and write from flash memory

#include <WString.h>

#include "input_cam.h"

uint8_t *get_cam()
{
    // Clear the stack
    // Assuming stack size is in words, not bytes
    // const uint32_t stackSize = uxTaskGetStackHighWaterMark(taskHandle);
    // uint32_t *stackStart = (uint32_t *)uxTaskGetStackStart(taskHandle);

    // // Clear the stack
    // memset(stackStart, 0, stackSize * sizeof(uint32_t));

    // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

    // pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    // Serial.println();
    //  esp_log_level_set("*", ESP_LOG_VERBOSE);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound())
    {
        config.frame_size = FRAMESIZE_QQVGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
        config.jpeg_quality = 12;
        config.fb_count = 3;
        config.grab_mode = CAMERA_GRAB_LATEST;
    }
    else
    {
        // config.frame_size = FRAMESIZE_VGA;
        config.frame_size = FRAMESIZE_96X96;
        config.jpeg_quality = 12;
        config.fb_count = 3;
    }

    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        while (true)
            ;
    }
    else
    {
        Serial.println("Camera Initialized");
    }

    // log_i("Initialize SD Card");
    // if (!SD_MMC.begin())
    // {
    //     Serial.println("SD Card Mount Failed");
    //     while (true)
    //         ;
    // }

    // uint8_t cardType = SD_MMC.cardType();
    // if (cardType == CARD_NONE)
    // {
    //     Serial.println("No SD Card attached");
    //     while (true)
    //         ;
    // }
    camera_fb_t *fb = NULL;
    // camera_fb_t *fb = (camera_fb_t *)malloc(sizeof(camera_fb_t));

    Serial.println("FB Initialized");

    // Take Picture with Camera
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        while (true)
            ;
    }
    else
    {
        Serial.println("Taking a picture...");
    }

    // ***************************************************************** EXTRACTING RGB VALUES **************************************************************************
    // Check the rgb data
    Serial.println("Extracting RGB Values...");
    void *ptrVal = NULL;
    uint32_t ARRAY_LENGTH = fb->width * fb->height * 3;
    ptrVal = heap_caps_malloc(ARRAY_LENGTH, MALLOC_CAP_SPIRAM);

    uint8_t *rgb = (uint8_t *)ptrVal;
    bool jpeg_converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb);

    if (!jpeg_converted)
    {
        Serial.println("error: failed to convert image to RGB data");
    }
    else
    {
        Serial.println("The first 10 pixel values are:");
        for (int i = 0; i < 10 * 3; i += 3)
        {
            Serial.print("[");
            Serial.print(String(rgb[i + 2]));
            Serial.print(",");
            Serial.print(String(rgb[i + 1]));
            Serial.print(",");
            Serial.print(String(rgb[i + 0]));
            Serial.println("]");
        }
    }
    esp_camera_fb_return(fb);
    return rgb;
}