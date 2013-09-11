/*
 * PathHandler.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef PATHHANDLER_H_
#define PATHHANDLER_H_

#include <debug/DebugUtil.h>
#include <pathfinding/WayPointDefines.h>
#include <math/Vec2.h>

// forward
//
namespace gps {
class WPPathfinder;
}

namespace cz {

// @brief This class will be used to handle the path of the zombies/units.
//        We will ask for a path from the current position of the unit and the
//        destination point. And we will be asking for the next position we have
//        to move using the current position of the unit and the next point in the
//        path. The user can choose to remove the last point or not.
//        This way we can abstract how or where we have to move through the path
//        and create a smooth path
//

class PathHandler
{
public:

    // We will define here when we consider that we are close enough to the
    // point to consider that we can just move to the next one.
    // The value will be the % of the radius, this means that if we want to
    // move to the next point in the path when we are a distance equal to the
    // radius then CLOSE_DIST_FACTOR = 1.f;
    // If we want to move to the next point where the center point of the unit
    // is in exactly the same place that the point then
    // CLOSE_DIST_FACTOR = 0.f; // note that this is not possible ever
    //
    static const float CLOSE_DIST_FACTOR; // = 0.5f

    // @brief This method should be called before constructing any PathHandler
    //        that will be used to set the WPPathfinder to be used.
    // @param pf        The WPPathfinder instance
    //
    static inline void
    setPathfinder(gps::WPPathfinder* pf);

public:
    PathHandler();
    ~PathHandler();

    // @brief Set the current radius to be used for this handler
    // @param radius    The radius
    //
    inline void
    setRadius(float radius);

    // @brief Create the path from the point start (where the unit is) and the
    //        goal position.
    //        We can also ask for a spline path instead of direct path
    // @param start     Unit current position
    // @param goal      The goal where we want to go
    // @param smooth    Use splined / smooth path?
    // @return true if path exists | false otherwise
    bool
    calculatePath(const core::Vector2& start,
                  const core::Vector2& goal,
                  bool smooth = false);

    // @brief Check if we still have more paths to go through
    //
    inline bool
    morePointsToGo(void) const;

    // @brief Return the number of points to go through
    //
    inline unsigned int
    numPointsToGo(void);

    // @brief Get the next point where we have to go. Note that we will be moving
    //        sequentially in the order of the points we get from the
    //        pathfinding system.
    //        We will also return if we are close enough to the current point where
    //        we are going, or false if is far away.
    // @param currentPos    The current position of the unit
    // @param point         The next point where we need to move the unit.
    // @return true if we are close enough and we probably want to remove the
    //         current point or false if we are far away and we must continue
    //         maintaining the current point.
    // @note We need to ensure that we are calling this method with points in
    //       the path
    bool
    getNextPoint(const core::Vector2& currentPos, core::Vector2& point);

    // @brief Remove the current node and move to the next one.
    //
    inline void
    removeCurrent(void);

    // @brief Clear the current path
    //
    inline void
    clear(void);

private:
    unsigned short mCurrentIndex;
    gps::WayPointPath mPath;
    float mCloseSqrDist;

    // the static instance of the pathfinder
    static gps::WPPathfinder* sPathfinder;
};









////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
PathHandler::setPathfinder(gps::WPPathfinder* pf)
{
    sPathfinder = pf;
}

////////////////////////////////////////////////////////////////////////////////
inline void
PathHandler::setRadius(float radius)
{
    ASSERT(radius > 0.f);
    mCloseSqrDist = (radius * CLOSE_DIST_FACTOR) * (radius * CLOSE_DIST_FACTOR);
}

inline bool
PathHandler::morePointsToGo(void) const
{
    return mPath.size > mCurrentIndex;
}

inline unsigned int
PathHandler::numPointsToGo(void)
{
    ASSERT(mCurrentIndex <= mPath.size);
    return mPath.size - mCurrentIndex;
}

inline void
PathHandler::removeCurrent(void)
{
    ++mCurrentIndex;
}

inline void
PathHandler::clear(void)
{
    mPath.size = 0;
    mCurrentIndex = 0;
}


} /* namespace cz */
#endif /* PATHHANDLER_H_ */
