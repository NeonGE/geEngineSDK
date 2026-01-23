/*****************************************************************************/
/**
 * @file    geMessageHandler.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Allows to transparently pass messages between different systems
 *
 * Message system that allows you to transparently pass messages between
 * different systems. Only usable in the Simulation thread
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtilities.h"
#include "geModule.h"

namespace geEngineSDK {
  using std::function;

  /**
   * @brief Allows you to transparently pass messages between different systems.
   * @note  Sim thread only.
   */
  class GE_UTILITIES_EXPORT MessageHandler : public Module<MessageHandler>
  {
   private:
    friend class HMessage;

    struct MessageHandlerData
    {
      uint32 id;
      function<void()> callback;
    };

   public:
    MessageHandler() = default;

    /**
     * @brief Sends a message to all subscribed listeners.
     */
    void
    send(MessageId message);

    /**
     * @brief Subscribes a message listener for the specified message. Provided
     *        callback will be triggered whenever that message gets sent.
     * @return  A handle to the message subscription that you can use to
     *          unsubscribe from listening.
     */
    HMessage
    listen(MessageId message, function<void()>& callback);

   private:
    void
    unsubscribe(uint32 handleId);

    Map<uint32, Vector<MessageHandlerData>> m_messageHandlers;
    Map<uint32, uint32> m_handlerIdToMessageMap;
    uint32 m_nextCallbackId = 1;
  };
}
