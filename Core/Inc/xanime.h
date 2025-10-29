/********************************************************************************
 * @Author: iLx1
 * @date: 2025-07-26 14:54:26
 * @filepath: \lvgl_simulator\user\xAnime\xanime.h
 * @description:  xanime 动画插件，用于创建动画控制器
 * @email: colorful_ilx1@163.com
 * @copyright: Copyright (c) iLx1, All Rights Reserved.
 ********************************************************************************/

 #ifndef XANIME_H
 #define XANIME_H

 #ifdef LV_LVGL_H_INCLUDE_SIMPLE
 #include "lvgl.h"
 #else
 #include "lvgl/lvgl.h"
 #endif
 #include <stdbool.h>
 #include <stdint.h>

 #ifdef __cplusplus
 extern "C" {
 #endif

 // 缓动函数枚举
 typedef enum {
   XANIME_EASE_LINEAR,
   XANIME_EASE_IN_SINE,
   XANIME_EASE_OUT_SINE,
   XANIME_EASE_IN_OUT_SINE,
   XANIME_EASE_IN_QUAD,
   XANIME_EASE_OUT_QUAD,
   XANIME_EASE_IN_OUT_QUAD,
   XANIME_EASE_IN_CUBIC,
   XANIME_EASE_OUT_CUBIC,
   XANIME_EASE_IN_OUT_CUBIC,
   XANIME_EASE_IN_QUART,
   XANIME_EASE_OUT_QUART,
   XANIME_EASE_IN_OUT_QUART,
   XANIME_EASE_IN_BACK,
   XANIME_EASE_OUT_BACK,
   XANIME_EASE_IN_OUT_BACK,
   XANIME_EASE_IN_ELASTIC,
   XANIME_EASE_OUT_ELASTIC,
   XANIME_EASE_IN_OUT_ELASTIC,
   XANIME_EASE_IN_BOUNCE,
   XANIME_EASE_OUT_BOUNCE,
   XANIME_EASE_IN_OUT_BOUNCE,
   XANIME_EASE_COUNT
 } xanime_easing_t;

 // 动画参数结构
 typedef struct {
   // X 位置 (INT32_MIN 表示不设置)
   char *x;
   // Y 位置
   char *y;
   // 宽度
   char *width;
   // 高度
   char *height;
   // 透明度 (0-255)
   char *opacity;
   // 旋转角度 (0-360)
   char *rotate;
   // 缩放
   char *scale;
   // 持续时间 (ms)
   char *dur;
   // 延迟时间 (ms)
   char *delay;
   // 循环次数 (0=不循环, -1=无限循环)
   char *loop;
   // 旋转中心 x
   char *pivot_x;
   // 旋转中心 y
   char *pivot_y;
   // 自动播放
   bool auto_play;
   // 从设定值反向执行动画
   bool is_from;
   // 缓动函数
   xanime_easing_t easing;
   // 动画完成回调
   lv_anim_ready_cb_t complete_cb;
   // 用户数据
   void *user_data;
 } xanime_param_t;

 typedef struct {
   uint16_t obj_num;
   lv_obj_t **obj_arr;
 } xanime_obj_t;

 // 内部动画结构
 typedef struct {
   // 目标对象
   xanime_obj_t obj;
   // 动画参数
   xanime_param_t params;
   // 内部状态
   bool is_playing;
 } xanime_t;

 void xanime_create_single(lv_obj_t *obj, xanime_param_t params);

 void xanime_create(xanime_obj_t obj, xanime_param_t params);

 xanime_t *xanime_create_single_rt(lv_obj_t *obj, xanime_param_t params);

 xanime_t *xanime_create_rt(xanime_obj_t obj, xanime_param_t params);

 xanime_t *xanime_start(xanime_t *anime);

 void xanime_delete(xanime_t *anime);

 void anime_param_handle(lv_anim_t *anim, xanime_t *anime, lv_obj_t *obj);

 void add_anim_property(xanime_t *anime, lv_anim_t *anim,
                        lv_anim_exec_xcb_t exec_cb, int32_t start,
                        const char *end);

 #ifdef __cplusplus
 }
 #endif

 #endif // XANIME_H
