#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Menu.h"
#include "OpenGLDrawer.h"
#include "Pair.h"

class CentralComponent
	: public Component
{
public:
	CentralComponent();
	~CentralComponent();

	void paint(Graphics&);
	void resized();

	Pair<std::unique_ptr<Menu>, std::function<void()> > configureMenu();

private:
#if JUCE_OPENGL
	OpenGLContext openGLContext;
#endif
	OpenGLDrawer openGLDrawer;
	SidePanel menuSidePanel;
	TextButton menuButton;

	TextButton shouldDraw;

	std::function<void()> shouldCallWhenResized;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CentralComponent)
};
