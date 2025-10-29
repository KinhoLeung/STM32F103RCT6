/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "page_manager.h"
#include "page_base.h"
#include "pm_factory.h"
#include "pm_log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static pm_manager_t *g_pm = NULL;
static bool g_is_on_a = true;

static void on_root_click(lv_event_t *e)
{
	(void)e;
	if(!g_pm) return;
	if(g_is_on_a){
		pm_manager_push(g_pm, "B", NULL);
		g_is_on_a = false;
	}else{
		pm_manager_back_home(g_pm);
		g_is_on_a = true;
	}
}
/* 简单页面 A */
static void page_a_onViewLoad(pm_page_t *p)
{
	lv_obj_set_size(p->root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(p->root, lv_color_hex(0x223344), 0);
    lv_obj_add_flag(p->root, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_t *label = lv_label_create(p->root);
	lv_label_set_text(label, "Page A\nClick to push -> Page B");
	lv_obj_center(label);
	lv_obj_add_event_cb(p->root, on_root_click, LV_EVENT_RELEASED, NULL);
}

static void page_b_onViewLoad(pm_page_t *p)
{
	lv_obj_set_size(p->root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(p->root, lv_color_hex(0x334455), 0);
    lv_obj_add_flag(p->root, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_t *label = lv_label_create(p->root);
	lv_label_set_text(label, "Page B\nClick to pop <- Page A");
	lv_obj_center(label);
	lv_obj_add_event_cb(p->root, on_root_click, LV_EVENT_RELEASED, NULL);
}

static const pm_page_vtable_t PAGE_A_VT = {
	.onCustomAttrConfig = NULL,
	.onViewLoad = page_a_onViewLoad,
	.onViewDidLoad = NULL,
	.onViewWillAppear = NULL,
	.onViewDidAppear = NULL,
	.onViewWillDisappear = NULL,
	.onViewDidDisappear = NULL,
	.onViewUnload = NULL,
	.onViewDidUnload = NULL,
};

static const pm_page_vtable_t PAGE_B_VT = {
	.onCustomAttrConfig = NULL,
	.onViewLoad = page_b_onViewLoad,
	.onViewDidLoad = NULL,
	.onViewWillAppear = NULL,
	.onViewDidAppear = NULL,
	.onViewWillDisappear = NULL,
	.onViewDidDisappear = NULL,
	.onViewUnload = NULL,
	.onViewDidUnload = NULL,
};

/* 工厂：根据类名创建页面 */
static pm_page_t *demo_factory_create(const char *class_name, void *user)
{
	(void)user;
	if(class_name == NULL) return NULL;
	if(strcmp(class_name, "PageA") == 0){
		return pm_page_create(&PAGE_A_VT);
	}
	if(strcmp(class_name, "PageB") == 0){
		return pm_page_create(&PAGE_B_VT);
	}
	return NULL;
}



/* Demo 入口：创建 manager，注册页面并展示 */
static void pm_overlay_timer_cb(lv_timer_t *timer)
{
	/* 在活动屏上放置全屏透明覆盖，绑定点击事件以切换页面 */
	lv_obj_t *overlay = lv_obj_create(lv_scr_act());
	lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
	lv_obj_set_style_bg_opa(overlay, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_add_event_cb(overlay, on_root_click, LV_EVENT_RELEASED, NULL);
	if(timer) lv_timer_del(timer);
}

void pm_demo_c_run(void)
{
	pm_factory_t fac = { demo_factory_create, NULL, NULL };
	pm_manager_t *m = pm_manager_create(&fac);
	if(!m) return;
	g_pm = m;

	/* 全局进入动画 */
	pm_manager_set_global_load_anim(m, PM_LOAD_ANIM_MOVE_LEFT, PM_PAGE_ANIM_TIME_DEFAULT, PM_PAGE_ANIM_PATH_DEFAULT);

	/* 注册并安装 */
	pm_manager_install(m, "PageA", "A");
	pm_manager_install(m, "PageB", "B");

	/* 设定根样式可选：此处留空使用默认 */
	/* pm_manager_set_root_default_style(m, &some_style); */

	/* 显示首页 */
	pm_manager_push(m, "A", NULL);

	/* 异步创建覆盖并绑定点击事件 */
	lv_timer_t *t = lv_timer_create_basic();
	lv_timer_set_cb(t, pm_overlay_timer_cb);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
  extern void lv_init(void);
  extern void lv_port_disp_init(void);
  extern void lv_port_indev_init(void);
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();


  pm_demo_c_run();
  printf("pm_demo_c_run\n");
  /* USER CODE END 2 */

  /* Init scheduler */         
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
