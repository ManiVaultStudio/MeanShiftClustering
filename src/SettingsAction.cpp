#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin) :
    GroupAction(meanShiftClusteringPlugin, true),
    _meanShiftAnalysisPlugin(meanShiftClusteringPlugin),
    _sigmaAction(this, "Sigma", 0.001, 1.0, 0.15, 0.15, 2),
    _colorByAction(this, "Color by", QStringList({"Pseudo-random colors", "Color map"}), "Pseudo-random colors", "Pseudo-random colors"),
    _colorMapAction(this, "Color map"),
    _randomSeedAction(this, "Random seed"),
    _computeAction(this, "Compute")
{
    setText("Mean-shift");

    _sigmaAction.setUpdateDuringDrag(false);
    _randomSeedAction.setUpdateDuringDrag(false);
    _computeAction.setVisible(false);

    const auto updateReadOnly = [this]() -> void {
        const auto enabled  = !isReadOnly();
        const auto colorBy  = static_cast<ColorBy>(_colorByAction.getCurrentIndex());

        _colorMapAction.setEnabled(enabled && colorBy == ColorBy::ColorMap);
        _randomSeedAction.setEnabled(enabled && colorBy == ColorBy::PsuedoRandomColors);
    };

    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this, updateReadOnly](const std::int32_t& currentIndex) {
        updateReadOnly();
    });

    updateReadOnly();
}

QMenu* SettingsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(text(), parent);

    menu->addAction(&_computeAction);

    return menu;
}
