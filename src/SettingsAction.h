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

    /** Get whether the settings are dirty */
    bool isComputationUpToDate() const;

    /**
     * Set whether computation results are up-to-date
     * @param computationUpToDate Whether computation results are up-to-date
     */
    void setComputationUpToDate(const bool& computationUpToDate);

public: // Action getters

    DecimalAction& getSigmaAction() { return _sigmaAction; }
    TriggerAction& getComputeAction() { return _computeAction; }

protected:
    MeanShiftClusteringPlugin*  _meanShiftAnalysisPlugin;       /** Pointer to mean-shift clustering plugin */
    DecimalAction               _sigmaAction;                   /** Sigma action */
    TriggerAction               _computeAction;                 /** Compute action */
    bool                        _computationUpToDate;           /** Whether computation results are up-to-date */
};