#pragma once

#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/posix.h>

#include <uORB/SubscriptionInterval.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_status.h>

#include "osdlib/lib.hpp"


using namespace time_literals;


class CanvasOsd : public ModuleBase<CanvasOsd>, public ModuleParams
{
public:
	CanvasOsd(const char *device);
	virtual ~CanvasOsd() = default;

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static CanvasOsd *instantiate(int argc, char *argv[]);

	/** @see ModuleBase::run() */
	void run() override;

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	/** @see ModuleBase::print_status() */
	int print_status() override;

private:
    void tick(Osd& osd, bool updated_params);

	/**
	 * @brief Opens the serial port.
	 * @return Returns true if the open was successful or ERRNO.
	 */
	int open_serial();

	/**
	 * @brief Closes the serial port.
	 * @return Returns 0 if success or ERRNO.
	 */
	int close_serial();

    char uart_device[64] {};
	int uart_fd{-1};

	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

	uORB::Subscription _battery_status_sub{ORB_ID(battery_status)};
	uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

	DEFINE_PARAMETERS(
		(ParamInt<px4::params::COSD_BATT_THRESH>) _param_battery_critical_threshold
	)
};
