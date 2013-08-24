/*
 * test.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: agustin
 */

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <vector>

#include <UnitTest++/UnitTest++.h>

#include <trigger_system/TriggerZone.h>
#include <trigger_system/TriggerSystem.h>
#include <trigger_system/TriggerSystemDefines.h>
#include <trigger_system/TriggerAgent.h>
#include <types/basics.h>

using namespace core;

typedef std::vector<core::TriggerZone> TZoneVec;
typedef std::vector<core::uint16_t> TZoneIDs;
typedef core::Vector2 V2;

// Define the structure used to receive signals event from the TriggerZones
//
struct EventReceiver {
    EventReceiver() : called(false) {};

    bool called;
    TriggerCallback::EventType event;
    core::uint16_t agentId;
    core::Vector2 agentPos;
    core::uint16_t zoneId;

    void
    receiveEvent(const TriggerCallback::EventInfo& evt)
    {
        debugGREEN("receiveEvent called zone id: %u\n", evt.zoneID);
        called = true;
        event = evt.type;
        agentId = evt.agent.id();
        agentPos = evt.agent.position();
        zoneId = evt.zoneID;
    }

    void
    registerSignal(TriggerSystem& ts, core::uint16_t zoneId)
    {
        connection = ts.addCallback(zoneId,
            boost::bind(&EventReceiver::receiveEvent, this, _1));
    }

    TriggerCallback::Connection connection;
};

typedef std::vector<EventReceiver> EventRecvVec;


static bool
checkReciver(TriggerCallback::EventType event,
             core::uint16_t agentId,
             const core::Vector2& agentPos,
             core::uint16_t zoneId,
             const EventReceiver& rcv)
{
    CHECK_EQUAL(event, rcv.event);
    CHECK_EQUAL(agentId, rcv.agentId);
    CHECK_EQUAL(agentPos.x, rcv.agentPos.x);
    CHECK_EQUAL(agentPos.y, rcv.agentPos.y);
    CHECK_EQUAL(zoneId, rcv.zoneId);

    return (event == rcv.event &&
        agentId == rcv.agentId &&
        agentPos.x == rcv.agentPos.x &&
        agentPos.y == rcv.agentPos.y &&
        zoneId == rcv.zoneId);
}

static bool
registerReceivers(TriggerSystem& ts,
                  const TZoneVec& zones,
                  EventRecvVec& receivers,
                  TZoneIDs& ids)
{
    CHECK(zones.size() == receivers.size());

    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(zones.size(), ids.size());
    for (core::size_t i = 0, size = zones.size(); i < size; ++i) {
        receivers[i].registerSignal(ts, ids[i]);
    }

    return true;
}

static inline void
resetReceivers(EventRecvVec& receivers)
{
    for (core::size_t i = 0, size = receivers.size(); i < size; ++i) {
        receivers[i].called = 0;
    }
}

static inline bool
checkReceiversCalled(EventRecvVec& r, bool value)
{
    for (core::size_t i = 0, size = r.size(); i < size; ++i) {
        if (value != r[i].called) {
            return false;
        }
    }
    return true;
}
static inline bool
checkReceiversCalledExcept(EventRecvVec& r, bool value, core::size_t index)
{
    for (core::size_t i = 0, size = r.size(); i < size; ++i) {
        if (i != index && value != r[i].called) {
            return false;
        }
    }
    return true;
}

TEST(CheckTriggerZone)
{
    TriggerZone z1,z2,z3,z4;
    AABB ab1(10,0,0,10), ab2(5,1,2,4), ab3(9,3,5.1,4), ab4(-100,-10,-200, 4);
    z1.setZone(ab1);
    z2.setZone(ab2);
    z3.setZone(ab3);
    z4.setZone(ab4);

    Vector2 p1(-4,-4), p2(0.1,0.1), p3(100,100);

    // check point inside
    CHECK(!z1.isPointInside(p1));
    CHECK(z1.isPointInside(p2));
    CHECK(!z1.isPointInside(p3));

    CHECK(!z2.isPointInside(p1));
    CHECK(!z2.isPointInside(p2));
    CHECK(!z2.isPointInside(p3));

    CHECK(!z3.isPointInside(p1));
    CHECK(!z3.isPointInside(p2));
    CHECK(!z3.isPointInside(p3));

    CHECK(!z4.isPointInside(p1));
    CHECK(!z4.isPointInside(p2));
    CHECK(!z4.isPointInside(p3));

    // check intersection
    CHECK(z1.intersects(z2));
    CHECK(z1.intersects(z3));
    CHECK(z2.intersects(z1));
    CHECK(z3.intersects(z1));
    CHECK(!z2.intersects(z3));
    CHECK(!z3.intersects(z2));
    CHECK(!z1.intersects(z4));
    CHECK(!z2.intersects(z4));
    CHECK(!z3.intersects(z4));
}

///////////////////////////////////////////////////////////////////////////////

