#include <gtest/gtest.h>

#include "../include/engine.h"
#include "../include/units.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
constexpr double AmbientPressure = 101325.0;
constexpr double AmbientTemperature = 298.15;
constexpr double Step = 1.0e-4;

GasSystem::Mix airMix() {
    GasSystem::Mix mix;
    mix.p_fuel = 0.0;
    mix.p_inert = 0.75;
    mix.p_o2 = 0.25;
    return mix;
}

TurboGroup::Parameters groupParameters(
    const std::vector<int> &exhaustIndices = { 0 },
    const std::vector<int> &scrollIndices = { 0 },
    const std::vector<int> &intakeIndices = { 0 })
{
    TurboGroup::Parameters p;
    p.enabled = true;
    p.shaftInertia = 0.10;
    p.frictionTorque = 0.01;
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
    p.exhaustSystemIndices = exhaustIndices;
    p.exhaustScrollIndices = scrollIndices;
    p.intakeIndices = intakeIndices;
    p.postTurbineExhaustIndex = exhaustIndices.front();
    return p;
}

class SyntheticEngine {
public:
    SyntheticEngine(
        const std::vector<TurboGroup::Parameters> &groups,
        int exhaustCount,
        int intakeCount,
        bool airOnly)
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
            exhaust.outletFlowRate = 0.20;
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
            intake.IdleThrottlePlatePosition = 1.0;
            intake.AirOnly = airOnly;
            engine.getIntake(i)->initialize(intake);
        }
        engine.configureForcedInductionGasPath();
    }

    ~SyntheticEngine() { engine.destroy(); }

    void processExhausts(double dt) {
        for (int i = 0; i < engine.getExhaustSystemCount(); ++i) {
            engine.getExhaustSystem(i)->process(dt);
        }
    }

    Engine engine;
};

void initializeSupply(GasSystem &supply, double pressure) {
    supply.initialize(pressure, 1.0, 850.0, airMix());
}

double feedScroll(GasSystem &supply, GasSystem *scroll, double dt = Step) {
    GasSystem::FlowParameters flow{};
    flow.k_flow = GasSystem::flowConstant(
        0.20 / units::AirMolecularMass,
        3.0 * AmbientPressure,
        2.0 * AmbientPressure,
        850.0,
        GasSystem::heatCapacityRatio(5));
    flow.dt = dt;
    flow.direction_x = 1.0;
    flow.crossSectionArea_0 = 0.005;
    flow.crossSectionArea_1 = 0.005;
    flow.system_0 = &supply;
    flow.system_1 = scroll;
    return std::max(0.0, GasSystem::flow(flow));
}

double drainIntake(GasSystem *intake, GasSystem &sink, double dt = Step) {
    GasSystem::FlowParameters flow{};
    flow.k_flow = GasSystem::flowConstant(
        0.15 / units::AirMolecularMass,
        AmbientPressure,
        0.20 * AmbientPressure,
        AmbientTemperature,
        GasSystem::heatCapacityRatio(5));
    flow.dt = dt;
    flow.direction_x = 1.0;
    flow.crossSectionArea_0 = 0.010;
    flow.crossSectionArea_1 = 0.010;
    flow.system_0 = intake;
    flow.system_1 = &sink;
    return std::max(0.0, GasSystem::flow(flow));
}

void expectStable(const TurboGroup &group) {
    const TurboGroup::Telemetry &t = group.telemetry();
    EXPECT_TRUE(std::isfinite(t.shaftSpeed));
    EXPECT_TRUE(std::isfinite(t.compressorMassFlow));
    EXPECT_TRUE(std::isfinite(t.turbineMassFlow));
    EXPECT_TRUE(std::isfinite(t.chargePlenumPressure));
    EXPECT_TRUE(std::isfinite(t.postTurbinePressure));
    EXPECT_GE(t.shaftSpeed, 0.0);
    EXPECT_LE(t.shaftSpeed, group.parameters().maxSpeed + 1.0e-9);
    EXPECT_GE(t.compressorMassFlow, 0.0);
    EXPECT_GE(t.turbineMassFlow, 0.0);
    EXPECT_GT(t.chargePlenumPressure, 0.0);
    EXPECT_GT(t.postTurbinePressure, 0.0);
    for (double pressure : t.preTurbinePressure) {
        EXPECT_TRUE(std::isfinite(pressure));
        EXPECT_GT(pressure, 0.0);
    }
}
}

