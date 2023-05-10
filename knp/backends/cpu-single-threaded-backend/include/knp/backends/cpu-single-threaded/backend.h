/**
 * @file backend.h
 * @brief Single threaded CPU backend class definition.
 * @author Artiom N.
 * @date 30.01.2023
 */

#pragma once

#include <knp/core/backend.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/devices/cpu.h>
#include <knp/neuron-traits/all_traits.h>
#include <knp/synapse-traits/all_traits.h>

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include <boost/mp11.hpp>


namespace knp::backends::single_threaded_cpu
{
class SingleThreadedCPUBackend : public knp::core::Backend
{
public:
    using SupportedNeurons = boost::mp11::mp_list<knp::neuron_traits::BLIFATNeuron>;
    using SupportedSynapses = boost::mp11::mp_list<knp::synapse_traits::DeltaSynapse>;
    using SupportedPopulations = boost::mp11::mp_transform<knp::core::Population, SupportedNeurons>;
    using SupportedProjections = boost::mp11::mp_transform<knp::core::Projection, SupportedSynapses>;

    using PopulationVariants = boost::mp11::mp_rename<SupportedPopulations, std::variant>;
    using ProjectionVariants = boost::mp11::mp_rename<SupportedProjections, std::variant>;

private:
    struct ProjectionWrapper
    {
        ProjectionVariants arg_;
        core::messaging::SynapticMessageQueue messages_;
    };

public:
    using PopulationContainer = std::vector<PopulationVariants>;
    using ProjectionContainer = std::vector<ProjectionWrapper>;

    // TODO: Make custom iterators.
    using PopulationIterator = PopulationContainer::iterator;
    using PopulationConstIterator = PopulationContainer::const_iterator;
    using ProjectionIterator = ProjectionContainer::iterator;
    using ProjectionConstIterator = ProjectionContainer::const_iterator;

public:
    ~SingleThreadedCPUBackend() = default;

public:
    static std::shared_ptr<SingleThreadedCPUBackend> create();

public:
    [[nodiscard]] bool plasticity_supported() const override { return true; }
    [[nodiscard]] std::vector<std::string> get_supported_neurons() const override;
    [[nodiscard]] std::vector<std::string> get_supported_synapses() const override;

public:
    /**
     * @brief Load populations to the backend.
     * @param populations vector of populations to load.
     */
    void load_populations(const std::vector<PopulationVariants> &populations);

    /**
     * @brief Load projections to the backend.
     * @param projections vector of projections to load.
     */
    void load_projections(const std::vector<ProjectionVariants> &projections);

public:
    PopulationIterator begin_populations();
    PopulationConstIterator begin_populations() const;
    PopulationIterator end_populations();
    PopulationConstIterator end_populations() const;

    ProjectionIterator begin_projections();
    ProjectionConstIterator begin_projections() const;
    ProjectionIterator end_projections();
    ProjectionConstIterator end_projections() const;

public:
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids UIDs of projections to remove.
     */
    void remove_projections(const std::vector<knp::core::UID> &uids) override {}

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids UIDs of populations to remove.
     */
    void remove_populations(const std::vector<knp::core::UID> &uids) override {}

public:
    /**
     * @brief Get a list of devices supported by the backend.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] std::vector<std::unique_ptr<knp::core::Device>> get_devices() const override;

public:
    void step() override;

    /**
     * @brief Subscribe internal endpoint to messages. Needed to send messages into the network
     * @tparam MessageType Message type
     * @param receiver Receiving object UID
     * @param senders a list of possible senders
     * @return subscription
     */
    template <typename MessageType>
    knp::core::Subscription<MessageType> &subscribe(
        const knp::core::UID &receiver, const std::vector<knp::core::UID> &senders)
    {
        return message_endpoint_.subscribe<MessageType>(receiver, senders);
    }

public:
    SingleThreadedCPUBackend();

protected:
    void init() override;

    /**
     * @brief Calculate the population of BLIFAT neurons.
     * @note Population will be changed during calculation.
     * @param population population of BLIFAT neurons to calculate.
     */
    void calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population);
    /**
     * @brief Calculate the projection of Delta synapses.
     * @note Projection will be changed during calculation.
     * @param projection projection of Delta synapses to calculate.
     * @param message_queue message queue for the projection.
     */
    void calculate_projection(
        knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection,
        core::messaging::SynapticMessageQueue &message_queue);

private:
    PopulationContainer populations_;
    ProjectionContainer projections_;
    core::MessageEndpoint message_endpoint_;
    size_t step_ = 0;
};


BOOST_DLL_ALIAS(knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::create, create_knp_backend)

}  // namespace knp::backends::single_threaded_cpu
