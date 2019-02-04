#include "CentralComponent.h"

CentralComponent::CentralComponent()
	: Component("Central Component")
	, openGLDrawer(&openGLContext)
	, menuSidePanel("Menu", 500, false)
{
	menuSidePanel.setShadowWidth(0);

	menuButton.setButtonText("menu");
	menuButton.onClick = [this] {menuSidePanel.showOrHide(!menuSidePanel.isPanelShowing()); };
	addAndMakeVisible(menuButton);

	shouldDraw.setButtonText("Draw");
	shouldDraw.onClick = [this] { menuSidePanel.showOrHide(false);};
	addAndMakeVisible(shouldDraw);

	auto configReturn = configureMenu();
	shouldCallWhenResized = std::move(configReturn.snd);
	menuSidePanel.setContent(configReturn.fst.release(), true);
	addAndMakeVisible(menuSidePanel);

	openGLContext.attachTo(*this);
}

CentralComponent::~CentralComponent() {
	openGLContext.detach();
	openGLContext.setRenderer(nullptr);
}

void CentralComponent::paint(Graphics&) {}

void CentralComponent::resized() {
	auto const renderingScale = static_cast<float>(openGLContext.getRenderingScale());
	auto const width = getWidth();
	auto const height = getHeight();
	//openGLDrawer.setBounds(0, 0, std::ceil(width * renderingScale) / pointSize, std::ceil(height * renderingScale) / pointSize);
	shouldCallWhenResized();
	menuButton.setBounds(0, 0, static_cast<int>(width * 0.05), static_cast<int>(height * 0.05));
	shouldDraw.setBounds(static_cast<int>(width * 0.05), 0, static_cast<int>(width * 0.10), static_cast<int>(height * 0.05));
}

Pair<std::unique_ptr<Menu>, std::function<void()> > CentralComponent::configureMenu() {
	using std::move;
	using std::make_unique;

	auto menu = make_unique<Menu>();
	std::vector<std::function<void()> > functionsForReturn;

	int radioGroupID = 1;

	//=========================================================================

	//=========================================================================

	return { move(menu),[funcs = move(functionsForReturn)] {for (auto & foo : funcs) foo(); } }; // menu && shouldCallWhenResized
}