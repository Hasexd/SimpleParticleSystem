#include <stdio.h>

#include "App.h"

int main()
{
	App app;

	AppInit(&app, "Simple Particle System", 1080, 720);
	AppRun(&app);
	AppShutdown(&app);
}