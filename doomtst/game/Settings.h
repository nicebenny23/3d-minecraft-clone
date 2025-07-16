#pragma once
namespace settings {
	struct GlobalSettings
	{
		bool viewmode;
		GlobalSettings() {
			viewmode = false;
		}
	};
	extern GlobalSettings Gamesettings;
	
	void setinitialsettings();
}