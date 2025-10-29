/********************************************************************************
 * @Author: iLx1
 * @date: 2025-07-26 14:54:32
 * @filepath: \lvgl_simulator\user\xAnime\xanime.c
 * @description:  xanime 动画插件，用于创建动画控制器
 * @email: colorful_ilx1@163.com
 * @copyright: Copyright (c) iLx1, All Rights Reserved.
 ********************************************************************************/

 #include "xanime.h"

 #include <ctype.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 static lv_anim_path_cb_t get_easing_func(xanime_easing_t easing);

 static int32_t str_to_int32(const char* str, bool* success);

 static int int32_to_str(int32_t value, char* buffer, size_t bufsize);

 static void zoom_exec_cb(void* var, int32_t v);

 static void opa_exec_cb(void* var, int32_t v);

 static void rotate_exec_cb(void* var, int32_t v);

 static bool is_image_object(lv_obj_t* obj);

 static int16_t get_x_percent(lv_obj_t* obj, char* percent);

 static int16_t get_y_percent(lv_obj_t* obj, char* percent);

 static int16_t get_width_percent(lv_obj_t* obj, char* percent);

 static int16_t get_height_percent(lv_obj_t* obj, char* percent);

 static void remove_percent_chars(char* str);

 static bool has_percent(const char* str);

 /********************************************************************************
  * @brief: 创建单个动画
  * @param {lv_obj_t} obj
  * @param {xanime_param_t} params
  * @return {*}
  ********************************************************************************/
 void xanime_create_single(lv_obj_t* obj, xanime_param_t params) {
   lv_obj_t* obj_arr[1] = {obj};
   xanime_create((xanime_obj_t){.obj_num = 1, .obj_arr = obj_arr}, params);
 }

 /********************************************************************************
  * @brief: 创建并执行动画控制器
  * @param {xanime_obj_t} obj
  * @param {xanime_param_t} params
  * @return {*}
  ********************************************************************************/
 void xanime_create(xanime_obj_t obj, xanime_param_t params) {
   if (obj.obj_num == 0) return;
   bool success = false;
   int32_t dur = str_to_int32(params.dur, &success);
   if (!success || dur <= 0) return;

   xanime_t anime;
   memset(&anime, 0, sizeof(xanime_t));

   // 复制对象数组
   anime.obj.obj_arr = obj.obj_arr;
   anime.obj.obj_num = obj.obj_num;

   anime.params.x = params.x;
   anime.params.y = params.y;
   anime.params.width = params.width;
   anime.params.height = params.height;
   anime.params.opacity = params.opacity;
   anime.params.rotate = params.rotate;
   anime.params.scale = params.scale;
   anime.params.dur = params.dur;
   anime.params.delay = params.delay;
   anime.params.easing = params.easing;
   anime.params.loop = params.loop;
   anime.params.pivot_x = params.pivot_x;
   anime.params.pivot_y = params.pivot_y;
   anime.params.complete_cb = params.complete_cb;
   anime.params.user_data = params.user_data;
   anime.params.is_from = params.is_from;

   // 初始化状态
   anime.is_playing = false;

   // 自动播放
   xanime_start(&anime);
 }

 /********************************************************************************
  * @brief: 创建单个动画有返回值，返回动画控制器指针
  * @param {lv_obj_t*} obj
  * @param {xanime_param_t} params
  * @return {*}
  ********************************************************************************/
 xanime_t* xanime_create_single_rt(lv_obj_t* obj, xanime_param_t params) {
   lv_obj_t* obj_arr[1] = {obj};
   return xanime_create_rt((xanime_obj_t){.obj_num = 1, .obj_arr = obj_arr},

                           params);
 }

 /********************************************************************************
  * @brief: 创建动画控制器有返回值，返回动画控制器指针
  * @param {xanime_obj_t} obj
  * @param {xanime_param_t} params
  * @return {*}
  ********************************************************************************/
 xanime_t* xanime_create_rt(xanime_obj_t obj, xanime_param_t params) {
   if (obj.obj_num == 0) return NULL;
   bool success = false;
   int32_t dur = str_to_int32(params.dur, &success);
   if (!success || dur <= 0) return NULL;

   xanime_t* anime = malloc(sizeof(xanime_t));
   if (!anime) return NULL;

   memset(anime, 0, sizeof(xanime_t));

   // 复制对象数组
   anime->obj.obj_arr = obj.obj_arr;
   anime->obj.obj_num = obj.obj_num;

   anime->params.x = params.x;
   anime->params.y = params.y;
   anime->params.width = params.width;
   anime->params.height = params.height;
   anime->params.opacity = params.opacity;
   anime->params.rotate = params.rotate;
   anime->params.scale = params.scale;
   anime->params.dur = params.dur;
   anime->params.delay = params.delay;
   anime->params.easing = params.easing;
   anime->params.loop = params.loop;
   anime->params.complete_cb = params.complete_cb;
   anime->params.user_data = params.user_data;
   anime->params.is_from = params.is_from;

   // 初始化状态
   anime->is_playing = false;

   // 自动播放
   if (params.auto_play) {
     xanime_start(anime);
   }

   return anime;
 }

 /********************************************************************************
  * @brief: 启动动画
  * @param {xanime_t*} anime
  * @return {*}
  ********************************************************************************/
 xanime_t* xanime_start(xanime_t* anime) {
   if (!anime || !anime->obj.obj_arr || anime->is_playing) return anime;

   if (anime->obj.obj_num == 0) {
     return NULL;
   }
   // 循环创建动画
   for (uint16_t i = 0; i < anime->obj.obj_num; i++) {
     lv_obj_t* cur_obj = anime->obj.obj_arr[i];
     // 更新最新布局
     lv_obj_update_layout(cur_obj);
     // 初始化动画
     lv_anim_t a;
     lv_anim_init(&a);
     lv_anim_set_var(&a, cur_obj);
     // lv_anim_set_time(&a, anime->params.dur);
     // lv_anim_set_delay(&a, anime->params.delay);
     // lv_anim_set_ready_cb(cur_obj, anime->params.complete_cb);
     // lv_anim_set_user_data(cur_obj, anime->params.user_data);
     // lv_anim_set_path_cb(&a, get_easing_func(anime->params.easing));

     anime_param_handle(&a, anime, cur_obj);
   }

   anime->is_playing = true;

   return anime;  // 返回控制器指针以支持链式调用
 }

 /********************************************************************************
  * @brief: 检查参数是否有效
  * @param {char*} param
  * @return {*}
  ********************************************************************************/
 bool check_param(char* param) {
   if (param == NULL || param[0] == '\0' || strcmp(param, "") == 0) {
     return false;
   }
   return true;
 }

 /********************************************************************************
  * @brief: 处理动画参数
  * @param {lv_anim_t*} anim
  * @param {xanime_t*} anime
  * @param {lv_obj_t*} obj
  * @return {*}
  ********************************************************************************/
 void anime_param_handle(lv_anim_t* anim, xanime_t* anime, lv_obj_t* obj) {
   // duration
   if (check_param(anime->params.dur)) {
     bool success = false;
     int32_t dur = str_to_int32(anime->params.dur, &success);
     if (!success) {
       printf("Error: Invalid dur value '%s'\n", anime->params.dur);
       return;
     }
     lv_anim_set_time(anim, dur);
   }
   // delay
   if (check_param(anime->params.delay)) {
     bool success = false;
     int32_t delay = str_to_int32(anime->params.delay, &success);
     if (!success) {
       printf("Error: Invalid delay value '%s'\n", anime->params.delay);
       return;
     }
     lv_anim_set_delay(anim, delay);
   }
   // complete callback
   if (anime->params.complete_cb != NULL) {
     lv_anim_set_ready_cb(anim, anime->params.complete_cb);
   }

   // loop
   if (check_param(anime->params.loop)) {
     bool success = false;
     int32_t loop = str_to_int32(anime->params.loop, &success);
     if (!success) {
       printf("Error: Invalid loop value '%s'\n", anime->params.loop);
       return;
     }
     lv_anim_set_repeat_count(anim, loop < 0 ? LV_ANIM_REPEAT_INFINITE : loop);
   }
   // user data
   if (anime->params.user_data != NULL) {
     lv_anim_set_user_data(anim, anime->params.user_data);
   }
  // easing
  if (anime->params.easing < XANIME_EASE_COUNT) {
    lv_anim_set_path_cb(anim, get_easing_func(anime->params.easing));
  }
   // x
   if (check_param(anime->params.x)) {
     int32_t start = lv_obj_get_x(obj);
     char x_str[12];
     if (has_percent(anime->params.x)) {
       // 复制字符串
       strcpy(x_str, anime->params.x);
       // 移除字符串中的%
       remove_percent_chars(x_str);
       int32_to_str(get_x_percent(obj, x_str), x_str, 12);
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_x, start, x_str);
     } else {
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_x, start,
                         anime->params.x);
     }
   }
   // y
   if (check_param(anime->params.y)) {
     int32_t start = lv_obj_get_y(obj);
     char y_str[12];
     if (has_percent(anime->params.y)) {
       // 复制字符串
       strcpy(y_str, anime->params.y);
       // 移除字符串中的%
       remove_percent_chars(y_str);
       int32_to_str(get_y_percent(obj, y_str), y_str, 12);
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_y, start, y_str);
     } else {
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_y, start,
                         anime->params.y);
     }
   }
   // width
   if (check_param(anime->params.width)) {
     int32_t start = lv_obj_get_width(obj);

     char width_str[12];
     if (has_percent(anime->params.width)) {
       // 复制字符串
       strcpy(width_str, anime->params.width);
       // 移除字符串中的%
       remove_percent_chars(width_str);
       int32_to_str(get_width_percent(obj, width_str), width_str, 12);
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_width, start,
                         width_str);
     } else {
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_width, start,
                         anime->params.width);
     }
   }
   // height
   if (check_param(anime->params.height)) {
     int32_t start = lv_obj_get_height(obj);
     char height_str[12];
     if (has_percent(anime->params.height)) {
       // 复制字符串
       strcpy(height_str, anime->params.height);
       // 移除字符串中的%
       remove_percent_chars(height_str);
       int32_to_str(get_height_percent(obj, height_str), height_str, 12);
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_height, start,
                         height_str);
     } else {
       add_anim_property(anime, anim, (lv_anim_exec_xcb_t)lv_obj_set_height, start,
                         anime->params.height);
     }
   }
   // opacity
  if (check_param(anime->params.opacity)) {
    int32_t start = lv_obj_get_style_opa(obj, LV_PART_MAIN);
     add_anim_property(anime, anim, opa_exec_cb, start, anime->params.opacity);
   }
   // rotate
  if (check_param(anime->params.rotate)) {
    int32_t start = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
     add_anim_property(anime, anim, rotate_exec_cb, start, anime->params.rotate);
   }
   // pivot_x
   if (check_param(anime->params.pivot_x)) {
     char pivot_str[12];
     int32_t pivot_x = 0;
     if (has_percent(anime->params.pivot_x)) {
       // 复制字符串
       strcpy(pivot_str, anime->params.pivot_x);
       // 移除字符串中的%
       remove_percent_chars(pivot_str);
       // 处理百分比
       lv_coord_t percent_x = str_to_int32(pivot_str, NULL);
       // 更新布局以获取图片宽高
       lv_obj_update_layout(obj);
       pivot_x = lv_obj_get_width(obj) * percent_x / 100;
     } else {
       pivot_x = str_to_int32(anime->params.pivot_x, NULL);
     }

     if (is_image_object(obj)) {
       lv_point_t pivot;
       lv_img_get_pivot(obj, &pivot);
       printf("pivot_x: %d, pivot_y: %d\n", pivot_x, pivot.y);
       lv_img_set_pivot(obj, pivot_x, pivot.y);
     } else {
       lv_obj_set_style_transform_pivot_x(obj, pivot_x, LV_PART_MAIN);
     }
   }
   // pivot_y
   if (check_param(anime->params.pivot_y)) {
     char pivot_str[12];
     int32_t pivot_y = 0;
     if (has_percent(anime->params.pivot_y)) {
       // 复制字符串
       strcpy(pivot_str, anime->params.pivot_y);
       // 移除字符串中的%
       remove_percent_chars(pivot_str);
       // 处理百分比
       lv_coord_t percent_y = str_to_int32(pivot_str, NULL);
       // 更新布局以获取图片宽高
       lv_obj_update_layout(obj);
       pivot_y = lv_obj_get_height(obj) * percent_y / 100;
     } else {
       pivot_y = str_to_int32(anime->params.pivot_y, NULL);
     }

     if (is_image_object(obj)) {
       lv_point_t pivot;
       lv_img_get_pivot(obj, &pivot);
       lv_img_set_pivot(obj, pivot.x, pivot_y);
     } else {
       lv_obj_set_style_transform_pivot_y(obj, pivot_y, LV_PART_MAIN);
     }
   }
   // scale
  if (check_param(anime->params.scale)) {
    int32_t start = lv_obj_get_style_transform_scale_x(obj, LV_PART_MAIN);
     add_anim_property(anime, anim, zoom_exec_cb, start, anime->params.scale);
   }
 }

 /********************************************************************************
  * @brief: 添加动画属性
  * @param {lv_anim_t*} anim
  * @param {lv_anim_exec_xcb_t} exec_cb
  * @param {int32_t} start
  * @param {char*} end
  * @return {*}
  ********************************************************************************/
 void add_anim_property(xanime_t* anime, lv_anim_t* anim, lv_anim_exec_xcb_t exec_cb,
                        int32_t start, const char* end) {
   bool success = false;
   int32_t end_int = str_to_int32(end, &success);
   if (!success) {
     printf("Error: Invalid end value '%s'\n", end);
     return;
   }
   lv_anim_set_exec_cb(anim, exec_cb);
   if (anime->params.is_from) {
     lv_anim_set_values(anim, end_int, start);
   } else {
     lv_anim_set_values(anim, start, end_int);
   }
   lv_anim_start(anim);
 }

 /********************************************************************************
  * @brief: 旋转动画回调函数
  * @param {void*} var
  * @param {int32_t} v
  * @return {*}
  ********************************************************************************/
 static void rotate_exec_cb(void* var, int32_t v) {
   if (is_image_object(var)) {
     lv_img_set_angle(var, v);
   } else {
     lv_obj_set_style_transform_rotation(var, v, LV_PART_MAIN);
   }
 }

 /********************************************************************************
  * @brief: 透明度动画回调函数
  * @param {void*} var
  * @param {int32_t} v
  * @return {*}
  ********************************************************************************/
 static void opa_exec_cb(void* var, int32_t v) {
   lv_obj_set_style_opa(var, (lv_opa_t)v, LV_PART_MAIN);
 }

 /********************************************************************************
  * @brief: 缩放动画回调函数
  * @param {void*} var
  * @param {int32_t} v
  * @return {*}
  ********************************************************************************/
 static void zoom_exec_cb(void* var, int32_t v) {
   lv_obj_set_style_transform_scale_x(var, v, LV_PART_MAIN);
   lv_obj_set_style_transform_scale_y(var, v, LV_PART_MAIN);
 }

 /********************************************************************************
  * @brief: 判断对象是否为图片对象
  * @param {lv_obj_t} *obj
  * @return {*}
  ********************************************************************************/
 static bool is_image_object(lv_obj_t* obj) {
   return lv_obj_has_class(obj, &lv_image_class);
 }

 /********************************************************************************
  * @brief: 获取缓动函数
  * @param {xanime_easing_t} easing
  * @return {*}
  ********************************************************************************/
 static lv_anim_path_cb_t get_easing_func(xanime_easing_t easing) {
   switch (easing) {
     case XANIME_EASE_LINEAR:
       return lv_anim_path_linear;
     case XANIME_EASE_IN_SINE:
       return lv_anim_path_ease_in;
     case XANIME_EASE_OUT_SINE:
       return lv_anim_path_ease_out;
     case XANIME_EASE_IN_OUT_SINE:
       return lv_anim_path_ease_in_out;
     case XANIME_EASE_IN_QUAD:
       return lv_anim_path_overshoot;
     case XANIME_EASE_OUT_QUAD:
       return lv_anim_path_bounce;
     case XANIME_EASE_IN_OUT_QUAD:
       return lv_anim_path_step;
     case XANIME_EASE_IN_CUBIC:
       return lv_anim_path_bounce;
     case XANIME_EASE_OUT_CUBIC:
       return lv_anim_path_overshoot;
     case XANIME_EASE_IN_OUT_CUBIC:
       return lv_anim_path_ease_in_out;
     case XANIME_EASE_IN_QUART:
       return lv_anim_path_ease_in;
     case XANIME_EASE_OUT_QUART:
       return lv_anim_path_ease_out;
     case XANIME_EASE_IN_OUT_QUART:
       return lv_anim_path_ease_in_out;
     case XANIME_EASE_IN_BACK:
       return lv_anim_path_overshoot;
     case XANIME_EASE_OUT_BACK:
       return lv_anim_path_overshoot;
     case XANIME_EASE_IN_OUT_BACK:
       return lv_anim_path_overshoot;
     case XANIME_EASE_IN_ELASTIC:
       return lv_anim_path_bounce;
     case XANIME_EASE_OUT_ELASTIC:
       return lv_anim_path_bounce;
     case XANIME_EASE_IN_OUT_ELASTIC:
       return lv_anim_path_bounce;
     case XANIME_EASE_IN_BOUNCE:
       return lv_anim_path_bounce;
     case XANIME_EASE_OUT_BOUNCE:
       return lv_anim_path_bounce;
     case XANIME_EASE_IN_OUT_BOUNCE:
       return lv_anim_path_bounce;
     default:
       return lv_anim_path_linear;
   }
 }

 /********************************************************************************
  * @brief: 字符串转换为int32_t
  * @param {char*} str
  * @param {bool*} success
  * @return {*}
  ********************************************************************************/
 static int32_t str_to_int32(const char* str, bool* success) {
   if (!str || !str[0]) {
     if (success) *success = false;
     return 0;
   }

   const char* p = str;
   bool negative = false;
   int32_t result = 0;

   // 跳过前导空格
   while (*p == ' ') p++;

   // 处理符号
   if (*p == '-') {
     negative = true;
     p++;
   } else if (*p == '+') {
     p++;
   }

   // 检查是否有有效数字
   if (!isdigit((unsigned char)*p)) {
     if (success) *success = false;
     return 0;
   }

   // 主转换循环
   const int32_t MAX_SAFE = INT32_MAX / 10;
   const int32_t LAST_DIGIT_MAX = INT32_MAX % 10;

   while (*p) {
     if (!isdigit((unsigned char)*p)) break;

     int digit = *p - '0';

     // 溢出检查 (在乘以10之前)
     if (result > MAX_SAFE ||
         (result == MAX_SAFE &&
          digit > (negative ? LAST_DIGIT_MAX + 1 : LAST_DIGIT_MAX))) {
       if (success) *success = false;
       return negative ? INT32_MIN : INT32_MAX;
     }

     result = result * 10 + digit;
     p++;
   }

   // 处理负值
   if (negative) {
     result = -result;
   }

   if (success) *success = true;
   return result;
 }

 /********************************************************************************
  * @brief: 删除动画控制器
  * @param {xanime_t*} anime
  * @return {*}
  ********************************************************************************/
 void xanime_delete(xanime_t* anime) {
   if (!anime) return;
   anime->is_playing = false;

   free(anime);
   anime = NULL;
 }

 /********************************************************************************
  * @brief: 获取对象的 X 轴位置百分比
  * @param {lv_obj_t*} obj
  * @param {char*} percent
  * @return {*}
  ********************************************************************************/
 static int16_t get_x_percent(lv_obj_t* obj, char* percent) {
   int16_t percent_int = str_to_int32(percent, NULL);
   int16_t parent_w = lv_obj_get_content_width(lv_obj_get_parent(obj));
   int16_t obj_w = lv_obj_get_width(obj);
   return ((parent_w - obj_w) * percent_int / 100);  // 居中计算
 }

 /********************************************************************************
  * @brief: 获取对象的 Y 轴位置百分比
  * @param {lv_obj_t*} obj
  * @param {char*} percent
  * @return {*}
  ********************************************************************************/
 static int16_t get_y_percent(lv_obj_t* obj, char* percent) {
   int16_t percent_int = str_to_int32(percent, NULL);
   lv_coord_t parent_h = lv_obj_get_content_height(lv_obj_get_parent(obj));
   lv_coord_t obj_h = lv_obj_get_height(obj);
   return ((parent_h - obj_h) * percent_int / 100);
 }

 /********************************************************************************
  * @brief: 获取对象的高度百分比
  * @param {lv_obj_t*} obj
  * @param {char*} percent
  * @return {*}
  ********************************************************************************/
 static int16_t get_height_percent(lv_obj_t* obj, char* percent) {
   int16_t percent_int = str_to_int32(percent, NULL);
   return ((lv_obj_get_content_height(lv_obj_get_parent(obj)) * percent_int) /
           100);
 }
 /********************************************************************************
  * @brief: 获取对象的宽度百分比
  * @param {lv_obj_t*} obj
  * @param {char*} percent
  * @return {*}
  ********************************************************************************/
 static int16_t get_width_percent(lv_obj_t* obj, char* percent) {
   int16_t percent_int = str_to_int32(percent, NULL);
   return ((lv_obj_get_content_width(lv_obj_get_parent(obj)) * percent_int) /
           100);
 }

 /********************************************************************************
  * @brief: 检查字符串是否包含 '%' 字符
  * @param {char*} str
  * @return {*}
  ********************************************************************************/
 static bool has_percent(const char* str) {
   if (!str) return false;
   while (*str) {
     if (*str == '%') return true;
     str++;
   }
   return false;
 }

 /********************************************************************************
  * @brief: 移除字符串中所有的 '%' 字符
  * @param {char*} str
  * @return {*}
  ********************************************************************************/
 static void remove_percent_chars(char* str) {
   if (!str) return;

   char* read_ptr = str;
   char* write_ptr = str;

   while (*read_ptr) {
     if (*read_ptr != '%') {
       *write_ptr = *read_ptr;
       write_ptr++;
     }
     read_ptr++;
   }
   *write_ptr = '\0';  // 终止字符串
 }

 /********************************************************************************
  * @brief: int32_t 转换为字符串
  * @param {int32_t} value
  * @param {char*} buffer
  * @param {size_t} bufsize
  * @return {*} 成功时返回字符串长度，失败返回-1
  ********************************************************************************/
 static int int32_to_str(int32_t value, char* buffer, size_t bufsize) {
   // 检查缓冲区是否有效且足够大 (最小需要12字节: -2147483648 + '\0')
   if (!buffer || bufsize < 12) return -1;

   char* p = buffer;
   bool negative = value < 0;
   uint32_t abs_value;

   // 处理最小负数特殊情况 (不能直接取绝对值)
   if (value == INT32_MIN) {
     abs_value = (uint32_t)INT32_MAX + 1;
   } else {
     abs_value = negative ? (uint32_t)(-value) : (uint32_t)value;
   }

   // 从末尾开始写入数字
   char* end = p;
   do {
     if ((size_t)(end - buffer) >= bufsize - 1) return -1;  // 缓冲区溢出保护
     *end++ = '0' + (abs_value % 10);
     abs_value /= 10;
   } while (abs_value > 0);

   // 添加负号
   if (negative) {
     if ((size_t)(end - buffer) >= bufsize - 1) return -1;
     *end++ = '-';
   }

   // 反转字符串
   *end = '\0';
   char* start = buffer;
   while (start < --end) {
     char tmp = *start;
     *start++ = *end;
     *end = tmp;
   }

   return (int)(end - buffer);
 }
