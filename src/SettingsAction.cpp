#include "SettingsAction.h"
#include "Application.h"
#include "MeanShiftClusteringPlugin.h"

#include <util/Serialization.h>

using namespace mv::gui;

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
    setEnabled(false);
    {
        mv::util::variantMapMustContain(variantMap, "Dimension1");
        mv::util::variantMapMustContain(variantMap, "Dimension2");
        mv::util::variantMapMustContain(variantMap, "Sigma");
        mv::util::variantMapMustContain(variantMap, "ColorBy");
        mv::util::variantMapMustContain(variantMap, "ColorMap");
        mv::util::variantMapMustContain(variantMap, "RandomSeed");
        mv::util::variantMapMustContain(variantMap, "UpdateColorsManually");

        _dimensionOneAction.fromVariantMap(variantMap["Dimension1"].toMap());
        _dimensionTwoAction.fromVariantMap(variantMap["Dimension2"].toMap());
        _sigmaAction.fromVariantMap(variantMap["Sigma"].toMap());
        _colorByAction.fromVariantMap(variantMap["ColorBy"].toMap());
        _colorMapAction.fromVariantMap(variantMap["ColorMap"].toMap());
        _randomSeedAction.fromVariantMap(variantMap["RandomSeed"].toMap());
        _updateColorsManuallyAction.fromVariantMap(variantMap["UpdateColorsManually"].toMap());
    }
    setEnabled(true);
}

QVariantMap SettingsAction::toVariantMap() const
{
    return {
        { "Dimension1", _dimensionOneAction.toVariantMap() },
        { "Dimension2", _dimensionTwoAction.toVariantMap() },
        { "Sigma", _sigmaAction.toVariantMap() },
        { "ColorBy", _colorByAction.toVariantMap() },
        { "ColorMap", _colorMapAction.toVariantMap() },
        { "RandomSeed", _randomSeedAction.toVariantMap() },
        { "UpdateColorsManually", _updateColorsManuallyAction.toVariantMap() },
    };
}
