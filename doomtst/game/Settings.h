#pragma once
namespace settings {
	struct GlobalSettings
	{
		bool viewmode;
		GlobalSettings() {
			viewmode = true;
		}
	};
	extern GlobalSettings Gamesettings;
	
	void setinitialsettings();
}