TEST(ForcedInductionRuntimeSmoke, NaturallyAspiratedSiKeepsOriginalStablePath) {
    SyntheticEngine fixture({}, 1, 1, false);
    Intake *intake = fixture.engine.getIntake(0);
    ExhaustSystem *exhaust = fixture.engine.getExhaustSystem(0);
    intake->getSystem()->reset(0.75 * AmbientPressure, AmbientTemperature, airMix());
    fixture.engine.setThrottle(0.25);
    const double intakeMolesBefore = intake->getSystem()->n();

    for (int i = 0; i < 200; ++i) intake->process(Step);

    EXPECT_FALSE(fixture.engine.getForcedInductionSystem()->enabled());
    EXPECT_FALSE(intake->hasForcedInductionFeed());
    EXPECT_FALSE(intake->isAirOnly());
    EXPECT_EQ(fixture.engine.getExhaustDestination(exhaust), exhaust->getSystem());
    EXPECT_GT(intake->getSystem()->n(), intakeMolesBefore);
    EXPECT_TRUE(std::isfinite(intake->getSystem()->pressure()));
    EXPECT_GT(intake->getSystem()->pressure(), 0.0);
    EXPECT_LE(intake->getSystem()->pressure(), 1.01 * AmbientPressure);
}

TEST(ForcedInductionRuntimeSmoke, ThrottledSiAirDeliveryRespondsToThrottle) {
    auto p = groupParameters();
    p.throttleEnabled = true;
    SyntheticEngine openFixture({ p }, 1, 1, false);
    SyntheticEngine closedFixture({ p }, 1, 1, false);
    openFixture.engine.setThrottle(0.0);
    closedFixture.engine.setThrottle(1.0);

    GasSystem openSupply;
    GasSystem closedSupply;
    GasSystem openSink;
    GasSystem closedSink;
    initializeSupply(openSupply, 3.0 * AmbientPressure);
    initializeSupply(closedSupply, 3.0 * AmbientPressure);
    openSink.initialize(0.70 * AmbientPressure, 1.0, AmbientTemperature, airMix());
    closedSink.initialize(0.70 * AmbientPressure, 1.0, AmbientTemperature, airMix());
    double openDelivery = 0.0;
    double closedDelivery = 0.0;

    for (int i = 0; i < 250; ++i) {
        TurboGroup *open = openFixture.engine.getForcedInductionSystem()->group(0);
        TurboGroup *closed = closedFixture.engine.getForcedInductionSystem()->group(0);
        feedScroll(openSupply, open->preTurbineSystem(0));
        feedScroll(closedSupply, closed->preTurbineSystem(0));
        openFixture.engine.processForcedInduction(Step);
        closedFixture.engine.processForcedInduction(Step);
        openDelivery += open->telemetry().intakeMassFlow * Step;
        closedDelivery += closed->telemetry().intakeMassFlow * Step;
        drainIntake(openFixture.engine.getIntake(0)->getSystem(), openSink);
        drainIntake(closedFixture.engine.getIntake(0)->getSystem(), closedSink);
        openFixture.processExhausts(Step);
        closedFixture.processExhausts(Step);
    }

    TurboGroup *open = openFixture.engine.getForcedInductionSystem()->group(0);
    TurboGroup *closed = closedFixture.engine.getForcedInductionSystem()->group(0);
    EXPECT_FALSE(openFixture.engine.getIntake(0)->isAirOnly());
    EXPECT_GT(openDelivery, 0.0);
    EXPECT_GT(openDelivery, 10.0 * closedDelivery);
    EXPECT_GT(open->telemetry().turbinePower, 0.0);
    expectStable(*open);
    expectStable(*closed);
}

TEST(ForcedInductionRuntimeSmoke, UnthrottledAirPathTransitionsFromPassiveToPoweredFlow) {
    SyntheticEngine fixture({ groupParameters() }, 1, 1, true);
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    GasSystem supply;
    GasSystem sink;
    initializeSupply(supply, 3.0 * AmbientPressure);
    sink.initialize(0.70 * AmbientPressure, 1.0, AmbientTemperature, airMix());
    fixture.engine.getIntake(0)->getSystem()->reset(
        0.75 * AmbientPressure, AmbientTemperature, airMix());

    double passiveFlow = 0.0;
    for (int i = 0; i < 30; ++i) {
        fixture.engine.processForcedInduction(Step);
        passiveFlow = std::max(passiveFlow, group->telemetry().compressorMassFlow);
        drainIntake(fixture.engine.getIntake(0)->getSystem(), sink);
    }

    double poweredFlow = 0.0;
    double deliveredAir = 0.0;
    for (int i = 0; i < 250; ++i) {
        feedScroll(supply, group->preTurbineSystem(0));
        fixture.engine.processForcedInduction(Step);
        poweredFlow = std::max(poweredFlow, group->telemetry().compressorPower);
        deliveredAir += group->telemetry().intakeMassFlow * Step;
        drainIntake(fixture.engine.getIntake(0)->getSystem(), sink);
        fixture.processExhausts(Step);
    }

    EXPECT_TRUE(fixture.engine.getIntake(0)->isAirOnly());
    EXPECT_GT(passiveFlow, 0.0);
    EXPECT_GT(group->rotatingAssembly()->shaftSpeed(), 0.0);
    EXPECT_GT(poweredFlow, 0.0);
    EXPECT_GT(deliveredAir, 0.0);
    expectStable(*group);
}

