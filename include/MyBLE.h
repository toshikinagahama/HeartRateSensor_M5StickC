#ifndef MYBLE_H
#define MYBLE_H
//マクロ定義
//UUIDはマクロで定義
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"        //サービスUUID
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" //キャラクタスティックUUID
#define DEVICE_NAME "HeartRate_M5StickC"                           //デバイス名

#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

/**
 * 
 * BLEクラス
 * 
 */
class MyBLE
{

public:
  BLEServer *pServer = NULL;                 //サーバー
  BLEService *pService = NULL;               //サービス
  BLECharacteristic *pCharacteristic = NULL; //キャラクタスティック
  BLEAdvertising *pAdvertising = NULL;       //アドバタイズ
  char id[64];                               //デバイスID

  //関数
  void initialize();
  void advertiseStart();
  void advertiseStop();
  void notify(char *);
};

#endif //MYBLE_H