/*
 This example code is in public domain.

 This example code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 This example shows how to receive touch event and the x,y value from the board.

 This example use vm_touch_register_event_callback() to register touch handler.
 when touch the lcd on the board,the touch event and x,y value will be sent to application
 through this handler.

 Just run this application,and see the catcher log or monitor log or lcd.
 */
#include "vmtype.h" 
#include "vmlog.h"
#include "vmsystem.h"
#include "vmpwr.h"
#include "vmgraphic.h"
#include "vmgraphic_font.h"
#include "vmtouch.h"
#include "vmdcl.h"
#include "vmboard.h"
#include "vmgsm_cell.h"
#include "vmdcl_pwm.h"
#include "stdio.h"
#include "vmgsm_sms.h"
#include "vmgsm_sim.h"
#include "vmgsm_tel.h"

VMCHAR g_buff[256];

static void make_voice_call();
static void handle_cell_info_change();
static void accept_incoming_call();

vm_graphic_frame_t g_frame;
const vm_graphic_frame_t* g_frame_blt_group[1];
static VMUINT8* g_font_pool;

static vm_gsm_cell_info_t g_info; /* cell information data */

static VMUINT32 backlight = 50;
static int is_incoming_call_present = 0;

static void font_init(void) {
	VM_RESULT result;
	VMUINT32 pool_size;

	result = vm_graphic_get_font_pool_size(0, 0, 0, &pool_size);
	if (VM_IS_SUCCEEDED(result)) {
		g_font_pool = (VMUINT8*) vm_malloc(pool_size);
		if (NULL != g_font_pool) {
			vm_graphic_init_font_pool(g_font_pool, pool_size);
		}
		vm_log_info("allocate font pool memory failed");
	} else {
		vm_log_info("get font pool size failed, result:%d", result);
	}
}

/**
 * max is 8191
 */
void lcd_backlight_level(VMUINT32 ulValue) {
	VM_DCL_HANDLE pwm_handle;
	vm_dcl_pwm_set_clock_t pwm_clock;
	vm_dcl_pwm_set_counter_threshold_t pwm_config_adv;
	vm_dcl_config_pin_mode(VM_PIN_P1, VM_DCL_PIN_MODE_PWM);
	pwm_handle = vm_dcl_open(PIN2PWM(VM_PIN_P1), vm_dcl_get_owner_id());
	vm_dcl_control(pwm_handle, VM_PWM_CMD_START, 0);
	pwm_config_adv.counter = 100;
	pwm_config_adv.threshold = ulValue;
	pwm_clock.source_clock = 0;
	pwm_clock.source_clock_division = 3;
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_CLOCK, (void *) (&pwm_clock));
	vm_dcl_control(pwm_handle, VM_PWM_CMD_SET_COUNTER_AND_THRESHOLD,
			(void *) (&pwm_config_adv));
	vm_dcl_close(pwm_handle);
}

void log_init(void) {
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	g_frame.width = 240;
	g_frame.height = 240;
	g_frame.color_format = VM_GRAPHIC_COLOR_FORMAT_16_BIT;
	g_frame.buffer = (VMUINT8*) vm_malloc_dma(
			g_frame.width * g_frame.height * 2);
	g_frame.buffer_length = (g_frame.width * g_frame.height * 2);
	g_frame_blt_group[0] = &g_frame;

	/* set color and draw bg*/
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, 0, 240, 240);

	color.r = 255;
	color.g = 0;
	color.b = 0;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, 160, 240, 240);

	vm_graphic_blt_frame(g_frame_blt_group, frame_position, (VMINT) 1);
}

void log_info(VMINT line, VMSTR str) {
	VMWCHAR s[260]; /* string's buffer */
	VMUINT32 size;
	vm_graphic_color_argb_t color; /* use to set screen and text color */
	vm_graphic_point_t frame_position[1] = { 0, 0 };

	vm_chset_ascii_to_ucs2(s, 260, str);
	/* set color and draw bg*/
	color.a = 0;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	vm_graphic_set_color(color);
	vm_graphic_draw_solid_rectangle(&g_frame, 0, line * 20, 240, 20);

	/* set color and draw text*/
	color.a = 0;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	vm_graphic_set_color(color);
	vm_graphic_set_font_size(18);
	vm_graphic_draw_text(&g_frame, 0, line * 20, s);

	/* flush the screen with text data */
	vm_graphic_blt_frame(g_frame_blt_group, frame_position, (VMINT) 1);
}

void handle_touchevt(VM_TOUCH_EVENT event, VMINT x, VMINT y) {
	vm_log_info("touch event=%d,touch x=%d,touch y=%d", event, x, y); /* output log to monitor or catcher */
	sprintf(g_buff, "touch event = %d, x=%d, y=%d", event, x, y);
	log_info(2, g_buff); /* output log to LCD if have */
	VMINT level = vm_pwr_get_battery_level();
	VMBOOL is_charging = vm_pwr_is_charging();
	sprintf(g_buff, "battery level: %d%%, state: %s", level,
			is_charging ? "charging" : "not charging");
	log_info(0, g_buff); // eggfly
	handle_cell_info_change();
	// eggfly
	if (is_incoming_call_present) {
		vm_log_info("accept_incoming_call called.");
		accept_incoming_call();
	} else if (y >= 160) {
		make_voice_call();
		log_info(7, "calling my phone!"); // eggfly
	}
}

