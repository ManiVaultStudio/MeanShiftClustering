#pragma once

#include "actions/Actions.h"

class QMenu;
class MeanShiftClusteringPlugin;

using namespace hdps::gui;

class SettingsAction : public GroupAction
{
public:
    SettingsAction(MeanShiftClusteringPlugin* meanShiftAnalysisPlugin);

    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    DecimalAction& getSigmaAction() { return _sigmaAction; }
    TriggerAction& getComputeAction() { return _computeAction; }

protected:
    DecimalAction    _sigmaAction;      /** Sigma action */
    TriggerAction    _computeAction;    /** Compute action */
};