/**
 * Model execution testing.
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/framework/io/out_converters/convert_set.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <generators.h>
#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, ModelExecutorLoad)
{
    namespace kt = knp::testing;

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    kt::DeltaProjection loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    kt::DeltaProjection input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};

    knp::core::UID input_uid = input_projection.get_uid();
    knp::core::UID output_uid = population.get_uid();

    knp::framework::Network network;
    network.add_population(std::move(population));
    network.add_projection(std::move(input_projection));
    network.add_projection(std::move(loop_projection));

    knp::core::UID i_channel_uid, o_channel_uid;

    knp::framework::Model model(std::move(network));
    model.add_input_channel(i_channel_uid, input_uid);
    model.add_output_channel(o_channel_uid, output_uid);

    auto input_gen = [](knp::core::messaging::Step step) -> knp::core::messaging::SpikeData
    {
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeData s;
            s.push_back(0);
            return s;
        }
        return knp::core::messaging::SpikeData();
    };

    knp::framework::ModelExecutor me(
        model, knp::testing::get_backend_path(),
        [&i_channel_uid, input_gen](const knp::core::UID &ic_uid, knp::core::MessageEndpoint &&ep)
            -> std::unique_ptr<knp::framework::input::InputChannel>
        {
            EXPECT_EQ(i_channel_uid, ic_uid);
            auto ich = std::make_unique<knp::framework::input::InputChannel>(ic_uid, std::move(ep), input_gen);
            return ich;
        },
        [&o_channel_uid](const knp::core::UID &oc_uid, knp::core::MessageEndpoint &&ep)
            -> std::unique_ptr<knp::framework::output::OutputChannel>
        {
            EXPECT_EQ(o_channel_uid, oc_uid);
            auto och = std::make_unique<knp::framework::output::OutputChannel>(oc_uid, std::move(ep));
            return och;
        });

    me.init();
    auto out_channel = me.get_output_channel(o_channel_uid);

    me.start([](size_t step) { return step < 20; });

    std::vector<knp::core::messaging::Step> results;
    const auto &spikes = out_channel->update();
    results.reserve(spikes.size());

    std::transform(
        spikes.cbegin(), spikes.cend(), std::back_inserter(results),
        [](const auto &spike_msg) { return spike_msg.header_.send_time_; });
    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<knp::core::messaging::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);
}
