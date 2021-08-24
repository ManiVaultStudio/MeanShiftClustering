#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftAnalysisPlugin) :
    GroupAction(meanShiftAnalysisPlugin, true),
    _sigmaAction(this, "Sigma"),
    _computeAction(this, "Compute")
{
    setText("Settings");
}

QMenu* SettingsAction::getContextMenu()
{
    auto menu = new QMenu(text());

    menu->addAction(&_computeAction);

    return menu;
}