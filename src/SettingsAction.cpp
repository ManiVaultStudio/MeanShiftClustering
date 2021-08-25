#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin) :
    GroupAction(meanShiftClusteringPlugin, true),
    _meanShiftAnalysisPlugin(meanShiftClusteringPlugin),
    _sigmaAction(this, "Sigma", 0.001, 1.0, 0.15, 0.15, 2),
    _computeAction(this, "Compute"),
    _computationUpToDate(false)
{
    setText("Mean-shift");

    const auto update = [this]() -> void {
        _meanShiftAnalysisPlugin->getMeanShift().setSigma(_sigmaAction.getValue());
    };

    connect(&_sigmaAction, &DecimalAction::valueChanged, this, [this, update](const double& value) {
        setComputationUpToDate(false);
        update();
    });

    update();
}

QMenu* SettingsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->addAction(&_computeAction);

    return menu;
}

bool SettingsAction::isComputationUpToDate() const
{
    return _computationUpToDate;
}

void SettingsAction::setComputationUpToDate(const bool& computationUpToDate)
{
    _computationUpToDate = computationUpToDate;

    _computeAction.setEnabled(!_computationUpToDate);
}
