/*
 * OpenALHandler.h
 *
 *  Created on: Dec 12, 2013
 *      Author: agustin
 */

#ifndef OPENALHANDLER_H_
#define OPENALHANDLER_H_

#include <string>
#include <vector>

// OpenAL includes
#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif


namespace mm {

// @brief This class will be used to configure and handle openal devices and
//        context's.
//

class OpenALHandler
{
public:

    // @brief we will support the creation of the default device
    //        to support backward compatibility. This createDefault
    //        probably will be never used in a release version since
    //        we need to create a device depending on the configuration.
    //
    OpenALHandler(bool createDefault = false);

    // @brief When the openal handler is destroyed the context and devices are
    //        closed and destroyed.
    //
    ~OpenALHandler();

    // @brief Check if we already have some device / context opened / created.
    //
    inline bool
    hasDevice(void) const;
    inline bool
    hasContext(void) const;

    // @brief Return the associated created device / context
    // @return the device / context created, or 0 otherwise
    //
    inline ALCdevice*
    device(void);
    inline const ALCdevice*
    device(void) const;
    inline ALCcontext*
    context(void);
    inline const ALCcontext*
    context(void) const;

    // @brief List all the devices we currently have in this computer.
    // @param devices       The resulting list of available devices
    //
    void
    getDevices(std::vector<std::string>& devices);

    // @brief Get the current device name, or empty string if we have not or
    //        some error occurs.
    // @return the current device name
    //
    std::string
    deviceName(void);

    // @brief Open and construct the openal device.
    // @param deviceID      The device id we want to open. If no name is provided
    //                      (empty string) then the default device will be used.
    // @return true on success | false otherwise.
    //
    bool
    openDevice(const std::string& name = "");

    // @brief This method will set a device to be used instead of creating a new
    //        one.
    // @param device        The device we want to use (requires != 0).
    // @note this method will remove the current device if we already have one
    //
    void
    useDevice(ALCdevice* device);

    // @brief Create a context and make it current if we need. To call this
    //        method you should ensure that we already have a device
    // @return true on success | false otherwise
    //
    bool
    createContext(void);

    // @brief Make this context as the current one.
    // @return true on success | false otherwise
    //
    bool
    makeContextCurrent(void);

    // @brief This call will suspend the current context (note this will hold
    //        - in some platforms - the openal processing and all the calls
    //        will be queued until the context is enabled again).
    //
    void
    suspendCurrentContext(void);

    // @brief This call will enabled the current context again (if it was
    //        suspended before).
    //
    void
    enableCurrentContextet(void);

    // @brief Destroy the current device / context.
    // @note that to be able to close a device you should first remove the
    //       context, if not, we will not be able to close the device.
    // @return true on success | false otherwise
    //
    bool
    closeDevice(void);
    bool
    destroyCurrentContext(void);


private:
    ALCcontext* mContext;
    ALCdevice* mDevice;
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline bool
OpenALHandler::hasDevice(void) const
{
    return mDevice != 0;
}
inline bool
OpenALHandler::hasContext(void) const
{
    return mContext != 0;
}

inline ALCdevice*
OpenALHandler::device(void)
{
    return mDevice;
}
inline const ALCdevice*
OpenALHandler::device(void) const
{
    return mDevice;
}
inline ALCcontext*
OpenALHandler::context(void)
{
    return mContext;
}
inline const ALCcontext*
OpenALHandler::context(void) const
{
    return mContext;
}

}

#endif /* OPENALHANDLER_H_ */
