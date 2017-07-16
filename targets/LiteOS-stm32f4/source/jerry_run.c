/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jerryscript.h"
#include "los_bsp_led.h"


/**
 * Set led value
 */
static jerry_value_t
set_led  (const jerry_value_t func_value, /**< function object */
          const jerry_value_t this_value, /**< this arg */
          const jerry_value_t *args_p, /**< function arguments */
          const jerry_length_t args_cnt) /**< number of function arguments */
{
  if (args_cnt != 2)
  {
    return jerry_create_boolean (false);
  }

  int ledPin = jerry_get_number_value (args_p[0]);
  int value = jerry_get_number_value (args_p[1]);

  LOS_EvbLedInit();
  LOS_EvbLedControl (ledPin, value);

  return jerry_create_boolean (true);
} /* set_led */

/**
 * Delay function
 */
static jerry_value_t
js_delay (const jerry_value_t func_value, /**< function object */
          const jerry_value_t this_value, /**< this arg */
          const jerry_value_t *args_p, /**< function arguments */
          const jerry_length_t args_cnt) /**< number of function arguments */
{
  if (args_cnt != 1)
  {

    return jerry_create_boolean (false);
  }

  int millisec = jerry_get_number_value (args_p[0]);

  LOS_TaskDelay (millisec);

  return jerry_create_boolean (true);
} /* js_delay */

/*
 * Init available js functions
 */
static void
init_jerry ()
{
  jerry_init (JERRY_INIT_SHOW_OPCODES | JERRY_INIT_DEBUGGER |JERRY_INIT_MEM_STATS);

  /* Create an empty JS object */
  jerry_value_t object = jerry_create_object ();

  jerry_value_t func_obj;
  jerry_value_t prop_name;

  func_obj = jerry_create_external_function (set_led);
  prop_name = jerry_create_string ((const jerry_char_t *) "setLed");
  jerry_set_property (object, prop_name, func_obj);
  jerry_release_value (prop_name);
  jerry_release_value (func_obj);

  func_obj = jerry_create_external_function (js_delay);
  prop_name = jerry_create_string ((const jerry_char_t *) "delay");
  jerry_set_property (object, prop_name, func_obj);
  jerry_release_value (prop_name);
  jerry_release_value (func_obj);

  /* Wrap the JS object (not empty anymore) into a jerry api value */
  jerry_value_t global_object = jerry_get_global_object ();

  /* Add the JS object to the global context */
  prop_name = jerry_create_string ((const jerry_char_t *) "test");
  jerry_set_property (global_object, prop_name, object);
  jerry_release_value (prop_name);
  jerry_release_value (object);
  jerry_release_value (global_object);
} /* init_jerry */

/**
 * Jerryscript simple test
 */
static void
test_jerry ()
{
  const jerry_char_t script[] = " \
    test.setLed(1, 0); \
    test.delay(2000); \
    test.setLed(1, 1); \
    test.delay(2000); \
  ";

  size_t script_size = strlen ((const char *) script);

  jerry_value_t eval_ret = jerry_eval (script, script_size, false);

  /* Free JavaScript value, returned by eval */
  jerry_release_value (eval_ret);
} /* test_jerry */


void run ()
{
  init_jerry ();

  test_jerry ();

  jerry_cleanup ();
} 



