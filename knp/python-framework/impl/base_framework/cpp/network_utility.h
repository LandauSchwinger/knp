/**
 * @file network_utility.h
 * @brief Auxiliary functions for Network bindings.
 * @author Vartenkov A.
 * @date 18.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>

#include <utility>


void network_add_projection_variant(knp::framework::Network &self, knp::core::AllProjectionsVariant proj)
{
    self.add_projection(std::move(proj));
}


void network_add_population_variant(knp::framework::Network &self, knp::core::AllPopulationsVariant pop)
{
    self.add_population(std::move(pop));
}


knp::core::AllPopulationsVariant network_get_population_variant(
    knp::framework::Network &self, const knp::core::UID &population_uid)
{
    return self.get_population(population_uid);
}


knp::core::AllProjectionsVariant network_get_projection_variant(
    knp::framework::Network &self, const knp::core::UID &projection_uid)
{
    return self.get_projection(projection_uid);
}
