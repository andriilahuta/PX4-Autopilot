/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* Notes:
 *  - Currently there's a lot of wasted processing here if certain displays are enabled.
 *    A relatively low-hanging fruit would be figuring out which display elements require
 *    information from what UORB topics and disable if the information isn't displayed.
 * 	(this is complicated by the fact that it's not a one-to-one mapping...)
 */

#include "msp_osd.hpp"

#include "msp_defines.h"

#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>

#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/power_monitor.h>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/airspeed_validated.h>
#include <uORB/topics/vehicle_air_data.h>

#include <lib/geo/geo.h>

#include "MspV1.hpp"

#include <math.h>
#include <matrix/math.hpp>
#include <lib/geo/geo.h>
#include <lib/modes/ui.hpp>

#include <px4_platform_common/events.h>

//OSD elements positions
//in betaflight configurator set OSD elements to your desired positions and in CLI type "set osd" to retreieve the numbers.
//234 -> not visible. Horizontally 2048-2074(spacing 1), vertically 2048-2528(spacing 32). 26 characters X 15 lines

// Currently working elements positions (hardcoded)

/* center col

Speed Power Alt
Rssi cell_voltage mah
craft name

*/

// Left
const uint16_t osd_gps_lat_pos = 2048;
const uint16_t osd_gps_lon_pos = 2080;
const uint16_t osd_gps_sats_pos = 2112;

// Center
// Top
const uint16_t osd_disarmed_pos = 2125;
const uint16_t osd_home_dir_pos = 2093;
const uint16_t osd_home_dist_pos = 2095;

// Bottom row 1
const uint16_t osd_gps_speed_pos = 2413;
const uint16_t osd_power_pos = 2415;
const uint16_t osd_altitude_pos = 2416;

// Bottom Row 2
const uint16_t osd_rssi_value_pos = 2445;
const uint16_t osd_avg_cell_voltage_pos = 2446;
const uint16_t osd_mah_drawn_pos = 2449;

// Bottom Row 3
const uint16_t osd_craft_name_pos = 2480;
const uint16_t osd_crosshairs_pos = 2319;

// Right
const uint16_t osd_main_batt_voltage_pos = 2073;
const uint16_t osd_current_draw_pos = 2103;


const uint16_t osd_numerical_vario_pos = LOCATION_HIDDEN;

MspOsd::MspOsd(const char *device) :
	ModuleParams(nullptr)
{
	// _display.set_period(_param_osd_scroll_rate.get() * 1000ULL);
	// _display.set_dwell(_param_osd_dwell_time.get() * 1000ULL);

	// back up device name for connection later
	strcpy(_device, device);

	// _is_initialized = true;
	PX4_INFO("MSP OSD running on %s", _device);



	// for (int i = 0; i < 10; i++) {
	// PX4_INFO("ctr===v3---%d", i + 1);
	// PX4_INFO("------------ctr 11111111111111111");
	// sleep(2);
	// // Osd osd2(0);
	// // osd = osd2;
	// PX4_INFO("------------ctr 22222222222222222");
	// sleep(2);
	// osd = Osd(_msp_fd);
	// // Osd osd(0);
	// // PX4_INFO("33333333333333333");
	// // sleep(2);
	// // osd.print();
	// // PX4_INFO("44444444444444444");
	// // sleep(2);
	// osd.setBlinkerEnabled(true);
	// PX4_INFO("------------ctr 33333333333333333");
	// sleep(2);
	// // osd.print();
	// // PX4_INFO("------------ctr 55555555555555555");
	// // sleep(2);
	// }
}

int MspOsd::close_serial()
{
	int ret = ::close(_msp_fd);

	if (ret != 0) {
		PX4_WARN("Could not close serial port");
	}

	return ret;
}

int MspOsd::open_serial()
{
	struct termios uart_config;
	_msp_fd = open(_device, O_RDWR | O_NONBLOCK);

	if (_msp_fd < 0) {
		PX4_WARN("Failed to open serial port");
		return PX4_ERROR;
	}

	tcgetattr(_msp_fd, &uart_config);
	cfsetspeed(&uart_config, B115200);
	uart_config.c_cflag &= ~(CSTOPB | PARENB | CRTSCTS);
	uart_config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	uart_config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	uart_config.c_oflag = 0;
	tcsetattr(_msp_fd, TCSANOW, &uart_config);

	return _msp_fd;
}

void MspOsd::run()
{
PX4_INFO("-----------run 1111111111111111111");

	open_serial();

	Osd osd(_msp_fd);
	osd.print(1);

	osd.setBlinkerEnabled(true);
	PX4_INFO("-----------run 22222222222222222222222");
	osd.print(2);

	while (!should_exit()) {
		px4_usleep(10000);
		osd.print(3);

		osd.setBattery(1.0, 2.0);

		osd.draw();
	}

	PX4_INFO("Exiting.");
	close_serial();


	// Check if parameters have changed
	// if (_parameter_update_sub.updated()) {
	// 	// clear update
	// 	parameter_update_s param_update;
	// 	_parameter_update_sub.copy(&param_update);
	// 	updateParams(); // update module parameters (in DEFINE_PARAMETERS)
	// 	parameters_update();
	// }

PX4_INFO("-----------run 77777777777777777777777");
	// avoid premature pessimization; if skip processing if we're effectively disabled
	// if (_param_osd_symbols.get() == 0) {
	// 	return;
	// }

	// update display message
	// {
		// vehicle_status_s vehicle_status{};
		// _vehicle_status_sub.copy(&vehicle_status);

		// vehicle_attitude_s vehicle_attitude{};
		// _vehicle_attitude_sub.copy(&vehicle_attitude);

		// log_message_s log_message{};
		// _log_message_sub.copy(&log_message);

		// const auto display_message = msp_osd::construct_display_message(
		// 				     vehicle_status,
		// 				     vehicle_attitude,
		// 				     log_message,
		// 				     _param_osd_log_level.get(),
		// 				     _display);
		// this->Send(MSP_NAME, &display_message);
	// }

	// MSP_FC_VARIANT
	// {
		// const auto msg = msp_osd::construct_FC_VARIANT();
		// this->Send(MSP_FC_VARIANT, &msg);
	// }

	// MSP_STATUS
	// {
		// vehicle_status_s vehicle_status{};
		// _vehicle_status_sub.copy(&vehicle_status);

		// const auto msg = msp_osd::construct_STATUS(vehicle_status);
		// this->Send(MSP_STATUS, &msg);
	// }

	// MSP_ANALOG
	// {
		// battery_status_s battery_status{};
		// _battery_status_sub.copy(&battery_status);

		// input_rc_s input_rc{};
		// _input_rc_sub.copy(&input_rc);

		// const auto msg = msp_osd::construct_ANALOG(
		// 			 battery_status,
		// 			 input_rc);
		// this->Send(MSP_ANALOG, &msg);
	// }

	// MSP_BATTERY_STATE
	// {
		// battery_status_s battery_status{};
		// _battery_status_sub.copy(&battery_status);

	// 	const auto msg = msp_osd::construct_BATTERY_STATE(battery_status);
	// 	this->Send(MSP_BATTERY_STATE, &msg);
	// }

	// MSP_RAW_GPS
	// {
		// sensor_gps_s vehicle_gps_position{};
		// _vehicle_gps_position_sub.copy(&vehicle_gps_position);

		// airspeed_validated_s airspeed_validated{};
		// _airspeed_validated_sub.copy(&airspeed_validated);

		// const auto msg = msp_osd::construct_RAW_GPS(
		// 			 vehicle_gps_position,
		// 			 airspeed_validated);
		// this->Send(MSP_RAW_GPS, &msg);
	// }

	// MSP_COMP_GPS
	// {
		// update heartbeat
		// _heartbeat = !_heartbeat;

		// home_position_s home_position{};
		// _home_position_sub.copy(&home_position);

		// vehicle_global_position_s vehicle_global_position{};
		// _vehicle_global_position_sub.copy(&vehicle_global_position);

		// construct and send message
	// 	const auto msg = msp_osd::construct_COMP_GPS(
	// 				 home_position,
	// 				 vehicle_global_position,
	// 				 _heartbeat);
	// 	this->Send(MSP_COMP_GPS, &msg);
	// }

	// MSP_ATTITUDE
	// {
		// vehicle_attitude_s vehicle_attitude{};
		// _vehicle_attitude_sub.copy(&vehicle_attitude);

	// 	const auto msg = msp_osd::construct_ATTITUDE(vehicle_attitude);
	// 	this->Send(MSP_ATTITUDE, &msg);
	// }

	// MSP_ALTITUDE
	// {
		// sensor_gps_s vehicle_gps_position{};
		// _vehicle_gps_position_sub.copy(&vehicle_gps_position);

		// vehicle_local_position_s vehicle_local_position{};
		// _vehicle_local_position_sub.copy(&vehicle_local_position);

		// construct and send message
	// 	const auto msg = msp_osd::construct_ALTITUDE(vehicle_gps_position, vehicle_local_position);
	// 	this->Send(MSP_ALTITUDE, &msg);
	// }

	// MSP_MOTOR_TELEMETRY
	// {
		// const auto msg = msp_osd::construct_ESC_SENSOR_DATA();
		// this->Send(MSP_ESC_SENSOR_DATA, &msg);
	// }

	// send full configuration
	// SendConfig();

	// const auto now = hrt_absolute_time();
	// if (vehicle_status.timestamp < (now - 1_s)) {
	// 	return;
	// }




//		vehicle_status_s vehicle_status{};
//		_vehicle_status_sub.copy(&vehicle_status);
//
//		vehicle_attitude_s vehicle_attitude{};
//		_vehicle_attitude_sub.copy(&vehicle_attitude);
//
//		battery_status_s battery_status{};
//		_battery_status_sub.copy(&battery_status);
//
//	matrix::Eulerf euler_attitude(matrix::Quatf(vehicle_attitude.q));
//	const auto yaw = math::degrees(euler_attitude.psi());
//	const auto pitch = math::degrees(euler_attitude.theta());
//	const auto roll = math::degrees(euler_attitude.phi());
//
//	const size_t FLIGHT_MODES_SIZE = 1;
//	FlightModeFlag flightModes[FLIGHT_MODES_SIZE] = {FlightModeFlag::_3D};
//PX4_INFO("-----------3333333333333333");
//	osd.setTime(static_cast<uint16_t>(vehicle_status.timestamp));
//	osd.setArmed(vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED);
//	osd.setFlightMode(mode_util::nav_state_names[vehicle_status.nav_state], flightModes, FLIGHT_MODES_SIZE);
//	osd.setBattery(battery_status.voltage_v * 10, battery_status.current_a * 100);
//	osd.setAttitude(pitch, roll, yaw);
//
//	osd.draw();
//
//
//	PX4_INFO("main-------TIME---%llu===", vehicle_status.timestamp);
//	// PX4_INFO("2222222222222222222 %u", static_cast<uint16_t>(vehicle_status.timestamp));
//
//	events::send<uint64_t>(
//		events::ID("canvas_osd"), events::Log::Error,
//		"main-------TIME---{1}===",
//		vehicle_status.timestamp
//	);



	// std::set<FlightModeFlag> flightModes{FlightModeFlag::_3D};
	// OsdParams params {
	// 	.armed = vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED,
	// 	.flightMode = mode_util::nav_state_names[vehicle_status.nav_state],
	// 	.battery = OsdBatteryParams{battery_status.voltage_v * 10, battery_status.current_a * 100},
	// 	.attitude = OsdAttitudeParams{pitch, roll, yaw},
	// };

	// writeMsp(encoder, writer, MspStatus {
	// 	.time = static_cast<uint16_t>(vehicle_status.timestamp),
	// 	.flightModes = flightModes
	// });
        // layout.tick(params);
        // painter.draw(layout);
}

void MspOsd::parameters_update()
{
	// update our display rate and dwell time
	// _display.set_period(hrt_abstime(_param_osd_scroll_rate.get() * 1000ULL));
	// _display.set_dwell(hrt_abstime(_param_osd_dwell_time.get() * 1000ULL));
}

int MspOsd::task_spawn(int argc, char *argv[])
{
	_task_id = px4_task_spawn_cmd("MspOsd",
				      SCHED_DEFAULT,
				      SCHED_PRIORITY_MAX,
				      160000,
				      (px4_main_t)&run_trampoline,
				      (char *const *)argv);

	if (_task_id < 0) {
		_task_id = -1;
		return -errno;
	}

	return 0;
}

MspOsd *MspOsd::instantiate(int argc, char *argv[])
{
	// initialize device
	const char *device = nullptr;
	bool error_flag = false;

	// loop through input arguments
	int myoptind = 1;
	int ch;
	const char *myoptarg = nullptr;

	while ((ch = px4_getopt(argc, argv, "d:", &myoptind, &myoptarg)) != EOF) {
		switch (ch) {
		case 'd':
			device = myoptarg;
			break;

		default:
			PX4_WARN("unrecognized flag");
			error_flag = true;
			break;
		}
	}

	if (error_flag) {
		return nullptr;
	}

	if (!device) {
		PX4_ERR("Missing device");
		return nullptr;
	}

	MspOsd *instance = new MspOsd(device);

	if (instance == nullptr) {
		PX4_ERR("alloc failed");
	}

	return instance;
}

int MspOsd::print_status()
{
	PX4_INFO("Running on %s", _device);
	// PX4_INFO("\tinitialization issues: %d", _performance_data.initialization_problems);
	// PX4_INFO("\tscroll rate: %d", static_cast<int>(_param_osd_scroll_rate.get()));
	// PX4_INFO("\tsuccessful sends: %lu", _performance_data.successful_sends);
	// PX4_INFO("\tunsuccessful sends: %lu", _performance_data.unsuccessful_sends);

	// print current display string
	// char msg[FULL_MSG_BUFFER];
	// _display.get(msg, hrt_absolute_time());
	// PX4_INFO("Current message: \n\t%s", msg);

	return 0;
}

int MspOsd::custom_command(int argc, char *argv[])
{
	return print_usage("Unrecognized command.");
}

int MspOsd::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
MSP telemetry streamer

### Implementation
Converts uORB messages to MSP telemetry packets

### Examples
CLI usage example:
$ canvas_osd

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("canvas_osd", "driver");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

extern "C" __EXPORT int canvas_osd_main(int argc, char *argv[])
{
	PX4_INFO("print===v1---");
	return MspOsd::main(argc, argv);

	// uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	// 	vehicle_status_s vehicle_status{};
	// 	_vehicle_status_sub.copy(&vehicle_status);

	// PX4_INFO("1111111111111111111 %llu", vehicle_status.timestamp);
	// // PX4_INFO("111111111111111111111111 %u", vehicle_status.timestamp);
	// PX4_INFO("222222222222222222222222 %u", static_cast<uint16_t>(vehicle_status.timestamp));


	// 	struct termios t;
	// 	int _msp_fd = open("/dev/ttyS0", O_RDWR | O_NONBLOCK);

	// 	if (_msp_fd < 0) {
	// 		// _performance_data.initialization_problems = true;
	// 		PX4_INFO("0000000000000");
	// 		return 1;
	// 	}

	// 	tcgetattr(_msp_fd, &t);
	// 	cfsetspeed(&t, B115200);
	// 	t.c_cflag &= ~(CSTOPB | PARENB | CRTSCTS);
	// 	t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	// 	t.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	// 	t.c_oflag = 0;
	// 	tcsetattr(_msp_fd, TCSANOW, &t);

	// 	Osd osd(_msp_fd);


	// for (int i = 0; i < 100; i++) {
	// 	PX4_INFO("print===v2---%d", i + 1);
	// 	// sleep(1);

	// 	PX4_INFO("11111111111111111");
	// 	// sleep(1);

	// 	// Osd osd(0);
	// 	// PX4_INFO("22222222222222222");
	// 	// sleep(1);

	// 	osd.setBlinkerEnabled(true);
	// 	PX4_INFO("33333333333333333");
	// 	sleep(1);

	// 	osd.setBattery(1.0, 2.0);
	// 	PX4_INFO("55555555555555555");
	// 	sleep(1);

	// 	osd.draw();
	// 	PX4_INFO("66666666666666666");
	// 	// sleep(1);
	// }

	// PX4_INFO("777777777777777 end");

	return 0;
}