static void handle_cell_info_change() {
	vm_gsm_cell_get_current_cell_info(&g_info);
	VMCHAR buf[256];
	sprintf(buf, "ar=%d, bsic=%d, rxlev=%d, mcc=%d, mnc=%d, lac=%d, ci=%d",
			g_info.arfcn, g_info.bsic, g_info.rxlev, g_info.mcc, g_info.mnc,
			g_info.lac, g_info.ci);
	vm_log_info(buf);
	log_info(4, buf);
	sprintf(buf, "GSM Signal Level: %d%%", g_info.rxlev * 100 / 63);
	log_info(5, buf);
}

/* tel action callback */
void vm_gsm_tel_call_actions_cb(vm_gsm_tel_call_actions_callback_data_t *data) {
	vm_log_info("vm_gsm_tel_call_actions_cb start");
	if (data == NULL) {
		vm_log_info("vm_gsm_tel_call_actions_cb return");
		return;
	}

	/* tel action type */
	switch (data->type_action) {
	case VM_GSM_TEL_CALL_ACTION_NO:
		vm_log_debug("No action.");
		break;
	case VM_GSM_TEL_CALL_ACTION_DIAL:
		vm_log_debug("Dial action");
		break;
	case VM_GSM_TEL_CALL_ACTION_ACCEPT:
		vm_log_debug("Accept action");
		break;
	case VM_GSM_TEL_CALL_ACTION_HOLD:
		vm_log_debug("Hold action");
		break;
	case VM_GSM_TEL_CALL_ACTION_END_SINGLE:
		vm_log_debug("End single action");
		break;
	}

	/* tel action result */
	switch (data->data_act_rsp.result_info.result) {
	case VM_GSM_TEL_OK:
		vm_log_debug("OK.");
		break;
	case VM_GSM_TEL_ERROR_UNSPECIFIED_ERROR:
		vm_log_debug("Unspecified error.");
		break;
	case VM_GSM_TEL_ERROR_CANCEL:
		vm_log_debug("Cancel.");
		break;
	case VM_GSM_TEL_ERROR_USER_ABORT:
		vm_log_debug("User abort.");
		break;
	case VM_GSM_TEL_ERROR_ACTION_NOT_ALLOWED:
		vm_log_debug("Action not allowed.");
		break;
	case VM_GSM_TEL_ERROR_NOT_ALLOWED_TO_DIAL:
		vm_log_debug("Not allowed to dial.");
		break;
	case VM_GSM_TEL_ERROR_NOT_ALLOWED_TO_ACCEPT:
		vm_log_debug("Not allowed to accept.");
		break;
	case VM_GSM_TEL_ERROR_NO_ACTIVE_CALL:
		vm_log_debug("No active call.");
		break;
	case VM_GSM_TEL_ERROR_CALLED_PARTY_BUSY:
		vm_log_debug("Called party busy.");
		break;
	case VM_GSM_TEL_ERROR_NETWORK_NOT_AVAILABLE:
		vm_log_debug("Network not available.");
		break;
	case VM_GSM_TEL_ERROR_CALLED_PARTY_NOT_ANSWERED:
		vm_log_debug("Called party not answered.");
		break;
	case VM_GSM_TEL_ERROR_SOS_NUMBER_ONLY:
		vm_log_debug("SOS number only.");
		break;
	case VM_GSM_TEL_ERROR_INVALID_NUMBER:
		vm_log_debug("Invalid number.");
		break;
	case VM_GSM_TEL_ERROR_EMPTY_NUMBER:
		vm_log_debug("Empty number.");
		break;
	case VM_GSM_TEL_ERROR_SOS_CALL_EXISTS:
		vm_log_debug("Exist SOS call.");
		break;
	case VM_GSM_TEL_ERROR_PREFERRED_MODE_PROHIBIT:
		vm_log_debug("Preferred mode prohibit.");
		break;
	case VM_GSM_TEL_ERROR_FLIGHT_MODE_PROHIBIT:
		vm_log_debug("Flight mode prohibit.");
		break;
	case VM_GSM_TEL_ERROR_LOW_BATTERY:
		vm_log_debug("Low battery.");
		break;
	case VM_GSM_TEL_ERROR_UCM_BUSY:
		vm_log_debug("Busy.");
		break;
	case VM_GSM_TEL_ERROR_VT_FALLBACK:
		vm_log_debug("VT fallback.");
		break;
	}

	vm_log_info("vm_gsm_tel_call_actions_cb end");
}

