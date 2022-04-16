#include "func.h"
#include <M5StickC.h>

int8_t show_battery_info()
{
  // バッテリー電圧表示
  // GetVbatData()の戻り値はバッテリー電圧のステップ数で、
  // AXP192のデータシートによると1ステップは1.1mV
  float vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  float current = M5.Axp.GetBatCurrent();
  if (current * current < 1.0)
  {
    current = M5.Axp.GetVBusCurrent();
  }

  // バッテリー残量表示
  // 簡易的に、線形で4.2Vで100%、3.0Vで0%とする
  int8_t bat_charge_p = int8_t((vbat - 3.0) / 1.2 * 100);
  if (bat_charge_p > 100)
  {
    bat_charge_p = 100;
  }
  else if (bat_charge_p < 0)
  {
    bat_charge_p = 0;
  }
  M5.Lcd.printf("Volt: %.2fV\n", vbat);
  M5.Lcd.printf("Charge: %3d%%\n", bat_charge_p);
  M5.Lcd.printf("Current: %.2fmA\n", current);
  // return bat_charge_p;
}