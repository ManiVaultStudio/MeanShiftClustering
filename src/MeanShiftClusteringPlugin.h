#pragma once

#include "AnalysisPlugin.h"
#include "SettingsAction.h"

#include "util/MeanShift.h"
#include "graphics/OffscreenBuffer.h"
#include "actions/Actions.h"

using namespace hdps::plugin;

class PointsPlugin;

class MeanShiftClusteringPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    MeanShiftClusteringPlugin(const PluginFactory* factory);

    ~MeanShiftClusteringPlugin(void) override;
    
    void init() override;

    hdps::MeanShift& getMeanShift() {
        return _meanShift;
    };

private:
    OffscreenBuffer     _offscreenBuffer;
    hdps::MeanShift     _meanShift;
    SettingsAction      _settingsAction;
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
    
    AnalysisPlugin* produce() override;

    hdps::DataTypes supportedDataTypes() const override;
};
