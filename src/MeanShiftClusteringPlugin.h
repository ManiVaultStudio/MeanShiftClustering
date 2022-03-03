#pragma once

#include "AnalysisPlugin.h"
#include "SettingsAction.h"

#include "util/MeanShift.h"
#include "graphics/OffscreenBuffer.h"
#include "actions/Actions.h"

#include <QRandomGenerator>

using namespace hdps::plugin;

class PointsPlugin;

class MeanShiftClusteringPlugin : public AnalysisPlugin
{
    Q_OBJECT

public:
    MeanShiftClusteringPlugin(const PluginFactory* factory);

    ~MeanShiftClusteringPlugin() override = default;
    
    void init() override;

    /** Establishes whether a computation can be executed */
    bool canCompute() const;

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
    Q_PLUGIN_METADATA(IID   "nl.BioVault.MeanShiftClusteringPlugin"
                      FILE  "MeanShiftClusteringPlugin.json")
    
public:
    MeanShiftClusteringPluginFactory(void) {}
    ~MeanShiftClusteringPluginFactory(void) override {}

    /** Returns the plugin icon */
    QIcon getIcon() const override;

    AnalysisPlugin* produce() override;

    hdps::DataTypes supportedDataTypes() const override;
};
