
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/concat.h"

/**
 * Phalcon\Events\Manager
 *
 * The new Phalcon Events Manager, offers an easy way to intercept and manipulate, if needed,
 * the normal flow of operation. With the EventsManager the developer can create hooks or
 * plugins that will offer monitoring of data, manipulation, conditional execution and much more.
 *
 */


PHP_METHOD(Phalcon_Events_Manager, __construct){

	zval *a0 = NULL;

	PHALCON_MM_GROW();

	PHALCON_INIT_VAR(a0);
	array_init(a0);
	zend_update_property(phalcon_events_manager_ce, this_ptr, SL("_events"), a0 TSRMLS_CC);

	PHALCON_MM_RESTORE();
}

/**
 * Attach a listener to the events manager
 *
 * @param string $eventType
 * @param object $handler
 */
PHP_METHOD(Phalcon_Events_Manager, attach){

	zval *event_type, *handler, *events;
	zval *a0 = NULL;
	int eval_int;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &event_type, &handler) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (Z_TYPE_P(handler) != IS_OBJECT) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_events_exception_ce, "Event handler must be an Object");
		return;
	}
	
	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, event_type);
	if (!eval_int) {
		PHALCON_INIT_VAR(a0);
		array_init(a0);
		phalcon_array_update_zval(&events, event_type, &a0, PH_COPY | PH_SEPARATE TSRMLS_CC);
	}
	
	phalcon_array_update_append_multi_2(&events, event_type, handler, 0 TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_events"), events TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Fires a event in the events manager causing that the acive listeners will be notified about it
 *
 * @param string $eventType
 * @param object $source
 * @param mixed  $data
 * @return mixed
 */
PHP_METHOD(Phalcon_Events_Manager, fire){

	zval *event_type, *source, *data = NULL, *colon, *event_parts;
	zval *exception_message, *type, *event_name;
	zval *status = NULL, *events, *fire_events, *handler = NULL, *event = NULL;
	zval *arguments = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	int eval_int;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &event_type, &source, &data) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!data) {
		PHALCON_INIT_NVAR(data);
	}
	
	PHALCON_INIT_VAR(colon);
	ZVAL_STRING(colon, ":", 1);
	
	PHALCON_INIT_VAR(event_parts);
	phalcon_fast_explode(event_parts, colon, event_type TSRMLS_CC);
	eval_int = phalcon_array_isset_long(event_parts, 1);
	if (!eval_int) {
		PHALCON_INIT_VAR(exception_message);
		PHALCON_CONCAT_SV(exception_message, "Invalid event type ", event_type);
		PHALCON_THROW_EXCEPTION_ZVAL(phalcon_events_exception_ce, exception_message);
		return;
	}
	
	PHALCON_INIT_VAR(type);
	phalcon_array_fetch_long(&type, event_parts, 0, PH_NOISY_CC);
	
	PHALCON_INIT_VAR(event_name);
	phalcon_array_fetch_long(&event_name, event_parts, 1, PH_NOISY_CC);
	
	PHALCON_INIT_VAR(status);
	
	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, type);
	if (eval_int) {
		PHALCON_INIT_VAR(fire_events);
		phalcon_array_fetch(&fire_events, events, type, PH_NOISY_CC);
		
		if (!phalcon_valid_foreach(fire_events TSRMLS_CC)) {
			return;
		}
		
		ah0 = Z_ARRVAL_P(fire_events);
		zend_hash_internal_pointer_reset_ex(ah0, &hp0);
		
		ph_cycle_start_0:
		
			if (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS) {
				goto ph_cycle_end_0;
			}
			
			PHALCON_GET_FOREACH_VALUE(handler);
			
			if (Z_TYPE_P(handler) == IS_OBJECT) {
				PHALCON_INIT_NVAR(event);
				object_init_ex(event, phalcon_events_event_ce);
				PHALCON_CALL_METHOD_PARAMS_3_NORETURN(event, "__construct", event_name, source, data, PH_CHECK);
				if (phalcon_is_instance_of(handler, SL("Closure") TSRMLS_CC)) {
					PHALCON_INIT_NVAR(arguments);
					array_init(arguments);
					phalcon_array_append(&arguments, event, PH_SEPARATE TSRMLS_CC);
					phalcon_array_append(&arguments, source, PH_SEPARATE TSRMLS_CC);
					phalcon_array_append(&arguments, data, PH_SEPARATE TSRMLS_CC);
					
					PHALCON_INIT_NVAR(status);
					PHALCON_CALL_USER_FUNC_ARRAY(status, handler, arguments);
				} else {
					if (phalcon_method_exists(handler, event_name TSRMLS_CC) == SUCCESS) {
						PHALCON_INIT_NVAR(status);
						PHALCON_CALL_METHOD_PARAMS_3(status, handler, Z_STRVAL_P(event_name), event, source, data, PH_NO_CHECK);
					}
				}
			}
			
			zend_hash_move_forward_ex(ah0, &hp0);
			goto ph_cycle_start_0;
			
		ph_cycle_end_0:
		if(0){}
		
	}
	
	
	RETURN_CCTOR(status);
}

/**
 * Returns all the attached listeners of a certain type
 *
 * @param string $type
 * @return array
 */
PHP_METHOD(Phalcon_Events_Manager, getListeners){

	zval *type, *events, *fire_events;
	int eval_int;

	PHALCON_MM_GROW();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(events);
	phalcon_read_property(&events, this_ptr, SL("_events"), PH_NOISY_CC);
	eval_int = phalcon_array_isset(events, type);
	if (eval_int) {
		PHALCON_INIT_VAR(fire_events);
		phalcon_array_fetch(&fire_events, events, type, PH_NOISY_CC);
		
		RETURN_CCTOR(fire_events);
	}
	
	PHALCON_MM_RESTORE();
}

