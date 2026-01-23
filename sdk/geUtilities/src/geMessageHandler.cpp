/*****************************************************************************/
/**
 * @file    geMessageHandler.cpp
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

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "geMessageHandler.h"

namespace geEngineSDK {
  using std::remove_if;

  Map<String, uint32> MessageId::m_uniqueMessageIds;
  uint32 MessageId::m_nextMessageId = 0;

  MessageId::MessageId(const String& name) {
    auto findIter = m_uniqueMessageIds.find(name);

    if (findIter != m_uniqueMessageIds.end()) {
      m_msgIdentifier = findIter->second;
    }
    else {
      m_msgIdentifier = m_nextMessageId;
      m_uniqueMessageIds[name] = m_nextMessageId++;
    }
  }

  HMessage::HMessage(uint32 id) : m_id(id) {}

  void
  HMessage::disconnect() {
    if (0 < m_id) {
      MessageHandler::instance().unsubscribe(m_id);
    }
  }

  void
  MessageHandler::send(MessageId message) {
    auto iterFind = m_messageHandlers.find(message.m_msgIdentifier);
    if (iterFind != m_messageHandlers.end()) {
      for (auto& handlerData : iterFind->second) {
        handlerData.callback();
      }
    }
  }

  HMessage
  MessageHandler::listen(MessageId message, function<void()>& callback) {
    uint32 callbackId = m_nextCallbackId++;

    MessageHandlerData data;
    data.id = callbackId;
    data.callback = callback;

    m_messageHandlers[message.m_msgIdentifier].push_back(data);
    m_handlerIdToMessageMap[callbackId] = message.m_msgIdentifier;

    return HMessage(callbackId);
  }

  void
  MessageHandler::unsubscribe(uint32 handleId) {
    uint32 msgId = m_handlerIdToMessageMap[handleId];

    auto iterFind = m_messageHandlers.find(msgId);
    if (iterFind != m_messageHandlers.end()) {
      Vector<MessageHandlerData>& handlerData = iterFind->second;

      handlerData.erase(remove_if(handlerData.begin(),
                        handlerData.end(),
                        [&](MessageHandlerData& x)
                        {
                          return x.id == handleId;
                        }),
                        handlerData.end());
    }

    m_handlerIdToMessageMap.erase(handleId);
  }

  void
  sendMessage(MessageId message) {
    MessageHandler::instance().send(message);
  }
}
