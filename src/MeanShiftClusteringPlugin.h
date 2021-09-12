#pragma once

#include "AnalysisPlugin.h"
#include "SettingsAction.h"

#include "util/MeanShift.h"
#include "graphics/OffscreenBuffer.h"
#include "actions/Actions.h"

#include <QRandomGenerator>

using namespace hdps::plugin;

class PointsPlugin;

class MeanShiftClusteringPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT

public:
    MeanShiftClusteringPlugin(const PluginFactory* factory);

    ~MeanShiftClusteringPlugin() override;
    
    void init() override;

private:
    OffscreenBuffer     _offscreenBuffer;       /** Off-screen buffer */
    hdps::MeanShift     _meanShift;             /** Mean-shift analysis */
    SettingsAction      _settingsAction;        /** Settings action */
    QRandomGenerator    _rng;                   /** Random number generator for pseudo-random colors */
};

class MeanShiftClusteringPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.MeanShiftClusteringPlugin"
                      FILE  "MeanShiftClusteringPlugin.json")
    
public:
    MeanShiftClusteringPluginFactory(void) {}
    ~MeanShiftClusteringPluginFactory(void) override {}
    
    /** Returns the plugin icon */
    QIcon getIcon() const override;

    AnalysisPlugin* produce() override;

    hdps::DataTypes supportedDataTypes() const override;
};
