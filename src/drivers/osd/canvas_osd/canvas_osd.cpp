#include <termios.h>

#include <lib/modes/ui.hpp>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/log.h>

#include "canvas_osd.hpp"


const unsigned TICK_PERIOD = 100000;


CanvasOsd::CanvasOsd(const char *device) :
	    ModuleParams(nullptr) {
	strcpy(uart_device, device);
	PX4_INFO("Canvas OSD running on %s", uart_device);
}

void CanvasOsd::tick(Osd& osd, bool updated_params) {
    vehicle_status_s vehicle_status{};
    _vehicle_status_sub.copy(&vehicle_status);

    vehicle_attitude_s vehicle_attitude{};
    _vehicle_attitude_sub.copy(&vehicle_attitude);

    battery_status_s battery_status{};
    _battery_status_sub.copy(&battery_status);

	matrix::Eulerf euler_attitude(matrix::Quatf(vehicle_attitude.q));
	const auto yaw = math::degrees(euler_attitude.psi());
	const auto pitch = math::degrees(euler_attitude.theta());
	const auto roll = math::degrees(euler_attitude.phi());

	const size_t FLIGHT_MODES_SIZE = 1;
	FlightModeFlag flightModes[FLIGHT_MODES_SIZE] = {FlightModeFlag::_3D};

    int battery_remaining_percent = static_cast<int>(battery_status.remaining * 100);
    bool is_battery_critical = battery_remaining_percent <= _param_battery_critical_threshold.get();

	osd.setTime(static_cast<uint16_t>(vehicle_status.timestamp));
	osd.setArmed(vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED);
	osd.setFlightMode(mode_util::nav_state_names[vehicle_status.nav_state], flightModes, FLIGHT_MODES_SIZE);
	osd.setBattery(battery_status.voltage_v, battery_status.current_a, battery_remaining_percent, is_battery_critical);
	osd.setAttitude(pitch, roll, yaw);

    osd.draw();
}

void CanvasOsd::run() {
	open_serial();

	Osd osd(uart_fd);
	osd.setBlinkerEnabled(true);

	while (!should_exit()) {
		px4_usleep(TICK_PERIOD);

        bool updated_params = false;
        if (_parameter_update_sub.updated()) {
            parameter_update_s param_update;
            _parameter_update_sub.copy(&param_update);  // clear update
            updateParams();
            updated_params = true;
        }

        tick(osd, updated_params);
	}

	PX4_INFO("Stopping Canvas OSD");
	close_serial();
}

int CanvasOsd::open_serial() {
    uart_fd = ::open(uart_device, O_RDWR | O_NONBLOCK);

	if (uart_fd < 0) {
        PX4_WARN("Failed to open serial port");
		return PX4_ERROR;
	}

    struct termios uart_config;

	tcgetattr(uart_fd, &uart_config);
	cfsetspeed(&uart_config, B115200);
	uart_config.c_cflag &= ~(CSTOPB | PARENB | CRTSCTS);
	uart_config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	uart_config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	uart_config.c_oflag = 0;
	tcsetattr(uart_fd, TCSANOW, &uart_config);

	return uart_fd;
}

int CanvasOsd::close_serial() {
	int ret = ::close(uart_fd);

	if (ret != 0) {
		PX4_WARN("Could not close serial port");
	}

	return ret;
}

int CanvasOsd::task_spawn(int argc, char *argv[]) {
	_task_id = px4_task_spawn_cmd(
        "canvas_osd",
        SCHED_DEFAULT,
        SCHED_PRIORITY_MAX,
        16000,
        (px4_main_t)&run_trampoline,
        (char *const *)argv
    );

	if (_task_id < 0) {
		_task_id = -1;
		return -errno;
	}

	return 0;
}

CanvasOsd *CanvasOsd::instantiate(int argc, char *argv[]) {
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

	CanvasOsd *instance = new CanvasOsd(device);

	if (instance == nullptr) {
		PX4_ERR("alloc failed");
	}

	return instance;
}

int CanvasOsd::print_status() {
	PX4_INFO("Running on %s", uart_device);

	return 0;
}

int CanvasOsd::custom_command(int argc, char *argv[]) {
	return print_usage("Unrecognized command.");
}

int CanvasOsd::print_usage(const char *reason) {
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Canvas MSP telemetry streamer

### Examples
CLI usage example:
$ canvas_osd

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("canvas_osd", "driver");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

extern "C" __EXPORT int canvas_osd_main(int argc, char *argv[]) {
	return CanvasOsd::main(argc, argv);
}