TEST(CreateAndRemoveAgent)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TriggerAgent* nullAgent = 0;

    CHECK(agent != 0);
    uint16_t id = agent->id();
    CHECK_EQUAL(agent, ts.getAgent(id));
    ts.removeAgent(agent);
    CHECK_EQUAL(nullAgent, ts.getAgent(id));

    agent = ts.createAgent();
    id = agent->id();
    CHECK_EQUAL(agent, ts.getAgent(id));
    ts.removeAgent(id);
    CHECK_EQUAL(nullAgent, ts.getAgent(id));
}

// Building the system
//

TEST(BuildingTheSystemSimple)
{
    TriggerSystem ts;

    TZoneVec zones;
    TZoneIDs ids;

    zones.resize(2);
    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(zones.size(), ids.size());
    CHECK(ids[0] != ids[1]);

    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    ts.destroy();
    CHECK_EQUAL(false, ts.isAlreadyBuilt());

    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(zones.size(), ids.size());
    CHECK(ids[0] != ids[1]);
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());
}

TEST(BuildingTheSystemWrong)
{
    TriggerSystem ts;
    TriggerZone z1;
    AABB bb(100,0,-100,100);
    z1.setZone(bb);

    TZoneVec zones;
    TZoneIDs ids;
    zones.resize(TS_NUM_COLORS+1, z1);

    // build and ensure that we cannot build that since we have more nodes in the
    // same place than the colors supported (the maximum overlapped zones)
    // colors or the implementation that is used
    CHECK_EQUAL(false, ts.build(zones, ids));
    CHECK_EQUAL(false, ts.isAlreadyBuilt());

    zones.resize(TS_NUM_COLORS, z1);

    CHECK_EQUAL(true, ts.build(zones, ids));
    CHECK_EQUAL(true, ts.isAlreadyBuilt());

    // check that we always have new ids
    core::size_t currentSize = ids.size();
    std::vector<uint16_t>::iterator it = std::unique(ids.begin(), ids.end());
    core::size_t uniqSize = std::distance(ids.begin(), it);
    CHECK_EQUAL(currentSize, uniqSize);

}

TEST(InitAgentBeforeBuilt)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();

    CHECK_EQUAL(TriggerCode::SystemNotBuilt, ts.initializeAgent(agent, V2(0,0)));
}


TEST(InitAgentUnreachablePosition)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    zones.push_back(TriggerZone(AABB(100,10,10,100)));
    receivers.resize(zones.size());
    CHECK(registerReceivers(ts, zones, receivers, ids));

    CHECK_EQUAL(TriggerCode::UnreachablePosition, ts.initializeAgent(agent, V2(0,0)));
}

TEST(InitAgentOkPosition)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    zones.push_back(TriggerZone(AABB(100,10,10,100)));
    receivers.resize(zones.size());
    CHECK(registerReceivers(ts, zones, receivers, ids));

    CHECK_EQUAL(TriggerCode::Ok, ts.initializeAgent(agent, V2(10,10)));

    TriggerAgent* agent2 = ts.createAgent();
    CHECK_EQUAL(TriggerCode::Ok, ts.initializeAgent(agent, V2(100,10)));
}


TEST(TestSingleZone)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    zones.push_back(TriggerZone(AABB(100,10,10,100)));
    receivers.resize(zones.size());
    CHECK(registerReceivers(ts, zones, receivers, ids));

    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(TriggerCode::Ok, ts.initializeAgent(agent, V2(10,10)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(V2(10,10), agent->position());

    CHECK(checkReciver(TriggerCallback::EventType::Entering,
                       agent->id(),
                       agent->position(),
                       ids[0],
                       receivers[0]));

    // move it out
    receivers[0].called = false;
    CHECK_EQUAL(TriggerCode::UnreachablePosition, agent->setPosition(V2(0,10)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK(checkReciver(TriggerCallback::EventType::Leaving,
                       agent->id(),
                       agent->position(),
                       ids[0],
                       receivers[0]));
    CHECK_EQUAL(V2(0,10), agent->position());

    // move it inside again
    receivers[0].called = false;
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(50,50)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK(checkReciver(TriggerCallback::EventType::Entering,
                       agent->id(),
                       V2(50,50),
                       ids[0],
                       receivers[0]));

}

TEST(TestPairOfOverlapedZones)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    zones.push_back(TriggerZone(AABB(10,0,0,10)));
    zones.push_back(TriggerZone(AABB(12,5,5,12)));
    receivers.resize(zones.size());
    CHECK(registerReceivers(ts, zones, receivers, ids));

    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(TriggerCode::UnreachablePosition, ts.initializeAgent(agent, V2(-1,-1)));
    CHECK_EQUAL(false, receivers[0].called);

    // move it inside the first one only
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(1,1)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(false, receivers[1].called);
    CHECK_EQUAL(receivers[0].event, TriggerCallback::EventType::Entering);

    // move it inside again including the second one
    // the first one shouldn't be updated
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(6,6)));
    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(true, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Entering);

    // move it to the second only
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(11,11)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(receivers[0].event, TriggerCallback::EventType::Leaving);
    CHECK_EQUAL(false, receivers[1].called);

    // move it outside
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::UnreachablePosition, agent->setPosition(V2(110,11)));
    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(true, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Leaving);

    // move it inside of both of them again
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(6,6)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Entering);
    CHECK_EQUAL(true, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Entering);
}

