#include "SettingsAction.h"

#include "MeanShiftClusteringPlugin.h"

#include <util/Serialization.h>

using namespace mv::gui;
using namespace mv::util;

SettingsAction::SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin) :
    GroupAction(meanShiftClusteringPlugin, "SettingsAction", true),
    _meanShiftClusteringPlugin(meanShiftClusteringPlugin),
    _dimensionOneAction(this, "Dimension 1"),
    _dimensionTwoAction(this, "Dimension 2"),
    _sigmaAction(this, "Sigma", 0.01, 0.5, 0.15, 3),
    _colorByAction(this, "Color by", QStringList({"Pseudo-random colors", "Color map"}), "Color map"),
    _colorMapAction(this, "Color map"),
    _randomSeedAction(this, "Random seed"),
    _updateColorsManuallyAction(this, "Update colors manually", false),
    _applyColorsAction(this, "Apply colors"),
    _computeAction(this, "Compute")
{
    setText("Settings");

    _sigmaAction.setUpdateDuringDrag(false);
    _randomSeedAction.setUpdateDuringDrag(false);
    _computeAction.setVisible(false);

    addAction(&_dimensionOneAction);
    addAction(&_dimensionTwoAction);
    addAction(&_sigmaAction);
    addAction(&_colorByAction);
    addAction(&_colorMapAction);
    addAction(&_randomSeedAction);
    addAction(&_updateColorsManuallyAction);
    addAction(&_applyColorsAction);
    addAction(&_computeAction);

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

void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    setEnabled(false);
    {
        _dimensionOneAction.fromParentVariantMap(variantMap);
        _dimensionTwoAction.fromParentVariantMap(variantMap);
        _sigmaAction.fromParentVariantMap(variantMap);
        _colorByAction.fromParentVariantMap(variantMap);
        _colorMapAction.fromParentVariantMap(variantMap);
        _randomSeedAction.fromParentVariantMap(variantMap);
        _updateColorsManuallyAction.fromParentVariantMap(variantMap);
    }
    setEnabled(true);
}

QVariantMap SettingsAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _dimensionOneAction.insertIntoVariantMap(variantMap);
    _dimensionTwoAction.insertIntoVariantMap(variantMap);
    _sigmaAction.insertIntoVariantMap(variantMap);
    _colorByAction.insertIntoVariantMap(variantMap);
    _colorMapAction.insertIntoVariantMap(variantMap);
    _randomSeedAction.insertIntoVariantMap(variantMap);
    _updateColorsManuallyAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
