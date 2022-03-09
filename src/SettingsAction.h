#pragma once

#include "actions/Actions.h"

class QMenu;
class MeanShiftClusteringPlugin;

using namespace hdps::gui;

/**
 * Settings action class
 *
 * Action class for mean-shift clustering settings
 *
 * @author Thomas Kroes
 */
class SettingsAction : public GroupAction
{
public:

    /** Color options */
    enum class ColorBy {
        PseudoRandomColors,     /** Use pseudo-random colors */
        ColorMap                /** Color by continuous color map */
    };

public:
    /** 
     * Constructor
     * @param meanShiftClusteringPlugin Pointer to mean-shift clustering plugin
     */
    SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    OptionAction& getDimensionOneAction() { return _dimensionOneAction; }
    OptionAction& getDimensionTwoAction() { return _dimensionTwoAction; }
    DecimalAction& getSigmaAction() { return _sigmaAction; }
    OptionAction& getColorByAction() { return _colorByAction; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    IntegralAction& getRandomSeedAction() { return _randomSeedAction; }
    ToggleAction& getUpdateColorsManuallyAction() { return _updateColorsManuallyAction; }
    TriggerAction& getApplyColorsAction() { return _applyColorsAction; }
    TriggerAction& getComputeAction() { return _computeAction; }

protected:
    MeanShiftClusteringPlugin*  _meanShiftClusteringPlugin;     /** Pointer to mean-shift clustering plugin */
    OptionAction                _dimensionOneAction;            /** First dimension action */
    OptionAction                _dimensionTwoAction;            /** Second dimension action */
    DecimalAction               _sigmaAction;                   /** Sigma action */
    OptionAction                _colorByAction;                 /** Color by options action */
    ColorMapAction              _colorMapAction;                /** Color map action */
    IntegralAction              _randomSeedAction;              /** Random seed action */
    ToggleAction                _updateColorsManuallyAction;    /** Update colors manually action */
    TriggerAction               _applyColorsAction;             /** Apply colors action */
    TriggerAction               _computeAction;                 /** Compute action */
};
