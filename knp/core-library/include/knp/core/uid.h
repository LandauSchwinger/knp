/**
 * @file uid.h
 * @brief UID class and routines.
 * @author Artiom N.
 * @date 01.02.2021
 */

#pragma once

#include <sstream>
#include <string>
#include <utility>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/**
* @brief Core library namespace.
*/
namespace knp::core
{

#if defined(_DEBUG) || defined(DEBUG)
#    define _ENABLE_PSEUDO_UID_GENERATOR 1
#endif

/**
 * @brief UID generator for entities.
 * @details You can use this class for debugging. UID is displayed as a 128-bit number.
 */
class continuously_uid_generator
{
public:
    ::boost::uuids::uuid operator()() const;
    /**
    * @brief Reset UID counter.
    */
    void reset(uint64_t initial_value = 1);
};

#if defined(_ENABLE_PSEUDO_UID_GENERATOR)
#    define uid_generator continuously_uid_generator
#else
#    define uid_generator ::boost::uuids::random_generator
#endif


/**
 * @brief The UID class is a definition of unique identifiers for entities.
 */

struct UID
{
    // TODO: Optimize it.
    explicit UID(bool random = true) : tag(random ? uid_generator()() : ::boost::uuids::nil_uuid()) {}
    explicit UID(const ::boost::uuids::uuid &guid) : tag(guid) {}
    explicit UID(::boost::uuids::uuid &&guid) : tag(std::move(guid)) {}
    UID(const UID &) = default;

    operator const ::boost::uuids::uuid &() const { return tag; }
    operator ::std::string() const
    {
        std::stringstream ss;

        ss << tag;

        return ss.str();
    }

    operator const bool() const { return !tag.is_nil(); }

    bool operator<(const UID &uid) const { return uid.tag < tag; }
    bool operator==(const UID &uid) const { return uid.tag == tag; }
    bool operator!=(const UID &uid) const { return uid.tag != tag; }

    ::boost::uuids::uuid tag;
};


inline ::std::ostream &operator<<(std::ostream &s, const UID &uid)
{
    s << uid.tag;
    return s;
}

}  // namespace knp::core
