/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearLine(25U);
    chThdSleepMilliseconds(500);
    palSetLine(25U);
    chThdSleepMilliseconds(500);
  }
}

/**
 * @brief   Core 1 OS instance.
 */
os_instance_t ch1;

#if (CH_CFG_NO_IDLE_THREAD == FALSE) || defined(__DOXYGEN__)
/**
 * @brief   Default instance idle thread working area.
 */
THD_WORKING_AREA(ch_c1_idle_thread_wa, PORT_IDLE_THREAD_STACK_SIZE);
#endif

/**
 * @brief   Core 1 OS instance configuration.
 */
static const os_instance_config_t core1_cfg = {
  .name             = "c1",
#if CH_DBG_ENABLE_STACK_CHECK == TRUE
  .mainthread_base  = &__c1_main_thread_stack_base__,
  .mainthread_end   = &__c1_main_thread_stack_end__,
#elif CH_CFG_USE_DYNAMIC == TRUE
  .mainthread_base  = NULL,
  .mainthread_end   = NULL,
#endif
#if CH_CFG_NO_IDLE_THREAD == FALSE
  .idlethread_base  = THD_WORKING_AREA_BASE(ch_c1_idle_thread_wa),
  .idlethread_end   = THD_WORKING_AREA_END(ch_c1_idle_thread_wa)
#endif
};

/**
 * Core 1 entry point.
 */
void c1_main(void) {

  /*
   * Starting a new OS instance running on this core.
   */
  chSchObjectInit(&ch1, &core1_cfg);

  /* It is alive now.*/
  chSysUnlock();

  /*
   * Setting up GPIOs.
   */
  palSetLineMode(0U, PAL_MODE_ALTERNATE_UART);
  palSetLineMode(1U, PAL_MODE_ALTERNATE_UART);
  palSetLineMode(25U, PAL_MODE_OUTPUT_PUSHPULL | PAL_RP_PAD_DRIVE12);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop.
   */
  while (true) {
    chThdSleepMilliseconds(500);
  }
}
