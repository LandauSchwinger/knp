/**
 * @file message_endpoint_impl.h.
 * @brief Bus implementation interface.
 * @kaspersky_support An. Vartenkov
 * @date 25.09.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/message_envelope.h>

namespace knp::core::messaging::impl
{
/**
 * @brief Base class for all message endpoint implementations.
 */
class MessageEndpointImpl
{
public:
    /**
     * @brief Receive a message from message bus.
     * @return message if a message was received, nothing otherwise.
     */
    virtual std::optional<MessageVariant> receive_message() = 0;

    /**
     * @brief Send a message to a message bus.
     * @param message message to send.
     */
    virtual void send_message(const MessageVariant &message) = 0;

    MessageEndpointImpl() = default;
    MessageEndpointImpl(const MessageEndpointImpl &) = default;
    MessageEndpointImpl(MessageEndpointImpl &&) = default;
    MessageEndpointImpl &operator=(const MessageEndpointImpl &) = default;
    MessageEndpointImpl &operator=(MessageEndpointImpl &&) = default;

    /**
     * @brief Default virtual destructor.
     */
    virtual ~MessageEndpointImpl() = default;
};
}  // namespace knp::core::messaging::impl
