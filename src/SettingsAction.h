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
        PsuedoRandomColors,     /** Use pseudo-random colors */
        ColorMap                /** Color by continuous color map */
    };

public:
    /** 
     * Constructor
     * @param meanShiftClusteringPlugin Pointer to mean-shift clustering plugin
     */
    SettingsAction(MeanShiftClusteringPlugin* meanShiftClusteringPlugin);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

public: // Action getters

    DecimalAction& getSigmaAction() { return _sigmaAction; }
    OptionAction& getColorByAction() { return _colorByAction; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    IntegralAction& getRandomSeedAction() { return _randomSeedAction; }
    ToggleAction& getUpdateColorsManuallyAction() { return _updateColorsManuallyAction; }
    TriggerAction& getApplyColorsAction() { return _applyColorsAction; }
    TriggerAction& getComputeAction() { return _computeAction; }

protected:
    MeanShiftClusteringPlugin*  _meanShiftAnalysisPlugin;       /** Pointer to mean-shift clustering plugin */
    DecimalAction               _sigmaAction;                   /** Sigma action */
    OptionAction                _colorByAction;                 /** Color by options action */
    ColorMapAction              _colorMapAction;                /** Color map action */
    IntegralAction              _randomSeedAction;              /** Random seed action */
    ToggleAction                _updateColorsManuallyAction;    /** Update colors manually action */
    TriggerAction               _applyColorsAction;             /** Apply colors action */
    TriggerAction               _computeAction;                 /** Compute action */
};