static void accept_incoming_call() {
	vm_gsm_tel_single_call_action_request_t req;
	// vm_gsm_tel_id_info_t tel_id;
	vm_gsm_tel_call_actions_data_t data;
	VM_RESULT ret;
	req.action_id.sim = VM_GSM_TEL_CALL_SIM_1;
	req.action_id.call_id = 1;
	req.action_id.group_id = 1;

	data.action = VM_GSM_TEL_CALL_ACTION_ACCEPT; /* set tel call action to accept calls */
	data.data_action = (void*) &req;
	data.user_data = (void*) NULL;
	data.callback = vm_gsm_tel_call_actions_cb;

	ret = vm_gsm_tel_call_actions(&data);
	if (VM_IS_SUCCEEDED(ret)) {
		vm_log_info("accept incoming request success");
	} else {
		vm_log_info("accept incoming request failed");
	}
}

/* voice call events call back function */
static void vm_gsm_tel_call_listener_cb(vm_gsm_tel_call_listener_data_t* data) {
	// eggfly mod
//	vm_gsm_tel_set_output_device(VM_GSM_TEL_DEVICE_LOUDSPK);
//	vm_gsm_tel_set_volume(VM_AUDIO_VOLUME_6);

	vm_log_info("vm_gsm_tel_call_listener_cb start");
	if (data == NULL) {
		vm_log_info("vm_gsm_tel_call_listener_cb return");
		return;
	}

	/* External call incoming */
	if (data->call_type == VM_GSM_TEL_INDICATION_INCOMING_CALL) {
		static VMUINT16 i = 1;
		vm_gsm_tel_call_info_t* call_info = (vm_gsm_tel_call_info_t*) data->data;
		char info[256];
		sprintf(info, "Incoming number is %s", (char*) call_info->num_uri);
		vm_log_debug(info);
		// eggfly
		is_incoming_call_present = 1;
		log_info(6, info);
	} else if (data->call_type == VM_GSM_TEL_INDICATION_OUTGOING_CALL) {
		vm_gsm_tel_call_info_t* call_info = (vm_gsm_tel_call_info_t*) data->data;
		vm_log_info("Outgoing, calling number is %s",
				(char* )call_info->num_uri);
	} else if (data->call_type == VM_GSM_TEL_INDICATION_CONNECTED) {
		vm_log_info((char* )"connect success");
	} else if (data->call_type == VM_GSM_TEL_INDICATION_CALL_ENDED) {
		vm_log_info((char* )"call end");
	}

	vm_log_info("vm_gsm_tel_call_listener_cb end");
}

/* make voice call */
static void make_voice_call() {
	char *num = "18601065423"; /* dial number*/
	VMINT number_count = 0;
	VM_GSM_SIM_ID sim_id;
	VMBOOL res;
	number_count = vm_gsm_sim_get_card_count();
	sim_id = vm_gsm_sim_get_active_sim_card();
	res = vm_gsm_sim_has_card();
	vm_log_debug(
			"sms read card count is %d, active sime card is %d, sim card result %d",
			number_count, sim_id, res);

	VM_RESULT result;
	vm_gsm_tel_call_actions_data_t vm_gsm_tel_call_action_data;
	vm_gsm_tel_dial_action_request_t vm_gsm_tel_dial_action_request;

	vm_gsm_tel_dial_action_request.sim = VM_GSM_TEL_CALL_SIM_1;
	vm_chset_ascii_to_ucs2((VMWSTR) vm_gsm_tel_dial_action_request.num_uri,
	VM_GSM_TEL_MAX_NUMBER_LENGTH, (VMSTR) num);
	vm_gsm_tel_dial_action_request.module_id = 0;
	vm_gsm_tel_dial_action_request.is_ip_dial = VM_FALSE;
	vm_gsm_tel_dial_action_request.phonebook_data = NULL;

	vm_gsm_tel_call_action_data.action = VM_GSM_TEL_CALL_ACTION_DIAL; /* set action to dial voice call */
	vm_gsm_tel_call_action_data.data_action =
			(void *) &vm_gsm_tel_dial_action_request;
	vm_gsm_tel_call_action_data.callback = vm_gsm_tel_call_actions_cb;

	result = vm_gsm_tel_call_actions(&vm_gsm_tel_call_action_data);
	if (VM_IS_SUCCEEDED(result))
		vm_log_info("make voice call success");
		else
		vm_log_info("make voice call failed");
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_EVENT_CREATE:
		break;
	case VM_EVENT_PAINT:
		log_init();
		log_info(1, "open application success");
		break;
	case VM_EVENT_CELL_INFO_CHANGE:
		handle_cell_info_change();
		/* After opening the cell, this event will occur when the cell info changes.
		 * The new data of the cell info can be obtained from here. */
		break;
	case VM_EVENT_QUIT:
		break;
	}
}

/* Entry point */
void vm_main(void) {
	/* register system events handler */
	lcd_st7789s_init();
	lcd_backlight_level(backlight);
	tp_gt9xx_init();
	font_init();

	// init cells
	VMINT result = vm_gsm_cell_open();
	vm_log_info("open result = %d", result);

	vm_pmng_register_system_event_callback(handle_sysevt);
	vm_touch_register_event_callback(handle_touchevt);
	vm_gsm_tel_call_reg_listener(vm_gsm_tel_call_listener_cb); /* register listener call back function */
}
