/*
 GadgetBle.h - Library for providing sensor data via Bluetooth.
 Created by Bjoern Muntwyler, November 7, 2020.
 Released under BSD-3 licence
 */
#ifndef Sensirion_GadgetBle_Lib_h
#define Sensirion_GadgetBle_Lib_h

#include "Arduino.h"

#include "esp_timer.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// BLE Characteristics and Service Uuids

static const char* const DOWNLOAD_SERVICE_UUID =
    "00008000-b38d-4985-720e-0f993a68ee41";
static const char* const LOGGER_INTERVAL_UUID =
    "00008001-b38d-4985-720e-0f993a68ee41";
static const char* const SAMPLE_CNT_CHAR_UUID =
    "00008002-b38d-4985-720e-0f993a68ee41";
static const char* const TRANSFER_NOTIFY_UUID =
    "00008004-b38d-4985-720e-0f993a68ee41";

// BLE Protocol Specifics

// TODO: Change this name to something more generic
static const char* const GADGET_NAME = "MyCO2";
static const size_t DOWNLOAD_PKT_SIZE = 20;
static const size_t MAX_SAMPLE_SIZE = 8; // TODO: Adapt depending on data type
static const size_t SAMPLE_BUFFER_SIZE_BYTES = 60000;

class GadgetBle {
  public:
    // CAUTION when adapting! GadgetBle::getPositionInSample will need
    // adjustment too!
    enum DataType {
        T_RH_V3,  // not fully supported yet
        T_RH_V4,  // not fully supported yet
        T_RH_CO2, // not fully supported yet
        T_RH_CO2_ALT,
        T_RH_CO2_PM25
    };
    enum Unit { T, RH, CO2, PM2P5 };
    explicit GadgetBle(DataType dataType);
    void begin();
    void writeTemperature(float temperature);
    void writeHumidity(float humidity);
    void writeCO2(float co2);
    void writePM2p5(float pm2p5);
    void commit();
    void handleEvents();
    String getDeviceIdString() {
        return _deviceIdString;
    }

  private:
    void _bleInit();
    int _getPositionInSample(Unit unit);
    void _writeValue(int convertedValue, Unit unit);
    void _updateAdvertising();
    void _updateConnectionState();
    bool _handleDownload();
    uint16_t _computeBufferSize();
    void _addCurrentSampleToHistory();
    int _computeRealSampleBufferSize();

    DataType _dataType;
    int _sampleSize;
    int _sampleCntPerPacket;
    uint16_t _sampleTypeDL;
    uint8_t _advSampleType;
    uint8_t _sampleTypeAdv;
    int _sampleBufferSize;

    BLEAdvertising* _bleAdvertising;
    BLE2902* _transferDescr;
    BLECharacteristic* _transferChar;
    BLECharacteristic* _sampleCntChar;

    String _deviceIdString;

    int64_t _lastCacheTime = 0;

    std::array<uint8_t, MAX_SAMPLE_SIZE> _currentSample = {};
    std::array<uint8_t, SAMPLE_BUFFER_SIZE_BYTES> _sampleBuffer = {};
    uint16_t _downloadSeqNumber = 0;
    bool _downloading = false;
};

#endif
