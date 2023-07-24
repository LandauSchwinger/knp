/**
 * @brief Set converter header
 * @author Vartenkov Andrey
 * @date 01.06.2023
 */

#pragma once

#include <knp/core/messaging/spike_message.h>

#include <set>
#include <vector>


/**
 * @brief Output channel namespace.
 */
namespace knp::framework::output
{

class ConvertToSet
{
public:
    /**
     * @brief ConvertToSet constructor.
     * @param output_size output vector size (usually corresponds to the size of an output population).
     */
    explicit ConvertToSet(size_t output_size) : output_size_(output_size) {}
    /**
     * @brief Get a set of recently spiked neuron indexes from the `message_list`.
     * @details The method ignores neuron indexes that are greater than the `output_size` value.
     * @param message_list list of spike messages that contain indexes of spiked neurons.
     * @return set of spiked neuron indexes.
     */
    std::set<core::messaging::SpikeIndex> operator()(const std::vector<core::messaging::SpikeMessage> &message_list)
    {
        std::set<core::messaging::SpikeIndex> result;
        for (auto &message : message_list)
        {
            result.insert(message.neuron_indexes_.cbegin(), message.neuron_indexes_.cend());
        }

        // Ignore extra neurons
        auto iter = result.lower_bound(output_size_);
        result.erase(iter, result.end());
        return result;
    }

private:
    const size_t output_size_;
};

}  // namespace knp::framework::output
