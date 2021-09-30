#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin) :
    GroupAction(meanShiftClusteringPlugin, true),
    _meanShiftAnalysisPlugin(meanShiftClusteringPlugin),
    _dimensionOneAction(this, "Dimension 1"),
    _dimensionTwoAction(this, "Dimension 2"),
    _sigmaAction(this, "Sigma", 0.01f, 1.0f, 0.15f, 0.15f, 2.0f),
    _colorByAction(this, "Color by", QStringList({"Pseudo-random colors", "Color map"}), "Pseudo-random colors", "Pseudo-random colors"),
    _colorMapAction(this, "Color map"),
    _randomSeedAction(this, "Random seed"),
    _updateColorsManuallyAction(this, "Update colors manually", false, false),
    _applyColorsAction(this, "Apply colors"),
    _computeAction(this, "Compute")
{
    setText("Mean-shift");

    /*
    _dimensionOneAction.setDefaultWidgetFlags(OptionAction::All);
    _dimensionTwoAction.setDefaultWidgetFlags(OptionAction::All);
    _sigmaAction.setDefaultWidgetFlags(IntegralAction::All);
    _colorByAction.setDefaultWidgetFlags(OptionAction::All);
    _colorMapAction.setDefaultWidgetFlags(ColorMapAction::Settings | ColorMapAction::ResetPushButton);
    _randomSeedAction.setDefaultWidgetFlags(IntegralAction::SpinBoxAndReset);
    _updateColorsManuallyAction.setDefaultWidgetFlags(ToggleAction::CheckBoxAndResetPushButton);
    */

    _sigmaAction.setUpdateDuringDrag(false);
    _randomSeedAction.setUpdateDuringDrag(false);
    _computeAction.setVisible(false);

    const auto updateReadOnly = [this]() -> void {
        const auto enabled  = !isReadOnly();
        const auto colorBy  = static_cast<ColorBy>(_colorByAction.getCurrentIndex());

        _colorMapAction.setEnabled(enabled && colorBy == ColorBy::ColorMap);
        _randomSeedAction.setEnabled(enabled && colorBy == ColorBy::PseudoRandomColors);
        _applyColorsAction.setEnabled(enabled && _updateColorsManuallyAction.isChecked());
    };

    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this, updateReadOnly](const std::int32_t& currentIndex) {
        updateReadOnly();
    });

    connect(&_updateColorsManuallyAction, &ToggleAction::toggled, this, [this, updateReadOnly](bool toggled) {
        updateReadOnly();
    });

    updateReadOnly();
}
