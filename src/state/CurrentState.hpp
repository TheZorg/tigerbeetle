/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_STATE_CURRENTSTATE_HPP
#define _TIBEE_STATE_CURRENTSTATE_HPP

#include <boost/functional/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/BasicTypes.hpp"
#include "quark/QuarkDatabase.hpp"
#include "state/AttributeKey.hpp"
#include "state/AttributePath.hpp"
#include "state/AttributeTree.hpp"
#include "value/Value.hpp"

namespace tibee
{
namespace state
{

/**
 * Current attribute.
 *
 * @author Francois Doray
 */
class CurrentState
{
public:
    typedef std::function<void (AttributeKey attribute, const value::Value* newValue)>
        OnAttributeChangeCallback;

    CurrentState(OnAttributeChangeCallback onAttributeChangeCallback);
    ~CurrentState();

    void SetTimestamp(timestamp_t ts) {
        _ts = ts;
    }
    timestamp_t timestamp() const {
        return _ts;
    }

    quark::Quark IntQuark(int val);
    quark::Quark Quark(const std::string& str);
    const std::string& String(quark::Quark quark) const;

    AttributeKey GetAttributeKey(const AttributePath& path);
    AttributeKey GetAttributeKeyStr(const AttributePathStr& pathStr);
    AttributeKey GetAttributeKey(AttributeKey root, const AttributePath& subPath);

    void SetAttribute(AttributeKey attribute, value::Value::UP value);
    void SetAttribute(AttributeKey attribute, const AttributePath& subPath, value::Value::UP value);
    void SetAttribute(const AttributePath& path, value::Value::UP value);

    void NullAttribute(AttributeKey attribute);
    void NullAttribute(AttributeKey attribute, const AttributePath& subPath);
    void NullAttribute(const AttributePath& path);

    const value::Value* GetAttributeValue(AttributeKey attribute);
    const value::Value* GetAttributeValue(AttributeKey attribute, const AttributePath& subPath);
    const value::Value* GetAttributeValue(const AttributePath& path);

    timestamp_t GetAttributeLastChange(AttributeKey attribute);
    timestamp_t GetAttributeLastChange(AttributeKey attribute, const AttributePath& subPath);
    timestamp_t GetAttributeLastChange(const AttributePath& path);

private:
    struct AttributeValue {
        AttributeValue();
        value::Value::UP value;
        timestamp_t since;
    };

    // Current timestamp.
    timestamp_t _ts;

    // Quark database.
    quark::QuarkDatabase<std::string> _quarks;

    // Quarks for integers 0 to 65535.
    std::vector<quark::Quark> _intQuarks;

    // Attribute tree.
    AttributeTree _attributeTree;

    // Attribute values.
    typedef std::unordered_map<size_t, AttributeValue> AttributeValues;
    AttributeValues _attributeValues;

    // Callback invoked when an attribute changes.
    OnAttributeChangeCallback _onAttributeChangeCallback;
};

}
}

#endif // _TIBEE_STATE_CURRENTSTATE_HPP
