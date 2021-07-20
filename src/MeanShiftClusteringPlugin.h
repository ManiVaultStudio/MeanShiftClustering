#pragma once

#include "AnalysisPlugin.h"

#include "MeanShiftSettingsWidget.h"

#include "util/MeanShift.h"
#include "graphics/OffscreenBuffer.h"

// =============================================================================
// View
// =============================================================================

using namespace hdps::plugin;

class PointsPlugin;

class MeanShiftAnalysisPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    MeanShiftAnalysisPlugin() :
        AnalysisPlugin("Mean Shift Analysis")
    {

    }

    ~MeanShiftAnalysisPlugin(void) override;
    
    void init() override;

    void onDataEvent(hdps::DataEvent* dataEvent);

    SettingsWidget* const getSettings() override;
public slots:
    void dataSetPicked(const QString& name);

    void startComputation();

private:
    OffscreenBuffer offscreen;
    hdps::MeanShift _meanShift;

    MeanShiftSettingsWidget _settings;
};

// =============================================================================
// Factory
// =============================================================================

class MeanShiftAnalysisPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.MeanShiftAnalysisPlugin"
                      FILE  "MeanShiftClusteringPlugin.json")
    
public:
    MeanShiftAnalysisPluginFactory(void) {}
    ~MeanShiftAnalysisPluginFactory(void) override {}
    
    AnalysisPlugin* produce() override;
};
