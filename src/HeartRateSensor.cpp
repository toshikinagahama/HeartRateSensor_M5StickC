#include "HeartRateSensor.h"
#include <M5StickC.h>
#include "global.h"
#include "MAX30100_PulseOximeter.h"

/**
 * 
 * 初期化
 * 
 */
void HeartRateSensor::initialize()
{

  if (!max30100.begin())
  {
    Serial.println("FAILED to open max30100 sensor");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS to open max30100 sensor");
  }
  max30100.setMode(MAX30100_MODE_SPO2_HR);
  max30100.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
  max30100.setLedsPulseWidth(PULSE_WIDTH);
  max30100.setSamplingRate(SAMPLING_RATE);
  max30100.setHighresModeEnabled(HIGHRES_MODE);
  irDCRemover = DCRemover(DC_REMOVER_ALPHA);
  redDCRemover = DCRemover(DC_REMOVER_ALPHA);
  cnt = 0;
}

/**
 * 
 * 測定リセット
 */
void HeartRateSensor::resetMeas()
{
  cnt = 0;
}

/**
 * 
 * 測定
 */

void HeartRateSensor::getValues()
{
  //サンプリング100Hz想定
  uint16_t tmp_ir_raw;
  uint16_t tmp_red_raw;
  // Heart_rate = (int)pox.getHeartRate();
  // Serial.printf("%d\r\n", Heart_rate);
  max30100.update();
  while (max30100.getRawValues(&tmp_ir_raw, &tmp_red_raw))
  {
    //値が0ならおかしいので前回値に修正。
    if (tmp_ir_raw == 0)
      ir_raw = ir_raw_last;
    else
      ir_raw = tmp_ir_raw;
    if (tmp_red_raw == 0)
      red_raw = red_raw_last;
    else
      red_raw = tmp_red_raw;

    //ローパス
    ir_lpf = lpf.step((float)(ir_raw));
    red_lpf = lpf.step((float)(red_raw));
    //差分
    ir_lpf_diff = ir_lpf - ir_lpf_last;
    red_lpf_diff = red_lpf - red_lpf_last;
    //配列に保存
    ir_lpf_diff_array[cnt % NUM_MOVEAVE] = ir_lpf_diff;
    red_lpf_diff_array[cnt % NUM_MOVEAVE] = red_lpf_diff;

    //移動平均計算
    ir_lpf_diff_moveave = 0.0;
    red_lpf_diff_moveave = 0.0;
    for (uint8_t i = 0; i < NUM_MOVEAVE; i++)
    {
      ir_lpf_diff_moveave += ir_lpf_diff_array[i];
      red_lpf_diff_moveave += red_lpf_diff_array[i];
    }
    ir_lpf_diff_moveave /= (float)NUM_MOVEAVE;
    red_lpf_diff_moveave /= (float)NUM_MOVEAVE;

    //移動平均値を配列に保存
    ir_lpf_diff_moveave_array[cnt % NUM_TH] = ir_lpf_diff_moveave;
    red_lpf_diff_moveave_array[cnt % NUM_TH] = red_lpf_diff_moveave;

    // ピーク検出
    if (ir_lpf_diff_moveave < ir_th_min06 && cnt > NUM_TH && pulse_flg)
    {
      pulse_index = cnt;
      is_sending = true;
      hr = 60.0 / ((float)(pulse_index - pulse_index_last) / 100.0);
      // Serial.printf("%f\n", 60.0 / ((float)(pulse_index - pulse_index_last) / 100.0));
      pulse_index_last = pulse_index;
      pulse_flg = false;

      // float spo2 = ((red_lpf_max - red_lpf_min) / ((red_lpf_max + red_lpf_min) / 2.0)) / ((ir_lpf_max - ir_lpf_min) / ((ir_lpf_max + ir_lpf_min) / 2.0));
      // Serial.printf("%f\n", spo2);

      ir_lpf_max = -10000.0;
      red_lpf_max = -10000.0;
      ir_lpf_min = 10000.0;
      red_lpf_min = 10000.0;
    }
    if (ir_lpf_diff_moveave > 0)
      pulse_flg = true;

    //最大値、最小値取得
    if (ir_lpf >= ir_lpf_min)
      ir_lpf_min = ir_lpf;
    if (ir_lpf <= ir_lpf_max)
      ir_lpf_max = ir_lpf;
    if (red_lpf >= red_lpf_min)
      red_lpf_min = red_lpf;
    if (red_lpf <= red_lpf_max)
      red_lpf_max = red_lpf;

    //しきい値更新
    float tmp_ir = 10000.0;
    float tmp_red = 10000.0;
    for (uint16_t i = 0; i < NUM_TH; i++)
    {
      if (tmp_ir > ir_lpf_diff_moveave_array[i])
        tmp_ir = ir_lpf_diff_moveave_array[i];
      if (tmp_red > red_lpf_diff_moveave_array[i])
        tmp_red = red_lpf_diff_moveave_array[i];
    }
    ir_th_min06 = tmp_ir * 0.6;
    red_th_min06 = tmp_red * 0.6;

    //前回値保存
    red_raw_last = red_raw;
    ir_raw_last = ir_raw;
    ir_lpf_last = ir_lpf;
    red_lpf_last = red_lpf;

    // Serial.printf("%f, %f\r\n", ir_lpf_diff_moveave, ir_th_min06);

    // Serial.printf("%f\r\n", ir_th_min06);
    // Serial.printf("%f\r\n", ir_lpf_diff_moveave);
    Serial.printf("IR: %d, RED: %d\r\n", ir_raw, red_raw);
    cnt += 1;
  }
}
