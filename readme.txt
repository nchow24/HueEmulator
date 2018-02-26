TO COMPILE PROGRAM: 'make'
TO EXECUTE/START SERVER: 'make start'

This project is a persistent web server/GUI for Phillips Hue users.

It was built on Linux and requires SteveyO Emulator and Wt Toolkit.

See mp4 file for brief demo.

Features:
	User account creation/login/logout
	Hue, brightness, saturation, and transition time changes for indiviudal lights
	Light groups with the same features as indvidual lights
	Adding and removing lights from groups
	Light scheduling 
	Preset and custom light modes
	Party light mode (hue looping with music)
	

General:
	Bug: a button needs to be clicked twice in order for the page to refresh with changes. Thus we have added an ‘Are you sure’ text that prompts user to click again. However the request goes through to the emulator on the first click (i.e., user is not truly able to change their mind after clicking the first time).
	When navigating through pages, use the buttons we put in the widget for navigation. Pages may have trouble loading if browser buttons/manually typed URLs are used instead.
	If the emulator is running on the same IP as the application, use loopback address 127.0.0.1.
	The port chosen on the emulator must match the port entered when registering the bridge.


Registration/Account Info:
	When logging in, click “Remember Me” so that login is not asked for again later on.
	After registering, immediately log out, then log back in and click remember me so that login is not asked for again later on.
	Bug: Please don’t use the account ‘guest’ as suggested on the log in page. We used it for debugging purposes.

Bridge Info
	Bug: Sometimes, adding a bridge may not cause the bridge’s button to appear. If this happens, enter the same information again and it should show up the next time.

Light/Group Info
	Turn on sound for party mode (music plays in the background).
	Adding groups will not work properly in (buttons of created groups will not show up) Emulator v7, but is working well in Emulator v6.

Scheduling Info
	Emulator v6 scheduling has problems with adding schedules after one has been deleted, just like with groups.
	Reoccurring scheduling is not implemented but we have partially worked on it, as seen in some of the widget objects on the scheduling page.
