#pragma once

namespace app::dbg::hid
{
	class DummyDevice : public app::hid::DevicePlatformBase
	{
		[[nodiscard]] uint GetPortMax() const override;

		bool IsActive(uint port) const override
		{
			return true;
		}

		void Poll(float time) override
		{
			printf("idk\n");
		}

		bool GetDeviceData(app::hid::ResSendDeviceData* data, uint port) override
		{
			return false;
		}
	};
}
