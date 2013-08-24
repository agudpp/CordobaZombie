/*
 * TriggerSystem.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#include "TriggerSystem.h"


namespace core {

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::initialize(TriggerAgent* agent)
{
    ASSERT(agent);
    if (!mMatrix.isBuilt()) {
        debugERROR("We are trying to initialize an agent but the system is not "
            "already built\n");
        return TriggerCode::SystemNotBuilt;
    }

    const Vector2& agentPos = agent->position();
    // check if the position of the agent is inside of our world
    if (!mMatrix.isPositionInside(agentPos)) {
        debugWARNING("You are trying to initialize the agent %u outside of the "
            "bounds: (%f, %f)\n", agent->id(), agentPos.x, agentPos.y);
        return TriggerCode::UnreachablePosition;
    }

    // get the associated cell and check for the elements that intersect our position
    const TriggerMatrix::Cell& cell = mMatrix.getCell(agentPos);
    cell.getElementsInPosition(agentPos, mCellElements);

    // iterate over each one of the zones and call its callbacks and configure
    // the color of the agent
    TriggerColor_t& currentColors = agent->currentColors;
    currentColors = 0;
    agent->lastCellID = cell.id;

    for (core::size_t i = 0, size = mCellElements.size(); i < size; ++i) {
        const TriggerMatrix::CellElement& element = *mCellElements[i];
        if (element.zoneElement.zone.isPointInside(agentPos)) {
            // entering in a new zone
            currentColors |= element.color;
            element.zoneElement.callbacks(
                TriggerCallback::EventInfo(TriggerCallback::EventType::Entering,
                                           element.zoneElement.id,
                                           *agent));
        }
    }

    return TriggerCode::Ok;
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::remapAgentToNewCell(TriggerAgent* agent,
                                   const TriggerMatrix::Cell& newCell) const
{
    ASSERT(agent);
    ASSERT(agent->lastCellID != newCell.id);

    // check for old zones that we just leave
    const TriggerColor_t& agentColor = agent->currentColors;
    TriggerColor_t newColor = 0;
    const Vector2& newPos = agent->position();

    if (agentColor != 0 && mMatrix.isIndexValid(agent->lastCellID)) {
        const TriggerMatrix::Cell& oldCell = mMatrix.getCell(agent->lastCellID);
        const TriggerMatrix::CellElementVec& oldElements = oldCell.elements;
        for (core::size_t i = 0, size = oldElements.size(); i < size; ++i) {
            const TriggerMatrix::CellElement& element = oldElements[i];
            if (element.color & agentColor) {
                // this one was a zone that was handled by the agent in the last
                // call to updatePosition()... we need to check if we are still inside
                if (!element.zoneElement.zone.isPointInside(newPos)) {
                    // we are outside, call the callback for this one
                    element.zoneElement.callbacks(
                        TriggerCallback::EventInfo(TriggerCallback::EventType::Leaving,
                                                   element.zoneElement.id,
                                                   *agent));
                } else {
                    // we are still inside, we need to remap it to the new color
                    const TriggerMatrix::CellElement* newElem =
                        newCell.findElementByZoneID(element.zoneElement.id);
                    ASSERT(newElem);
                    newColor |= newElem->color;
                }
            }
        }
    }

    // now check for the new possible zones ONLY in the new cell
    const TriggerMatrix::CellElementVec& newElements = newCell.elements;
    for (core::size_t i = 0, size = newElements.size(); i < size; ++i) {
        const TriggerMatrix::CellElement& element = newElements[i];

        // we need to check if there are a new zone that we are not tracking
        if ((newColor & element.color) == 0 &&
            element.zoneElement.zone.isPointInside(newPos)) {
            // new zone to track
            newColor |= element.color;
            element.zoneElement.callbacks(
                TriggerCallback::EventInfo(TriggerCallback::EventType::Entering,
                                           element.zoneElement.id,
                                           *agent));
        }
    }
    // update the agent information
    agent->lastCellID = newCell.id;
    agent->currentColors = newColor;

    return TriggerCode::Ok;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
TriggerSystem::TriggerSystem()
{
    mCellElements.reserve(TS_NUM_COLORS); // just in case
}

////////////////////////////////////////////////////////////////////////////
TriggerSystem::~TriggerSystem()
{
}

////////////////////////////////////////////////////////////////////////////
//                    Characters associated methods
//

////////////////////////////////////////////////////////////////////////////
TriggerAgent*
TriggerSystem::createAgent(void)
{
    std::shared_ptr<TriggerAgent> agent(new TriggerAgent(*this));
    mAgents.push_back(agent);
    agent->setID(mAgents.size() - 1);
    return agent.get();
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::removeAgent(core::uint16_t id)
{
    if (id >= mAgents.size()) {
        debugWARNING("Trying to remove agent %ui and we have %ul agents\n",
            id, mAgents.size());
        return;
    }

    // if we have only one..
    if (mAgents.size() == 1) {
        mAgents.clear();
        return;
    }

    // swap with the last and remove this one
    core::size_t size = mAgents.size()-1;
    mAgents[id] = mAgents[size];
    mAgents[id]->setID(id);
    mAgents.pop_back();
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::initializeAgent(TriggerAgent* agent, const Vector2& pos)
{
    ASSERT(agent);
    ASSERT(agent->id() < mAgents.size());
    ASSERT(agent->id() == mAgents[agent->id()]->id());

    agent->mPosition = pos;
    return initialize(agent);
}

////////////////////////////////////////////////////////////////////////////
TriggerCode
TriggerSystem::updateAgentPos(TriggerAgent* agent, const Vector2& pos)
{
    ASSERT(agent);
    ASSERT(agent->id() < mAgents.size());
    ASSERT(agent->id() == mAgents[agent->id()]->id());

    // update the position of the agent
    agent->mPosition = pos;
    TriggerColor_t& oldColors = agent->currentColors;

    // check if we are outside of the target map
    if (!mMatrix.isPositionInside(pos)) {
        // need to clear all the zones and clear the agent
        if (mMatrix.isIndexValid(agent->lastCellID) && oldColors != 0) {
            // we was in a valid cell, get the cell and clean up
            const TriggerMatrix::Cell& oldCell = mMatrix.getCell(agent->lastCellID);
            for (core::size_t i = 0, size = oldCell.elements.size(); i < size; ++i) {
                const TriggerMatrix::CellElement& element = oldCell.elements[i];
                if (element.color & oldColors) {
                    // we was on this cell before, call the callbacks for this one
                    element.zoneElement.callbacks(
                        TriggerCallback::EventInfo(TriggerCallback::EventType::Leaving,
                                                   element.zoneElement.id,
                                                   *agent));
                }
            }
        }
        oldColors = 0;
        return TriggerCode::UnreachablePosition;
    }

    // we are inside of the normal zone... get the new cell position
    const TriggerMatrix::Cell& cell = mMatrix.getCell(pos);

    // check now if we are on a new cell
    if (cell.id != agent->lastCellID) {
        // we are in a new cell... remap stuff and return
        return remapAgentToNewCell(agent, cell);
    }

    // we are still in the same cell... do the usual checking
    TriggerColor_t newColor = 0;
    const TriggerMatrix::CellElementVec& elements = cell.elements;
    for (core::size_t i = 0, size = elements.size(); i < size; ++i) {
        const TriggerMatrix::CellElement& element = elements[i];

        if (element.zoneElement.zone.isPointInside(pos)) {
            newColor |= element.color;
            // we are inside of this zone.. check  if we was inside before
            if (!(element.color & oldColors)) {
                // is a new one call the callbacks
                element.zoneElement.callbacks(
                    TriggerCallback::EventInfo(TriggerCallback::EventType::Entering,
                                               element.zoneElement.id,
                                               *agent));
            }
        } else {
            // we are outside of this zone... check if we was before here
            if (element.color & oldColors) {
                // yeah! we was before here... call the leaving callback
                element.zoneElement.callbacks(
                    TriggerCallback::EventInfo(TriggerCallback::EventType::Leaving,
                                               element.zoneElement.id,
                                               *agent));
            }
        }
    }

    // reassign the new colors
    oldColors = newColor;

    // check if we are on an unreachable zone
    return newColor == 0 ? TriggerCode::UnreachablePosition : TriggerCode::Ok;
}


////////////////////////////////////////////////////////////////////////////
//                       Building methods
//


TriggerCallback::Connection
TriggerSystem::addCallback(core::uint16_t zoneID,
                           const TriggerCallback::Signal::slot_type& subscriber)
{
    return mMatrix.getZoneElement(zoneID).callbacks.connect(subscriber);
}

////////////////////////////////////////////////////////////////////////////
bool
TriggerSystem::build(const std::vector<TriggerZone>& zones, std::vector<core::uint16_t>& ids)
{
    if (isAlreadyBuilt()) {
        debugERROR("The system is already built, call destroy() first\n");
        return false;
    }

    if (zones.empty()) {
        debugERROR("No zones were added to build the system\n");
        return false;
    }

    // create the matrix with the default values for now...
    return mMatrix.build(zones, ids);;
}

////////////////////////////////////////////////////////////////////////////
void
TriggerSystem::destroy(void)
{
    mMatrix.destroy();
}

} /* namespace core */
