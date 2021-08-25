#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftAnalysisPlugin) :
    GroupAction(meanShiftAnalysisPlugin, true),
    _sigmaAction(this, "Sigma", 1.0, 100.0, 1.0, 1.0, 1),
    _computeAction(this, "Compute")
{
    setText("Mean-shift");
}

QMenu* SettingsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->addAction(&_computeAction);

    return menu;
}