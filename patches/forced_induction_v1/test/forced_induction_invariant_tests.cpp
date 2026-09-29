#include <gtest/gtest.h>

#include "../include/engine.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
constexpr double AmbientPressure = 101325.0;
constexpr double AmbientTemperature = 298.15;

GasSystem::Mix airMix() {
    GasSystem::Mix mix;
    mix.p_fuel = 0.0;
    mix.p_inert = 0.75;
    mix.p_o2 = 0.25;
    return mix;
}

TurboGroup::Parameters groupParameters(int exhaustIndex = 0, int intakeIndex = 0) {
    TurboGroup::Parameters p;
    p.enabled = true;
    p.shaftInertia = 0.10;
    p.frictionTorque = 0.0;
    p.maxSpeed = 2000.0;
    p.maxPressureRatio = 2.0;
    p.compressorEfficiency = 0.70;
    p.turbineEfficiency = 0.70;
    p.designMassFlow = 0.20;
    p.turbineDesignPressureRatio = 2.0;
    p.turbineDesignTemperature = 800.0;
    p.preTurbineVolume = 0.010;
    p.preTurbineArea = 0.005;
    p.compressorInletVolume = 0.010;
    p.compressorDischargeVolume = 0.010;
    p.coolerVolume = 0.010;
    p.chargePlenumVolume = 0.020;
    p.chargeArea = 0.010;
    p.inletFlowRate = 0.20;
    p.passiveCompressorFlowRate = 0.20;
    p.coolerFlowRate = 0.20;
    p.chargeFlowRate = 0.20;
    p.turbineFlowRate = 0.20;
    p.exhaustSystemIndices = { exhaustIndex };
    p.exhaustScrollIndices = { 0 };
    p.intakeIndices = { intakeIndex };
    p.postTurbineExhaustIndex = exhaustIndex;
    return p;
}

class MinimalEngine {
public:
    MinimalEngine(
        const std::vector<TurboGroup::Parameters> &groups,
        int exhaustCount = 1,
        int intakeCount = 1,
        bool airOnly = true,
        bool closedIntake = false)
    {
        Engine::Parameters p{};
        p.exhaustSystemCount = exhaustCount;
        p.intakeCount = intakeCount;
        p.forcedInduction.groups = groups;
        engine.initialize(p);

        for (int i = 0; i < exhaustCount; ++i) {
            ExhaustSystem::Parameters exhaust{};
            exhaust.length = 2.0;
            exhaust.collectorCrossSectionArea = 0.010;
            exhaust.primaryTubeLength = 1.0;
            exhaust.primaryFlowRate = 0.20;
            exhaust.outletFlowRate = 0.0;
            exhaust.velocityDecay = 0.0;
            engine.getExhaustSystem(i)->initialize(exhaust);
        }

        for (int i = 0; i < intakeCount; ++i) {
            Intake::Parameters intake{};
            intake.volume = 0.020;
            intake.CrossSectionArea = 0.010;
            intake.InputFlowK = GasSystem::k_28inH2O(500.0);
            intake.IdleFlowK = 0.0;
            intake.RunnerFlowRate = 0.20;
            intake.IdleThrottlePlatePosition = closedIntake ? 1.0 : 0.0;
            intake.AirOnly = airOnly;
            engine.getIntake(i)->initialize(intake);
        }
        engine.configureForcedInductionGasPath();
    }

    ~MinimalEngine() { engine.destroy(); }

    Engine engine;
};

void setState(GasSystem *system, double pressure, double temperature) {
    system->reset(pressure, temperature, airMix());
}

void seedTurbine(MinimalEngine &fixture, std::size_t groupIndex = 0) {
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(groupIndex);
    setState(group->preTurbineSystem(0), 2.0 * AmbientPressure, 800.0);
    setState(
        fixture.engine.getExhaustSystem(group->parameters().postTurbineExhaustIndex)->getSystem(),
        AmbientPressure,
        400.0);
}
}

TEST(ForcedInductionDisabledPathInvariant, TurboDisabledSiUsesOriginalRoutes) {
    MinimalEngine fixture({}, 1, 1, false);
    ExhaustSystem *exhaust = fixture.engine.getExhaustSystem(0);
    Intake *intake = fixture.engine.getIntake(0);

    EXPECT_FALSE(fixture.engine.getForcedInductionSystem()->enabled());
    EXPECT_EQ(fixture.engine.getExhaustDestination(exhaust), exhaust->getSystem());
    EXPECT_FALSE(intake->hasForcedInductionFeed());
    EXPECT_FALSE(intake->isAirOnly());

    setState(intake->getSystem(), 0.80 * AmbientPressure, AmbientTemperature);
    const double before = intake->getSystem()->n();
    intake->process(1.0e-3);
    EXPECT_GT(intake->getSystem()->n(), before);
}