TEST(TestPairOfNonOverlapedZones)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    zones.push_back(TriggerZone(AABB(10,0,0,10)));
    zones.push_back(TriggerZone(AABB(10,20,0,25)));
    receivers.resize(zones.size());
    CHECK(registerReceivers(ts, zones, receivers, ids));

    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(TriggerCode::UnreachablePosition, ts.initializeAgent(agent, V2(-1,-1)));
    CHECK_EQUAL(false, receivers[0].called);

    // move it inside the first one only
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(1,1)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(false, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Entering);

    // move it to the second only
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(22,5)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(receivers[0].event, TriggerCallback::EventType::Leaving);
    CHECK_EQUAL(true, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Entering);

    // move it again in the second only
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(23,6)));
    CHECK_EQUAL(false, receivers[0].called);
    CHECK_EQUAL(false, receivers[1].called);

    // move it to the first one again
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(1,5)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(receivers[0].event, TriggerCallback::EventType::Entering);
    CHECK_EQUAL(true, receivers[1].called);
    CHECK_EQUAL(receivers[1].event, TriggerCallback::EventType::Leaving);

    // move outside
    resetReceivers(receivers);
    CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(V2(15,6)));
    CHECK_EQUAL(true, receivers[0].called);
    CHECK_EQUAL(receivers[0].event, TriggerCallback::EventType::Leaving);
    CHECK_EQUAL(false, receivers[1].called);
}

TEST(MultipleZonesOneAgent)
{
    TriggerSystem ts;
    TriggerAgent* agent = ts.createAgent();
    TZoneVec zones;
    TZoneIDs ids;
    EventRecvVec receivers;

    // create multiple zones
    const float boxWidth = 5.0f,
                boxHeight = 5.0f;
    const V2 topLeft(-boxWidth * 0.5f, boxHeight * 0.5f),
             bottomRight(boxWidth * 0.5f, -boxHeight * 0.5f);
    const core::size_t numBoxesX = 10,
                       numBoxesY = 10;

    zones.resize(numBoxesX * numBoxesY);
    receivers.resize(zones.size());

    // locate the boxes
    const V2 halfBox(boxWidth * 0.5f, boxHeight * 0.5f);
    for (core::size_t i = 0; i < numBoxesX; ++i) {
        for (core::size_t j = 0; j < numBoxesY; ++j) {
            const V2 middle = halfBox + V2(boxWidth * i, boxHeight * j);
            zones[i*numBoxesX + j].setZone(AABB(middle + topLeft, middle + bottomRight));
        }
    }

    // now construct the system
    CHECK(registerReceivers(ts, zones, receivers, ids));
    CHECK_EQUAL(TriggerCode::UnreachablePosition, ts.initializeAgent(agent, V2(-1,-1)));
    CHECK_EQUAL(true, checkReceiversCalled(receivers, false));

    // first check will be moving exactly in each box and detect
    for (core::size_t i = 0; i < numBoxesX; ++i) {
        for (core::size_t j = 0; j < numBoxesY; ++j) {
            const core::size_t index = i*numBoxesX + j;
            const V2 middle = halfBox + V2(boxWidth * i, boxHeight * j);
            CHECK_EQUAL(TriggerCode::Ok, agent->setPosition(middle));

            // check if all the receivers are in the state we expect
            if (index == 0) {
                // check only for the first case
                CHECK_EQUAL(true, checkReceiversCalledExcept(receivers, false, index));
                CHECK_EQUAL(true, receivers[index].called);
                CHECK_EQUAL(TriggerCallback::EventType::Entering, receivers[index].event);
                CHECK_EQUAL(ids[index], receivers[index].zoneId);
                receivers[index].called = false;
                continue;
            }

            // if not, we have to check for the current one and the last one
            const core::size_t lastIndex = index - 1;
            CHECK_EQUAL(true, receivers[index].called);
            CHECK_EQUAL(true, receivers[lastIndex].called);
            CHECK_EQUAL(TriggerCallback::EventType::Entering, receivers[index].event);
            CHECK_EQUAL(TriggerCallback::EventType::Leaving, receivers[lastIndex].event);
            CHECK_EQUAL(ids[index], receivers[index].zoneId);
            CHECK_EQUAL(ids[lastIndex], receivers[lastIndex].zoneId);
            receivers[index].called = false;
            receivers[lastIndex].called = false;
            CHECK_EQUAL(true, checkReceiversCalled(receivers, false));
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
// TODO:
// * Test with multiple agents (triggering different events depending on the zone
// flag mask
// * Test with multiple signals to the same zone
// * Multiple zones

int
main(void)
{
    return UnitTest::RunAllTests();
}