TEST(ForcedInductionRuntimeSmoke, ParallelGroupsRemainIndependentWhileFeedingSharedIntake) {
    auto first = groupParameters({ 0 }, { 0 }, { 0 });
    auto second = groupParameters({ 1 }, { 0 }, { 0 });
    SyntheticEngine fixture({ first, second }, 2, 1, true);
    ForcedInductionSystem *system = fixture.engine.getForcedInductionSystem();
    TurboGroup *group0 = system->group(0);
    TurboGroup *group1 = system->group(1);
    GasSystem supply0;
    GasSystem supply1;
    GasSystem sink;
    initializeSupply(supply0, 3.0 * AmbientPressure);
    initializeSupply(supply1, 2.5 * AmbientPressure);
    sink.initialize(0.70 * AmbientPressure, 1.0, AmbientTemperature, airMix());
    double delivery0 = 0.0;
    double delivery1 = 0.0;

    for (int i = 0; i < 250; ++i) {
        feedScroll(supply0, group0->preTurbineSystem(0));
        feedScroll(supply1, group1->preTurbineSystem(0));
        fixture.engine.processForcedInduction(Step);
        delivery0 += group0->telemetry().intakeMassFlow * Step;
        delivery1 += group1->telemetry().intakeMassFlow * Step;
        drainIntake(fixture.engine.getIntake(0)->getSystem(), sink);
        fixture.processExhausts(Step);
    }

    EXPECT_NE(group0->preTurbineSystem(0), group1->preTurbineSystem(0));
    EXPECT_NE(group0->rotatingAssembly(), group1->rotatingAssembly());
    EXPECT_TRUE(system->managesIntake(0));
    EXPECT_GT(group0->rotatingAssembly()->shaftSpeed(), 0.0);
    EXPECT_GT(group1->rotatingAssembly()->shaftSpeed(), 0.0);
    EXPECT_GT(delivery0, 0.0);
    EXPECT_GT(delivery1, 0.0);
    expectStable(*group0);
    expectStable(*group1);
}

TEST(ForcedInductionRuntimeSmoke, TwinScrollRoutingRetainsPulseSeparationOnOneShaft) {
    auto p = groupParameters({ 0, 1 }, { 0, 1 }, { 0 });
    p.inletChannelCount = 2;
    SyntheticEngine fixture({ p }, 2, 1, true);
    TurboGroup *group = fixture.engine.getForcedInductionSystem()->group(0);
    GasSystem *scroll0 = group->preTurbineSystem(0);
    GasSystem *scroll1 = group->preTurbineSystem(1);
    GasSystem supply0;
    GasSystem supply1;
    GasSystem sink;
    initializeSupply(supply0, 3.0 * AmbientPressure);
    initializeSupply(supply1, 2.8 * AmbientPressure);
    sink.initialize(0.70 * AmbientPressure, 1.0, AmbientTemperature, airMix());

    EXPECT_NE(scroll0, scroll1);
    EXPECT_EQ(
        fixture.engine.getExhaustDestination(fixture.engine.getExhaustSystem(0)),
        scroll0);
    EXPECT_EQ(
        fixture.engine.getExhaustDestination(fixture.engine.getExhaustSystem(1)),
        scroll1);
    const double scroll0Before = scroll0->n();
    const double scroll1Before = scroll1->n();
    EXPECT_GT(feedScroll(supply0, scroll0), 0.0);
    EXPECT_GT(scroll0->n(), scroll0Before);
    EXPECT_DOUBLE_EQ(scroll1->n(), scroll1Before);

    double maximumTurbinePower = 0.0;
    for (int i = 0; i < 250; ++i) {
        if ((i % 2) == 0) feedScroll(supply0, scroll0);
        else feedScroll(supply1, scroll1);
        fixture.engine.processForcedInduction(Step);
        maximumTurbinePower = std::max(maximumTurbinePower, group->telemetry().turbinePower);
        drainIntake(fixture.engine.getIntake(0)->getSystem(), sink);
        fixture.processExhausts(Step);
    }

    ASSERT_EQ(group->telemetry().preTurbinePressure.size(), 2u);
    EXPECT_GT(maximumTurbinePower, 0.0);
    EXPECT_GT(group->rotatingAssembly()->shaftSpeed(), 0.0);
    expectStable(*group);
}