TEST(ForcedInductionCompressorInvariant, PassiveAtRestAndRealPoweredFlowWhenSpinning) {
    MinimalEngine fixture({ groupParameters() });
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    setState(fixture.engine.getIntake(0)->getSystem(), 0.70 * AmbientPressure, AmbientTemperature);

    double maxPassiveFlow = 0.0;
    for (int i = 0; i < 50; ++i) {
        fixture.engine.processForcedInduction(1.0e-4);
        maxPassiveFlow = std::max(maxPassiveFlow, group->telemetry().compressorMassFlow);
    }
    EXPECT_DOUBLE_EQ(group->rotatingAssembly()->shaftSpeed(), 0.0);
    EXPECT_DOUBLE_EQ(group->telemetry().compressorPressureRatio, 1.0);
    EXPECT_DOUBLE_EQ(group->telemetry().compressorPower, 0.0);
    EXPECT_GT(maxPassiveFlow, 0.0);
    EXPECT_LE(group->telemetry().chargePlenumPressure, 1.001 * AmbientPressure);

    group->rotatingAssembly()->advanceShaft(0.01, 100000.0, 0.0);
    const double speedBeforeCompression = group->rotatingAssembly()->shaftSpeed();
    fixture.engine.processForcedInduction(1.0e-4);
    EXPECT_GT(group->telemetry().compressorMassFlow, 0.0);
    EXPECT_GT(group->telemetry().compressorPower, 0.0);
    EXPECT_LT(group->rotatingAssembly()->shaftSpeed(), speedBeforeCompression);
}

TEST(ForcedInductionTurbineInvariant, RoutedRunnerGasCrossesRestrictionAndPowersShaft) {
    MinimalEngine fixture({ groupParameters() });
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    GasSystem *scroll = group->preTurbineSystem(0);
    GasSystem *destination = fixture.engine.getExhaustDestination(fixture.engine.getExhaustSystem(0));
    EXPECT_EQ(destination, scroll);

    GasSystem runner;
    runner.initialize(3.0 * AmbientPressure, 0.002, 900.0, airMix());
    GasSystem::FlowParameters flow{};
    flow.k_flow = GasSystem::flowConstant(
        0.20 / units::AirMolecularMass,
        3.0 * AmbientPressure,
        AmbientPressure,
        900.0,
        GasSystem::heatCapacityRatio(5));
    flow.dt = 1.0e-4;
    flow.direction_x = 1.0;
    flow.crossSectionArea_0 = 0.005;
    flow.crossSectionArea_1 = 0.005;
    flow.system_0 = &runner;
    flow.system_1 = destination;
    const double scrollMolesBefore = scroll->n();
    EXPECT_GT(GasSystem::flow(flow), 0.0);
    EXPECT_GT(scroll->n(), scrollMolesBefore);

    seedTurbine(fixture);
    fixture.engine.processForcedInduction(2.0e-4);
    EXPECT_GT(group->telemetry().turbineMassFlow, 0.0);
    EXPECT_GT(group->telemetry().turbinePower, 0.0);
    EXPECT_GT(group->telemetry().preTurbinePressure.front(), group->telemetry().postTurbinePressure);
    EXPECT_GT(group->rotatingAssembly()->shaftSpeed(), 0.0);
}

TEST(ForcedInductionEnergyInvariant, TurbineExtractionIsBoundedByTransferredGasEnergy) {
    MinimalEngine fixture({ groupParameters() });
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    seedTurbine(fixture);
    const double energyPerMole = group->preTurbineSystem(0)->kineticEnergyPerMol();
    constexpr double dt = 2.0e-4;

    fixture.engine.processForcedInduction(dt);
    const double transferredMoles =
        group->telemetry().turbineMassFlow * dt / units::AirMolecularMass;
    const double extractedEnergy = group->telemetry().turbinePower * dt;
    EXPECT_GT(transferredMoles, 0.0);
    EXPECT_LE(extractedEnergy, transferredMoles * energyPerMole + 1.0e-8);
}

