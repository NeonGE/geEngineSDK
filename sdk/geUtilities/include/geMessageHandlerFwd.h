/*****************************************************************************/
/**
 * @file    geMessageHandlerFwd.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Classes used for the global messaging system.
 *
 * Forward declaration of all the objects needed to send messages with the
 * global messaging system
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

namespace geEngineSDK {
  /**
   * @brief Identifier for message used with the global messaging system.
   * @note  Primary purpose of this class is to avoid expensive string compare
   *        (i.e. button names), and instead use a unique message identifier
   *        for compare. Generally you want to create one of these using the
   *        message name, and then store it for later use.
   * @note  This class is not thread safe and should only be used on the
   *        simulation thread.
   */
  class GE_UTILITIES_EXPORT MessageId
  {
   public:
    MessageId() = default;
    explicit MessageId(const String& name);

    bool
    operator==(const MessageId& rhs) const {
      return (m_msgIdentifier == rhs.m_msgIdentifier);
    }

   private:
    friend class MessageHandler;

    static Map<String, uint32> m_uniqueMessageIds;
    static uint32 m_nextMessageId;

    uint32 m_msgIdentifier = 0;
  };

  /**
   * @brief Handle to a subscription for a specific message in the global messaging system.
   */
  class GE_UTILITIES_EXPORT HMessage
  {
   public:
    HMessage() = default;

    /**
     * @brief	Disconnects the message listener so it will no longer receive
     *        events from the messaging system.
     */
    void
    disconnect();

   private:
    friend class MessageHandler;
    
    explicit HMessage(uint32 id);

    uint32 m_id = 0;
  };

  /**
   * @brief Sends a message using the global messaging system.
   * @note  Simulation thread only.
   */
  void GE_UTILITIES_EXPORT
  sendMessage(MessageId message);

  class MessageHandler;
}
