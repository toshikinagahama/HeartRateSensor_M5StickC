#ifndef HEARTRATESENSOR_H
#define HEARTRATESENSOR_H
#include <M5StickC.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include "MAX30100_Filters.h"

//define
#define SAMPLING_RATE MAX30100_SAMPRATE_100HZ
#define IR_LED_CURRENT MAX30100_LED_CURR_11MA
#define RED_LED_CURRENT MAX30100_LED_CURR_11MA
#define PULSE_WIDTH MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE true
#define DC_REMOVER_ALPHA 0.95
#define NUM_MOVEAVE 10 //移動平均数
#define NUM_TH 200     //しきい値用配列数

//姿勢クラス
class HeartRateSensor
{

public:
  MAX30100 max30100;
  uint16_t ir_raw = 0;                              //生値
  uint16_t red_raw = 0;                             //生値
  uint16_t ir_raw_last = 0;                         //前回値
  uint16_t red_raw_last = 0;                        //前回値
  float ir_lpf = 0.0;                               //フィルタ値
  float red_lpf = 0.0;                              //フィルタ値
  float ir_lpf_last = 0.0;                          //フィルタ値
  float red_lpf_last = 0.0;                         //フィルタ値
  float ir_lpf_min = 0.0;                           //最小値
  float ir_lpf_max = 0.0;                           //最大値
  float red_lpf_min = 0.0;                          //最小値
  float red_lpf_max = 0.0;                          //最大値
  float ir_lpf_diff = 0.0;                          //差分値
  float red_lpf_diff = 0.0;                         //差分値
  float ir_lpf_diff_array[NUM_MOVEAVE] = {0.0};     //10点の移動平均計算用の配列
  float red_lpf_diff_array[NUM_MOVEAVE] = {0.0};    //10点の移動平均計算用の配列
  float ir_lpf_diff_moveave = 0.0;                  //移動平均値
  float red_lpf_diff_moveave = 0.0;                 //移動平均値
  float ir_lpf_diff_moveave_array[NUM_TH] = {0.0};  //移動平均値保存用配列
  float red_lpf_diff_moveave_array[NUM_TH] = {0.0}; //移動平均値保存用配列
  float ir_th_min06 = 0.0;                          //最小値0.6倍しきい値
  float red_th_min06 = 0.0;                         //最小値0.6倍しきい値
  uint64_t cnt = 0;
  uint64_t pulse_index = 0;      //心拍検出用index
  uint64_t pulse_index_last = 0; //前回値
  bool pulse_flg = true;         //心拍検出用フラグ
  bool is_sending = false;       //通信用フラグ
  float hr = 0.0;                //心拍
  DCRemover irDCRemover;
  DCRemover redDCRemover;
  FilterBuLp1 lpf;

  /**
   * 
   * 初期化
   * 
   */
  void initialize();
  void getValues();
  void resetMeas();
};

#endif // HEARTRATESENSOR_H