TEST(ForcedInductionOptionalDeviceInvariant, DisabledDevicesAreInertAndEnabledDevicesMoveGas) {
    {
        auto p = groupParameters();
        p.wastegatePosition = 1.0;
        p.compressorBypassPosition = 1.0;
        p.vgtPosition = 0.0;
        MinimalEngine fixture({ p });
        MinimalEngine reference({ groupParameters() });
        TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
        TurboGroup *referenceGroup = reference.engine.getForcedInductionSystem()->group(0);
        seedTurbine(fixture);
        seedTurbine(reference);
        fixture.engine.processForcedInduction(2.0e-4);
        reference.engine.processForcedInduction(2.0e-4);
        EXPECT_DOUBLE_EQ(group->telemetry().wastegateMassFlow, 0.0);
        EXPECT_DOUBLE_EQ(group->telemetry().bypassMassFlow, 0.0);
        EXPECT_DOUBLE_EQ(
            group->telemetry().turbineMassFlow,
            referenceGroup->telemetry().turbineMassFlow);
    }

    {
        auto p = groupParameters();
        p.wastegateEnabled = true;
        p.wastegateFlowRate = 0.20;
        p.wastegatePosition = 1.0;
        MinimalEngine fixture({ p });
        TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
        seedTurbine(fixture);
        const double postBefore = fixture.engine.getExhaustSystem(0)->getSystem()->n();
        fixture.engine.processForcedInduction(2.0e-4);
        EXPECT_GT(group->telemetry().wastegateMassFlow, 0.0);
        EXPECT_GT(fixture.engine.getExhaustSystem(0)->getSystem()->n(), postBefore);
    }

    {
        auto p = groupParameters();
        p.compressorBypassEnabled = true;
        p.compressorBypassFlowRate = 0.20;
        p.compressorBypassPosition = 1.0;
        p.compressorBypassRecirculates = false;
        MinimalEngine fixture({ p });
        TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
        group->rotatingAssembly()->advanceShaft(0.01, 20000000.0, 0.0);
        fixture.engine.processForcedInduction(2.0e-4);
        EXPECT_GT(group->telemetry().bypassMassFlow, 0.0);
    }

    auto open = groupParameters();
    open.vgtEnabled = true;
    open.vgtMinFlowFactor = 0.20;
    open.vgtPosition = 1.0;
    auto closed = open;
    closed.vgtPosition = 0.0;
    MinimalEngine openFixture({ open });
    MinimalEngine closedFixture({ closed });
    seedTurbine(openFixture);
    seedTurbine(closedFixture);
    openFixture.engine.processForcedInduction(2.0e-4);
    closedFixture.engine.processForcedInduction(2.0e-4);
    EXPECT_GT(
        openFixture.engine.getForcedInductionSystem()->group(0)->telemetry().turbineMassFlow,
        closedFixture.engine.getForcedInductionSystem()->group(0)->telemetry().turbineMassFlow);
}

TEST(ForcedInductionMultiGroupInvariant, SeparateScrollsAndShaftsMayFeedSharedIntake) {
    auto first = groupParameters(0, 0);
    auto second = groupParameters(1, 0);
    MinimalEngine fixture({ first, second }, 2, 1);
    ForcedInductionSystem *system = fixture.engine.getForcedInductionSystem();
    TurboGroup *group0 = system->group(0);
    TurboGroup *group1 = system->group(1);

    ASSERT_NE(group0, nullptr);
    ASSERT_NE(group1, nullptr);
    EXPECT_NE(group0->preTurbineSystem(0), group1->preTurbineSystem(0));
    EXPECT_EQ(
        fixture.engine.getExhaustDestination(fixture.engine.getExhaustSystem(0)),
        group0->preTurbineSystem(0));
    EXPECT_EQ(
        fixture.engine.getExhaustDestination(fixture.engine.getExhaustSystem(1)),
        group1->preTurbineSystem(0));
    EXPECT_TRUE(system->managesIntake(0));

    group0->rotatingAssembly()->advanceShaft(0.01, 100000.0, 0.0);
    EXPECT_GT(group0->rotatingAssembly()->shaftSpeed(), 0.0);
    EXPECT_DOUBLE_EQ(group1->rotatingAssembly()->shaftSpeed(), 0.0);
}

TEST(ForcedInductionMassBalanceInvariant, ClosedTransfersConserveMolesAndReportedChargeMatchesDelivery) {
    auto p = groupParameters();
    p.wastegateEnabled = true;
    p.wastegateFlowRate = 0.20;
    p.wastegatePosition = 1.0;
    MinimalEngine fixture({ p });
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    GasSystem *scroll = group->preTurbineSystem(0);
    GasSystem *post = fixture.engine.getExhaustSystem(0)->getSystem();
    seedTurbine(fixture);

    group->rotatingAssembly()->advanceShaft(0.01, 100000.0, 0.0);
    for (int i = 0; i < 5; ++i) fixture.engine.processForcedInduction(1.0e-4);

    const double exhaustMolesBefore = scroll->n() + post->n();
    const double intakeMolesBefore = fixture.engine.getIntake(0)->getSystem()->n();
    constexpr double dt = 1.0e-4;
    fixture.engine.processForcedInduction(dt);
    const double exhaustMolesAfter = scroll->n() + post->n();
    const double intakeMolesAfter = fixture.engine.getIntake(0)->getSystem()->n();
    const double reportedIntakeMoles =
        group->telemetry().intakeMassFlow * dt / units::AirMolecularMass;

    EXPECT_NEAR(exhaustMolesAfter, exhaustMolesBefore, exhaustMolesBefore * 1.0e-12);
    EXPECT_GT(reportedIntakeMoles, 0.0);
    EXPECT_NEAR(
        intakeMolesAfter - intakeMolesBefore,
        reportedIntakeMoles,
        std::max(1.0e-12, reportedIntakeMoles * 1.0e-10));
}
