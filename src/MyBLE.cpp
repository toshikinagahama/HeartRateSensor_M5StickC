#include <M5StickC.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "MyBLE.h"
#include "global.h"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    M5.Lcd.println("Connect");
    IsConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    M5.Lcd.println("Disconnect");
    IsConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  //Read時
  void onRead(BLECharacteristic *pCharacteristic)
  {
    M5.Lcd.println("read");
    pCharacteristic->setValue("Hello World!");
  }

  //Write時
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    // M5.Lcd.println("write");
    std::string value = pCharacteristic->getValue();
    if (value == "1111")
    {
      IsMeasStop = false;
    }
    else if (value == "2222")
    {
      IsMeasStop = true;
    }
    else if (value == "3333")
    {
    }
    // Serial.println(value.c_str());
    M5.Lcd.println(value.c_str());
  }
};

// ペアリング処理用
class MySecurity : public BLESecurityCallbacks
{
  bool onConfirmPIN(uint32_t pin)
  {
    return false;
  }

  uint32_t onPassKeyRequest()
  {
    Serial.println("ONPassKeyRequest");
    return 123456;
  }

  void onPassKeyNotify(uint32_t pass_key)
  {
    // ペアリング時のPINの表示
    Serial.println("onPassKeyNotify number");
    Serial.println(pass_key);

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("PIN");
    M5.Lcd.println(pass_key);
  }

  bool onSecurityRequest()
  {
    Serial.println("onSecurityRequest");
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
  {
    Serial.println("onAuthenticationComplete");
    if (cmpl.success)
    {
      // ペアリング完了
      Serial.println("auth success");
    }
    else
    {
      // ペアリング失敗
      Serial.println("auth failed");
    }
  }
};

/**
 * 
 * 初期化
 * 
 */
void MyBLE::initialize()
{

  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
  BLEDevice::setSecurityCallbacks(new MySecurity());
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setKeySize(16);

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
  pSecurity->setCapability(ESP_IO_CAP_OUT);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  uint32_t passkey = 123456;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  pAdvertising = pServer->getAdvertising();
}

/**
 * 
 * アドバタイズスタート
 */
void MyBLE::advertiseStart()
{
  pService->start();
  pAdvertising->start();
}

/**
 * 
 * アドバタイズストップ
 */
void MyBLE::advertiseStop()
{
  pService->stop();
  pAdvertising->stop();
}

/**
 * 
 * notify
 */
void MyBLE::notify(char *val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}